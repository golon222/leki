/* =====================================================================
 *  PillBoxWeek.ino  -  Pudelko TYGODNIOWE na leki
 *  Seeed XIAO ESP32-C3
 *
 *  Plytka: XIAO_ESP32C3 | USB CDC On Boot: Enabled
 *          Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)
 *
 *  ---------------------------------------------------------------------
 *  CZYM TO SIE ROZNI OD PUDELKA DZIENNEGO
 *
 *  Tam jedna komora i kontaktron: "wieczko ruszylo" znaczy "dawka wzieta".
 *  Tu SIEDEM klapek na JEDNYM pinie - kazda ma wlasny rezystor, wiec
 *  napiecie na D1 mowi, KTORA sie otworzyla. Ten sam pin budzi uklad ze snu
 *  (napiecie spada ponizej progu zera) i mierzy (po wybudzeniu, przez ADC).
 *
 *  Zasada jest jedna: OTWARTE = WZIETE. Bez liczenia tabletek, bez
 *  zmiennych dawek, bez INR. Pudelko ma przypomniec, jesli do danej
 *  godziny klapka nie zostala otwarta - i tyle.
 *
 *  ---------------------------------------------------------------------
 *  CO ZOSTALO ZMIERZONE NA PLYTCE (2026-09-22)
 *
 *    komora      1    2    3    4    5    6    7     zamkniete
 *    napiecie  105  170  300  388  557  633  761    1125 mV
 *
 *  Wybudzanie z glebokiego snu przez najwyzsza galaz (761 mV) - dziala,
 *  sprawdzone. Margines do progu zera (825 mV) wynosi 64 mV, wiec
 *  ZADEN rezystor galezi nie moze byc wiekszy niz 3 kOhm: komora 7
 *  przestalaby budzic pudelko.
 * ===================================================================== */

#include "config.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#if GAUGE_ENABLED
  #include <Wire.h>
#endif
#include <ArduinoJson.h>
#include <time.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#if OTA_ENABLED
  #include <Update.h>
  #include "esp_ota_ops.h"
  #include "esp_partition.h"
#endif
#if PORTAL_ENABLED
  #include <WebServer.h>
  #include <DNSServer.h>
#endif

#if !defined(PILLBOX_WEEK_CONFIG_VERSION)
#error "config.h nie pasuje do tego szkicu - pobierz oba pliki na nowo."
#endif

/*  WERSJA KODU - W TYM PLIKU, obok FW_VERSION, ktore siedzi w config.h.

    TO NIE JEST DUBLOWANIE. Kosztowalo cala runde diagnostyki: Kuba
    podmienil `config.h` (bo tam dolozylem stala), a `PillBoxWeek.ino`
    zostawil stary. Pudelko meldowalo sie wiec jako `0.2.0` i wygladalo na
    zaktualizowane, podczas gdy chodzil na nim kod sprzed naprawy - szukalem
    bledu w czyms, czego na plytce w ogole nie bylo.

    Numer wersji, ktory mieszka w NAGLOWKU, opisuje naglowek. Ten opisuje
    program. Gdy sie rozjada, log krzyczy o tym w pierwszej linii.        */
#define KOD_WERSJA "0.12.0"

/*  Po tym napisie pudelko poznaje config.h wzięty prosto z repozytorium -
    czyli "nie ma zadnej sieci", a nie "ma siec o takiej nazwie". Bez tego
    rozroznienia pudelko wgrane "jak jest" probowaloby laczyc sie
    z placeholderem w kolko, zamiast otworzyc portal.

    MUSI zgadzac sie z wartoscia WIFI_SSID w config.h z repozytorium
    i kontrola statyczna to sprawdza - rozjazd sprawilby, ze portal nie
    otworzy sie nigdy, a to jedyna droga bez kabla.                    */
#define SSID_PLACEHOLDER "TUTAJ_WPISZ_SIEC"

#define LOG(...)  Serial.printf(__VA_ARGS__)

#if OTA_ENABLED
/*  TYP STOI TU, A NIE PRZY SWOJEJ SEKCJI - i to nie jest kwestia gustu.

    Arduino generuje prototypy WSZYSTKICH funkcji i wkleja je na poczatek
    pliku, przed pierwsza linijka kodu. Prototyp `otaOpisDecyzji(OtaDecyzja)`
    trafial wiec nad definicje typu i kompilacja konczyla sie bledem
    "'OtaDecyzja' was not declared in this scope" - ale WYLACZNIE przy
    budowaniu przez sciezke .ino. Ten sam plik jako .cpp budowal sie bez
    slowa (B21/D26: przez miesiac sprawdzalismy tylko .cpp i firmware nie
    dawal sie wgrac). Pudelko dzienne trzyma ten typ wysoko z tego samego
    powodu.                                                             */
enum OtaDecyzja {
  OTA_ROB = 0,          // wszystko sie zgadza - pobieraj
  OTA_NIC_NOWEGO,       // ta sama suma co juz mam
  OTA_BEZ_HASLA,        // pamiec trwala nie ma hasla do bazy
  OTA_KOLEJKA,          // sa niewyslane zdarzenia - one maja pierwszenstwo
  OTA_BATERIA,          // za malo pradu i nie stoi na ladowarce
  OTA_PODDANO,          // OTA_MAX_FAILS prob z rzedu bez skutku
  OTA_ZEPSUTA,          // ta wersja juz raz nie wstala
  OTA_ZLY_OPIS          // plik z opisem nie ma sensu (rozmiar, suma)
};
#endif

#if TG_ENABLED
/*  Ten typ stoi tu z tego samego powodu co OtaDecyzja wyzej: prototypy
    generowane przez Arduino trafiaja przed pierwsza linijke kodu.      */
enum TgDecyzja {
  TG_WYSLIJ = 0,        // jest co wyslac, jest komu i nie jest za pozno
  TG_NIC,               // nic nie czeka
  TG_BRAK_BOTA,         // nikt nie podlaczyl bota w aplikacji
  TG_ZA_STARE           // powstalo dawno - wysylka bylaby dezinformacja
};
#endif

static const uint16_t PROGI[8]  = PROGI_KLAPEK;

/* =====================================================================
 *  PAMIEC RTC  -  przezywa deep sleep, ginie po odlaczeniu zasilania
 * ===================================================================== */
RTC_DATA_ATTR uint32_t rtcWybudzen      = 0;
RTC_DATA_ATTR int32_t  rtcOstatniDzien  = -1;   // numer doby ostatniego otwarcia
RTC_DATA_ATTR int8_t   rtcOstatniaKomora= -1;   // i ktora komora to byla
RTC_DATA_ATTR int32_t  rtcDobaZnana     = -1;   // ostatnia doba, ktora pudelko WIDZIALO
RTC_DATA_ATTR uint8_t  rtcAlarmMaska    = 0;    // ktore sloty juz sie wyczerpaly w tej dobie
RTC_DATA_ATTR int32_t  rtcAlarmDzien    = -1;
RTC_DATA_ATTR int8_t   rtcAlarmSlot     = -1;   // slot, ktory wlasnie ponawiamy
RTC_DATA_ATTR uint8_t  rtcAlarmPonowien = 0;
RTC_DATA_ATTR bool     rtcCzasPewny     = false;
RTC_DATA_ATTR uint8_t  rtcPuste         = 0;    // wybudzenia z pinu, ktore nic nie wykryly
RTC_DATA_ATTR uint16_t rtcPusteRazem    = 0;    // ile ich bylo w ogole - idzie do statusu

/*  AKTUALIZACJA PRZEZ WIFI. `rtcOtaMsg` przezywa sen, zeby powod odmowy
    dojechal do aplikacji takze wtedy, gdy w chwili odmowy nie bylo juz
    sieci. "Nie podalo powodu" jest najgorsza z mozliwych odpowiedzi dla
    kogos, kto stoi nad pudelkiem.                                       */
RTC_DATA_ATTR bool     rtcOtaProsba     = false;
RTC_DATA_ATTR uint32_t rtcOtaTs         = 0;    // kiedy zlozono zlecenie
RTC_DATA_ATTR int32_t  rtcOtaNagl       = 0;    // ile bajtow zapowiedzial serwer
RTC_DATA_ATTR char     rtcOtaMsg[64]    = "";
RTC_DATA_ATTR char     rtcOtaWersja[16] = "";

/*  Przycisk zwarty na stale budzilby uklad w kolko - ten sam rodzaj petli
    co pusta klapka (B30), tylko z innego pinu. Licznik rosnie przy kazdym
    wybudzeniu, po ktorym styk nadal jest zwarty.                        */
RTC_DATA_ATTR uint8_t  rtcPrzyciskZwarty = 0;

/*  POWIADOMIENIA NA TELEFON. Zamiar zapisujemy w chwili, w ktorej
    powstal, a wysylamy przed snem - dane o leku ida pierwsze (zasada 11).
    `rtcTgMsg` przezywa sen, zeby powod dojechal do aplikacji takze wtedy,
    gdy w chwili niepowodzenia nie bylo juz sieci.                       */
RTC_DATA_ATTR int8_t   rtcTgSlot        = -1;   // ktore przypomnienie przepadlo
RTC_DATA_ATTR uint32_t rtcTgSlotTs      = 0;    // kiedy - do liczenia wieku
RTC_DATA_ATTR bool     rtcTgBattCzeka   = false;
RTC_DATA_ATTR bool     rtcTgBattZgloszona = false;
RTC_DATA_ATTR bool     rtcTgTestProsba  = false;
RTC_DATA_ATTR char     rtcTgMsg[48]     = "";

/*  STAN KLAPEK ZGLOSZONY DO APLIKACJI (D137).

    `rtcKlapkiZglosz` to nie jest „stan klapek", tylko „stan, ktory baza
    NA PEWNO przyjela": -1 nic nie zglaszalismy, 0 zamkniete, 1 otwarte.
    Roznica jest cala tresc tej zmiennej - meldunek uznajemy za dostarczony
    dopiero po potwierdzonym zapisie, wiec nieudany wraca przy nastepnym
    wybudzeniu zamiast przepasc (zasada 6, ta sama lekcja co D111).     */
RTC_DATA_ATTR int8_t   rtcKlapkiZglosz  = -1;
RTC_DATA_ATTR uint32_t rtcOpenSince     = 0;    // od kiedy otwarte

/* =====================================================================
 *  STAN BIEZACEGO WYBUDZENIA
 * ===================================================================== */
Preferences nvs;
String  idToken;
int     battProcent   = -1;
float   battVolt      = 0;
int     komoraPoStarcie = -3;      // -1 zamkniete, -2 kilka naraz, 0..6 komora
uint16_t mvPoStarcie   = 0;        // surowy odczyt drabinki z chwili wybudzenia
bool    czasZsync     = false;
/*  JEDEN POMIAR NA WYBUDZENIE, nie odczyt w kazdym miejscu, ktore go
    potrzebuje. W pudelku dziennym dwa odczyty kontaktronu oddalone
    o sekunde pisaly to samo pole i drugi nadpisywal pierwszy - poprawne
    „zamkniete" zamienialo sie w „otwarte" i nikt tego juz nie prostowal
    (D96). Tutaj ten sam blad byl do popelnienia dokladnie tak samo.  */
bool    otwarteTeraz  = false;
/*  SKAD WZIAL SIE PROCENT BATERII. Idzie do statusu, bo to jest jedyna
    rzecz, ktora rozstrzyga pytanie "czy czujnik w ogole gada" - a do
    tego pytania wracalismy juz kilka razy (D136, D139).              */
const char* battZrodlo = "brak";

/*  STAN CZUJNIKA - OSOBNO OD ZRODLA POMIARU (D142).

    `battZrodlo` mowi, SKAD jest procent. To nie wystarcza, bo dwa
    zupelnie rozne klopoty koncza sie tak samo ("dzielnik"): czujnika
    NIE MA na magistrali - i to naprawia sie lutownica - albo czujnik
    JEST, lecz jeszcze nie policzyl - i to naprawia sie odczekaniem
    minuty. Kuba: "nie bede patrzyl na monitor, zobacze w aplikacji",
    wiec rozroznienie musi dojechac do telefonu, nie tylko do logu.

    "ok" | "czeka" | "cichy" - puste znaczy "pudelko bez czujnika".   */
const char* gaugeStan  = "";

/*  Napiecie z dzielnika zmierzone W TEJ SAMEJ CHWILI co odczyt
    z czujnika. -1 znaczy "nie bylo czego porownywac". Jedyny cel:
    rozstrzygnac zagadke 2,32 V bez monitora portu (D141).           */
float   battVoltDz     = -1.0f;

/* Harmonogram przypomnien. To sa godziny PRZYPOMNIEN, nie pory brania -
   dokladnie jak w pudelku dziennym. Domyslnie jedna, 20:00.            */
#define SLOTOW_MAX 4
int slotyMin[SLOTOW_MAX] = { 20*60 };
int slotowIle = 1;

/* =====================================================================
 *  1.  DRABINKA  -  ktora klapka jest otwarta
 * ===================================================================== */
uint16_t czytajKlapki(int probek = 64) {
  uint32_t suma = 0;
  for (int i = 0; i < probek; i++) suma += analogReadMilliVolts(PIN_KLAPKI);
  return suma / probek;
}

/* -1 = wszystko zamkniete, -2 = kilka naraz, 0..6 = numer komory.

   Progi stoja w config.h i pochodza Z POMIARU, nie z obliczen. Rezystory
   maja tolerancje, a roznica miedzy wyliczonym a zmierzonym napieciem
   siega kilku miliwoltow - przy odstepie 65 mV miedzy komora 1
   a komora 2 to jeszcze nic, ale zgadywanie zamiast pomiaru bylo
   dokladnie tym, czego caly ten projekt unika.                        */
int ktoraKomora(uint16_t mV) {
  if (mV > PROGI[7]) return -1;
  if (mV < PROGI[0]) return -2;
  for (int i = 6; i >= 0; i--) if (mV >= PROGI[i]) return i;
  return -2;
}

/* Czy ktorakolwiek klapka jest teraz otwarta. */
bool klapkiOtwarte();

/*  NUMER, NIE DZIEN TYGODNIA (D140).

    Do 0.10.0 stalo tu "PON".."ND" - drabinka nadawala komorom dni
    tygodnia. Kuba: "chce zrezygnowac z tych dni, bo to i tak bez sensu,
    jak sie zmienia polaczenie". Ma racje: przypisanie dnia zyje
    w kolejnosci rezystorow, a nie w niczym, co pudelko moze sprawdzic.
    Jeden przelozony przewod i pudelko mowi "PON" o srodowej klapce -
    z pelna pewnoscia i bez sladu w logu.

    Numer jest tym, co pudelko NAPRAWDE wie: komora o najnizszym
    napieciu to 1. Zgadza sie z liczba piknien przy otwarciu i przy
    autotescie, wiec log i ucho mowia to samo.

    UWAGA: bufor jest statyczny, wiec nie wolaj tego dwa razy w jednym
    printf - drugie wywolanie nadpisze pierwsze.                      */
const char* opisKomory(int k) {
  if (k == -1) return "zamkniete";
  if (k == -2) return "kilka naraz";
  if (k < 0 || k >= 7) return "?";
  static char buf[12];
  snprintf(buf, sizeof(buf), "komora %d", k + 1);
  return buf;
}

/* Czy ktorakolwiek klapka jest teraz otwarta. Stoi wysoko, bo korzysta
   z niej takze status wysylany do aplikacji - a ten jest w sekcji 7. */
bool klapkiOtwarte() { return ktoraKomora(czytajKlapki(16)) != -1; }

/* =====================================================================
 *  2.  BUZZER
 * ===================================================================== */
bool buzzerGotowy = false;
bool ponowAlarm   = false;      // wracamy za ALARM_PONOW_MIN minut

void buzzerInit() {
  if (!buzzerGotowy) { ledcAttach(PIN_BUZZER, BUZZER_HZ, 10); buzzerGotowy = true; }
}
void pik(uint32_t hz, uint32_t ms) {
  buzzerInit();
  ledcWriteTone(PIN_BUZZER, hz);
  delay(ms);
  ledcWriteTone(PIN_BUZZER, 0);
}

/* Tyle piknięc, ktora to komora: 1 ... 7. Kuba prosil wprost,
   zeby dalo sie sprawdzic pudelko na sluch, bez podlaczania komputera. */
void pikniecia(int ile) {
  for (int i = 0; i < ile; i++) { pik(BUZZER_HZ, 130); if (i < ile-1) delay(190); }
}
void beepAck()        { pik(BUZZER_HZ, 90); }
void beepBlad()       { pik(700, 260); }
void beepKilkaNaraz() { pik(700, 700); }
void beepAlarm()      { for (int i=0;i<3;i++){ pik(BUZZER_HZ,180); delay(120);} }
void beepBateria()    { for (int i=0;i<2;i++){ pik(1200,300); delay(180);} }

/* =====================================================================
 *  3.  BATERIA
 *  Dzielnik 100k/100k z BAT+, wiec napiecie ogniwa to dwa razy odczyt.
 *  Pin 3V3 dalby stale 3,3 V niezaleznie od stanu ogniwa - to jest
 *  wyjscie stabilizatora i mierzenie go nie mowi nic o baterii.
 * ===================================================================== */
#if GAUGE_ENABLED
/*  JEDEN REJESTR Z CZUJNIKA. Zwraca false przy kazdym potknieciu - brak
    czujnika na magistrali jest tu zwyklym, spodziewanym wynikiem, a nie
    awaria: ten sam program ma chodzic na plytce z czujnikiem i bez.

    DWA ODCZYTY DO OSOBNYCH ZMIENNYCH, nie w jednym wyrazeniu. Kolejnosc
    obliczania argumentow `|` jest w C++ NIEOKRESLONA, wiec zapis
    `(Wire.read() << 8) | Wire.read()` potrafi przeczytac bajty odwrotnie
    - raz na jednym kompilatorze dobrze, raz na innym zle. Klasyczna
    pulapka, ktorej nie widac, dopoki nie zmieni sie rdzenia.         */
bool gaugeRejestr(uint8_t rej, uint16_t& wynik) {
  Wire.beginTransmission(GAUGE_ADDR);
  Wire.write(rej);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((int)GAUGE_ADDR, 2) != 2) return false;
  const uint8_t hi = Wire.read();
  const uint8_t lo = Wire.read();
  wynik = ((uint16_t)hi << 8) | lo;
  return true;
}

/*  Napiecie i stan naladowania z czujnika.

    VCELL (0x02): krok 78,125 uV. SOC (0x04): gorny bajt to cale procenty,
    dolny - 1/256 procenta.

    Wynik poza granicami zdrowego rozsadku traktujemy jak brak czujnika.
    MAX17048 po podlaczeniu ogniwa potrzebuje chwili, zanim poda sensowny
    procent, a przez ten czas lepiej wziac odczyt z dzielnika niz podac
    liczbe, ktorej sami byśmy nie uwierzyli.                          */
bool gaugeCzytaj(float& napiecie, int& procent) {
  /*  TRZY PROBY, NIE JEDNA (D141).

      Pierwsze wlaczenie po przylutowaniu jest jedynym momentem, w ktorym
      ta sciezka naprawde cos rozstrzyga - a akurat wtedy jest najmniej
      pewna: MAX17048 po podaniu zasilania potrzebuje chwili, zanim poda
      sensowny procent. Jedna nieudana proba zapisalaby "czujnika nie ma"
      na cale wybudzenie i Kuba zobaczylby w aplikacji kreske, stojac nad
      swiezo polutowanym czujnikiem.

      Przy braku czujnika kosztuje to kilkaset milisekund na wybudzenie -
      przy radiu liczonym w sekundach jest to nizej progu zauwazalnosci. */
  for (int proba = 0; proba < 3; proba++) {
    if (proba) delay(60);
    uint16_t vcell = 0, soc = 0;
    if (!gaugeRejestr(0x02, vcell)) continue;
    if (!gaugeRejestr(0x04, soc))   continue;

    const float v = vcell * 0.000078125f;
    const float p = soc / 256.0f;
    if (v < GAUGE_MIN_V || v > GAUGE_MAX_V) continue;
    if (p < 0.0f || p > 100.0f)             continue;

    napiecie = v;
    procent  = (int)(p + 0.5f);
    if (procent > 100) procent = 100;
    return true;
  }
  return false;
}

/*  CZY UKLAD W OGOLE ODPOWIADA NA MAGISTRALI.

    Rozroznia dwie rzeczy, ktore inaczej wygladaja w logu identycznie:
    czujnika NIE MA (zle przewody, zly adres, brak zasilania modulu)
    kontra czujnik JEST, ale jeszcze nie podaje sensownej liczby.
    Pierwsze naprawia sie lutownica, drugie - odczekaniem minuty, wiec
    pomylenie ich kosztuje wieczor przy rozlutowanym module.

    Rejestr VERSION (0x08) odpowiada natychmiast po wlaczeniu, nie czeka
    na zaden model ogniwa.                                             */
bool gaugeObecny(uint16_t& wersja) {
  return gaugeRejestr(0x08, wersja);
}
#endif  /* GAUGE_ENABLED */

/*  NAPIECIE Z DZIELNIKA NA PLYTCE.

    Osobna funkcja, bo ten odczyt sluzy teraz dwóm rzeczom: jest zapasem,
    gdy czujnika nie ma, i jest punktem porownania, gdy czujnik
    odpowiedzial (D141). Dwie kopie tej petli rozjechalyby sie przy
    pierwszej poprawce wspolczynnika.                                  */
float dzielnikVolt() {
  uint32_t suma = 0;
  for (int i = 0; i < 32; i++) suma += analogReadMilliVolts(PIN_BATERIA);
  return (suma / 32.0f) * 2.0f / 1000.0f;
}

void czytajBaterie() {
#if GAUGE_ENABLED
  /*  CZUJNIK MA PIERWSZENSTWO, dzielnik jest zapasem - nie odwrotnie.

      Dzielnik na tej plytce melduje 2,32 V, czyli wartosc niemozliwa
      przy dzialajacym radiu (D136), i nie wiemy dlaczego. Czujnik mierzy
      ogniwo bezposrednio i liczy procent wlasnym modelem LiPo, wiec gdy
      odpowiada, jest po prostu lepszym zrodlem.

      Gdy nie odpowiada - bo go nie ma albo przewod odszedl - schodzimy
      nizej bez slowa skargi. Pudelko bez czujnika ma dzialac tak samo
      jak przed jego dolozeniem.                                       */
  Wire.begin(PIN_SDA, PIN_SCL);
  if (gaugeCzytaj(battVolt, battProcent)) {
    battZrodlo = "max17048";
    gaugeStan  = "ok";
    LOG("[BAT] czujnik: %d%%  %.2f V\n", battProcent, battVolt);
    /*  DZIELNIK MIERZYMY TAKZE WTEDY, GDY CZUJNIK ODPOWIEDZIAL.

        Do rozstrzygniecia JEDNEGO otwartego pytania
        z CLAUDE.md: dzielnik melduje 2,32 V i nie wiemy, czy to zly
        wspolczynnik (do skalibrowania), czy brak kontaktu (do
        przelutowania). Czujnik jest pierwszym wiarygodnym punktem
        odniesienia, jaki ta plytka kiedykolwiek miala, wiec roznica
        dwoch pomiarow w jednej chwili odpowiada na to sama.

        Jedzie do statusu jako `voltDz`, bo log czyta sie przez kabel,
        a telefon ma sie przy sobie zawsze (D142). Kosztuje 32 odczyty
        ADC, czyli ulamek milisekundy, i nie zmienia ani jednej liczby,
        ktora aplikacja pokazuje jako stan baterii.                   */
    const float vDz = dzielnikVolt();
    battVoltDz = vDz;
    LOG("[BAT] dzielnik w tej samej chwili: %.2f V (czujnik %.2f V, iloraz %.2f)\n",
        vDz, battVolt, vDz > 0.01f ? battVolt / vDz : 0.0f);
    return;
  }
  uint16_t wersja = 0;
  gaugeStan = gaugeObecny(wersja) ? "czeka" : "cichy";
  if (strcmp(gaugeStan, "czeka") == 0)
    LOG("[BAT] czujnik ODPOWIADA (wersja 0x%04X), ale nie podal jeszcze procentu - "
        "biore odczyt z dzielnika\n", wersja);
  else
    LOG("[BAT] czujnik nie odpowiada - biore odczyt z dzielnika\n");
#endif

  battVolt = dzielnikVolt();

  /* Krzywa uproszczona: 4,20 V = 100%, 3,30 V = 0%. Nie jest liniowa
     naprawde, ale do ostrzezenia "laduj" wystarcza, a udawanie precyzji
     bez pomiaru rozladowania byloby zgadywaniem.                      */
  /*  ODCZYT NIEMOZLIWY TO "NIE WIEM", NIE "0%".

      Z logu Kuby: `[BAT] 0%  2.32 V` - przy dzialajacym radiu i -39 dBm.
      To nie moze byc prawda: stabilizator na XIAO nie wyciagnie 3,3 V
      z ogniwa przy 2,32 V, wiec przy takim napieciu plytka bylaby
      martwa, a nie gadatliwa. Zepsuty jest POMIAR (u Kuby urwany pad
      BAT+, wiec dzielnik wisi w powietrzu), nie bateria.

      Zglaszane "0%" kosztowalo podwojnie: falszywy alarm "laduj"
      w aplikacji i dwa dodatkowe pikniecia przy KAZDYM wybudzeniu.
      Lepiej nie wiedziec niz wiedziec zle - `-1` aplikacja juz umie
      pokazac jako brak danych.                                        */
  if (battVolt < BATT_MIN_SENS_V) {
    battProcent = -1;
    battZrodlo  = "brak";
    LOG("[BAT] odczyt %.2f V niemozliwy przy dzialajacej plytce - zglaszam brak danych\n",
        battVolt);
    return;
  }
  float p = (battVolt - 3.30f) / (4.20f - 3.30f) * 100.0f;
  battProcent = (int)(p < 0 ? 0 : (p > 100 ? 100 : p));
  battZrodlo  = "dzielnik";
}

/* =====================================================================
 *  4.  KOLEJKA OFFLINE
 *  Zdarzenie NIGDY nie ginie przez brak sieci. Kasujemy je z pamieci
 *  dopiero po potwierdzonym 2xx z bazy - ta zasada jest w tym projekcie
 *  twarda i nie ma od niej wyjatku po stronie danych o leku.
 * ===================================================================== */
int kolejkaIle() { return nvs.getInt("q_ile", 0); }

void kolejkaDodaj(const String& rec) {
  int ile = kolejkaIle();
  if (ile >= KOLEJKA_MAX) {
    /* Pelna kolejka: zdejmujemy NAJSTARSZY wpis, nie najnowszy.
       Przy czterdziestu zaleglych zdarzeniach swieze otwarcie mowi
       wiecej niz to sprzed dwoch tygodni.                            */
    for (int i = 1; i < ile; i++) {
      char a[12], b[12];
      snprintf(a, sizeof(a), "q%d", i);
      snprintf(b, sizeof(b), "q%d", i-1);
      nvs.putString(b, nvs.getString(a, ""));
    }
    ile--;
  }
  char k[12]; snprintf(k, sizeof(k), "q%d", ile);
  nvs.putString(k, rec);
  nvs.putInt("q_ile", ile + 1);
  LOG("[Q  ] do kolejki (%d): %s\n", ile + 1, rec.c_str());
}

/*  Podmienia OSTATNI wpis, ten wlasnie dolozony. Potrzebne, gdy zegar
    zsynchronizowal sie juz po zakolejkowaniu zdarzenia: rekord trzeba
    przepisac z prawdziwa data.

    Wczesniej stalo tu kolejkaZdejmij() + kolejkaDodaj(), czyli zdjecie
    wpisu NAJSTARSZEGO - a to przy niepustej kolejce kasowalo cudze,
    zalegle otwarcie zamiast poprawic wlasne.                         */
void kolejkaPodmienOstatni(const String& rec) {
  int ile = kolejkaIle();
  if (ile <= 0) { kolejkaDodaj(rec); return; }
  char k[12]; snprintf(k, sizeof(k), "q%d", ile - 1);
  nvs.putString(k, rec);
}

String kolejkaPierwszy() {
  if (kolejkaIle() <= 0) return "";
  return nvs.getString("q0", "");
}

void kolejkaZdejmij() {
  int ile = kolejkaIle();
  if (ile <= 0) return;
  for (int i = 1; i < ile; i++) {
    char a[12], b[12];
    snprintf(a, sizeof(a), "q%d", i);
    snprintf(b, sizeof(b), "q%d", i-1);
    nvs.putString(b, nvs.getString(a, ""));
  }
  nvs.putInt("q_ile", ile - 1);
}

/* =====================================================================
 *  5.  CZAS I DOBA LEKOWA
 *  Granica doby to DAY_START_HOUR - ta sama wartosc co w aplikacji.
 *  Rozjazd tych dwoch liczb znaczylby, ze pudelko i telefon licza inne
 *  dni, a kalendarz zaczalby sam sie poprawiac w tle.
 * ===================================================================== */
/*  Numer doby lekowej - kolejna liczba calkowita, wiec wolno ja
    odejmowac ("wczoraj" to po prostu doba-1).

    Liczymy z czasu LOKALNEGO, nie wprost z epoki. Wersja "epoka minus
    trzy godziny, podzielone przez dobe" stawiala granice na 3:00 UTC,
    czyli o 4:00 albo 5:00 w Polsce - zaleznie od czasu letniego.
    Klapka otwarta o 4:30 trafialaby wtedy do dnia poprzedniego.      */
static int32_t dniOdEry(int r, int m, int d) {        // algorytm Hinnanta
  r -= (m <= 2);
  const int32_t  era = (r >= 0 ? r : r - 399) / 400;
  const uint32_t yoe = (uint32_t)(r - era * 400);
  const uint32_t doy = (153u * (uint32_t)(m + (m > 2 ? -3 : 9)) + 2u) / 5u + (uint32_t)d - 1u;
  const uint32_t doe = yoe * 365u + yoe/4u - yoe/100u + doy;
  return era * 146097 + (int32_t)doe - 719468;
}

int32_t numerDoby(time_t t) {
  if (t < 1600000000) return -1;               // zegar jeszcze nie ustawiony
  time_t przesuniety = t - (time_t)DAY_START_HOUR * 3600;
  struct tm lt;
  localtime_r(&przesuniety, &lt);
  return dniOdEry(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday);
}
int32_t dzisDoba() { return numerDoby(time(nullptr)); }

/*  Chwila, o ktorej zaczela sie doba lekowa zawierajaca `t`.
    Punkt odniesienia dla domykania dni - patrz domknijDoby().        */
time_t poczatekDoby(time_t t) {
  time_t przesuniety = t - (time_t)DAY_START_HOUR * 3600;
  struct tm lt;
  localtime_r(&przesuniety, &lt);
  return t - (time_t)(lt.tm_hour * 3600L + lt.tm_min * 60L + lt.tm_sec);
}

int minutyDnia() {
  time_t t = time(nullptr);
  struct tm lt;
  localtime_r(&t, &lt);
  return lt.tm_hour * 60 + lt.tm_min;
}

/* Ktory slot przypomnienia wypada teraz (+/- 30 min), albo -1. */
int slotTeraz() {
  int m = minutyDnia();
  for (int i = 0; i < slotowIle; i++)
    if (abs(m - slotyMin[i]) <= 30) return i;
  return -1;
}

/* =====================================================================
 *  6.  WiFi
 * ===================================================================== */
bool wifiPolacz() {
  if (WiFi.status() == WL_CONNECTED) return true;

  WiFi.mode(WIFI_STA);
  String ssid = nvs.getString("ssid", WIFI_SSID);
  String pass = nvs.getString("wpass", WIFI_PASS);
  LOG("[NET] lacze z '%s'\n", ssid.c_str());
  WiFi.begin(ssid.c_str(), pass.c_str());

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_TIMEOUT_S * 1000UL)
    delay(120);

  if (WiFi.status() != WL_CONNECTED) { LOG("[NET] brak sieci\n"); return false; }
  LOG("[NET] polaczono, %d dBm\n", WiFi.RSSI());

  /* Zegar synchronizujemy PRZY KAZDYM polaczeniu. Zegar w ESP32 dryfuje
     kilkanascie minut na miesiac, a przy przypomnieniu o leku to jest
     roznica miedzy "o 20:00" a "kiedy sie uda".                       */
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);   // Polska, z czasem letnim
  tzset();
  for (int i = 0; i < 40 && time(nullptr) < 1600000000; i++) delay(150);
  czasZsync   = time(nullptr) > 1600000000;
  rtcCzasPewny = czasZsync || rtcCzasPewny;
  LOG("[NET] czas %s\n", czasZsync ? "zsynchronizowany" : "NIEZNANY");
  return true;
}

void wifiWylacz() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

/* =====================================================================
 *  7.  FIREBASE
 * ===================================================================== */
WiFiClientSecure klient;
bool klientGotowy = false;

/* Haslo czytamy z NVS, nie wprost z config.h.

   To ta sama decyzja co w pudelku dziennym (D59, ograniczenie 10
   w CLAUDE.md): binarke buduje automat z publicznego repozytorium, wiec
   wkompilowane haslo byloby jego wyciekiem. config.h trzyma placeholder
   i jest tylko ZIARNEM przy pierwszym wgraniu kablem.                 */
String hasloDoLogowania() {
  String z = nvs.getString("haslo", "");
  if (z.length()) return z;
  return String(DEVICE_PASSWORD);
}

bool hasloWPamieci() { return nvs.getString("haslo", "").length() > 0; }

/* ZWRACA WYNIK, i to nie jest kosmetyka (zasada 9, D38): portal kasuje
   haslo z powrotem, gdy baza je odrzuci, a do tego musi wiedziec, czy
   w ogole sie zapisalo. Zapis do NVS potrafi sie nie udac po cichu.  */
bool hasloUtrwal(const String& h) {
  if (h == "TUTAJ_WPISZ_HASLO" || !h.length()) return false;
  if (nvs.getString("haslo", "") == h) return true;
  nvs.putString("haslo", h);
  /* Potwierdzenie ODCZYTEM ZWROTNYM. Zapis do NVS potrafi sie nie udac
     po cichu, a haslo, ktorego pudelko nie ma, odcina je od bazy - czyli
     od jedynej drogi naprawy bez kabla.                               */
  if (nvs.getString("haslo", "") == h) { LOG("[FB ] haslo zapisane w pamieci\n"); return true; }
  LOG("[FB ] UWAGA: haslo NIE zapisalo sie\n");
  return false;
}

#if TG_ENABLED
/* --- BOT TELEGRAM W PAMIECI TRWALEJ ----------------------------------
   Token bota jest sekretem tej samej klasy co haslo do WiFi: kto go ma,
   pisze w imieniu bota i czyta wszystko, co ktos do niego napisze.
   Dlatego idzie ta sama droga i podlega tej samej zasadzie 9 (D38, D67):
   aplikacja -> baza -> zapis w NVS -> ODCZYT KONTROLNY -> dopiero potem
   kasowanie z bazy. W `config.h` stac nie moze z tego samego powodu co
   haslo do bazy: binarke buduje automat z publicznego repozytorium.

   Token nie trafia ANI DO LOGU, ANI DO STATUSU. Do aplikacji idzie
   wylacznie `tg` (jest/nie ma) i `tgMsg` (co sie stalo).             */
bool tgSkonfigurowany() {
  return nvs.getString("tgTok", "").length() > 0
      && nvs.getString("tgChat", "").length() > 0;
}

/* Zapis obu wartosci naraz, potwierdzony odczytem. Zwraca false takze
   wtedy, gdy zapisala sie tylko jedna polowa - polowiczna konfiguracja
   wygladalaby jak dzialajaca, a nie byla.                             */
bool tgUtrwal(const String& token, const String& chat) {
  if (!token.length() || token.length() > TG_TOKEN_MAX) return false;
  if (!chat.length()  || chat.length()  > TG_CHAT_MAX)  return false;
  nvs.putString("tgTok",  token);
  nvs.putString("tgChat", chat);
  return nvs.getString("tgTok", "")  == token
      && nvs.getString("tgChat", "") == chat;
}

void tgZapomnij() {
  nvs.remove("tgTok");
  nvs.remove("tgChat");
}

/* --- Czy w ogole wysylac - i dlaczego nie ----------------------------
   ZNAK W ZNAK ta sama funkcja co w PillBox.ino - kontrola statyczna
   porownuje oba ciala, tak samo jak przy `otaDecyzja()`. Wydzielona
   z wysylki po to, zeby dalo sie ja przetestowac bez sieci i bez pamieci
   trwalej; sama wysylka to czyste we/wy.

   KOLEJNOSC PYTAN JEST TRESCIA. Najpierw "czy jest o czym pisac" - bo
   przy pustej skrzynce nie wolno wlaczyc radia ani na sekunde. Potem
   "czy jest komu" i dopiero na koncu "czy nie za pozno".

   Nieznany czas nie jest powodem do milczenia: gdy nie umiemy zmierzyc
   wieku wiadomosci, wysylamy ja. Milkniemy wylacznie wtedy, gdy wiemy
   na pewno - ta sama zasada co przy dniach bez leku.                 */
TgDecyzja tgDecyzja(bool botPodlaczony, bool cosCzeka,
                    uint32_t tsPowstania, uint32_t teraz) {
  if (!cosCzeka)      return TG_NIC;
  if (!botPodlaczony) return TG_BRAK_BOTA;
  if (tsPowstania && teraz && teraz > tsPowstania &&
      teraz - tsPowstania > (uint32_t)TG_MAX_WIEK_S) return TG_ZA_STARE;
  return TG_WYSLIJ;
}
#endif  /* TG_ENABLED */

bool firebaseZaloguj() {
  if (idToken.length()) return true;

  if (!klientGotowy) { klient.setInsecure(); klient.setTimeout(10); klientGotowy = true; }

  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(10000);
  String url = String("https://identitytoolkit.googleapis.com/v1/"
                      "accounts:signInWithPassword?key=") + WEB_API_KEY;
  if (!http.begin(klient, url)) return false;
  http.addHeader("Content-Type", "application/json");

  JsonDocument req;
  req["email"]             = DEVICE_EMAIL;
  req["password"]          = hasloDoLogowania();
  req["returnSecureToken"] = true;
  String body; serializeJson(req, body);

  int code = http.POST(body);
  String odp = (code > 0) ? http.getString() : "";
  http.end();

  if (code != 200) {
    LOG("[FB ] logowanie HTTP %d: %s\n", code, odp.c_str());
    return false;
  }
  JsonDocument doc;
  if (deserializeJson(doc, odp)) return false;
  idToken = doc["idToken"].as<String>();
  if (!idToken.length()) return false;

  LOG("[FB ] zalogowano\n");
  hasloUtrwal(hasloDoLogowania());
  return true;
}

int rtdbWyslij(const char* metoda, const String& sciezka,
               const String& body, String* odp = nullptr) {
  if (!klientGotowy) { klient.setInsecure(); klient.setTimeout(10); klientGotowy = true; }
  HTTPClient http;
  http.setReuse(true);
  http.setConnectTimeout(8000);
  http.setTimeout(10000);
  String url = String("https://") + RTDB_HOST + sciezka + "?auth=" + idToken;
  if (!http.begin(klient, url)) return -1;
  http.addHeader("Content-Type", "application/json");
  int code = http.sendRequest(metoda, body);
  if (odp && code > 0) *odp = http.getString();
  http.end();
  if (code == 401 || code == 403) { idToken = ""; }   // token do wymiany
  return code;
}

/* Rekord w kolejce: "ts;typ;bateria;napiecie;slot"
   Ten sam format co w pudelku dziennym - dzieki temu aplikacja czyta
   oba urzadzenia tym samym kodem.                                     */
String zrobRekord(const char* typ, int slot, uint32_t ts) {
  char buf[80];
  snprintf(buf, sizeof(buf), "%lu;%s;%d;%.3f;%d",
           (unsigned long)ts, typ, battProcent, battVolt, slot);
  return String(buf);
}

/* Wysyla jedno zdarzenie. Zwraca kod HTTP, bo dzwoniacy musi odroznic
   "sprobuj pozniej" od "to nie przejdzie nigdy".

   UWAGA na galaz `events`: ma w regulach `$other: false`, wiec DOLOZENIE
   choc jednego nieznanego pola odrzuca CALY wpis kodem 400. Numer komory
   jedzie wiec w polu `slot`, ktore juz istnieje - nie w nowym polu.   */
int wyslijZdarzenie(const String& rec) {
  int p1 = rec.indexOf(';');
  int p2 = rec.indexOf(';', p1 + 1);
  int p3 = rec.indexOf(';', p2 + 1);
  int p4 = rec.indexOf(';', p3 + 1);
  if (p1 < 0 || p2 < 0 || p3 < 0 || p4 < 0) return 400;   // uszkodzony

  uint32_t ts = (uint32_t)rec.substring(0, p1).toInt();

  JsonDocument doc;
  doc["ts"]      = (ts > 1600000000UL) ? ts : 0;   // 0 = czas nieznany
  doc["type"]    = rec.substring(p1 + 1, p2);
  /*  Pomiaru, ktorego nie bylo, NIE WYSYLAMY. Reguly przyjmuja w zdarzeniu
      kazda liczbe, wiec -1 by przeszlo - i aplikacja pokazywalaby "0%"
      albo "-1%" jako fakt z urzadzenia. Brak pola czyta sie uczciwie
      jako brak pomiaru.                                              */
  const int bat = rec.substring(p2 + 1, p3).toInt();
  if (bat >= 0) {
    doc["battery"] = bat;
    doc["volt"]    = rec.substring(p3 + 1, p4).toFloat();
  }
  doc["slot"]    = rec.substring(p4 + 1).toInt();
  doc["fw"]      = FW_VERSION;

  String body; serializeJson(doc, body);
  String odp;
  int code = rtdbWyslij("POST", "/devices/" DEVICE_ID "/events.json", body, &odp);
  if (code == 200) LOG("[FB ] zdarzenie OK: %s\n", body.c_str());
  else             LOG("[FB ] zdarzenie HTTP %d: %s\n     baza: %s\n",
                       code, body.c_str(), odp.c_str());
  return code;
}

/* Opróznia kolejke. Wpis znika DOPIERO po 200.
   Wyjatek: 400 znaczy, ze baza nie przyjmie go nigdy - zostawiony
   blokowalby wszystkie zdarzenia za soba.                             */
void oproznijKolejke() {
  int prob = 0;
  while (kolejkaIle() > 0 && prob++ < KOLEJKA_MAX) {
    String rec = kolejkaPierwszy();
    if (!rec.length()) { kolejkaZdejmij(); continue; }
    int code = wyslijZdarzenie(rec);
    if (code == 200)      kolejkaZdejmij();
    else if (code == 400) { LOG("[Q  ] baza odrzuca na stale - zdejmuje\n"); kolejkaZdejmij(); }
    else break;                                   // siec - sprobujemy pozniej
  }
}

#if OTA_ENABLED
/*  Deklaracja zapowiadajaca. Status stoi w tym pliku PRZED sekcja
    aktualizacji, a potrzebuje z niej jednej funkcji. Szkic .ino dostaje
    prototypy od preprocesora Arduino, ale ten sam plik kompilujemy TAKZE
    jako .cpp (B21/D26) - a tam ich nie ma i brak tej linijki zatrzymalby
    budowanie.                                                          */
String otaSumaWgranej();
#endif

bool wyslijStatus() {
  JsonDocument doc;
  /*  `status/battery` ma w regulach zakres 0..100, wiec -1 odrzucilby
      CALY status - a razem z nim wersje programu, sile sygnalu i stan
      kolejki, czyli wszystko, z czego widac, ze pudelko zyje. Brakujacy
      pomiar po prostu pomijamy.                                       */
  if (battProcent >= 0) doc["battery"] = battProcent;
  /*  NAPIECIE WYSYLAMY ZAWSZE, takze przy niemozliwym odczycie.

      Do 0.6.0 szlo razem z procentem - wiec przy zepsutym pomiarze nie
      szlo NIC i z zewnatrz nie dalo sie zobaczyc nawet tego, co plytka
      naprawde mierzy. Ta plytka melduje 2,32 V, czyli wartosc, przy
      ktorej by nie chodzila; dopoki nie wiemy, czy ta liczba chociaz
      RUSZA SIE przy ladowaniu, nie da sie zdecydowac, czy to dzielnik,
      czy zly pin. Jedna liczba w statusie kosztuje kilkanascie bajtow
      i jest jedynym pomiarem, jaki mamy.

      Procentu przy tym NIE zgadujemy: reguly bazy daja mu zakres 0..100,
      a 0% znaczy "naladuj natychmiast" - falszywy alarm o tym samym
      ciezarze co przegapiona dawka.                                  */
  doc["volt"]    = battVolt;
  /*  SKAD jest ten pomiar. Bez tego pola "62%" z czujnika i "62%"
      z dzielnika wygladaja identycznie, a to one wlasnie rozstrzygaja,
      czy czujnik gada - jedyne pytanie, ktore w tej sprawie zostalo. */
  doc["battSrc"] = battZrodlo;
#if GAUGE_ENABLED
  /*  STAN CZUJNIKA, A NIE TYLKO ZRODLO LICZBY (D142).

      "czeka" i "cichy" to z punktu widzenia procentu to samo - dzielnik
      - a z punktu widzenia czlowieka dwie rozne roboty: odczekac minute
      albo wziac lutownice. Pudelko wie, ktora to; aplikacja bez tego
      pola nie ma jak sie dowiedziec.                                 */
  doc["gauge"]   = gaugeStan;
  /*  Dzielnik zmierzony obok czujnika. Idzie tylko wtedy, gdy naprawde
      bylo co zmierzyc - zero wygladaloby jak pomiar.                 */
  if (battVoltDz >= 0) doc["voltDz"] = battVoltDz;
#endif
  doc["fw"]      = FW_VERSION;
  doc["rssi"]    = WiFi.RSSI();
  doc["queue"]   = kolejkaIle();
  doc["wakes"]   = (uint32_t)rtcWybudzen;
  /*  Puste wybudzenia sa OBJAWEM, nie ciekawostka: jesli ta liczba rosnie,
      pudelko budzi sie z pinu bez powodu i trzeba na to spojrzec.      */
  doc["puste"]   = (uint32_t)rtcPusteRazem;
  /*  `lastSeen`, NIE `ts` - i to nie jest kosmetyka (D137).

      Do 0.7.0 pudelko tygodniowe wysylalo w statusie pole `ts`, ktorego
      aplikacja NIE CZYTA NIGDZIE. Chwile „ostatnio widziane" bierze
      z `lastSeen` - w kilkunastu miejscach naraz: ostatnia synchronizacja,
      ostrzezenie o milczeniu, swiezosc meldunku o klapce, prognoza
      baterii. Wszystkie one dostawaly zero i zachowywaly sie tak, jakby
      pudelko nie odezwalo sie nigdy.

      Jedno zle nazwane pole, a objawow tyle, ile miejsc je czyta.     */
  doc["lastSeen"] = (uint32_t)time(nullptr);
  /*  OTWARTA KLAPKA - to pole aplikacja pokazuje jako baner „Pudelko jest
      otwarte". Do 0.7.0 pudelko tygodniowe NIE WYSYLALO GO W OGOLE, wiec
      baner nie mial jak sie zapalic ani razu (D137).

      Bierzemy `otwarteTeraz`, a nie swiezy odczyt pinu: jeden pomiar na
      wybudzenie, jedna prawda.                                        */
  doc["boxOpen"]   = otwarteTeraz;
  if (otwarteTeraz && rtcOpenSince) doc["openSince"] = rtcOpenSince;
#if OTA_ENABLED
  /*  Stan aktualizacji jedzie TYMI SAMYMI polami co w pudelku dziennym -
      aplikacja czyta oba urzadzenia jednym kodem. `otaMsg` to jedyne
      miejsce, z ktorego czlowiek dowiaduje sie, DLACZEGO pudelko nie
      zaktualizowalo sie po nacisnieciu przycisku.                      */
  doc["otaMsg"]    = rtcOtaMsg;
  doc["otaWersja"] = rtcOtaWersja;
  doc["otaHaslo"]  = hasloWPamieci();
  doc["otaProsba"] = rtcOtaProsba;
  doc["otaFail"]   = nvs.getUShort("otaFail", 0);
  doc["otaBad"]    = nvs.getString("otaBad", "");
  doc["otaMd5"]    = otaSumaWgranej();
#endif
#if TG_ENABLED
  /*  TOKEN TU NIE WCHODZI i nie ma go jak tu wprowadzic - `tg` mowi
      tylko, czy bot jest podlaczony, `tgMsg` co sie stalo z ostatnia
      wiadomoscia. Status czyta aplikacja, a przez nia kazdy, kto ma
      dostep do pudelka.                                             */
  doc["tg"]    = tgSkonfigurowany();
  doc["tgMsg"] = rtcTgMsg;
#endif
  String body; serializeJson(doc, body);
  const int code = rtdbWyslij("PATCH", "/devices/" DEVICE_ID "/status.json", body);
  /*  Za zgloszone uznajemy DOPIERO potwierdzony zapis. Bez tego warunku
      nieudany meldunek o otwartej klapce przepadlby po cichu, a pudelko
      uznaloby sprawe za zalatwiona.                                    */
  if (code == 200) rtcKlapkiZglosz = otwarteTeraz ? 1 : 0;
  return code == 200;
}

/*  MELDUNEK O KLAPCE - obie polowy, o ktore chodzi: „od razu, ze otwarte"
    i „od razu, ze zamkniete".

    Wysylamy WYLACZNIE przy zmianie stanu wzgledem tego, co baza na pewno
    ma. Status i tak jedzie przy kazdym zdarzeniu, wiec zwykle nie kosztuje
    to nic dodatkowego; radio wlaczamy sami tylko wtedy, gdy klapka
    zmienila stan, a nic innego nie kazalo nam sie tym razem laczyc.   */
void zglosKlapki(bool otwarte) {
  otwarteTeraz = otwarte;
  if (otwarte && !rtcOpenSince)
    rtcOpenSince = rtcCzasPewny ? (uint32_t)time(nullptr) : 0;
  if (!otwarte) rtcOpenSince = 0;

  if (rtcKlapkiZglosz == (otwarte ? 1 : 0)) return;    // baza juz to wie

  if (WiFi.status() != WL_CONNECTED && !wifiPolacz()) {
    LOG("[LID] brak sieci - stan klapki zglosze przy nastepnym wybudzeniu\n");
    return;
  }
  if (!firebaseZaloguj()) {
    LOG("[LID] brak logowania - stan klapki poczeka\n");
    return;
  }
  LOG("[LID] zglaszam: klapka %s\n", otwarte ? "OTWARTA" : "zamknieta");
  wyslijStatus();
}

/* Pobiera harmonogram przypomnien z bazy i zapisuje w NVS, zeby pudelko
   znalo go takze bez sieci.                                           */
void pobierzUstawienia() {
  String odp;
  int code = rtdbWyslij("GET", "/devices/" DEVICE_ID "/config.json", "", &odp);
  if (code != 200) { LOG("[FB ] config HTTP %d\n", code); return; }

  JsonDocument doc;
  if (deserializeJson(doc, odp)) return;

#if TG_ENABLED
  /* --- Bot Telegram przyslany z aplikacji ----------------------------
     TA SAMA SKRZYNKA I TA SAMA KOLEJNOSC co przy hasle (zasada 9):
     najpierw zapis do pamieci trwalej, potem sprawdzenie, czy sie udal,
     i DOPIERO WTEDY kasowanie sekretu z bazy.

     Odwrotna kolejnosc dawalaby stan, w ktorym aplikacja pokazuje "bot
     podlaczony", pudelko go nie ma, a token zniknal z bazy - czyli
     trzeba zakladac nowego u BotFathera.                             */
  JsonObject tg = doc["tgNowy"].as<JsonObject>();
  if (!tg.isNull()) {
    String tok  = tg["token"] | "";
    String chat = tg["chat"]  | "";
    if (!tok.length() || !chat.length()) {
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "odrzucony: brak tokenu albo czatu");
    } else if (tgUtrwal(tok, chat)) {
      int kod = rtdbWyslij("DELETE", "/devices/" DEVICE_ID "/config/tgNowy.json", "");
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "bot przyjety, kasowanie tokenu HTTP %d", kod);
      LOG("[TG ] bot przyjety z aplikacji, kasowanie tokenu z bazy: HTTP %d\n", kod);
    } else {
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "zapis bota do pamieci NIEUDANY");
      LOG("[TG ] nie udalo sie zapisac bota - token ZOSTAJE w bazie do nastepnej proby\n");
    }
  }

  /* --- Polecenie w sprawie bota: odlacz / napisz probna --------------
     WYJATEK: `test` kasuje sie w `tgWyslijZalegle()` i dopiero po udanej
     wysylce. Ten przycisk ma rozstrzygnac, czy bot dziala - skasowanie
     zlecenia tutaj znaczyloby, ze proba przepada przy pierwszym braku
     sieci, a czlowiek widzi cisze i nie wie, czy to bot, czy siec.   */
  JsonObject tgc = doc["tgCmd"].as<JsonObject>();
  if (!tgc.isNull()) {
    String akcja = tgc["akcja"] | "";
    if (akcja == "usun") {
      tgZapomnij();
      rtcTgSlot = -1; rtcTgSlotTs = 0;
      rtcTgBattCzeka = false; rtcTgTestProsba = false;
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "bot odlaczony");
      int kod = rtdbWyslij("DELETE", "/devices/" DEVICE_ID "/config/tgCmd.json", "");
      LOG("[TG ] bot odlaczony na zadanie aplikacji (kasowanie HTTP %d)\n", kod);
    } else if (akcja == "test") {
      rtcTgTestProsba = true;
      LOG("[TG ] aplikacja prosi o wiadomosc probna - wysle przed snem\n");
    } else {
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "nieznane polecenie bota");
      rtdbWyslij("DELETE", "/devices/" DEVICE_ID "/config/tgCmd.json", "");
    }
  }
#endif

#if OTA_ENABLED
  /*  Zlecenie aktualizacji. To tylko PRZYSPIESZACZ - `otaSprobuj()` i tak
      dopyta baze tuz przed proba, bo zlecenie moze dojechac w trakcie
      tego wybudzenia (D62). Bez tego dopytania "kliknij i pudelko
      przyjmie" nie dzialalo w pudelku dziennym przez kilka wersji.     */
  if (!doc["otaCmd"].isNull()) {
    rtcOtaProsba = true;
    rtcOtaTs     = doc["otaCmd"]["ts"] | (uint32_t)0;
    LOG("[OTA] w bazie stoi zlecenie aktualizacji (z %lu)\n", (unsigned long)rtcOtaTs);
  }
#endif

  JsonArray sch = doc["schedule"].as<JsonArray>();
  if (!sch.isNull() && sch.size()) {
    int ile = 0;
    for (JsonVariant v : sch) {
      if (ile >= SLOTOW_MAX) break;
      String hm = v.as<String>();
      int dwukropek = hm.indexOf(':');
      if (dwukropek < 0) continue;
      int g = hm.substring(0, dwukropek).toInt();
      int m = hm.substring(dwukropek + 1).toInt();
      if (g < 0 || g > 23 || m < 0 || m > 59) continue;
      slotyMin[ile++] = g * 60 + m;
    }
    if (ile) {
      slotowIle = ile;
      nvs.putInt("sl_ile", ile);
      for (int i = 0; i < ile; i++) {
        char k[10]; snprintf(k, sizeof(k), "sl%d", i);
        nvs.putInt(k, slotyMin[i]);
      }
      LOG("[FB ] harmonogram: %d przypomnien, pierwsze o %02d:%02d\n",
          ile, slotyMin[0]/60, slotyMin[0]%60);
    }
  }
}

void wczytajHarmonogram() {
  int ile = nvs.getInt("sl_ile", 0);
  if (ile <= 0 || ile > SLOTOW_MAX) return;
  for (int i = 0; i < ile; i++) {
    char k[10]; snprintf(k, sizeof(k), "sl%d", i);
    slotyMin[i] = nvs.getInt(k, 20*60);
  }
  slotowIle = ile;
}

/* =====================================================================
 *  8.  DOMYKANIE DOB
 *
 *  Dzien lekowy rozstrzyga sie z KONCEM doby, nie przy ostatnim
 *  przypomnieniu (D64). Ktos moze otworzyc klapke o 23:50 - zgloszenie
 *  "missed" o 20:30 byloby wiec klamstwem, ktore aplikacja musialaby
 *  potem odkrecac.
 *
 *  Znacznik czasu wpisu "missed" celuje w OSTATNIA MINUTE domykanej
 *  doby, nie w "teraz". Wyslany z biezaca data wpadlby do doby, ktora
 *  wlasnie sie zaczela, i malowal na czerwono dzien jeszcze nierozegrany.
 * ===================================================================== */
#define DOB_WSTECZ_MAX 14        // dluzsza przerwa = pudelko bylo martwe

void domknijDoby() {
  if (!rtcCzasPewny) return;
  time_t teraz = time(nullptr);
  int32_t dzis = numerDoby(teraz);
  if (dzis < 0) return;

  /*  Pierwsze uruchomienie albo pamiec RTC skasowana odlaczeniem
      zasilania: o dniach wstecz nie wiemy NIC i nie zgadujemy ich.
      "Brak danych" w kalendarzu jest uczciwy, "nie wzieta" nie.     */
  if (rtcDobaZnana < 0 || rtcDobaZnana > dzis) { rtcDobaZnana = dzis; return; }
  if (rtcDobaZnana == dzis) return;

  int ile = (int)(dzis - rtcDobaZnana);
  if (ile > DOB_WSTECZ_MAX) ile = DOB_WSTECZ_MAX;

  const time_t poczatek = poczatekDoby(teraz);
  for (int wstecz = ile; wstecz >= 1; wstecz--) {          // od najstarszej: kolejka jest FIFO
    uint32_t ts   = (uint32_t)(poczatek - (time_t)(wstecz - 1) * 86400 - 60);
    int32_t  doba = numerDoby((time_t)ts);
    if (doba < rtcDobaZnana || doba >= dzis) continue;     // zmiana czasu letniego

    /*  rtcOstatniDzien pamieta tylko OSTATNIE otwarcie - i to wystarcza.
        Kazde otwarcie budzi pudelko, a kazde wybudzenie domyka zalegle
        doby, wiec doba z otwarciem moze byc tu wylacznie ta ostatnia.  */
    if (doba == rtcOstatniDzien) { LOG("[DAY] doba domknieta otwarciem\n"); continue; }

    kolejkaDodaj(zrobRekord("missed", 0, ts));
    LOG("[DAY] doba bez otwarcia -> missed (ts %lu)\n", (unsigned long)ts);
  }
  rtcDobaZnana = dzis;
}

/* =====================================================================
 *  9.  ZGLOSZENIE ZDARZENIA
 *  Do kolejki NAJPIERW, dopiero potem proba wyslania. Odwrotna kolejnosc
 *  gubi zdarzenie, gdy siec padnie w polowie.
 * ===================================================================== */
void zglos(const char* typ, int slot) {
  uint32_t ts = (uint32_t)time(nullptr);
  String rec = zrobRekord(typ, slot, ts);
  kolejkaDodaj(rec);

  if (!wifiPolacz()) { LOG("[EV ] bez sieci - zostaje w kolejce\n"); return; }

  /* Zegar mogl sie wlasnie zsynchronizowac. Jesli rekord powstal bez
     daty, zapisujemy go jeszcze raz - z prawdziwa.                    */
  if (czasZsync && ts < 1600000000UL)
    kolejkaPodmienOstatni(zrobRekord(typ, slot, (uint32_t)time(nullptr)));

  if (!firebaseZaloguj()) { LOG("[EV ] brak logowania - zostaje w kolejce\n"); return; }
  pobierzUstawienia();
  oproznijKolejke();
  wyslijStatus();
}

/*  Zapisuje otwarcie komory - JEDYNE miejsce, w ktorym to robimy.

    Ta sama komora drugi raz tego samego dnia to zagladanie do pudelka,
    nie druga dawka - a przy klapce, ktora nie domyka sie za pierwszym
    razem, takze zwykle odbicie styku.                                */
void zapiszOtwarcie(int k) {
  if (k < 0) return;
  int32_t d = rtcCzasPewny ? dzisDoba() : -1;
  if (d >= 0 && d == rtcOstatniDzien && k == rtcOstatniaKomora) {
    LOG("[EV ] ta sama komora juz dzis zgloszona - nie powtarzam\n");
    return;
  }
  zglos("open", k);
  /*  Dobe zapisujemy DOPIERO gdy znamy czas. Bez zegara wpis i tak czeka
      w kolejce, a zgadnieta doba skasowalaby te prawdziwa.            */
  if (d < 0 && czasZsync) d = dzisDoba();
  if (d >= 0) {
    rtcOstatniDzien   = d;
    rtcOstatniaKomora = (int8_t)k;
    if (rtcDobaZnana < 0) rtcDobaZnana = d;
  }
}

/* =====================================================================
 *  10.  ALARM  -  przypomnienie, ze klapka nie zostala otwarta
 * ===================================================================== */

/*  Zwraca komore, ktora przerwala alarm, albo -1.

    TO NIE JEST DROBIAZG. Alarm przerwany otwarciem to najczestszy sposob,
    w jaki ta tabletka bedzie brana - pudelko piszczy, ona otwiera klapke.
    Wybudzenie z pinu juz sie wtedy NIE zdarzy, bo uklad nie spi, a zanim
    zasnie, klapka zdazy sie zamknac. Bez zwrocenia tej komory dokladnie
    ta dawka - ta, o ktora pudelko samo poprosilo - nie trafialaby nigdzie. */
int zagrajAlarm() {
  for (int i = 0; i < ALARM_POWTORZEN; i++) {
    beepAlarm();
    /* Przerywamy, gdy ktos w trakcie otworzyl klapke - dalsze pikanie
       po wzieciu tabletki jest dokladnie tym, na co Kuba narzekal
       w pudelku dziennym (D56).                                       */
    int k = ktoraKomora(czytajKlapki(16));
    if (k >= 0) { LOG("[ALM] otwarto: %s - przerywam\n", opisKomory(k)); return k; }
    delay(ALARM_PRZERWA_MS);
  }
  return -1;
}

/* =====================================================================
 *  11.  AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)
 *
 *  Przeniesione z pudelka dziennego (D59, D63) i dzialajace tam samo -
 *  potwierdzone na plytce 2026-08-16. Tutaj powod jest jeszcze mocniejszy
 *  niz tam: pudelko tygodniowe stoi u kogos innego, a jedyna droga do
 *  poprawki byl dotad kabel i moj komputer.
 *
 *  DLACZEGO KOD JEST SKOPIOWANY, A NIE WSPOLNY: ograniczenie 1 z CLAUDE.md.
 *  Wspolny szkic z rozgalezieniem znaczylby, ze kazda zmiana w pudelku
 *  dziewczyny dotyka kodu pilnujacego Warfinu.
 *
 *  Zeby kopia nie rozjechala sie z oryginalem, `otaDecyzja()` jest tu
 *  ZNAK W ZNAK ta sama funkcja co w PillBox.ino - i kontrola statyczna
 *  to sprawdza, porownujac oba ciala. Dwie kopie decyzji, ktore moga sie
 *  rozjechac, to ten sam blad co dwie kopie obserwacji wieczka (D111).
 *  Testy C++ uruchamiaja te funkcje raz; identycznosc rozciaga ich wynik
 *  na oba pudelka.
 * ===================================================================== */
#if OTA_ENABLED

OtaDecyzja otaDecyzja(bool hasloJest, int wKolejce, int battPct, bool naLadowarce,
                      uint8_t nieudane, uint32_t teraz, uint32_t ostatniaProba,
                      uint32_t rozmiar, uint32_t tsZlecenia,
                      const String& sumaZdalna, const String& sumaLokalna,
                      const String& sumaZla) {
  if (sumaZdalna.length() != 32) return OTA_ZLY_OPIS;
  if (rozmiar < OTA_MIN_BIN_SIZE || rozmiar > OTA_MAX_BIN_SIZE) return OTA_ZLY_OPIS;
  if (sumaZdalna == sumaLokalna) return OTA_NIC_NOWEGO;
  if (sumaZla.length() == 32 && sumaZdalna == sumaZla) return OTA_ZEPSUTA;
  if (!hasloJest)   return OTA_BEZ_HASLA;
  if (wKolejce > 0) return OTA_KOLEJKA;
  if (!naLadowarce && battPct >= 0 && battPct < OTA_MIN_BATT_PCT) return OTA_BATERIA;
  const bool swiezeZlecenie = tsZlecenia && tsZlecenia > ostatniaProba;
  if (!swiezeZlecenie && nieudane >= OTA_MAX_FAILS) return OTA_PODDANO;
  (void)teraz;
  return OTA_ROB;
}

const char* otaOpisDecyzji(OtaDecyzja d) {
  switch (d) {
    case OTA_ROB:         return "pobieram";
    case OTA_NIC_NOWEGO:  return "aktualne";
    case OTA_BEZ_HASLA:   return "brak hasla w pamieci pudelka";
    case OTA_KOLEJKA:     return "czekam na wyslanie zaleglych zdarzen";
    case OTA_BATERIA:     return "za malo baterii - postaw na ladowarke";
    case OTA_PODDANO:     return "trzy proby bez skutku - poddalem sie";
    case OTA_ZEPSUTA:     return "ta wersja juz raz nie wstala";
    case OTA_ZLY_OPIS:    return "opis wersji na serwerze jest niepoprawny";
  }
  return "nieznany stan";
}

/* Suma programu, ktory NAPRAWDE siedzi w pudelku - albo pusty napis.
   Zapisana suma obowiazuje WYLACZNIE dla wersji, przy ktorej powstala:
   wgranie kablem nie przechodzi tedy, wiec bez tego warunku pudelko
   liczyloby po sumie programu, ktorego juz w nim nie ma (D59).       */
String otaSumaWgranej() {
  if (nvs.getString("otaFw", "") != String(FW_VERSION)) return String("");
  return nvs.getString("otaMd5", "");
}

/* Licznik proby PODNOSIMY PRZED pobraniem, nie po nim. Aktualizacja,
   ktora zawiesza plytke w polowie, nie podniosłaby go nigdy - i pudelko
   wchodziloby w to samo zawieszenie przy kazdym wybudzeniu.          */
void otaZanotujProbe(uint32_t teraz) {
  nvs.putUShort("otaFail", nvs.getUShort("otaFail", 0) + 1);
  if (teraz) nvs.putUInt("otaTs", teraz);
}

void otaWyzerujLicznik() { nvs.putUShort("otaFail", 0); }

/* CZY W BAZIE STOI ZLECENIE - pytamy TU, tuz przed proba.
   `pobierzUstawienia()` leci na POCZATKU wybudzenia, a aktualizacja na
   koncu; zlecenie zlozone w miedzyczasie byloby niewidoczne i pudelko
   wychodzilo by po cichu, a aplikacja pisalaby "laczylo sie i nic nie
   zrobilo" (D62).                                                     */
bool otaZlecenieWBazie(uint32_t& tsZlecenia) {
  String odp;
  const int code = rtdbWyslij("GET", "/devices/" DEVICE_ID "/config/otaCmd.json", "", &odp);
  if (code != 200 || odp.length() < 2 || odp == "null") return false;

  JsonDocument doc;
  if (deserializeJson(doc, odp) != DeserializationError::Ok) return false;
  if (!doc["ts"].isNull()) tsZlecenia = doc["ts"].as<uint32_t>();
  return true;
}

/* Pobiera SAM OPIS - kilkaset bajtow zamiast 1,1 MB. */
bool otaPobierzOpis(String& wersja, String& md5, uint32_t& rozmiar) {
  WiFiClientSecure c;
  c.setInsecure();
  c.setTimeout(15);

  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(12000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  String url = String(OTA_BASE_URL) + OTA_JSON_FILE;
  if (!http.begin(c, url)) { LOG("[OTA] nie moge otworzyc polaczenia\n"); return false; }

  int code = http.GET();
  String payload = (code > 0) ? http.getString() : String();
  http.end();

  if (code != 200) { LOG("[OTA] opis wersji: HTTP %d\n", code); return false; }

  JsonDocument doc;
  if (deserializeJson(doc, payload) != DeserializationError::Ok) {
    LOG("[OTA] opis wersji nie jest poprawnym JSON-em\n");
    return false;
  }
  wersja  = doc["wersja"]  | "";
  md5     = doc["md5"]     | "";
  rozmiar = doc["rozmiar"] | 0UL;
  md5.toLowerCase();
  LOG("[OTA] serwer ma wersje %s (%lu B, %s)\n",
      wersja.c_str(), (unsigned long)rozmiar, md5.c_str());
  return true;
}

bool otaWgraj(const String& md5, uint32_t rozmiar) {
  /* ZWALNIAMY KANAL DO BAZY, ZANIM OTWORZYMY DRUGI. Dwa polaczenia TLS
     naraz to okolo 100 kB samych buforow mbedTLS na ukladzie, ktory ma
     400 kB - a rownolegle leci zapis do flasha i stos WiFi. W pudelku
     dziennym to byla najprawdopodobniejsza przyczyna restartu w trakcie
     pobierania. `rtdbWyslij()` odbuduje polaczenie samo.              */
  const uint32_t wolnePrzed = ESP.getFreeHeap();
  klient.stop();
  LOG("[OTA] pamiec przed pobieraniem: %u B (po zamknieciu bazy: %u B)\n",
      (unsigned)wolnePrzed, (unsigned)ESP.getFreeHeap());

  WiFiClientSecure c;
  c.setInsecure();
  c.setTimeout(20);

  HTTPClient http;
  http.setConnectTimeout(10000);
  /* Limit na POJEDYNCZY odczyt. `setTimeout()` bierze uint16_t, wiec
     90000 obciela by sie po cichu do 24464 ms (B21 w innym przebraniu). */
  http.setTimeout(OTA_HTTP_READ_MS);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  /* HTTP/1.0: w tej wersji protokolu serwer nie ma jak wybrac trybu
     "chunked", ktorego zapis firmware nie obsluguje. Bez tej linii
     GitHub Pages potrafi odpowiedziec chunkiem i pobranie konczy sie,
     zanim ruszy.                                                      */
  http.useHTTP10(true);

  String url = String(OTA_BASE_URL) + OTA_BIN_FILE;
  if (!http.begin(c, url)) return false;

  int code = http.GET();
  if (code != 200) {
    LOG("[OTA] pobieranie programu: HTTP %d\n", code);
    http.end();
    return false;
  }

  /* Brak dlugosci w naglowku to NIE jest blad - znamy ja z opisu, ktory
     pobralismy chwile wczesniej, i to on jest zrodlem prawdy.         */
  const int len = http.getSize();
  rtcOtaNagl = (int32_t)len;
  if (len > 0 && (uint32_t)len != rozmiar) {
    LOG("[OTA] rozmiar sie nie zgadza: opis %lu, naglowek %d\n",
        (unsigned long)rozmiar, len);
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "plik na serwerze ma inny rozmiar");
    http.end();
    return false;
  }

  if (!Update.begin((size_t)rozmiar)) {
    LOG("[OTA] partycja nie przyjmuje %lu B - czy podzial to na pewno min_spiffs?\n",
        (unsigned long)rozmiar);
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "program nie miesci sie w partycji");
    http.end();
    return false;
  }
  Update.setMD5(md5.c_str());

  LOG("[OTA] pobieram %d B...\n", len);
  size_t zapisane = Update.writeStream(http.getStream());
  http.end();

  if (zapisane != (size_t)rozmiar) {
    /* ILE udalo sie pobrac rozroznia dwie zupelnie rozne sytuacje:
       "0 z 1,1 MB" znaczy, ze polaczenie nie ruszylo, a "900 kB z 1,1 MB"
       - ze urwalo sie w trakcie i warto podejsc blizej routera.       */
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg),
             "pobrano %u kB z %lu kB - podejdz blizej routera",
             (unsigned)(zapisane / 1024), (unsigned long)(rozmiar / 1024));
    LOG("[OTA] przerwane po %u B z %lu\n", (unsigned)zapisane, (unsigned long)rozmiar);
    Update.abort();
    return false;
  }
  if (!Update.end(true)) {
    const uint8_t blad = Update.getError();
    if (blad == UPDATE_ERROR_MD5)
      snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "plik dojechal uszkodzony - sprobuj ponownie");
    else
      snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "ESP32 odrzucil zapis (blad %u)", blad);
    LOG("[OTA] suma kontrolna albo zapis odrzucone (blad %u)\n", blad);
    return false;
  }
  return Update.isFinished();
}

/* --- Po restarcie: czy nowa wersja w ogole wstaje ---------------------
   Arduino buduje ESP32 BEZ automatycznego rollbacku bootloadera, wiec
   robimy wlasny licznik. Kazdy start z niepotwierdzona wersja go podnosi,
   dojscie do `idzSpac()` - kasuje. Po OTA_BOOT_TRIES wersja ladzie na
   czarnej liscie i wracamy na poprzednia partycje.

   ZOSTAJE DZIURA, ktorej bez rollbacku bootloadera nie da sie zakleic:
   program, ktory wysypie sie ZANIM dojdzie tutaj, nie podniesie licznika
   i petli startow nikt nie przerwie. Wtedy zostaje kabel.            */
void otaSprawdzPoStarcie() {
  const String pend = nvs.getString("otaPend", "");
  if (pend.length() != 32) return;

  uint16_t proby = nvs.getUShort("otaBoot", 0) + 1;
  nvs.putUShort("otaBoot", proby);

  LOG("[OTA] start %u z niepotwierdzona wersja %s\n", proby, pend.c_str());
  if (proby <= OTA_BOOT_TRIES) return;

  LOG("[OTA] ta wersja nie dochodzi do konca - wracam do poprzedniej\n");
  nvs.putString("otaBad", pend);
  nvs.remove("otaPend");
  nvs.putUShort("otaBoot", 0);

  const esp_partition_t* poprzednia = esp_ota_get_next_update_partition(nullptr);
  if (poprzednia && esp_ota_set_boot_partition(poprzednia) == ESP_OK) {
    LOG("[OTA] przelaczono - restart\n");
    delay(100);
    esp_restart();
  }
  LOG("[OTA] nie udalo sie przelaczyc partycji - zostaje jak jest\n");
}

/* Program przeszedl cala swoja droge i zasypia normalnie. To jest dowod,
   ze wersja dziala - mocniejszy niz "wstala", a nie wymagajacy zasiegu
   WiFi (pudelko bez sieci tez musi moc potwierdzic).                  */
void otaPotwierdzDzialanie() {
  const String pend = nvs.getString("otaPend", "");
  if (pend.length() != 32) return;

  nvs.putString("otaMd5", pend);
  nvs.putString("otaFw",  FW_VERSION);     // dla KTOREJ wersji ta suma jest prawdziwa
  nvs.remove("otaPend");
  nvs.putUShort("otaBoot", 0);
  nvs.putUShort("otaFail", 0);
  nvs.remove("otaTs");                     // sukces nie jest powodem do przerwy

  esp_ota_mark_app_valid_cancel_rollback();
  LOG("[OTA] wersja %s potwierdzona jako dzialajaca\n", FW_VERSION);

  /* Slyszalny dowod, ze aktualizacja przez WiFi doszla do konca. Gra raz
     w zyciu kazdej wersji - dokladnie tutaj, bo dopiero tu wiadomo, ze
     nowy program nie tylko sie zapisal, ale i przezyl cala swoja pierwsza
     droge. Wersja cofnieta przez licznik startow nigdy tu nie dojdzie,
     wiec cisza tez cos znaczy.                                        */
  pik(1600, 120); delay(60); pik(2100, 120); delay(60); pik(2700, 220);
}

/* Powod odmowy idzie do aplikacji OD RAZU. `otaSprobuj()` chodzi
   w `idzSpac()`, czyli JUZ PO zwyklym statusie - bez tego aplikacja
   pokazywalaby pogodne "zlecone, czekaj" jeszcze przez wiele godzin,
   podczas gdy pudelko wlasnie odmowilo i nie zamierza nic robic.     */
void otaZglos() {
  if (WiFi.status() == WL_CONNECTED && idToken.length()) wyslijStatus();
}

/* --- CALOSC: sprawdz, zdecyduj, ewentualnie wgraj --------------------
   Wolane z jednego miejsca - z `idzSpac()`, czyli po tym, jak pudelko
   zrobilo juz wszystko, po co wstalo: zdarzenie zapisane i potwierdzone,
   kolejka oprozniona, status wyslany. To jest zasada 11 z CLAUDE.md:
   minuta radia nie moze wcisnac sie miedzy otwarcie klapki a zapis.

   Nie wraca, jesli aktualizacja sie powiodla - konczy restartem.      */
void otaSprobuj() {
  /* Bez radia nie ma jak zapytac i nie ma jak pobrac. Zlecenie poczeka
     do nastepnego wybudzenia - nic nie ginie.                         */
  if (!rtcOtaProsba && WiFi.status() != WL_CONNECTED) return;

  if (WiFi.status() != WL_CONNECTED && !wifiPolacz()) {
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "nie zlapalem sieci przed snem");
    LOG("[OTA] brak sieci przy zasypianiu - sprobuje przy nastepnym wybudzeniu\n");
    return;
  }
  if (!firebaseZaloguj()) {
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "nie moge sie zalogowac do bazy");
    LOG("[OTA] brak logowania - aktualizacja czeka\n");
    return;
  }

  /* Dopytanie o zlecenie - dopiero tutaj, bo dopiero tu mamy pewny token. */
  if (!rtcOtaProsba) {
    uint32_t tsSwieze = 0;
    if (!otaZlecenieWBazie(tsSwieze)) return;   // aplikacja naprawde o nic nie prosi
    rtcOtaProsba = true;
    rtcOtaTs     = tsSwieze;
    LOG("[OTA] zlecenie dojechalo w trakcie wybudzenia (z %lu) - biore je teraz\n",
        (unsigned long)tsSwieze);
  }

  const uint32_t teraz = rtcCzasPewny ? (uint32_t)time(nullptr) : 0;

  String wersja, md5;
  uint32_t rozmiar = 0;
  if (!otaPobierzOpis(wersja, md5, rozmiar)) {
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "nie moge pobrac opisu wersji");
    otaZanotujProbe(teraz);
    otaZglos();
    return;
  }

  const uint16_t nieudane = nvs.getUShort("otaFail", 0);
  const uint32_t ostatnia = nvs.getUInt("otaTs", 0);

  /* `naLadowarce` to twarde `false`: to pudelko nie wie, czy stoi na
     ladowarce - nie ma pomiaru pradu ladowania. Falsz jest tu strona
     OSTROZNA (prog baterii obowiazuje zawsze), a nie wygodna.         */
  const OtaDecyzja d = otaDecyzja(
      hasloWPamieci(), kolejkaIle(), battProcent, false,
      (uint8_t)(nieudane > 255 ? 255 : nieudane), teraz, ostatnia, rozmiar,
      rtcOtaTs, md5, otaSumaWgranej(), nvs.getString("otaBad", ""));

  snprintf(rtcOtaWersja, sizeof(rtcOtaWersja), "%s", wersja.c_str());
  snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "%s", otaOpisDecyzji(d));
  LOG("[OTA] decyzja: %s\n", otaOpisDecyzji(d));

  /* Prosbe spelniona - albo taka, ktorej dalsze proby nic nie dadza -
     kasujemy, zeby przycisk w aplikacji nie zostal wcisniety na zawsze. */
  if (d == OTA_NIC_NOWEGO || d == OTA_ZEPSUTA || d == OTA_PODDANO) {
    rtdbWyslij("DELETE", "/devices/" DEVICE_ID "/config/otaCmd.json", "");
    rtcOtaProsba = false;
    if (d == OTA_NIC_NOWEGO) otaWyzerujLicznik();
    otaZglos();
    return;
  }
  if (d != OTA_ROB) { otaZglos(); return; }

  if (nieudane >= OTA_MAX_FAILS) {
    otaWyzerujLicznik();
    LOG("[OTA] nowe zlecenie po serii niepowodzen - licznik od zera\n");
  }

  otaZanotujProbe(teraz);

  /* Dwa pikniecia: "zaczynam, zaraz zamilkne na minute". Bez tego pudelko
     wyglada na zawieszone, a brzeczyk jest jedynym kanalem, ktorym mowi
     cokolwiek bez telefonu.                                           */
  beepAck(); delay(150); beepAck();

  if (!otaWgraj(md5, rozmiar)) {
    if (!rtcOtaMsg[0] || strstr(rtcOtaMsg, "pobieram"))
      snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "pobieranie nie doszlo do konca");
    LOG("[OTA] nieudane (%s) - stary program zostaje bez zmian\n", rtcOtaMsg);
    LOG("[OTA] naglowek %ld, wolne %u kB\n",
        (long)rtcOtaNagl, (unsigned)(ESP.getFreeHeap() / 1024));
    beepBlad();
    otaZglos();
    return;
  }

  /* Wgrane. Suma idzie do pamieci jako NIEPOTWIERDZONA - potwierdzi ja
     dopiero nowy program, gdy dojdzie do konca swojej pierwszej drogi. */
  nvs.putString("otaPend", md5);
  nvs.putUShort("otaBoot", 0);

  /* Polecenie kasujemy PRZED restartem - po nim nie wrocimy juz tutaj.
     Gdyby kasowanie nie doszlo, nowy program zobaczy te sama prosbe,
     policzy sume jako "aktualne" i skasuje ja wtedy. Samo sie naprawia. */
  rtdbWyslij("DELETE", "/devices/" DEVICE_ID "/config/otaCmd.json", "");

  LOG("[OTA] wgrane %s - restart na nowa wersje\n",
      rtcOtaWersja[0] ? rtcOtaWersja : "?");
  nvs.end();                      // za esp_restart() nie ma juz nic
  beepAck();
  delay(200);
  esp_restart();
}

#endif  /* OTA_ENABLED */

/* =====================================================================
 *  12.  POWIADOMIENIA NA TELEFON  (bot Telegram)
 *
 *      Dzwonek slychac w pokoju. Wiadomosc dociera wszedzie - i o to tu
 *      chodzi. Pudelko dzwoni do pustego mieszkania, a czlowiek dowiaduje
 *      sie o pominietej tabletce dopiero wieczorem.
 *
 *      DLACZEGO WYSYLA PUDELKO. Telefon spi razem z wlascicielem, a iOS
 *      nie budzi stron dodanych do ekranu glownego - aplikacja fizycznie
 *      nie ma jak niczego przypomniec o 20:00. Pudelko w tej chwili jest
 *      wybudzone, bo wlasnie skonczylo dzwonic. To jedyne miejsce
 *      w calym ukladzie, ktore wtedy zyje.
 *
 *      CENA, powiedziana wprost takze w aplikacji: powiadomienie wymaga,
 *      zeby PUDELKO mialo internet. Bez sieci nie przyjdzie nic. To ta
 *      sama granica, ktora obowiazuje zdarzenia jadace do kalendarza -
 *      nie nowa slabosc, tylko ta sama.
 *
 *      CZEGO TU NIE MA, w odroznieniu od pudelka dziennego: zapasu
 *      tabletek i terminu INR. Tamto pudelko pilnuje Warfinu i liczy
 *      tabletki w opakowaniu; to ma przypomniec o klapce i tyle (D126).
 * ===================================================================== */
#if TG_ENABLED

/* Samo zapytanie. Zwraca true wylacznie przy HTTP 200 od Telegrama -
   od tego zalezy, czy skasujemy czekajace powiadomienie (zasada 6).

   TOKEN IDZIE W ADRESIE, WIEC ADRESU NIE LOGUJEMY. Telegram nie zna
   innej drogi; nasza jest nie wpisac go do niczego, co da sie potem
   wkleic w zgloszeniu. W logu zostaje sam kod odpowiedzi.

   ZWALNIAMY KANAL DO BAZY, ZANIM OTWORZYMY DRUGI - ten sam powod co
   przy pobieraniu programu: dwa konteksty TLS naraz to okolo 100 kB
   na ukladzie, ktory ma 400 kB. `rtdbWyslij()` odbuduje kanal sam.  */
bool tgWyslijTekst(const String& tekst) {
  const String token = nvs.getString("tgTok", "");
  const String chat  = nvs.getString("tgChat", "");
  if (!token.length() || !chat.length()) return false;

  klient.stop();

  WiFiClientSecure c;
  c.setInsecure();
  c.setTimeout(15);

  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(12000);

  String url = String("https://") + TG_HOST + "/bot" + token + "/sendMessage";
  if (!http.begin(c, url)) { LOG("[TG ] nie moge otworzyc polaczenia\n"); return false; }

  /* Tresc budujemy ArduinoJsonem, a nie skladaniem napisow - cudzyslow
     albo znak nowej linii w tekscie zepsulby caly pakiet.            */
  JsonDocument doc;
  doc["chat_id"] = chat;
  doc["text"]    = tekst;
  String body;
  serializeJson(doc, body);

  http.addHeader("Content-Type", "application/json");
  const int code = http.POST(body);
  http.end();

  LOG("[TG ] wiadomosc: HTTP %d\n", code);
  return code == 200;
}

/* --- Zamiar: zapamietaj, ze jest o czym napisac ----------------------
   Rozdzielenie zamiaru od wysylki jest celowe. Alarm konczy sie w polowie
   wybudzenia, a radio zabrane w tym miejscu weszloby miedzy nieodebrane
   przypomnienie a zapis zdarzenia. Dane ida pierwsze; wiadomosc czeka
   na `idzSpac()`, tak samo jak aktualizacja (zasada 11 i 12).       */
void tgZglosNieodebrane(int slot) {
  rtcTgSlot   = (int8_t)slot;
  rtcTgSlotTs = rtcCzasPewny ? (uint32_t)time(nullptr) : 0;
  LOG("[TG ] przypomnienie %d bez odzewu - napisze przed snem\n", slot);
}

/* Bateria: jedna wiadomosc na rozladowanie, nie jedna na wybudzenie.
   Bez znacznika pudelko ponizej progu pisaloby przy KAZDYM otwarciu
   klapki, czyli codziennie. Znacznik zdejmuje sie po naladowaniu powyzej
   TG_BATT_RESET_PCT - zostawiony na stale znaczylby, ze druga wiadomosc
   nie przyjdzie nigdy, a ogniwo rozladuje sie jeszcze wiele razy.

   `battProcent < 0` to BRAK POMIARU, nie pusta bateria (tak zglasza sie
   niemozliwy odczyt, B30) - i wlasnie dlatego nie wolno na nim pisac.
   Dzis to nie jest teoria: ta plytka melduje 2,32 V, czyli nic.     */
void tgSprawdzBaterie() {
  if (battProcent >= TG_BATT_RESET_PCT) { rtcTgBattZgloszona = false; return; }
  if (battProcent < 0 || battProcent > BATT_WARN_PCT) return;
  if (rtcTgBattZgloszona || rtcTgBattCzeka) return;
  rtcTgBattCzeka = true;
  LOG("[TG ] bateria %d%% - napisze przed snem\n", battProcent);
}

/* Zdanie, ktore czlowiek przeczyta na telefonie.

   Godzina bierze sie z harmonogramu, nie z zegara: to pora PRZYPOMNIENIA
   (zasada 4b) i wlasnie ona ma stac w wiadomosci.

   NAZWY KLAPKI TU NIE MA I TO JEST ZMIANA, NIE BRAK (D140). Stalo tu
   "nie otworzyl klapki PON" - nazwa dnia wyliczona z numeru doby, przy
   zalozeniu, ze rezystory sa polutowane w kolejnosci tygodnia. Zalozenia
   tego pudelko nie umie sprawdzic, a wiadomosc podawala je jako fakt.
   Wiadomosc, ktora wskazuje ZLA klapke, jest gorsza od tej, ktora nie
   wskazuje zadnej: czlowiek otwiera nie te przegrodke i bierze dawke
   z innego dnia.

   Zdanie NIE mowi "nie wzielas" jako faktu, tylko opisuje to, co pudelko
   naprawde wie: dzwonilo i nikt nie otworzyl klapki. Tabletke da sie
   wziac z blistra lezacego obok - klamstwo w tym miejscu podkopaloby
   zaufanie do wszystkich pozostalych wiadomosci.                    */
String tgTekstNieodebrane(int slot) {
  char godz[8] = "";
  if (slot >= 0 && slot < slotowIle)
    snprintf(godz, sizeof(godz), "%02d:%02d", slotyMin[slot]/60, slotyMin[slot]%60);

  String s = "⏰ Pudełko: tabletka nieodebrana\n\n";
  if (godz[0]) s += "Przypomnienie " + String(godz) + " — ";
  s += "pudełko dzwoniło i nikt nie otworzył klapki.";
  s += "\n\nJeśli wzięłaś ją bez otwierania pudełka, zaznacz dzień ręcznie w aplikacji.";
  return s;
}

String tgTekstBateria() {
  String s = "🔋 Pudełko: słaba bateria\n\n";
  s += "Zostało " + String(battProcent) + "% — naładuj pudełko.";
  s += "\n\nPrzy pustym ogniwie nie zadzwoni i nie przyśle powiadomienia.";
  return s;
}

/* --- Wysylka: TU I TYLKO TU  ------------------------------------------
   Wolane z `idzSpac()`, czyli po zapisie zdarzenia, wyslaniu statusu
   i oproznieniu kolejki - z tego samego powodu, dla ktorego stad rusza
   aktualizacja (zasada 11).

   PRZED aktualizacja, i to jest wazne w tej kolejnosci: udana
   aktualizacja konczy sie restartem, wiec wiadomosc wyslana za nia nie
   poszlaby wcale (zasada 12).

   Przy pustej skrzynce funkcja wychodzi PRZED wlaczeniem radia. Cisza
   nie kosztuje tu nic - ani miliampera, ani sekundy czuwania.       */
void tgWyslijZalegle() {
  tgSprawdzBaterie();

  const bool cosCzeka = (rtcTgSlot >= 0) || rtcTgBattCzeka || rtcTgTestProsba;
  const uint32_t teraz = rtcCzasPewny ? (uint32_t)time(nullptr) : 0;
  /* Wiek liczymy dla nieodebranego przypomnienia - ono jedno traci sens
     ze starosci. Ostrzezenie o baterii jest prawdziwe tak dlugo, jak
     ogniwo jest slabe, a wiadomosc probna wysyla sie na zadanie.     */
  const uint32_t tsWieku = (rtcTgSlot >= 0) ? rtcTgSlotTs : 0;

  const TgDecyzja d = tgDecyzja(tgSkonfigurowany(), cosCzeka, tsWieku, teraz);
  if (d == TG_NIC) return;

  if (d == TG_BRAK_BOTA) {
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "bot niepodlaczony - nie mam komu pisac");
    LOG("[TG ] jest o czym napisac, ale bot nie jest podlaczony\n");
    /* Znacznikow NIE kasujemy: bot moze dojechac z aplikacji w ciagu
       najblizszych minut, a wtedy wiadomosc jeszcze ma sens.        */
    return;
  }

  if (d == TG_ZA_STARE) {
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "przypomnienie za stare - nie wyslalem");
    LOG("[TG ] czekajace powiadomienie starsze niz %d s - kasuje je\n", TG_MAX_WIEK_S);
    rtcTgSlot   = -1;
    rtcTgSlotTs = 0;
    return;
  }

  if (WiFi.status() != WL_CONNECTED && !wifiPolacz()) {
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "brak sieci - wiadomosc czeka");
    LOG("[TG ] brak sieci - wiadomosc poczeka do nastepnego wybudzenia\n");
    return;
  }

  /* KAZDA rzecz kasuje sie osobno i dopiero po swoim wlasnym HTTP 200
     (zasada 6). Wspolny warunek na koncu gubilby wiadomosc, ktora
     przeszla, razem z ta, ktora nie przeszla.                       */
  int wyslane = 0, nieudane = 0;

  if (rtcTgSlot >= 0) {
    if (tgWyslijTekst(tgTekstNieodebrane(rtcTgSlot))) {
      rtcTgSlot   = -1;
      rtcTgSlotTs = 0;
      wyslane++;
    } else nieudane++;
  }

  if (rtcTgBattCzeka) {
    if (tgWyslijTekst(tgTekstBateria())) {
      rtcTgBattCzeka     = false;
      rtcTgBattZgloszona = true;        // do naladowania juz o tym nie piszemy
      wyslane++;
    } else nieudane++;
  }

  if (rtcTgTestProsba) {
    const bool ok = tgWyslijTekst(
        "✅ Pudełko: wiadomość próbna\n\n"
        "Bot działa. Tak wyglądają powiadomienia z pudełka.");
    if (ok) { rtcTgTestProsba = false; wyslane++; }
    else    nieudane++;
    /* Zlecenie z bazy kasujemy TYLKO po udanej probie. Nieudana ma wrocic
       przy nastepnym wybudzeniu - inaczej "wyslij probna" konczylo by sie
       cisza, ktorej nie da sie odroznic od zepsutego bota.           */
    if (ok && firebaseZaloguj())
      rtdbWyslij("DELETE", "/devices/" DEVICE_ID "/config/tgCmd.json", "");
  }

  if (nieudane)
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "Telegram odmowil (%d z %d)",
             nieudane, wyslane + nieudane);
  else
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "wyslane: %d", wyslane);

  /* Powod dojezdza OD RAZU, nie przy nastepnym wybudzeniu. Radio jeszcze
     zyje, wiec meldunek nic nie kosztuje, a bez niego ekran przez wiele
     godzin twierdzilby, ze wszystko w porzadku.                     */
  if ((nieudane || wyslane) && firebaseZaloguj()) wyslijStatus();
}

#endif  /* TG_ENABLED */

/* =====================================================================
 *  13.  PORTAL KONFIGURACJI WiFi
 *
 *      Pudelko tworzy wlasna siec WiFi. Laczysz sie z nia telefonem,
 *      otwiera sie strona, wybierasz siec z listy i wpisujesz haslo.
 *      Zadnej aplikacji, zadnego kabla, zadnego komputera.
 *
 *      PO CO TO TU JEST: pudelko stoi u kogos innego. Zmiana routera,
 *      przeprowadzka albo zabranie go do siebie znaczyly dotad "przynies
 *      mi je, wgram nowy config.h". Portal fizyczny zostaje na zawsze
 *      (zasada 9) wlasnie dlatego, ze jest jedyna droga niezalezna od
 *      sieci i od bazy.
 *
 *      DLACZEGO NIE BLUETOOTH: kontroler BLE w rdzeniu arduino-esp32
 *      3.3.x wywala sie na ESP32-C3 juz przy inicjalizacji i restartuje
 *      plytke w petli. Sprawdzone w pudelku dziennym.
 * ===================================================================== */
#if PORTAL_ENABLED

static String htmlEscape(const String& in) {
  String o;
  for (size_t i = 0; i < in.length(); i++) {
    char c = in[i];
    if      (c == '&')  o += "&amp;";
    else if (c == '<')  o += "&lt;";
    else if (c == '>')  o += "&gt;";
    else if (c == '"')  o += "&quot;";
    else if (c == '\'') o += "&#39;";
    else o += c;
  }
  return o;
}

/* Strona jest JASNA i rozowa, a nie granatowa jak w pudelku dziennym -
   ten sam motyw co aplikacja tego konta (D129). Nie chodzi o ozdobe:
   czlowiek, ktory otwiera te strone, widzi ja obok aplikacji i ma od
   razu wiedziec, ze to to samo urzadzenie.                           */
static String portalPage(int found) {
  String o = F("<!doctype html><html lang=pl><meta charset=utf-8>"
      "<meta name=viewport content='width=device-width,initial-scale=1'>"
      "<title>Pudelko</title><style>"
      "body{font-family:-apple-system,BlinkMacSystemFont,sans-serif;background:#fbeef2;"
      "color:#3a2230;padding:26px 20px;max-width:420px;margin:0 auto}"
      "h2{font-size:20px;margin:0 0 4px}p{color:#7c6070;font-size:14px;margin:0 0 20px}"
      "label{font-size:12px;color:#7a6270;text-transform:uppercase;letter-spacing:.06em}"
      "select,input{width:100%;padding:13px;margin:6px 0 16px;border-radius:11px;"
      "border:1px solid #e3c8d4;background:#fff;color:#3a2230;font-size:16px;"
      "box-sizing:border-box;-webkit-appearance:none}"
      "button{width:100%;padding:15px;border:0;border-radius:11px;background:#a74f80;"
      "color:#fff6fa;font-size:16px;font-weight:600}</style>"
      "<h2>Pudelko na leki</h2><p>Wybierz siec WiFi.</p>"
      "<form action='/save' method='POST'><label>Siec</label><select name='s'>");
  for (int i = 0; i < found; i++) {
    String e = htmlEscape(WiFi.SSID(i));
    o += "<option value=\"" + e + "\">" + e + "  (" + String((int)WiFi.RSSI(i)) + " dBm)</option>";
  }
  o += F("</select><label>Haslo</label><input name='p' type='password' autocomplete='off'>");

  /* Haslo urzadzenia do bazy - pole pojawia sie TYLKO wtedy, gdy pamiec
     trwala go nie ma. Binarka z automatu hasla nie zna (ograniczenie 3),
     wiec gdyby pamiec kiedys przepadla, pudelko nie mialoby czym zalogowac
     sie do bazy - a bez bazy nie ma zdalnej drogi, zeby mu je podac.
     Ukrywamy je, dopoki haslo siedzi w pamieci, zeby nie kusilo do
     wpisywania czegokolwiek przy zwyklej zmianie sieci.               */
  if (!hasloWPamieci())
    o += F("<label>Haslo urzadzenia (baza)</label>"
           "<input name='d' type='password' autocomplete='off'>"
           "<p style='margin:-8px 0 16px;font-size:12px'>Pudelko nie ma zapisanego "
           "hasla do bazy. Bez niego polaczy sie z WiFi, ale nie z aplikacja.</p>");

  o += F("<button type=submit>Polacz</button></form>");
  return o;
}

void startPortalWifi() {
  /* Trzy pikniecia = "jestem w trybie konfiguracji". */
  for (int i = 0; i < 3; i++) { pik(2200, 90); delay(90); }

  WiFi.persistent(true);
  WiFi.disconnect(true, false);
  WiFi.mode(WIFI_AP_STA);

  /* Skan PRZED uruchomieniem punktu dostepowego - pozniej jest wolniejszy
     i potrafi zrywac polaczenie telefonu.                              */
  int found = WiFi.scanNetworks();
  if (found < 0) found = 0;
  LOG("[AP ] znaleziono %d sieci\n", found);

  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress ip = WiFi.softAPIP();
  LOG("[AP ] siec '%s', otworz http://%s\n", AP_SSID, ip.toString().c_str());

  DNSServer dns;
  dns.start(53, "*", ip);            // kazda domena -> nasza strona
  WebServer server(80);

  bool done = false;
  String pendingSsid, pendingPass, pendingDevPass;

  server.on("/", [&]() { server.send(200, "text/html; charset=utf-8", portalPage(found)); });

  server.on("/save", HTTP_POST, [&]() {
    pendingSsid    = server.arg("s");
    pendingPass    = server.arg("p");
    pendingDevPass = server.arg("d");   // puste, gdy pole bylo ukryte
    server.send(200, "text/html; charset=utf-8",
      F("<!doctype html><meta charset=utf-8><meta name=viewport content='width=device-width'>"
        "<body style='font-family:-apple-system,sans-serif;background:#fbeef2;color:#3a2230;"
        "padding:40px 24px;text-align:center'>"
        "<h2>Lacze sie...</h2><p style='color:#7c6070'>Mozesz zamknac to okno.<br>"
        "Pudelko potwierdzi dzwiekiem.</p>"));
    done = true;
  });

  /* iOS sprawdza polaczenie pod losowymi adresami - kazdy odsylamy na strone. */
  server.onNotFound([&]() {
    server.sendHeader("Location", String("http://") + ip.toString(), true);
    server.send(302, "text/plain", "");
  });

  server.begin();

  /* DRUGIE NACISNIECIE PRZYCISKU KONCZY PAROWANIE.

     Punkt dostepowy to najdrozszy tryb pracy tego ukladu, wiec kazda
     sekunda krocej to realna oszczednosc. W pudelku dziennym te role
     pelni zamkniecie wieczka; tutaj wieczka nie ma (siedem klapek,
     kazda osobno), wiec zostaje przycisk - ten sam, ktorym sie tu
     weszlo. Czekamy najpierw, az zostanie PUSZCZONY, inaczej jedno
     przytrzymanie zamykaloby portal natychmiast po otwarciu.         */
  while (digitalRead(PIN_PRZYCISK) == LOW) delay(20);

  bool przerwane = false;
  uint32_t t0 = millis();
  while (!done && millis() - t0 < (uint32_t)PORTAL_TIMEOUT_S * 1000UL) {
    dns.processNextRequest();
    server.handleClient();
    if (digitalRead(PIN_PRZYCISK) == LOW) {
      delay(120);                                  // odbicie styku
      if (digitalRead(PIN_PRZYCISK) == LOW) { przerwane = true; break; }
    }
    delay(5);
  }

  server.stop();
  dns.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);

  if (przerwane) {
    LOG("[AP ] przycisk - koncze parowanie\n");
    /* Dwa opadajace tony: "zamykam sklep". Latwe do odroznienia od
       trzech wznoszacych, ktore oznaczaly wejscie w tryb konfiguracji. */
    pik(3000, 120); pik(2300, 200);
    return;
  }
  if (!done) {
    LOG("[AP ] czas minal - nikt sie nie polaczyl\n");
    beepBlad();
    return;
  }

  LOG("[AP ] lacze z siecia '%s'...\n", pendingSsid.c_str());
  WiFi.begin(pendingSsid.c_str(), pendingPass.c_str());

  uint32_t t1 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t1 < WIFI_TIMEOUT_S * 1000UL + 10000UL)
    delay(200);

  if (WiFi.status() != WL_CONNECTED) {
    LOG("[AP ] nie udalo sie - zle haslo albo siec 5 GHz\n");
    beepBlad(); delay(300); beepBlad();
    return;
  }
  LOG("[AP ] polaczono, IP=%s\n", WiFi.localIP().toString().c_str());

  /* SIEC ZAPISUJEMY DOPIERO PO UDANYM POLACZENIU.

     Odwrotna kolejnosc kasowalaby dzialajaca siec na rzecz literowki -
     a razem z nia jedyna droge do pudelka poza portalem. To jest
     zasada 9 z CLAUDE.md, widziana od strony portalu. `wifiPolacz()`
     czyta dokladnie te dwa klucze, wiec nowa siec obowiazuje od
     nastepnego wybudzenia bez zadnego dodatkowego kroku.             */
  nvs.putString("ssid",  pendingSsid);
  nvs.putString("wpass", pendingPass);
  if (nvs.getString("ssid", "") == pendingSsid) LOG("[AP ] siec zapisana w pamieci\n");
  else                                          LOG("[AP ] UWAGA: siec NIE zapisala sie\n");

  /* Zegar przy okazji - radio i tak stoi. */
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
  tzset();
  for (int i = 0; i < 40 && time(nullptr) < 1600000000; i++) delay(150);
  if (time(nullptr) > 1600000000) { czasZsync = true; rtcCzasPewny = true; }

  /* Haslo urzadzenia podane w portalu jest KANDYDATEM, nie prawda.
     Zapisujemy je, probujemy zalogowac - i jesli baza je odrzuci,
     kasujemy z powrotem. Zostawione blokowaloby na stale to poprawne
     z config.h przy nastepnym wgraniu kablem, czyli literowka
     w portalu kosztowalaby cala droge powrotna.                      */
  bool zPortalu = false;
  if (pendingDevPass.length() && !hasloWPamieci()) {
    if (hasloUtrwal(pendingDevPass)) {
      zPortalu = true;
      LOG("[AP ] haslo urzadzenia zapisane - sprawdzam je w bazie\n");
    } else {
      LOG("[AP ] haslo urzadzenia NIE zapisalo sie do pamieci\n");
    }
  }

  if (firebaseZaloguj()) {
    pobierzUstawienia();
    oproznijKolejke();
    wyslijStatus();
  } else if (zPortalu) {
    nvs.remove("haslo");
    LOG("[AP ] baza odrzucila to haslo - skasowane, sprobuj jeszcze raz\n");
    beepBlad();
  }
  beepAck();
}

#endif  /* PORTAL_ENABLED */

/* =====================================================================
 *  14.  SEN
 * ===================================================================== */
uint64_t sekundDoNastepnego() {
  /*  Przypomnienie, ktore nikogo nie zastalo, wraca za chwile - to jest
      cala jego wartosc. Dopiero po ALARM_PONOWIEN probach slot milknie
      do konca doby.                                                   */
  if (ponowAlarm) return (uint64_t)ALARM_PONOW_MIN * 60;
  if (!rtcCzasPewny) return 15 * 60;              // bez zegara: co 15 minut

  int teraz = minutyDnia();
  int najblizej = 24 * 60;
  for (int i = 0; i < slotowIle; i++) {
    int d = slotyMin[i] - teraz;
    if (d <= 0) d += 24 * 60;
    if (d < najblizej) najblizej = d;
  }
  /* Granica doby tez musi nas obudzic - tam zapada decyzja "missed". */
  int doGranicy = DAY_START_HOUR * 60 - teraz;
  if (doGranicy <= 0) doGranicy += 24 * 60;
  if (doGranicy < najblizej) najblizej = doGranicy;

  if (kolejkaIle() > 0 && najblizej > 30) najblizej = 30;   // zalegle zdarzenia
  return (uint64_t)najblizej * 60;
}

void idzSpac() {
  /*  Kolejnosc w tej funkcji, od gory: czekanie na zamkniecie klapki
      i meldunek o niej, potem powiadomienie na telefon, potem
      aktualizacja, dopiero na koncu wylaczenie radia i sen.          */
  /*  KLAPKA ZOSTAWIONA OTWARTA JEST PULAPKA, i to nie teoretyczna.
      Wybudzanie reaguje na POZIOM niski, nie na zbocze: przy otwartej
      klapce pin jest nisko caly czas, wiec pudelko obudziloby sie
      natychmiast po zasnieciu - i tak w kolko, az do rozladowania
      ogniwa, zasmiecajac po drodze kolejke powtorzonymi otwarciami.

      Czekamy wiec chwile na zamkniecie. Gdy nie nastepuje (ktos wlasnie
      napelnia pudelko), zasypiamy na SAM ZEGAR i wracamy za chwile.

      TO CZEKANIE STOI TERAZ PRZED WYLACZENIEM RADIA i to jest cala
      naprawa D137. Wczesniej szlo za `wifiWylacz()`, wiec w chwili,
      w ktorej klapka sie zamykala, nie bylo juz czym tego zglosic -
      aplikacja dowiadywalaby sie o zamknieciu dopiero przy nastepnym
      wybudzeniu z zegara, czyli za godziny. Kosztuje to kilkanascie
      sekund radia przy otwartej klapce; Kuba prosil o obie polowy
      naraz ("od razu, ze otwarte" i "od razu, ze zamkniete") i to jest
      ich cena.                                                        */
  uint32_t start = millis();
  while (klapkiOtwarte() && millis() - start < CZEKAJ_ZAMKNIECIE_S * 1000UL) delay(200);
  const bool otwarte = klapkiOtwarte();
  if (otwarte) LOG("[SEN] klapka nadal otwarta - usypiam na sam zegar\n");

  /*  Stan koncowy idzie do aplikacji ZANIM zgasimy radio. Wysyla sie
      tylko wtedy, gdy rozni sie od tego, co baza na pewno ma.        */
  zglosKlapki(otwarte);

#if TG_ENABLED
  /*  POWIADOMIENIE IDZIE PIERWSZE Z TRZECH RZECZY PRZED SNEM (zasada 12).

      Udana aktualizacja konczy sie RESTARTEM, wiec wiadomosc wyslana za
      nia nie poszlaby wcale - a jest to akurat wiadomosc o nieodebranej
      tabletce. Przy pustej skrzynce `tgWyslijZalegle()` wychodzi PRZED
      wlaczeniem radia, wiec cisza nie kosztuje nic.                   */
  tgWyslijZalegle();
#endif
#if OTA_ENABLED
  /*  KOLEJNOSC JEST TU CALA TRESCIA (zasada 11 z CLAUDE.md).

      `otaPotwierdzDzialanie()` idzie PIERWSZE i bezwarunkowo: dojscie
      do tego miejsca znaczy, ze program przeszedl cala swoja droge -
      i to jest jedyny dowod, jakiego potrzebujemy, zeby uznac swiezo
      wgrana wersje za dzialajaca. Nie wymaga sieci, wiec pudelko bez
      zasiegu tez potrafi potwierdzic.

      `otaSprobuj()` idzie DRUGIE i tylko stad. Wywolane z obslugi klapki
      wcisneloby minute radia miedzy otwarcie a zapis zdarzenia - a to
      jest dokladnie ta jedna sciezka, ktora ma byc najszybsza.
      Nie wraca, jesli wgralo nowa wersje: konczy restartem.            */
  otaPotwierdzDzialanie();
  otaSprobuj();
#endif
  wifiWylacz();

  /*  HAMULEC NA PETLE WYBUDZEN.

      Nie zna przyczyny i nie musi. Pudelko, ktore budzi sie z pinu raz za
      razem i za kazdym razem nie ma czego zglosic, jest zepsute - wszystko
      jedno czy przez blad w kodzie (B30), czy przez styk, ktory nie puszcza.
      Bez hamulca taka petla konczy sie rozladowanym ogniwem i melodia
      grajaca do skutku; z hamulcem konczy sie wpisem w logu i statusie.

      SWIADOMA CENA: przez PO_PUSTYCH_SEN_S pin nie budzi, wiec otwarcie
      klapki w tym oknie moze zostac zauwazone dopiero przy nastepnym
      wybudzeniu albo wcale. Zgadzam sie na to, bo pudelko w petli i tak
      nie zglasza niczego - a zamiast psuc sie po cichu, teraz to widac.  */
  bool hamulec = false;
  if (rtcPuste >= PUSTE_WYBUDZENIA_MAX) {
    hamulec  = true;
    rtcPuste = 0;                       // po drzemce probujemy jeszcze raz
    LOG("[SEN] %u pustych wybudzen z rzedu - rozbrajam pin na %d s\n",
        (unsigned)PUSTE_WYBUDZENIA_MAX, PO_PUSTYCH_SEN_S);
  }

  uint64_t sek = hamulec ? (uint64_t)PO_PUSTYCH_SEN_S
               : otwarte ? (uint64_t)SEN_PRZY_OTWARTEJ_S : sekundDoNastepnego();
  LOG("[SEN] spie na %llu min (kolejka: %d)\n", sek / 60, kolejkaIle());
  Serial.flush();
  nvs.end();

  /* =================================================================
     PIN MUSI WROCIC DO TRYBU CYFROWEGO. TO NIE JEST PORZADKOWANIE.

     ZMIERZONE NA PLYTCE, nie wydedukowane (B30). Pudelko budzilo sie
     w kolko - z logu Kuby: "wybudzenie 72", "73", "83", "84" co kilka
     sekund, kazde przez pin klapek, i kazde meldujace `klapka:
     zamkniete`. Czyli przetwornik widzial 1125 mV (zamkniete), a
     komparator wybudzania w tej samej chwili widzial ZERO.

     Powod: `analogReadMilliVolts()` przestawia pad w tryb ANALOGOWY,
     a to WYLACZA bufor wejscia cyfrowego. Wybudzanie z glebokiego snu
     czyta pin wlasnie tym buforem - wylaczony daje stale zero, czyli
     warunek "stan niski" spelniony od razu po zasnieciu. W kolko, az do
     rozladowania ogniwa, z pikaniem przy kazdym przebiegu.

     Znalismy to juz z tej samej plytki: przy pierwszych probach
     `digitalRead()` po `analogRead()` zwracal LOW przy 2858 mV na
     wejsciu. Ta sama przyczyna, inny objaw - i nie przenioslem tej
     wiedzy do usypiania.

     `gpio_hold_en()` domyka sprawe od drugiej strony. ESP-IDF sam
     ustawia podciagniecie w `esp_deep_sleep_start()` wedlug trybu
     wybudzania i potrafi tym przestawic prog calej drabinki; zatrzask
     sprawia, ze konfiguracja z tej chwili zostaje nietknieta do
     wybudzenia. Pudelko dzienne ma to samo obejscie i z tego samego
     zrodla (espressif/esp-idf#12183).
     ================================================================= */
  pinMode(PIN_KLAPKI, INPUT);

  /* Wewnetrzne podciagniecia WYLACZAMY. Rownolegle do naszego 10 kOhm
     podnosilyby wszystkie napiecia drabinki - komora 7 (761 mV)
     przekroczylaby prog zera i przestalaby budzic pudelko.            */
  gpio_pullup_dis((gpio_num_t)PIN_KLAPKI);
  gpio_pulldown_dis((gpio_num_t)PIN_KLAPKI);

  /* =================================================================
     PRZYCISK TEZ BUDZI - I TU AKURAT SIE DA.

     W pudelku dziennym sie NIE DALO i to jest wazna roznica, a nie
     niedopatrzenie: `esp_deep_sleep_enable_gpio_wakeup()` przyjmuje
     JEDNA maske i JEDEN wspolny poziom, a tam kontaktron budzi stanem
     WYSOKIM (magnes odsuniety), przycisk NISKIM (zwarcie do masy).
     Pogodzic sie tego nie dalo bez lutowania, wiec przycisk dostal tam
     inny gest - przytrzymanie przy resecie.

     Tutaj OBA piny budza stanem niskim: drabinka klapek spada ponizej
     progu zera, przycisk zwiera do masy. Jedna maska, jeden poziom,
     jedno wywolanie - i pudelko reaguje na przycisk natychmiast, a nie
     dopiero przy najblizszym wybudzeniu z zegara.

     DWA WARUNKI, BEZ KTORYCH TO SIE ZAPETLA:
       1. podciagniecie przycisku musi PRZEZYC sen (`gpio_pullup_en`
          + zatrzask) - pin bez niego plywa i czyta sie jako zero;
       2. nie uzbrajamy przycisku, ktory WLASNIE jest wcisniety ani
          zwartego od kilku wybudzen - inaczej uklad budzi sie
          w tej samej milisekundzie, w ktorej zasnal.
     ================================================================= */
  gpio_pullup_en((gpio_num_t)PIN_PRZYCISK);
  gpio_pulldown_dis((gpio_num_t)PIN_PRZYCISK);

  /*  Przycisk WCISNIETY w chwili zasypiania uzbraja sie na wlasne
      zwarcie - obudzilby uklad w tej samej milisekundzie. Licznik
      `rtcPrzyciskZwarty` lapie drugi przypadek: styk, ktory budzi raz
      za razem, choc przy odczycie zdazyl juz odskoczyc.               */
  const bool przyciskWolny = digitalRead(PIN_PRZYCISK) == HIGH;

  uint64_t maska = 0;
  if (!otwarte && !hamulec) maska |= BIT(PIN_KLAPKI);
#if PORTAL_ENABLED
  if (hamulec || !przyciskWolny) {
    if (!przyciskWolny) LOG("[SEN] przycisk wcisniety - nie uzbrajam go\n");
  } else if (rtcPrzyciskZwarty >= PRZYCISK_ZWARTY_MAX) {
    /* Po drzemce probujemy jeszcze raz - tak samo jak przy pustych
       wybudzeniach z klapki. Cena: jedno nacisniecie moze przepasc.   */
    rtcPrzyciskZwarty = 0;
    LOG("[SEN] przycisk budzil %d razy z rzedu - rozbrajam go na ten sen\n",
        PRZYCISK_ZWARTY_MAX);
  } else {
    maska |= BIT(PIN_PRZYCISK);
  }
#endif
  if (maska) esp_deep_sleep_enable_gpio_wakeup(maska, ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_sleep_enable_timer_wakeup(sek * 1000000ULL);

  gpio_hold_en((gpio_num_t)PIN_KLAPKI);
  gpio_hold_en((gpio_num_t)PIN_PRZYCISK);
  gpio_deep_sleep_hold_en();
  esp_deep_sleep_start();
}

/* =====================================================================
 *  15.  SETUP  -  cala logika. loop() nigdy nie jest osiagany.
 * ===================================================================== */
void setup() {
  /* ---- ODCZYT KLAPKI JEST PIERWSZY I TO NIE JEST KOSMETYKA ----
     Klapka potrafi wrocic w sekunde, a Serial.begin z czekaniem na monitor
     to juz za pozno. W tescie na plytce pudelko obudzilo sie poprawnie,
     ale zobaczylo komore JUZ ZAMKNIETA i nie wiedzialo, ktora to byla. */
  esp_sleep_wakeup_cause_t powod = esp_sleep_get_wakeup_cause();

  /*  Zatrzask z `idzSpac()` trzeba zdjac, zanim pad wroci do trybu
      analogowego - inaczej przetwornik czyta pin, ktory wciaz jest
      trzymany w konfiguracji cyfrowej. To dwa zapisy do rejestru,
      mikrosekundy, wiec zasada "odczyt klapki jest pierwszy" zostaje
      nienaruszona.                                                    */
  gpio_deep_sleep_hold_dis();
  gpio_hold_dis((gpio_num_t)PIN_KLAPKI);
  gpio_hold_dis((gpio_num_t)PIN_PRZYCISK);

  /*  KTORY PIN NAS OBUDZIL - odczyt rejestru, mikrosekundy. Musi byc
      TU, przed czymkolwiek, co dotyka pinow: stan jest zatrzasniety
      z chwili wybudzenia i nic go pozniej nie odtworzy.               */
  const uint32_t maskaWybudzenia = (uint32_t)esp_sleep_get_gpio_wakeup_status();

  analogSetAttenuation(ADC_2_5db);
  analogReadResolution(12);
  if (powod == ESP_SLEEP_WAKEUP_GPIO) {
    mvPoStarcie     = czytajKlapki();
    komoraPoStarcie = ktoraKomora(mvPoStarcie);
  }

  Serial.begin(115200);
  delay(300);
  rtcWybudzen++;

  nvs.begin("pbweek", false);
#if OTA_ENABLED
  /*  PIERWSZE, co robimy po otwarciu pamieci - zanim cokolwiek innego
      zdazy sie wysypac. Licznik startow z niepotwierdzona wersja jest
      jedyna obrona przed programem, ktory sie nie uruchamia.          */
  otaSprawdzPoStarcie();
#endif
  wczytajHarmonogram();
  pinMode(PIN_PRZYCISK, INPUT_PULLUP);
  czytajBaterie();

  /*  STAN KLAPEK USTALAMY RAZ, TU - zanim cokolwiek wysle status.

      Przy wybudzeniu z pinu wiemy to juz z pomiaru zrobionego w pierwszej
      linijce setup(): komora rozpoznana znaczy klapke otwarta. Przy kazdym
      innym wybudzeniu trzeba spytac drabinki.

      Jeden pomiar, jedna prawda (D96): status wysylany z `zglos()` niesie
      dokladnie to samo, co pozniejszy meldunek z `idzSpac()`.         */
  otwarteTeraz = (powod == ESP_SLEEP_WAKEUP_GPIO && komoraPoStarcie >= 0)
                 ? true : klapkiOtwarte();
  if (otwarteTeraz && !rtcOpenSince && rtcCzasPewny)
    rtcOpenSince = (uint32_t)time(nullptr);
  if (!otwarteTeraz) rtcOpenSince = 0;

  LOG("\n===== PillBoxWeek %s  (wybudzenie %lu) =====\n",
      KOD_WERSJA, (unsigned long)rtcWybudzen);
  if (strcmp(FW_VERSION, KOD_WERSJA) != 0)
    LOG("[!!!] UWAGA: config.h mowi %s, a program jest %s - PLIKI SA Z ROZNYCH WERSJI.\n"
        "      Podmien OBA pliki z firmware/PillBoxWeek i wgraj jeszcze raz.\n",
        FW_VERSION, KOD_WERSJA);
  LOG("[BAT] %d%%  %.2f V\n", battProcent, battVolt);

  /*  Zegar ESP32 chodzi przez caly deep sleep, wiec doby domykamy JESZCZE
      PRZED radiem i przed zapisem biezacego zdarzenia. Kolejnosc w kolejce
      wychodzi wtedy chronologiczna sama z siebie: wczorajsze "missed",
      potem dzisiejsze otwarcie.                                        */
  domknijDoby();

#if PORTAL_ENABLED
  /* ---------- A0. OBUDZIL NAS PRZYCISK ----------
     Jedna maska budzi z dwoch pinow, wiec `powod` mowi tylko "GPIO".
     Ktory to byl, wie zatrzasniety rejestr odczytany na samym poczatku.
     Gdy zapalily sie OBA bity, pierwszenstwo ma klapka: dane o leku ida
     przed konfiguracja sieci, zawsze.                                 */
  if (powod == ESP_SLEEP_WAKEUP_GPIO && (maskaWybudzenia & BIT(PIN_PRZYCISK))
      && !(maskaWybudzenia & BIT(PIN_KLAPKI))) {
    if (rtcPrzyciskZwarty < 255) rtcPrzyciskZwarty++;
    LOG("[BTN] przycisk - portal konfiguracji WiFi (%u z rzedu)\n",
        (unsigned)rtcPrzyciskZwarty);
    startPortalWifi();
    idzSpac();
  }
  rtcPrzyciskZwarty = 0;        // obudzilo nas cokolwiek innego
#endif

  /* ---------- A. OBUDZILA NAS KLAPKA ---------- */
  if (powod == ESP_SLEEP_WAKEUP_GPIO) {
    int k = komoraPoStarcie;
    /*  SUROWE MILIWOLTY, nie samo slowo. "zamkniete" nie mowi, czy pin byl
        przy 1125 mV (naprawde zamkniete), czy przy 950 (styk, ktory nie
        puszcza do konca) - a to dwie zupelnie rozne usterki. Bez tej
        liczby kazdy nastepny log konczy sie hipoteza zamiast rozstrzygniecia. */
    LOG("[EV ] klapka: %s  (%u mV, progi %u..%u)\n",
        opisKomory(k), mvPoStarcie, PROGI[0], PROGI[7]);

    if (k >= 0) {
      rtcPuste = 0;                       // pin powiedzial cos sensownego
      pikniecia(k + 1);                   // potwierdzenie na sluch
      zapiszOtwarcie(k);
      beepAck();
    } else if (k == -2) {
      /* Kilka klapek naraz to NAPELNIANIE, nie dawka. Zapisanie tego jako
         wziecia zmyliloby kalendarz na caly tydzien do przodu.         */
      rtcPuste = 0;                       // pin powiedzial cos sensownego
      LOG("[EV ] kilka klapek naraz - napelnianie, nie zapisuje dawki\n");
      beepKilkaNaraz();
    } else {
      /*  MILCZYMY. Pudelko obudzilo sie, ale nie ma czym tego wyjasnic -
          a dzwiek, na ktory nie da sie zareagowac, uczy ignorowac
          pudelko. Przy pomylce w usypianiu (B30) to wlasnie ten
          pojedynczy pisk zamienil sie w melode grajaca bez konca.
          Slad zostaje w logu i na liczniku pustych wybudzen.          */
      if (rtcPuste < 255)     rtcPuste++;
      if (rtcPusteRazem < 65535) rtcPusteRazem++;
      LOG("[EV ] klapka zdazyla sie zamknac - nie pikam (puste z rzedu: %u)\n", rtcPuste);
    }
    if (battProcent >= 0 && battProcent <= BATT_WARN_PCT) { delay(300); beepBateria(); }
    idzSpac();
  }

  /* ---------- B. OBUDZIL NAS ZEGAR ---------- */
  if (powod == ESP_SLEEP_WAKEUP_TIMER) {
    wifiPolacz();
    if (WiFi.status() == WL_CONNECTED && firebaseZaloguj()) {
      pobierzUstawienia();
      domknijDoby();          // zegar mogl stac sie pewny dopiero teraz
      oproznijKolejke();
      wyslijStatus();
    }

    int32_t doba = dzisDoba();

    /*  Przypomnienie tylko wtedy, gdy dzis jeszcze nie otwierano.
        Milczenie przy niepewnym zegarze jest tu swiadome: to pudelko
        przypomina, a przypomnienie o losowej porze uczy ignorowac
        pudelko - co jest gorsze niz jego brak.                       */
    int slot = slotTeraz();
    if (slot >= 0 && rtcCzasPewny && doba >= 0) {
      if (rtcAlarmDzien != doba) {
        rtcAlarmDzien = doba; rtcAlarmMaska = 0;
        rtcAlarmSlot = -1;    rtcAlarmPonowien = 0;
      }
      if (rtcAlarmSlot != slot) { rtcAlarmSlot = (int8_t)slot; rtcAlarmPonowien = 0; }

      bool juzDzis    = (rtcOstatniDzien == doba);
      bool wyczerpany = rtcAlarmMaska & (1 << slot);
      if (juzDzis || wyczerpany) {
        LOG("[ALM] cisza (%s)\n", juzDzis ? "dzis juz otwarte" : "slot wyczerpany");
      } else {
        LOG("[ALM] przypomnienie, slot %d (proba %d)\n", slot, rtcAlarmPonowien + 1);
        int przerwane = zagrajAlarm();
        if (przerwane >= 0) { zapiszOtwarcie(przerwane); beepAck(); }
#if TG_ENABLED
        /* --- Powiadomienie po KAZDYM nieodebranym, nie po ostatnim ---
           Zdarzenie "missed" powstaje dopiero z koncem doby (D64), bo
           wczesniejsze malowaloby dzien na czerwono o 20:00, a tabletka
           moze pojsc o 22:00. To dotyczy DANYCH.

           Wiadomosc na telefon jest czyms innym: ma dotrzec wtedy, gdy
           jeszcze da sie cos z tym zrobic. O 23:00 na przypominanie jest
           po prostu pozno.                                            */
        else tgZglosNieodebrane(slot);
#endif
        rtcAlarmPonowien++;
        /*  Wracamy, dopoki zostaly proby. Slot zamykamy dopiero po
            ostatniej - inaczej jedno pikniecie o 20:00 bylo calym
            przypomnieniem na ten dzien.                              */
        if (rtcAlarmPonowien < ALARM_PONOWIEN) ponowAlarm = true;
        else rtcAlarmMaska |= (1 << slot);
      }
    }
    idzSpac();
  }

  /* ---------- C. ZIMNY START ---------- */
  LOG("[   ] zimny start\n");
  pik(BUZZER_HZ, 80);

  /* Przycisk trzymany przy starcie = autotest. Trzy pikniecia, potem
     odczyt drabinki na glos - zeby dalo sie sprawdzic pudelko bez
     komputera, juz zamkniete w obudowie.                             */
  if (digitalRead(PIN_PRZYCISK) == LOW) {
#if PORTAL_ENABLED
    /*  DLUGIE PRZYTRZYMANIE = PORTAL, krotkie = autotest.

        Oba gesty wchodza przez ten sam przycisk, bo innego nie ma.
        Rozroznia je czas, a nie kolejnosc - dzieki temu nikt nie musi
        pamietac, w ktorej chwili puscic. Trzymasz dalej: portal.
        Puszczasz: pudelko sprawdza sie na sluch, jak dotad.          */
    uint32_t trzymam = millis();
    while (digitalRead(PIN_PRZYCISK) == LOW && millis() - trzymam < PORTAL_HOLD_MS)
      delay(20);
    if (digitalRead(PIN_PRZYCISK) == LOW) {
      LOG("[BTN] przytrzymany przycisk - portal konfiguracji WiFi\n");
      startPortalWifi();
      idzSpac();
    }
#endif
    LOG("[TST] autotest\n");
    delay(400);
    for (int i = 0; i < 3; i++) { pik(BUZZER_HZ, 120); delay(160); }
    uint16_t mV = czytajKlapki();
    int k = ktoraKomora(mV);
    LOG("[TST] drabinka: %u mV -> %s\n", mV, opisKomory(k));
    delay(500);
    if (k >= 0) pikniecia(k + 1); else beepKilkaNaraz();
    delay(500);
    beepBateria();
  }

#if PORTAL_ENABLED
  /*  BEZ ZAPISANEJ SIECI NIE MA CZEGO PROBOWAC.

      `config.h` z repozytorium trzyma placeholder, wiec pudelko wgrane
      "jak jest" nie zna zadnej sieci - i bez portalu nie mialoby jak
      sie jej dowiedziec. To jest ta sama droga powrotna co przy hasle:
      jedyna, ktora nie potrzebuje ani sieci, ani bazy, ani kabla.    */
  if (nvs.getString("ssid", WIFI_SSID) == String(SSID_PLACEHOLDER)) {
    LOG("[BTN] brak zapisanej sieci - portal konfiguracji\n");
    startPortalWifi();
  }
#endif

  zglos("boot", 0);
  /*  Po zimnym starcie pamiec RTC jest pusta. domknijDoby() zapisze
      biezaca dobe jako punkt wyjscia i NIE zglosi niczego wstecz -
      pudelko nie wie, co dzialo sie, gdy bylo bez zasilania.         */
  domknijDoby();
  idzSpac();
}

void loop() { }
