/* =====================================================================
 *  PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder
 *  Plytka : Seeed Studio XIAO ESP32-C3
 *  Autor  : projekt Kuba
 *
 *  BIBLIOTEKI (Menedzer bibliotek Arduino):
 *    - ArduinoJson  (Benoit Blanchon)  >= 7.0
 *    - reszta jest czescia rdzenia "esp32 by Espressif Systems" (2.0.14+ / 3.x)
 *
 *  PROJEKT SKLADA SIE Z DWOCH PLIKOW:
 *    PillBox.ino  - ten plik, kod programu (zaczyna sie od tego naglowka)
 *    config.h     - ustawienia (zaczyna sie od "config.h - Inteligentne...")
 *  Oba musza lezec w folderze o nazwie PillBox.
 *
 *  USTAWIENIA PLYTKI:
 *    Board            : XIAO_ESP32C3
 *    USB CDC On Boot  : Enabled   (zeby dzialal Serial przez USB-C)
 *    Partition Scheme : Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)
 *
 *    UWAGA - TO SIE ZMIENILO W 1.38.0 (D59). Do 1.37.0 bylo tu
 *    "Huge APP (3MB No OTA/1MB SPIFFS)", czyli JEDNA partycja programu.
 *    Aktualizacja przez WiFi zapisuje nowy program do DRUGIEJ partycji
 *    i dopiero potem sie na nia przelacza - przy jednej nie ma dokad
 *    pisac. Nowy podzial daje 2 x 1,875 MB, w czym program zajmuje ~65%.
 *
 *    Ta zmiana wymaga JEDNEGO wgrania kablem. Pamiec NVS lezy pod tym
 *    samym adresem w obu podzialach, wiec kolejka dawek, lista sieci
 *    i haslo urzadzenia to przezyja - pod warunkiem, ze NIE zaznaczysz
 *    "Erase All Flash Before Sketch Upload".
 *
 *    SPIFFS spada z 1 MB do 190 kB. Ten projekt nie uzywa SPIFFS wcale,
 *    wiec nie ma to znaczenia - ale gdyby kiedys mial, to jest ten limit.
 *
 *  CYKL ZYCIA:
 *    deep sleep  -->  wybudzenie (kontaktron / przycisk / timer)  -->
 *    obsluga  -->  ewentualne WiFi + Firebase  -->  deep sleep
 * ===================================================================== */

#include "config.h"

/*  Zabezpieczenie przed pomyleniem plikow. Jesli config.h zawiera cos
 *  innego niz ustawienia, kompilator powie to wprost, zamiast sypac
 *  setkami niezrozumialych bledow.                                    */
#ifndef PILLBOX_CONFIG_VERSION
  #error "config.h ma niewlasciwa tresc - pobierz oba pliki (PillBox.ino i config.h) na nowo."
#endif

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <esp_sleep.h>
#include <soc/soc_caps.h>       // SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
#include <esp_wifi.h>
#include <driver/gpio.h>
#include <nvs.h>                // nvs_get_stats - ile miejsca zostalo w pamieci
#include <time.h>
#include <Update.h>             // OTA: zapis nowego programu do drugiej partycji
#include <esp_ota_ops.h>        // OTA: przelaczanie partycji i powrot do starej

/* =====================================================================
 *  PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania)
 * ===================================================================== */
RTC_DATA_ATTR uint32_t rtcBootCount     = 0;
RTC_DATA_ATTR int8_t   rtcPendingSlot   = -1;   // slot ktory wlasnie dzwoni
RTC_DATA_ATTR uint8_t  rtcAlarmRetries  = 0;    // ile prob juz bylo
RTC_DATA_ATTR bool     rtcTimeValid     = false;
RTC_DATA_ATTR uint32_t rtcLastOpenTs    = 0;    // antydrganiowo dla kontaktronu
RTC_DATA_ATTR char     rtcSchedule[96]  = {0};  // "08:00|20:00"
RTC_DATA_ATTR int16_t  rtcTzOffsetMin   = DEFAULT_TZ_OFFSET;
RTC_DATA_ATTR uint32_t rtcTakenDay      = 0;    // YYYYMMDD ostatniej zapisanej dawki
RTC_DATA_ATTR int16_t  rtcPillsLeft     = -1;   // z Firebase; -1 = nieznane
RTC_DATA_ATTR uint32_t rtcRolloverDay   = 0;    // ostatnia rozliczona doba
RTC_DATA_ATTR uint8_t  rtcRetryCount    = 0;    // nieudane proby wyslania
RTC_DATA_ATTR uint8_t  rtcStuckButton   = 0;    // ile razy z rzedu przycisk byl wcisniety
RTC_DATA_ATTR float    rtcLastVoltage   = 0.0f; // do wykrycia podlaczenia ladowarki
RTC_DATA_ATTR bool     rtcCutoff        = false;// tryb ochrony rozladowanego ogniwa
RTC_DATA_ATTR bool     rtcCharging      = false;// stoi na ladowarce
RTC_DATA_ATTR float    rtcVoltMax       = 0.0f; // szczyt napiecia w tym ladowaniu
RTC_DATA_ATTR uint8_t  rtcChargeIdle    = 0;    // pomiary bez wzrostu z rzedu
RTC_DATA_ATTR uint8_t  rtcWysokieZRzedu = 0;    // ile odczytow z rzedu bylo wysokich
RTC_DATA_ATTR bool     rtcBlokWysokie   = false;// nie wracaj do ladowania po odpieciu
RTC_DATA_ATTR uint32_t rtcChargeSinceTs = 0;    // kiedy podlaczono kabel
RTC_DATA_ATTR uint8_t  rtcChargeFromPct = 255;  // procent sprzed ladowania
RTC_DATA_ATTR uint32_t rtcTokenExp      = 0;    // do kiedy wazny token Firebase
RTC_DATA_ATTR uint32_t rtcTakenTs       = 0;    // KIEDY zapisano dawke (takze bez zegara)
RTC_DATA_ATTR uint32_t rtcAlarmDoneDay  = 0;    // doba, w ktorej alarm juz sie odbyl
RTC_DATA_ATTR uint16_t rtcAlarmDoneMask = 0;    // ...i KTORE przypomnienia (bit = slot)
RTC_DATA_ATTR uint32_t rtcAlarmDoneTs   = 0;    // ...i kiedy, gdy zegar nieznany
RTC_DATA_ATTR uint32_t rtcLastPushTs    = 0;    // kiedy ostatnio poszedl status
RTC_DATA_ATTR int16_t  rtcLastPushPct   = -1;   // z jakim procentem baterii
RTC_DATA_ATTR uint16_t rtcLogWyslanyIdx = 0xFFFF;// stan czarnej skrzynki przy ostatniej wysylce

/* --- Dni bez leku (rozpisanie tygodniowe + wyjatki na daty) ------------
   Dawki w dziesiatych czesciach tabletki: 10 = jedna, 5 = polowka, 0 = dzien
   bez leku. DOSE_NIEZNANA = nie wiemy nic o tym dniu i wtedy DZWONIMY.
   Indeks tygodnia to tm_wday, czyli 0 = niedziela - ta sama konwencja co
   Date.getDay() w aplikacji, zeby oba konce liczyly ten sam dzien.      */
RTC_DATA_ATTR uint8_t  rtcDoseWeek[7]   = { DOSE_NIEZNANA, DOSE_NIEZNANA,
                                            DOSE_NIEZNANA, DOSE_NIEZNANA,
                                            DOSE_NIEZNANA, DOSE_NIEZNANA,
                                            DOSE_NIEZNANA };
RTC_DATA_ATTR uint32_t rtcDoseExDay[DOSE_EX_MAX] = { 0 };  // YYYYMMDD wyjatku
RTC_DATA_ATTR uint8_t  rtcDoseExVal[DOSE_EX_MAX] = { 0 };  // ...i dawka na ten dzien
RTC_DATA_ATTR uint8_t  rtcDoseExCount   = 0;
RTC_DATA_ATTR bool     rtcDosingLoaded  = false;// czy wczytano juz z pamieci trwalej

/* Ktora siec z listy zadzialala ostatnio. Zaczynanie prob od niej sprawia,
   ze w domu polaczenie kosztuje dokladnie tyle samo, co przed cala ta
   zmiana - jedna probe.                                                 */
RTC_DATA_ATTR uint8_t  rtcNetOstatnia   = 0;

/* Co sie stalo z ostatnia siecia przyslana z aplikacji. Idzie do statusu,
   zeby nie trzeba bylo ZGADYWAC, czemu siec "nie chce sie wyslac" - raz juz
   kosztowalo to runde zgadywania.                                        */
RTC_DATA_ATTR char     rtcNetMsg[48]    = "";
/* Aktualizacja programu przez WiFi (D59). Prosba przychodzi z aplikacji
   i zyje tylko do najblizszej proby - w RTC, a nie w NVS, bo po restarcie
   i tak przeczytamy ja z bazy na nowo. Komunikat i wersja ida do statusu,
   zeby aplikacja umiala napisac, NA CO pudelko czeka.                  */
RTC_DATA_ATTR bool     rtcOtaProsba     = false;
/* Prosba o rozejrzenie sie za sieciami WiFi. Zyje tylko do najblizszego
   snu - po wykonaniu kasujemy zlecenie w bazie, a po restarcie i tak
   przeczytamy je z bazy na nowo.                                       */
RTC_DATA_ATTR bool     rtcScanProsba    = false;
RTC_DATA_ATTR char     rtcOtaMsg[56]    = "";
RTC_DATA_ATTR char     rtcOtaWersja[16] = "";
/* CO SERWER POWIEDZIAL O DLUGOSCI PLIKU. To nie jest ozdoba, tylko POMIAR.
   Cala naprawa z 1.42.1 opiera sie na zalozeniu, ze GitHub Pages odpowiada
   porcjowo (`chunked`) i nie podaje `Content-Length` - a tego nikt nie
   zmierzyl, bo z serwerem rozmawia wylacznie pudelko. -1 znaczy "serwer nie
   podal dlugosci" i potwierdza hipoteze; liczba dodatnia ja obala i kaze
   szukac gdzie indziej. Zero znaczy "jeszcze nie pytalismy".            */
RTC_DATA_ATTR int32_t  rtcOtaNagl       = 0;
/* KIEDY zlozono zlecenie. Sluzy do jednego: odroznienia "ktos wlasnie
   nacisnal przycisk" od "to samo zlecenie probuje sie wykonac kolejny
   raz". Swiadome zadanie omija dobowy odstep miedzy probami - odstep
   istnieje przeciwko petli automatycznych ponowien, nie przeciwko
   czlowiekowi stojacemu nad pudelkiem.                                */
RTC_DATA_ATTR uint32_t rtcOtaTs        = 0;

/* --- Pudelko zostawione otwarte --------------------------------------- */
RTC_DATA_ATTR uint32_t rtcOpenSinceTs   = 0;    // kiedy zauwazylismy otwarcie
RTC_DATA_ATTR uint32_t rtcNextWarnTs    = 0;    // kiedy nastepny sygnal
RTC_DATA_ATTR uint16_t rtcOpenWarnCount = 0;    // ile sygnalow juz bylo
RTC_DATA_ATTR bool     rtcOpenReported  = false;// aplikacja wie, ze jest otwarte
RTC_DATA_ATTR bool     rtcOpenClearPend = false;// trzeba jej zglosic zamkniecie
RTC_DATA_ATTR bool     rtcStatusDirty   = false;// ostatnia wysylka statusu NIE doszla
RTC_DATA_ATTR bool     rtcArmedForClose = false;// spimy czekajac na ZAMKNIECIE
RTC_DATA_ATTR uint8_t  rtcBattPct       = 255;  // wygladzony procent; 255 = brak historii
RTC_DATA_ATTR uint8_t  rtcBattUp        = 0;    // ile odczytow z rzedu bylo wyzszych

/* Dwa liczniki cichych strat. Ida do statusu, wiec aplikacja moze o nich
   krzyknac - inaczej byloby to jedyne miejsce w projekcie, gdzie dane
   znikaja w sposob z zalozenia niewykrywalny.                          */
RTC_DATA_ATTR uint16_t rtcNvsFail       = 0;    // nieudane zapisy do pamieci trwalej
/* KTORY klucz nie zapisal sie OSTATNIO. Sam licznik mowi "cos przepadlo",
   ale nie mowi co - a to jest roznica miedzy "nic sie nie stalo" (token,
   wpis diagnostyczny) a "zgubiona dawka" (wpis kolejki). Bez tego kazdy
   taki alarm u Kuby konczyl sie zgadywaniem. Klucze NVS w tym pliku sa
   budowane w buforach char[8] ("q119", "n2p", "lb31"...), wiec 10 znakow
   starcza z zapasem, razem z koncowym zerem.                            */
RTC_DATA_ATTR char     rtcNvsFailKey[10] = "";

/* HISTORIA nieudanych zapisow, nie tylko ostatni. Kuba: "może w bazie
   damy jakiś zapis, żeby te nieudane tam wrzucał" - z samej LICZBY i
   OSTATNIEGO klucza nie da sie sprawdzic hipotezy typu "to sie dzieje
   po alarmie" - trzeba widziec CZAS kazdego incydentu z osobna.

   Pierscien w PAMIECI RTC, nie w NVS: zapisywanie awarii NVS przez zapis
   do NVS jest cyrkularne - gdyby to WLASNIE ta pamiec akurat szwankowala,
   probowalibysmy udokumentowac awarie w miejscu, ktore wlasnie zawodzi.
   RTC przezywa deep sleep (to jedyny tryb snu, jaki to urzadzenie zna)
   i nie kosztuje ani jednego cyklu zapisu flash.

   rtcNvsFailLogTotal rosnie z kazdym niepowodzeniem, tak jak rtcNvsFail -
   wiecej niz NVS_FAILLOG_SLOTS nadpisuje najstarsze wpisy w pierscieniu.
   rtcNvsFailLogSent pamieta, ile z nich juz POTWIERDZONO w bazie, zeby
   pushStatus() nie wysylal w kolko tego samego.                        */
#define NVS_FAILLOG_SLOTS 8
RTC_DATA_ATTR uint32_t rtcNvsFailLogTs[NVS_FAILLOG_SLOTS]     = {0};
RTC_DATA_ATTR char     rtcNvsFailLogKey[NVS_FAILLOG_SLOTS][10] = {{0}};
RTC_DATA_ATTR uint16_t rtcNvsFailLogTotal = 0;
RTC_DATA_ATTR uint16_t rtcNvsFailLogSent  = 0;

RTC_DATA_ATTR uint16_t rtcQueueDropped  = 0;    // wpisy zdjete z kolejki bez wyslania

/* --- Powiadomienia na telefon (bot Telegram, D67) ---------------------
   Zamiar, nie tresc. Trzymamy NUMER przypomnienia i CZAS jego powstania,
   a zdanie budujemy dopiero przy wysylce - dzieki temu w cennej pamieci
   RTC leza dwie liczby zamiast bufora na tekst.

   Czas nie jest ozdoba, tylko warunkiem: powiadomienie starsze niz
   TG_MAX_WIEK_S kasujemy zamiast wysylac. Pudelko bywa offline dobe,
   a "nie wziales tabletki o 20:00" dostarczone nazajutrz w poludnie
   informuje o czyms, co dawno przestalo byc prawda.

   rtcTgSlot = -1 znaczy "nie ma czego wysylac". Slot, a nie sama flaga,
   bo w wiadomosci ma stac GODZINA tego przypomnienia - inaczej dwa
   przypomnienia w ciagu wieczora daja dwie identyczne wiadomosci i nie
   wiadomo, ktora jest ktora.                                          */
RTC_DATA_ATTR int8_t   rtcTgSlot        = -1;   // nieodebrane przypomnienie do zgloszenia
RTC_DATA_ATTR uint32_t rtcTgSlotTs      = 0;    // ...i kiedy powstalo
RTC_DATA_ATTR bool     rtcTgBattCzeka   = false;// ostrzezenie o baterii do wyslania
/* Ostrzezenie o baterii ma przyjsc RAZ na rozladowanie, a nie przy kazdym
   wybudzeniu ponizej progu (wyrazna prosba Kuby: "tylko jeden jak pudelko
   ma malo bateri"). Znacznik kasuje sie sam po naladowaniu powyzej
   TG_BATT_RESET_PCT - inaczej druga wiadomosc nie przyszlaby nigdy.   */
RTC_DATA_ATTR bool     rtcTgBattZgloszona = false;
/* Prosba z aplikacji o wiadomosc probna. Zyje do najblizszego snu -
   po wykonaniu kasujemy zlecenie w bazie, tak samo jak `wifiCmd`.    */
RTC_DATA_ATTR bool     rtcTgTestProsba  = false;
/* Co sie stalo z ostatnia wiadomoscia. Idzie do statusu, bo inaczej
   "bot nie pisze" ma trzy nieodroznialne przyczyny: pudelko nie ma
   tokenu, ma i Telegram odmowil, albo nie bylo jeszcze o czym pisac.  */
RTC_DATA_ATTR char     rtcTgMsg[56]     = "";

/* =====================================================================
 *  STAN GLOBALNY
 * ===================================================================== */
/* --- TYPY WLASNE STOJA PRZED PIERWSZA FUNKCJA W PLIKU. NIE PRZENOSIC. ---

   Arduino IDE samo dopisuje prototypy wszystkich funkcji i wstawia je
   TUZ PRZED PIERWSZA DEFINICJA FUNKCJI w pliku. Wygenerowany prototyp
   `const char* wakeName(WakeReason w);` trafia wiec wyzej niz definicja
   `enum WakeReason` - i kompilacja pada na "WakeReason was not declared
   in this scope", w miejscu, ktore wyglada na zupelnie poprawne.

   TO JUZ RAZ ZATRZYMALO PROJEKT NA MIESIAC. Enumy stoly kiedys nizej,
   pod zmiennymi globalnymi, i wszystko dzialalo - bo pierwsza funkcja
   w pliku byla wtedy awakeTooLong(), zdefiniowana JESZCZE NIZEJ. Dopiero
   naprawa B5 dolozyla nvsPutStr() na samej gorze i przesunela punkt
   wstawiania prototypow ponad enumy. Od tej chwili ZADNA wersja firmware
   nie dala sie wgrac z Arduino IDE - a nikt tego nie zauwazyl, bo
   tests/kompiluj_firmware.sh kompiluje jako .cpp i ten etap omija (D17).

   Zasada: kazdy typ uzywany w SYGNATURZE jakiejkolwiek funkcji musi byc
   zadeklarowany powyzej pierwszej definicji funkcji. Pilnuje tego audyt
   (tests/audit_firmware.py) oraz kompilacja przez prawdziwa sciezke .ino
   w tests/kompiluj_firmware.sh.                                        */
enum WakeReason { WAKE_BOOT, WAKE_REED, WAKE_BUTTON, WAKE_TIMER, WAKE_CLOSED };

/* Powody odmowy aktualizacji przez WiFi (D59). Osobne wartosci, nie samo
   true/false, bo aplikacja ma napisac Kubie, NA CO pudelko czeka - "za
   malo baterii" i "poddalem sie po trzech probach" to dwie zupelnie rozne
   wiadomosci, a jedyna wspolna cecha jest ta, ze nic sie nie dzieje.

   Stoi TUTAJ, nie przy samej funkcji, wlasnie z powodu opisanego wyzej:
   `otaDecyzja()` zwraca ten typ, wiec musi byc znany przed pierwsza
   definicja funkcji w pliku. Audyt to sprawdza.

   Znaczniki @extract sprawiaja, ze testy dostaja TEN enum, a nie wlasna
   kopie - inaczej dopisanie tu powodu odmowy zostawialoby testy przy
   starej liscie i nikt by tego nie zauwazyl.                          */
/* @extract-begin */
enum OtaDecyzja {
  OTA_ROB = 0,          // wszystko sie zgadza - pobieraj
  OTA_NIC_NOWEGO,       // ta sama suma co juz mam
  OTA_BEZ_HASLA,        // pamiec trwala nie ma hasla do bazy
  OTA_KOLEJKA,          // sa niewyslane dawki - one maja pierwszenstwo
  OTA_BATERIA,          // za malo pradu i nie stoi na ladowarce
  OTA_PODDANO,          // OTA_MAX_FAILS prob z rzedu bez skutku
  OTA_ZEPSUTA,          // ta wersja juz raz nie wstala
  OTA_ZLY_OPIS          // plik z opisem nie ma sensu (rozmiar, suma)
};
/* @extract-end */

/* Co zrobic z czekajacym powiadomieniem na telefon (D67).

   Osobne wartosci, a nie true/false, z tego samego powodu co przy OTA:
   "nie mam komu wyslac" i "za pozno, zeby to mialo sens" konczy sie tak
   samo - cisza - a znaczy co innego i co innego trzeba z tym zrobic.
   Pierwsze kaze podlaczyc bota w aplikacji, drugie jest normalna praca
   urzadzenia, ktore wrocilo do sieci po dobie offline.

   Stoi TUTAJ, przed pierwsza definicja funkcji, bo `tgDecyzja()` zwraca
   ten typ (B21/D26). Znaczniki @extract oddaja go testom, zeby nie
   pracowaly na wlasnej kopii listy.                                    */
/* @extract-begin */
enum TgDecyzja {
  TG_WYSLIJ = 0,        // jest co wyslac, jest komu i nie jest za pozno
  TG_NIC,               // nic nie czeka
  TG_BRAK_BOTA,         // nikt nie podlaczyl bota w aplikacji
  TG_ZA_STARE           // powstalo dawno - wysylka bylaby dezinformacja
};
/* @extract-end */

/* Co uzytkownik pokazal przyciskiem po otwarciu wieczka. */
enum Gest { GEST_BRAK, GEST_TEST, GEST_PORTAL };

Preferences prefs;

/* Jedno miejsce notujace niepowodzenie - wolane z OBU funkcji nizej, zeby
   poprawka w jednej z nich nie zapomniala o drugiej (raz juz tak bylo -
   D46 dopisywalo rtcNvsFailKey w dwoch kopiach z rzedu).                */
static void zanotujNvsFail(const char* key) {
  if (rtcNvsFail < 65535) rtcNvsFail++;
  strncpy(rtcNvsFailKey, key, sizeof(rtcNvsFailKey) - 1);
  rtcNvsFailKey[sizeof(rtcNvsFailKey) - 1] = 0;

  uint16_t slot = rtcNvsFailLogTotal % NVS_FAILLOG_SLOTS;
  rtcNvsFailLogTs[slot] = rtcTimeValid ? (uint32_t)time(nullptr) : 0;
  strncpy(rtcNvsFailLogKey[slot], key, sizeof(rtcNvsFailLogKey[slot]) - 1);
  rtcNvsFailLogKey[slot][sizeof(rtcNvsFailLogKey[slot]) - 1] = 0;
  if (rtcNvsFailLogTotal < 65535) rtcNvsFailLogTotal++;
}

/* --- Zapisy do NVS, ktore nie udaja, ze sie udaly ---------------------
   putString() i putUShort() zwracaja liczbe zapisanych bajtow, a ZERO gdy
   zapis przepadl (pamiec pelna, uszkodzony wpis). Kod wyrzucal te wartosc
   do kosza, wiec nieudany zapis wygladal dokladnie tak samo jak udany -
   a w kolejce oznaczal cicha utrate dawki. Rodzina bledu 3.5.

   Stoja TUTAJ, przed pierwszym uzyciem - reszta pliku czyta je od gory.  */
bool nvsPutStr(const char* key, const String& val) {
  /* PUSTA WARTOSC TO PRZYPADEK SZCZEGOLNY, nie awaria.

     putString("") zwraca ZERO - dokladnie tyle samo, co zapis nieudany.
     Odroznic tego sie nie da, wiec pustej wartosci nie zapisujemy wcale:
     kasujemy klucz. Przy odczycie wychodzi na to samo (getString oddaje
     wtedy wartosc domyslna, czyli pusta), a wynik jest jednoznaczny.

     Bez tego kazdy zapis pustego napisu - haslo do sieci OTWARTEJ, pusta
     lista wyjatkow dawkowania - podnosil licznik "utrata danych" i kazal
     aplikacji krzyczec o awarii, ktorej nie bylo. Gorzej: `wifiSiecDodaj()`
     zwracalo wtedy false, wiec haslo nie bylo kasowane z bazy, a siec
     wygladala na nieprzyjeta mimo poprawnego zapisu.                     */
  if (val.length() == 0) {
    prefs.remove(key);
    return true;
  }
  if (prefs.putString(key, val) > 0) return true;
  zanotujNvsFail(key);
  LOG("[NVS] ZAPIS NIEUDANY: %s\n", key);
  return false;
}

bool nvsPutU16(const char* key, uint16_t val) {
  if (prefs.putUShort(key, val) > 0) return true;
  zanotujNvsFail(key);
  LOG("[NVS] ZAPIS NIEUDANY: %s\n", key);
  return false;
}

/* --- Ile miejsca zostalo w pamieci trwalej ----------------------------
   nvsPutStr() mowi, ze zapis PRZEPADL. Nie mowi, ile jeszcze zostalo -
   a to jest roznica miedzy "wlasnie zaczely ginac dane" a "za dwa dni
   zaczna". Kolejka, dziennik wieczka, czarna skrzynka i token siedza w
   tej samej partycji 20 kB; gdy sie zapelni, kazdy z nich zaczyna cicho
   gubic wpisy. Aplikacja ma to widziec ZANIM zniknie pierwsza dawka.

   Zwraca liczbe wolnych wpisow, albo -1 gdy nie da sie odczytac.       */
int nvsWolneWpisy() {
  nvs_stats_t st;
  if (nvs_get_stats(nullptr, &st) != ESP_OK) return -1;
  return (int)st.free_entries;
}

int    batteryPercentage    = 0;    // po wygladzeniu - to trafia do aplikacji
int    batteryRawPercentage = 0;    // prosto z krzywej, do diagnostyki
float  realBatteryVoltage   = 0.0f;

String slots[12];          // godziny "HH:MM"
int    slotCount = 0;

/* Same typy stoja wyzej, przed pierwsza funkcja - patrz komentarz tam. */
WakeReason wakeReason = WAKE_BOOT;

bool boxOpenWarned  = false;    // czy w tym wybudzeniu zabrzmial sygnal "otwarte"
bool byloOtwarte    = false;    // czy w tym wybudzeniu wieczko bylo otwarte
uint32_t msZamkniecia = 0;      // millis() w chwili wykrycia zamkniecia
bool portalRequested = false;   // przycisk trzymany przy resecie -> portal WiFi
Gest gestPoOtwarciu  = GEST_BRAK;  // co uzytkownik pokazal po otwarciu wieczka

bool batterySaver = false;      // true = za niskie napiecie, nie wlaczamy radia
bool timeSyncedThisWake = false;

bool syncTimeNTP();             // deklaracja - wifiConnect() wola ja od razu
String logbookJson();           // czarna skrzynka - definicje nizej
void   setTakenDay(uint32_t day);   // wola ja syncTimeNTP() przy odzyskaniu zegara
void   logbookPrint();
void   note(const char* co);    // odnotuj, co sie stalo w tym wybudzeniu
Gest   czekajNaZamkniecieIGest(uint32_t limitMs);
void   autoTest();
void   startWifiPortal();

/* --- Bezpiecznik czasowy -----------------------------------------------
   Chroni przed zawieszeniem sie na wieki, ale NIE moze przerwac operacji,
   ktore z natury trwaja dlugo: dzwoniacego alarmu (do 120 s) czy portalu
   konfiguracyjnego (do 5 min). Dlatego limit jest ruchomy - po takiej
   operacji przesuwamy go, zeby zostal czas na wyslanie danych.
   Bez tego dluzsze wpisywanie hasla WiFi konczyloby sie konfiguracja,
   po ktorej urzadzenie i tak nie zdazyloby sie nigdzie polaczyc.        */
uint32_t awakeDeadlineMs = AWAKE_LIMIT_MS;

bool awakeTooLong() { return millis() > awakeDeadlineMs; }

void extendAwake(uint32_t ms) {
  uint32_t d = millis() + ms;
  if (d > awakeDeadlineMs) awakeDeadlineMs = d;
}

/* =====================================================================
 *  1.  POMIAR BATERII
 *      Blok zgodny 1:1 ze specyfikacja projektu - nie zmieniac matematyki.
 * ===================================================================== */
/* Mediana z 5 probek. Pojedynczy odczyt ADC w ESP32 potrafi skakac o 2-3%,
   przez co procent baterii "tanczyl" miedzy wybudzeniami. Mediana odrzuca
   skrajne przypadki lepiej niz srednia (jeden zaklocony odczyt nie psuje
   wyniku). Sama matematyka przeliczenia pozostaje bez zmian.            */
int readBatteryRaw() {
  int v[5];
  for (int i = 0; i < 5; i++) { v[i] = analogRead(PIN_BATTERY); delay(3); }
  for (int i = 0; i < 4; i++)
    for (int j = i + 1; j < 5; j++)
      if (v[j] < v[i]) { int t = v[i]; v[i] = v[j]; v[j] = t; }
  return v[2];
}

/* ---------------------------------------------------------------------
   KRZYWA ROZLADOWANIA OGNIWA LiPo

   Ogniwo litowo-polimerowe NIE rozladowuje sie liniowo. Z 4,20 V spada do
   okolic 4,00 V bardzo szybko, potem przez wieksza czesc pojemnosci tkwi
   miedzy 3,85 a 3,75 V, a pod koniec leci gwaltownie w dol. Rozciagniecie
   prostej od 3,3 do 4,2 V dawalo wiec wskazanie, ktore na gorze pedzilo,
   w srodku stalo, a na dole klamalo grubo na plus: przy 3,70 V pokazywalo
   44%, choc realnie zostawalo kilkanascie procent.

   To nie byla tylko kosmetyka. Prog ostrzegawczy 10% wypadal przy 3,39 V,
   czyli tuz nad napieciem odciecia - sygnal "naladuj mnie" odzywal sie
   praktycznie w chwili, gdy pudelko i tak juz zasypialo. Teraz 10% to
   ok. 3,69 V, czyli ostrzezenie z realnym wyprzedzeniem.

   Punkty ponizej to typowa krzywa spoczynkowa ogniwa LiPo przy malym
   obciazeniu - a takie wlasnie mamy, bo pomiar robimy przed wlaczeniem
   radia. Miedzy punktami interpolujemy liniowo.
   --------------------------------------------------------------------- */
/* @extract-begin */
struct BattCurvePoint { float v; uint8_t pct; };
static const BattCurvePoint BATT_CURVE[] = {
  {4.20f,100}, {4.15f, 95}, {4.11f, 90}, {4.08f, 85}, {4.02f, 80},
  {3.98f, 75}, {3.95f, 70}, {3.91f, 65}, {3.87f, 60}, {3.85f, 55},
  {3.84f, 50}, {3.82f, 45}, {3.80f, 40}, {3.79f, 35}, {3.77f, 30},
  {3.75f, 25}, {3.73f, 20}, {3.71f, 15}, {3.69f, 10}, {3.61f,  5},
  {3.27f,  0}
};
static const int BATT_CURVE_N = sizeof(BATT_CURVE) / sizeof(BATT_CURVE[0]);
/* @extract-end */

int battPercentFromCurve(float v) {
  if (v >= BATT_CURVE[0].v)              return 100;
  if (v <= BATT_CURVE[BATT_CURVE_N-1].v) return 0;
  for (int i = 1; i < BATT_CURVE_N; i++) {
    if (v >= BATT_CURVE[i].v) {
      const BattCurvePoint& hi = BATT_CURVE[i-1];
      const BattCurvePoint& lo = BATT_CURVE[i];
      float t = (v - lo.v) / (hi.v - lo.v);
      int p = (int)lround(lo.pct + t * (float)(hi.pct - lo.pct));
      return p < 0 ? 0 : (p > 100 ? 100 : p);
    }
  }
  return 0;
}

/* Wygladzanie miedzy wybudzeniami.

   Dwa problemy naraz. Po pierwsze szum ADC: w plaskiej czesci krzywej
   10 mV to az 3 punkty procentowe, wiec surowy odczyt "tanczy". Po drugie
   wskazanie skaczace w gore i w dol wyglada jak awaria.

   Rozwiazanie: ograniczamy krok. Miedzy dwoma wybudzeniami wskazanie moze
   spasc najwyzej o BATT_STEP_DOWN punktow i urosnac najwyzej o
   BATT_STEP_UP. Przy realnym zuzyciu rzedu 1% na dobe to i tak z duzym
   zapasem, a pojedynczy zanizony pomiar przesuwa wskazanie o kilka
   punktow zamiast o dwadziescia.

   Ruch w gore jest celowo dopuszczony, choc bateria sama sie nie laduje.
   Bez tego kazde chwilowe zaklocenie zostawaloby w pamieci na zawsze i
   wskazanie dryfowaloby coraz nizej. Jeden punkt na wybudzenie jest zbyt
   powolny, zeby wygladac na awarie, a wystarcza, by blad sam sie zagoil.
   Prawdziwe ladowanie obslugujemy osobno - patrz resetBatteryFilter.   */
void resetBatteryFilter() { rtcBattPct = 255; rtcBattUp = 0; }

/* Czy pudelko stoi na ladowarce.

   Pojedynczy wzrost napiecia mowi tylko "cos sie wlasnie zaczelo" - a do
   zywego podgladu w aplikacji potrzebny jest STAN, ktory trwa. Stad ta
   funkcja: wchodzi w tryb ladowania na wzroscie, a wychodzi dopiero na
   spadku wzgledem szczytu. Odlaczenie kabla widac natychmiast, bo ogniwo
   pod obciazeniem od razu siada.

   Plateau przy pelnym ogniwie jest tu osobnym przypadkiem. Powyzej
   CHARGE_FULL_V napiecie przestaje rosnac, mimo ze kabel wciaz tkwi -
   gdyby liczyc to jako koniec ladowania, aplikacja gasilaby wskaznik
   dokladnie w momencie, w ktorym naladowanie dobiega konca.

   CHARGE_IDLE_MAX jest bezpiecznikiem. Gdyby wykrycie sie zaklinowalo,
   pudelko budziloby sie co minute i zjadlo ogniwo w dobe.            */
/* Zapis momentu, w ktorym skonczylo sie ladowanie.

   Idzie do pamieci NIEULOTNEJ, a nie do RTC - bo najczestszym powodem
   ladowania jest rozladowane ogniwo, a wtedy pamiec RTC i tak przepada.
   Trzymamy dwie ostatnie daty: z nich aplikacja policzy, ile realnie
   starczylo na jednym ladowaniu.                                      */
void zapiszKoniecLadowania() {
  if (!rtcTimeValid) return;
  prefs.begin(NVS_NAMESPACE, false);
  uint32_t poprzednie = prefs.getUInt("chgEnd", 0);
  if (poprzednie) prefs.putUInt("chgPrev", poprzednie);
  prefs.putUInt("chgEnd", (uint32_t)time(nullptr));
  prefs.end();
  LOGLN("[CHG] zapisano date ladowania");
}

bool trackCharging(float v, float prev) {
  if (v < 2.0f) {                       // brak ogniwa albo pomiar bez sensu
    rtcCharging = false; rtcChargeIdle = 0; rtcVoltMax = 0.0f;
    return false;
  }
  const bool rosnie  = prev > 2.0f && v > prev + CHARGE_RISE_V;

  /* Samo napiecie tez jest dowodem - i to bylo potrzebne.

     Wykrywanie po WZROSCIE zaklada, ze pudelko zmierzy napiecie przed
     podlaczeniem kabla i po nim. Zalozenie pada w dwoch czestych
     sytuacjach: gdy wgrywasz firmware przy podpietym kablu (pamiec RTC
     sie kasuje, wiec pierwszy pomiar to juz 4,2 V i zadnego wzrostu
     nigdy nie bedzie) oraz gdy kabel wchodzi w trakcie snu.

     Ogniwo odlaczone od ladowarki nie utrzymuje 4,15 V - opada. Dwa
     wysokie odczyty z rzedu to wiec kabel, a nie pelne ogniwo.

     rtcBlokWysokie pilnuje, zebysmy zaraz po odpieciu nie wskoczyli w
     ladowanie z powrotem: napiecie jest wtedy jeszcze wysokie i bez tej
     blokady tryb migalby w kolko.                                    */
  const bool wysokie = v >= CHARGE_FULL_V;
  if (!wysokie) rtcBlokWysokie = false;
  const bool zWysokiego = wysokie && !rtcBlokWysokie && rtcWysokieZRzedu >= 1;
  if (wysokie) { if (rtcWysokieZRzedu < 255) rtcWysokieZRzedu++; }
  else           rtcWysokieZRzedu = 0;

  if (rosnie || zWysokiego) {
    /* Wejscie w tryb ladowania - zapamietujemy moment i stan wyjsciowy.
       Na tych dwoch liczbach aplikacja opiera odliczanie do pelna, wiec
       ustawiamy je TYLKO przy wejsciu, nie przy kazdym pomiarze.      */
    if (!rtcCharging) {
      rtcChargeSinceTs = rtcTimeValid ? (uint32_t)time(nullptr) : 0;
      rtcChargeFromPct = rtcBattPct;      // 255 = nieznany, aplikacja to rozumie
    }
    rtcCharging   = true;
    rtcChargeIdle = 0;
  } else if (rtcCharging) {
    if (v < rtcVoltMax - CHARGE_DROP_V) {
      rtcCharging = false; rtcChargeIdle = 0;      // kabel wyciagniety
      rtcBlokWysokie = true;                       // nie wracaj na samym napieciu
      zapiszKoniecLadowania();
    } else if (v >= CHARGE_FULL_V) {
      rtcChargeIdle = 0;                           // pelne, ale nadal na kablu
    } else {
      if (rtcChargeIdle < 255) rtcChargeIdle++;
      if (rtcChargeIdle >= CHARGE_IDLE_MAX) { rtcCharging = false; rtcChargeIdle = 0; }
    }
  }
  if (rtcCharging) { if (v > rtcVoltMax) rtcVoltMax = v; }
  else             { rtcVoltMax = v; }
  return rtcCharging;
}

int battSmooth(int raw) {
  if (rtcBattPct == 255) { rtcBattPct = (uint8_t)raw; rtcBattUp = 0; return raw; }

  int prev = rtcBattPct;
  int diff = raw - prev;

  /* Martwa strefa. Poprzednia wersja poprawiala wskazanie przy kazdej
     roznicy, wiec przy szumie rzedu kilku punktow procent drgal w gore
     i w dol praktycznie co wybudzenie - dokladnie to, czego mial nie robic. */
  if (diff > -BATT_DEADBAND && diff < BATT_DEADBAND) { rtcBattUp = 0; return prev; }

  int out;
  if (diff < 0) {
    /* Spadek przyjmujemy od razu, ale ograniczony co do wielkosci. */
    rtcBattUp = 0;
    out = prev + (diff < -BATT_STEP_DOWN ? -BATT_STEP_DOWN : diff);
  } else {
    /* Wzrost musi sie potwierdzic. Jeden wyzszy odczyt to przypadek;
       kilka pod rzad oznacza, ze ogniwo naprawde odzyskalo napiecie.  */
    if (rtcBattUp < 255) rtcBattUp++;
    if (rtcBattUp < BATT_RISE_STREAK) return prev;
    rtcBattUp = 0;
    out = prev + BATT_STEP_UP;
  }

  if (out < 0)   out = 0;
  if (out > 100) out = 100;
  rtcBattPct = (uint8_t)out;
  return out;
}

void readBattery() {
  pinMode(PIN_BATTERY, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_BATTERY, ADC_11db);
  delay(5);
  (void)analogRead(PIN_BATTERY);            // odczyt "na sucho", stabilizacja S/H

  /* --- pomiar napiecia: matematyka bez zmian, zgodna ze specyfikacja --- */
  int rawValue = readBatteryRaw();
  float pinVoltage = (rawValue / 4095.0) * 3.3;
  float rawBatteryVoltage = pinVoltage * 2.0;
  // Wyliczony empirycznie wspolczynnik kalibracji (4.20V / 4.56V)
  const float CALIBRATION_FACTOR = 0.921;
  realBatteryVoltage = rawBatteryVoltage * CALIBRATION_FACTOR;

  /* --- przeliczenie na procent: krzywa LiPo zamiast prostej --- */
  batteryRawPercentage = battPercentFromCurve(realBatteryVoltage);
  batteryPercentage    = battSmooth(batteryRawPercentage);

  LOG("[BAT] raw=%d  U=%.3fV  %d%% (z krzywej %d%%)\n",
      rawValue, realBatteryVoltage, batteryPercentage, batteryRawPercentage);
}

/* =====================================================================
 *  2.  BUZZER  (pasywny piezo -> PWM przez LEDC)
 * ===================================================================== */
void buzzerInit() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(PIN_BUZZER, BUZZER_FREQ_HZ, 10);
#else
  ledcSetup(0, BUZZER_FREQ_HZ, 10);
  ledcAttachPin(PIN_BUZZER, 0);
#endif
}

void buzzerTone(uint32_t freq) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWriteTone(PIN_BUZZER, freq);
#else
  ledcWriteTone(0, freq);
#endif
}

/* Ten sam ton, ale ciszej. Piezo gra glosnoscia proporcjonalna do
   wypelnienia przebiegu - przy 10% jest wyraznie slyszalny, a nie wierci
   w uchu. Uzywane tam, gdzie sygnal ma trwac dlugo.                    */
void buzzerTonCicho(uint32_t freq, uint32_t duty) {
  buzzerTone(freq);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(PIN_BUZZER, duty);
#else
  ledcWrite(0, duty);
#endif
}

void buzzerOff() {
  buzzerTone(0);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(PIN_BUZZER, 0);
#else
  ledcWrite(0, 0);
#endif
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);            // brak napiecia DC na piezo = 0 mA
}

/* Krotki sygnal potwierdzenia (np. "zapisalem otwarcie"). */
void beepAck() {
  buzzerInit();
  buzzerTone(BUZZER_FREQ_HZ);
  delay(60);
  buzzerTone(BUZZER_FREQ_HZ * 3 / 2);
  delay(80);
  buzzerOff();
}

/* ---------------------------------------------------------------------
   WYSOKOSC TONOW A SLYSZALNOSC

   Piezo 23 mm ma rezonans w okolicy 2700 Hz i poza nim gra DRAMATYCZNIE
   ciszej - przy 700 Hz bywa nawet o 20-30 dB slabszy. Pierwotnie sygnal
   bledu gral wlasnie na 700 Hz, niska bateria na 800, a konczace sie
   opakowanie na 1100. W praktyce oznaczalo to, ze pudelko owszem
   sygnalizowalo problem, tylko nie bylo tego slychac - i wygladalo, jakby
   nie robilo nic.

   Dlatego wszystkie sygnaly siedza teraz miedzy 2000 a 3400 Hz, a rozniuje
   je RYTM, nie wysokosc. Rytm slychac tak samo dobrze na kazdej glosnosci.
   --------------------------------------------------------------------- */

/* Blad / brak sieci - trzy szybkie, natarczywe pipniecia. */
void beepErr() {
  buzzerInit();
  for (int i = 0; i < 3; i++) { buzzerTone(2300); delay(110); buzzerTone(0); delay(80); }
  buzzerOff();
}

/* "Zapisalem, wysle pozniej" - jedno pipniecie i krotkie potwierdzenie.
   Spokojniejsze niz blad, bo dawka NIE zostala zgubiona: siedzi w pamieci
   pudelka i poleci przy najblizszej okazji. Uzytkownik ma wiedziec, ze
   otwarcie zostalo zauwazone, nawet gdy router akurat nie dziala.       */
void beepQueued() {
  buzzerInit();
  buzzerTone(2700); delay(70);
  buzzerTone(0);    delay(70);
  buzzerTone(2200); delay(260);
  buzzerOff();
}

/* "JUZ DZIS BRALES" - opadajaca seria, wyraznie inna od potwierdzenia.
   Piec szybkich tonow w dol + dlugi niski. Trudno pomylic z ACK.       */
void beepAlreadyTaken() {
  buzzerInit();
  const uint16_t f[5] = {3400, 3100, 2800, 2500, 2200};
  for (int r = 0; r < 2; r++) {
    for (int i = 0; i < 5; i++) { buzzerTone(f[i]); delay(70); }
    buzzerTone(0); delay(120);
  }
  buzzerTone(2000); delay(400);
  buzzerOff();
}

/* Nowa wersja programu wstala i dziala - fanfara w gore (D59).

   PO CO OSOBNY DZWIEK. Aktualizacja przez WiFi jest jedyna rzecza w tym
   urzadzeniu, ktora dzieje sie CALKOWICIE bez udzialu czlowieka: pudelko
   samo pobiera, samo sie restartuje i samo decyduje, czy nowa wersja
   nadaje sie do uzytku. Bez sygnalu jedynym sladem byla liczba w
   aplikacji - a ta przychodzi z opoznieniem i nie mowi, czy program
   naprawde WSTAL, czy tylko sie zapisal.

   Ten dzwiek gra dokladnie raz: w chwili, gdy swiezo wgrana wersja
   przeszla cala swoja droge i zostala uznana za dzialajaca. Jest
   ROSNACY i dluzszy niz cokolwiek innego w tym pliku, zeby nie dalo sie
   go pomylic z przypomnieniem o leku (te sa krotkie i powtarzalne) ani
   z ostrzezeniem "juz dzis brales" (to opada).                        */
void beepNowaWersja() {
  buzzerInit();
  const uint16_t f[6] = {1500, 1800, 2100, 2400, 2700, 3000};
  for (int i = 0; i < 6; i++) { buzzerTone(f[i]); delay(90); }
  buzzerTone(0); delay(120);
  buzzerTone(3000); delay(450);
  buzzerOff();
}

/* Niski zapas tabletek - trzy dlugie niskie piknięcia po potwierdzeniu. */
void beepLowStock() {
  buzzerInit();
  for (int i = 0; i < 3; i++) { buzzerTone(2500); delay(250); buzzerTone(0); delay(150); }
  buzzerOff();
}

/* "PADAM - naladuj mnie". Charakterystyczny, opadajacy zew: dwa dlugie
   tony schodzace w dol, powtorzone. Slychac go po kazdym potwierdzeniu
   dawki, wiec dowiesz sie o niskiej baterii bez zagladania do telefonu. */
void beepLowBattery(bool critical) {
  buzzerInit();
  int reps = critical ? 3 : 2;
  for (int r = 0; r < reps; r++) {
    buzzerTone(3100); delay(critical ? 320 : 200);
    buzzerTone(2200); delay(critical ? 450 : 280);
    buzzerTone(0);    delay(200);
  }
  buzzerOff();
}

/* "ZOSTAWILES MNIE OTWARTE" - jeden rowny, dluzszy ton.
   Celowo nie jest to melodia ani seria: wszystkie pozostale sygnaly w tym
   urzadzeniu sa zlozone (rosnace, opadajace, powtarzane), wiec pojedynczy
   plaski ton slychac jako cos wyraznie innego, a przy tym nie jest
   natretny - ma przypomniec, a nie zdenerwowac.                          */
void beepBoxOpen() {
  buzzerInit();
  buzzerTone(OPEN_WARN_TONE_HZ);
  delay(OPEN_WARN_MS);
  buzzerOff();
}

/* Potwierdzenie wykrycia ladowarki - rosnaca tercja. */
void beepCharging() {
  buzzerInit();
  const uint16_t f[4] = {2200, 2600, 3000, 3400};
  for (int i = 0; i < 4; i++) { buzzerTone(f[i]); delay(90); }
  buzzerOff();
}

/* =====================================================================
 *  3.  GPIO / WYBUDZANIE
 * ===================================================================== */
void configureInputs() {
  pinMode(PIN_REED,   REED_MODE);
  pinMode(PIN_BUTTON, BUTTON_MODE);
}

bool boxIsOpen()      { return digitalRead(PIN_REED)   == REED_OPEN_LEVEL; }
bool buttonPressed()  { return digitalRead(PIN_BUTTON) == BUTTON_PRESS_LEVEL; }

const char* wakeName(WakeReason w) {
  switch (w) {
    case WAKE_BOOT:   return "reset/wlaczenie";
    case WAKE_REED:   return "KONTAKTRON (otwarto pudelko)";
    case WAKE_BUTTON: return "przycisk";
    case WAKE_TIMER:  return "timer";
    case WAKE_CLOSED: return "zamkniecie wieczka";
  }
  return "?";
}

WakeReason detectWakeReason() {
  esp_sleep_wakeup_cause_t c = esp_sleep_get_wakeup_cause();
  switch (c) {
    case ESP_SLEEP_WAKEUP_TIMER:
      return WAKE_TIMER;
    case ESP_SLEEP_WAKEUP_GPIO: {
      /* Pytamy uklad, KTORY pin nas obudzil, zamiast zgadywac z biezacego
         stanu wejsc.

         Poprzednia wersja odczytywala piny dopiero tutaj - a od wybudzenia
         do tego miejsca mija ponad sekunda (start rdzenia, Serial, pomiar
         baterii). Jesli otworzyles pudelko, wzialeś tabletke i zamknales
         je w dwie sekundy, kontaktron byl juz z powrotem w stanie
         spoczynkowym. Przycisk tez nie byl wcisniety, wiec kod wybieral
         ostatnia galaz: "krotkie tapniecie przycisku" - i zamiast zapisac
         dawke, pudelko wchodzilo na piec minut w tryb konfiguracji WiFi.
         Stad brak wpisu w aplikacji i brak ostrzegawczego pikniecia.

         Rejestr wybudzenia jest zatrzaskiwany sprzetowo w chwili zdarzenia,
         wiec nie ma znaczenia, jak szybko zamknales wieczko.            */
      uint64_t src = 0;
#if defined(SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP) && SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
      src = esp_sleep_get_gpio_wakeup_status();
#endif
      LOG("[WAKE] maska zrodel: 0x%08lX  (kontaktron=GPIO%d, przycisk=GPIO%d)\n",
          (unsigned long)src, GPIO_REED, GPIO_BUTTON);

      /* Ze snu budzi teraz TYLKO kontaktron (patrz komentarz w goToSleep),
         wiec kazde wybudzenie z GPIO oznacza ruch wieczka. Sprawdzamy
         maske dla porzadku i logu diagnostycznego.                     */
      if (src & (1ULL << GPIO_BUTTON))
        return WAKE_BUTTON;                 // nie powinno wystapic

      return rtcArmedForClose ? WAKE_CLOSED : WAKE_REED;
    }
    default:
      return WAKE_BOOT;                     // POWERON / RESET / USB
  }
}

/* =====================================================================
 *  4.  HARMONOGRAM
 * ===================================================================== */
void parseSchedule(const String& s) {
  slotCount = 0;
  int start = 0;
  while (start < (int)s.length() && slotCount < 12) {
    int sep = s.indexOf('|', start);
    if (sep < 0) sep = s.length();
    String item = s.substring(start, sep);
    item.trim();
    if (item.length() == 5 && item.charAt(2) == ':') slots[slotCount++] = item;
    start = sep + 1;
  }
}

void loadSchedule() {
  String s = String(rtcSchedule);
  if (s.length() < 5) {                     // zimny start -> NVS
    prefs.begin(NVS_NAMESPACE, true);
    s = prefs.getString("sched", DEFAULT_SCHEDULE);
    rtcTzOffsetMin = prefs.getShort("tz", DEFAULT_TZ_OFFSET);
    prefs.end();
    strncpy(rtcSchedule, s.c_str(), sizeof(rtcSchedule) - 1);
  }
  parseSchedule(s);
  LOG("[SCH] %s  (tz=%d min, %d slotow)\n", s.c_str(), rtcTzOffsetMin, slotCount);
}

void saveSchedule(const String& s, int16_t tz) {
  prefs.begin(NVS_NAMESPACE, false);
  nvsPutStr("sched", s);
  prefs.putShort("tz", tz);
  prefs.end();
  strncpy(rtcSchedule, s.c_str(), sizeof(rtcSchedule) - 1);
  rtcTzOffsetMin = tz;
  parseSchedule(s);
}

/* Czas lokalny w minutach od polnocy. */
int localMinutesOfDay(time_t utc) {
  time_t local = utc + (time_t)rtcTzOffsetMin * 60;
  struct tm tmv;
  gmtime_r(&local, &tmv);
  return tmv.tm_hour * 60 + tmv.tm_min;
}

int slotMinutes(int i) {
  return slots[i].substring(0, 2).toInt() * 60 + slots[i].substring(3, 5).toInt();
}

/* Dzien LEKOWY jako liczba YYYYMMDD - wygodne do porownan "czy juz dzis".
   To nie jest zwykla data kalendarzowa. Doba lekowa konczy sie dopiero
   o DAY_START_HOUR, wiec tabletka wzieta o 01:00 czy 02:00 nalezy jeszcze
   do dnia poprzedniego. Odejmujemy te godziny przed odczytaniem daty -
   caly kod porownujacy dni dostaje wtedy wlasciwa odpowiedz sam z siebie. */
uint32_t localDayNumber(time_t utc) {
  time_t local = utc + (time_t)rtcTzOffsetMin * 60 - (time_t)DAY_START_HOUR * 3600;
  struct tm tmv;
  gmtime_r(&local, &tmv);
  return (uint32_t)(tmv.tm_year + 1900) * 10000UL + (tmv.tm_mon + 1) * 100UL + tmv.tm_mday;
}

/* Ktory slot pasuje do podanego momentu (+/- MATCH_WINDOW_MIN)? -1 = zaden. */
int matchSlot(time_t utc) {
  if (!rtcTimeValid || slotCount == 0) return -1;
  int now = localMinutesOfDay(utc);
  int best = -1, bestDiff = 100000;
  for (int i = 0; i < slotCount; i++) {
    int d = abs(now - slotMinutes(i));
    if (d > 720) d = 1440 - d;              // przez polnoc
    if (d <= MATCH_WINDOW_MIN && d < bestDiff) { bestDiff = d; best = i; }
  }
  return best;
}

/* Sekundy do minuty po najblizszym koncu doby LEKOWEJ (czyli po
   DAY_START_HOUR, nie po polnocy). Wczesniejsze budzenie nie mialoby sensu:
   o 00:01 wciaz trwa wczorajsza doba i nie ma jeszcze czego rozliczac.   */
uint32_t secondsToDayBoundary(time_t utc) {
  if (!rtcTimeValid) return HOUSEKEEP_MAX_S;
  time_t shifted = utc + (time_t)rtcTzOffsetMin * 60 - (time_t)DAY_START_HOUR * 3600;
  uint32_t secOfDay = (uint32_t)(shifted % 86400);
  return 86400UL - secOfDay + 60UL;
}

/* =====================================================================
 *  4a.  DNI BEZ LEKU
 *
 *  Dawka jest nadal JEDNA dziennie (ONE_DOSE_PER_DAY) - zmienna jest tylko
 *  liczba tabletek w tej jednej dawce. Pudelko potrzebuje z tego dokladnie
 *  jednej informacji: czy dzis przypada ZERO, bo wtedy nie ma o czym
 *  przypominac.
 *
 *  ZASADA, ta sama co po stronie aplikacji: rozpisanie NIEPELNE odrzucamy
 *  w calosci. Brakujacy dzien odczytany jako zero to cichy dzien bez leku
 *  przeciwzakrzepowego - a to jest dokladnie ten rodzaj bledu, ktorego to
 *  urzadzenie ma nie popelniac. Nie wiemy = dzwonimy.
 * ===================================================================== */

/* Dzien tygodnia doby LEKOWEJ: 0 = niedziela.
   To `tm_wday`, czyli ta sama konwencja co `Date.getDay()` w aplikacji -
   dzieki temu oba konce trafiaja w ten sam dzien bez przeliczania.
   Przesuniecie o DAY_START_HOUR jak w localDayNumber(): tabletka wzieta
   o 01:00 nalezy jeszcze do dnia poprzedniego, takze przy liczeniu
   dnia tygodnia.                                                        */
int localWeekday(time_t utc) {
  time_t local = utc + (time_t)rtcTzOffsetMin * 60 - (time_t)DAY_START_HOUR * 3600;
  struct tm tmv;
  gmtime_r(&local, &tmv);
  return tmv.tm_wday;
}

/* "2026-08-14" -> 20260814. Zero, gdy to nie jest data. */
uint32_t dateKeyToNum(const char* k) {
  if (!k) return 0;
  int len = strlen(k);
  if (len != 10 || k[4] != '-' || k[7] != '-') return 0;
  for (int i = 0; i < len; i++)
    if (i != 4 && i != 7 && (k[i] < '0' || k[i] > '9')) return 0;
  return (uint32_t)atol(String(k).substring(0, 4).c_str()) * 10000UL
       + (uint32_t)atol(String(k).substring(5, 7).c_str()) * 100UL
       + (uint32_t)atol(String(k).substring(8, 10).c_str());
}

/* Ile tabletek (w dziesiatych) przypada na te dobe lekowa.
   DOSE_NIEZNANA znaczy "nie wiem" - nigdy "zero".                       */
uint8_t dawkaNaDobe(uint32_t dayNum, int wday) {
  for (uint8_t i = 0; i < rtcDoseExCount && i < DOSE_EX_MAX; i++)
    if (rtcDoseExDay[i] == dayNum) return rtcDoseExVal[i];
  if (wday < 0 || wday > 6) return DOSE_NIEZNANA;
  return rtcDoseWeek[wday];
}

/* Czy dzisiejsza doba lekowa jest rozpisana BEZ leku.
   Zwraca true WYLACZNIE przy pewnym zerze: bez zegara nie wiemy, ktory jest
   dzien, wiec dzwonimy normalnie.                                        */
bool dzisBezLeku() {
  if (!rtcTimeValid) return false;
  time_t now = time(nullptr);
  return dawkaNaDobe(localDayNumber(now), localWeekday(now)) == 0;
}

/* Rozpisanie tygodniowe z postaci "255|10|10|5|10|10|15" (dziesiate czesci
   tabletki, 255 = nieznana). Cokolwiek innego niz SIEDEM poprawnych liczb
   kasuje rozpisanie w calosci.                                           */
void parseDoseWeek(const String& s) {
  uint8_t tmp[7];
  int n = 0, start = 0;
  while (n < 7 && start <= (int)s.length()) {
    int sep = s.indexOf('|', start);
    if (sep < 0) sep = s.length();
    String it = s.substring(start, sep);
    it.trim();
    if (!it.length()) break;
    long v = it.toInt();
    tmp[n++] = (v >= 0 && v <= 100) ? (uint8_t)v : DOSE_NIEZNANA;
    if (sep >= (int)s.length()) break;
    start = sep + 1;
  }
  for (int i = 0; i < 7; i++) rtcDoseWeek[i] = (n == 7) ? tmp[i] : DOSE_NIEZNANA;
}

/* Wyjatki z postaci "20260814:0|20260815:5". */
void parseDoseEx(const String& s) {
  rtcDoseExCount = 0;
  int start = 0;
  while (start < (int)s.length() && rtcDoseExCount < DOSE_EX_MAX) {
    int sep = s.indexOf('|', start);
    if (sep < 0) sep = s.length();
    String it = s.substring(start, sep);
    int dwukropek = it.indexOf(':');
    if (dwukropek > 0) {
      uint32_t d = (uint32_t)atol(it.substring(0, dwukropek).c_str());
      long v = it.substring(dwukropek + 1).toInt();
      if (d >= 20000101UL && v >= 0 && v <= 100) {
        rtcDoseExDay[rtcDoseExCount] = d;
        rtcDoseExVal[rtcDoseExCount] = (uint8_t)v;
        rtcDoseExCount++;
      }
    }
    if (sep >= (int)s.length()) break;
    start = sep + 1;
  }
}

void saveDosing(const String& tydzien, const String& wyjatki) {
  prefs.begin(NVS_NAMESPACE, false);
  nvsPutStr("dw",  tydzien);
  nvsPutStr("dex", wyjatki);
  prefs.end();
  parseDoseWeek(tydzien);
  parseDoseEx(wyjatki);
  rtcDosingLoaded = true;
}

/* Po twardym restarcie pamiec RTC jest wyzerowana, a rozpisanie musi
   przetrwac - inaczej pierwsze wybudzenie po resecie zadzwoniloby w dniu
   odstawienia. Dlatego to samo, co robi loadSchedule() dla godzin.      */
void loadDosing() {
  if (rtcDosingLoaded) return;
  prefs.begin(NVS_NAMESPACE, true);
  String w = prefs.getString("dw",  "");
  String e = prefs.getString("dex", "");
  prefs.end();
  parseDoseWeek(w);
  parseDoseEx(e);
  rtcDosingLoaded = true;
  LOG("[DOS] rozpisanie: %s  wyjatki: %u\n",
      w.length() ? w.c_str() : "(brak)", (unsigned)rtcDoseExCount);
}

/* =====================================================================
 *  4b.  PUDELKO ZOSTAWIONE OTWARTE
 *
 *  Zamiast czuwac przez kwadrans (co zjadaloby bateria), pudelko normalnie
 *  spi i tylko przycina dlugosc snu tak, zeby obudzic sie dokladnie na
 *  moment sygnalu. Jedno wybudzenie to ulamek sekundy pracy procesora,
 *  wiec pilnowanie otwartego wieczka przez cala noc kosztuje mniej niz
 *  jedno polaczenie z WiFi.
 * ===================================================================== */

/* Ile sekund zostalo do nastepnego sygnalu ostrzegawczego. */
uint32_t openWarnSecondsLeft() {
  if (rtcOpenSinceTs == 0) return OPEN_WARN_FIRST_S;      // dopiero otwarto
  if (rtcTimeValid && rtcNextWarnTs) {
    uint32_t now = (uint32_t)time(nullptr);
    return now >= rtcNextWarnTs ? 5 : (rtcNextWarnTs - now);
  }
  /* Bez wiarygodnego zegara odmierzamy dlugoscia samego snu. */
  return rtcOpenWarnCount == 0 ? OPEN_WARN_FIRST_S : OPEN_WARN_REPEAT_S;
}

/* =====================================================================
 *  4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego
 *
 *  Zapisuje KAZDA zmiane stanu kontaktronu: kiedy wieczko sie otworzylo,
 *  kiedy zamknelo. Po co: nie wiemy jeszcze, czy pudelko w plecaku melduje
 *  otwarcia, ktorych nikt nie zrobil. Bez pomiaru mozemy tylko zgadywac,
 *  a zgadywanie w tym projekcie juz raz kosztowalo godziny.
 *
 *  DLACZEGO OSOBNY BUFOR, A NIE ZWYKLA KOLEJKA:
 *  queuePush() przy zapelnieniu NADPISUJE NAJSTARSZY wpis (patrz linia z
 *  "pelno -> nadpisz najstarszy"). Wieczko trzesace sie w plecaku
 *  wygenerowaloby setki zdarzen i po cichu wyrzucilo z kolejki zapisy
 *  DAWEK. Dziennik wieczka nie moze zaszkodzic danym o leku, wiec ma
 *  wlasne miejsce i wlasny limit.
 *
 *  Po zapelnieniu NIE nadpisujemy - zostawiamy najstarsze wpisy i tylko
 *  liczymy, ile przepadlo. Do diagnozy wazniejszy jest POCZATEK zjawiska
 *  ("od ktorej godziny zaczelo swirowac") niz jego koniec, a sam licznik
 *  strat mowi o skali: "64 zapisane + 900 zgubionych" to zupelnie inna
 *  historia niz "6 zapisanych".
 *
 *  Cale to rozwiazanie jest TYMCZASOWE - do wyjecia, gdy juz bedziemy
 *  wiedziec, czy problem istnieje.
 * ===================================================================== */
/* Wartosc awaryjna, gdy config.h jest starszy niz ta funkcja. Blok jest
   wyciagany do testow razem z kodem, zeby testy sprawdzaly DOKLADNIE ten
   sam limit, ktory zadziala na plytce.                                  */
/* @extract-begin */
#ifndef LIDLOG_SLOTS
  #define LIDLOG_SLOTS 64
#endif
/* @extract-end */

void lidLogAdd(bool otwarte) {
  prefs.begin(NVS_NAMESPACE, false);
  uint16_t cnt = prefs.getUShort("llCnt", 0);
  if (cnt < LIDLOG_SLOTS) {
    char klucz[8];
    snprintf(klucz, sizeof(klucz), "ll%u", cnt);
    /* Format: czas;stan;powod_wybudzenia   (czas 0 = zegar jeszcze nieznany) */
    char linia[40];
    snprintf(linia, sizeof(linia), "%lu;%u;%s",
             (unsigned long)(rtcTimeValid ? time(nullptr) : 0),
             otwarte ? 1u : 0u, wakeName(wakeReason));
    /* Licznik do gory tylko wtedy, gdy tresc naprawde weszla - inaczej
       w dzienniku siedzialby pusty slot udajacy zapis.               */
    if (nvsPutStr(klucz, linia)) nvsPutU16("llCnt", cnt + 1);
    LOG("[LID] %s (wpis %u/%d)\n", otwarte ? "OTWARTE" : "zamkniete",
        cnt + 1, LIDLOG_SLOTS);
  } else {
    uint16_t zgubione = prefs.getUShort("llLost", 0);
    if (zgubione < 65000) nvsPutU16("llLost", zgubione + 1);
    LOG("[LID] dziennik pelny - zgubionych %u\n", zgubione + 1);
  }
  prefs.end();
}

uint16_t lidLogCount() {
  prefs.begin(NVS_NAMESPACE, true);
  uint16_t c = prefs.getUShort("llCnt", 0);
  prefs.end();
  return c;
}

/* Ucieczka znakow specjalnych w JSON. Stoi tutaj, a nie przy czarnej
   skrzynce nizej, bo lidLogJson() jest jej PIERWSZYM uzyciem - a w tym
   pliku obowiazuje zasada "definicja przed uzyciem" (pilnuje jej audyt). */
static String jsonEscape(const String& in) {
  String o;
  for (int i = 0; i < in.length(); i++) {
    char c = in.charAt(i);
    if      (c == '"')  o += "\\\"";
    else if (c == '\\') o += "\\\\";
    else if (c == '\n') o += "\\n";
    else if (c == '\r') o += "\\r";
    else if (c == '\t') o += "\\t";
    else if ((unsigned char)c < 0x20) continue;   // znaki sterujace pomijamy
    else o += c;
  }
  return o;
}

/* Dziennik jako JSON do wyslania. {"zgubione":N,"wpisy":["ts;stan;powod",...]} */
String lidLogJson() {
  prefs.begin(NVS_NAMESPACE, true);
  uint16_t cnt = prefs.getUShort("llCnt", 0);
  uint16_t zgubione = prefs.getUShort("llLost", 0);
  String out = "{\"zgubione\":";
  out += zgubione;
  out += ",\"wpisy\":[";
  bool pierwszy = true;
  for (uint16_t i = 0; i < cnt; i++) {
    char klucz[8];
    snprintf(klucz, sizeof(klucz), "ll%u", i);
    String l = prefs.getString(klucz, "");
    if (!l.length()) continue;
    if (!pierwszy) out += ",";
    pierwszy = false;
    /* Escapujemy tak samo jak logbookJson(). Tresc lepi dzis snprintf,
       wiec cudzyslow sie w niej nie znajdzie - ale blizniacza funkcja
       obok jest zabezpieczona, a niesymetryczna obrona w module, ktory
       wedlug zasady 8 nie moze uszkodzic danych o leku, to proszenie
       sie o klopoty przy pierwszej zmianie formatu.                  */
    out += "\"" + jsonEscape(l) + "\"";
  }
  out += "]}";
  prefs.end();
  return out;
}

/* Kasujemy DOPIERO po potwierdzonym zapisie w bazie - ta sama zasada, co
   przy pushStatus(). Wyczyszczenie na wiare oznaczaloby, ze nieudana
   wysylka po cichu niszczy jedyna kopie pomiaru.                        */
void lidLogClear() {
  prefs.begin(NVS_NAMESPACE, false);
  uint16_t cnt = prefs.getUShort("llCnt", 0);
  for (uint16_t i = 0; i < cnt; i++) {
    char klucz[8];
    snprintf(klucz, sizeof(klucz), "ll%u", i);
    prefs.remove(klucz);
  }
  nvsPutU16("llCnt", 0);
  nvsPutU16("llLost", 0);
  prefs.end();
  LOG("[LID] dziennik wyslany i skasowany (%u wpisow)\n", cnt);
}

/* Czy sa jakies NIEPOTWIERDZONE wpisy dziennika nieudanych zapisow. */
bool nvsFailLogDoWyslania() {
  return rtcNvsFailLogSent != rtcNvsFailLogTotal;
}

/* JSON tylko z wpisow, ktore jeszcze nie doszly do bazy.

   Pierscien ma NVS_FAILLOG_SLOTS miejsc - jesli miedzy polaczeniami
   zdarzy sie ich wiecej, najstarsze juz zostaly nadpisane. Wysylamy wiec
   przeciecie: to, co jeszcze niewyslane, ORAZ to, co wciaz jest w
   pierscieniu - a nie probujemy zrekonstruowac tego, co juz przepadlo.  */
String nvsFailLogJson() {
  uint16_t od = rtcNvsFailLogSent;
  if (rtcNvsFailLogTotal - od > NVS_FAILLOG_SLOTS)
    od = rtcNvsFailLogTotal - NVS_FAILLOG_SLOTS;

  String out = "{\"wpisy\":[";
  bool pierwszy = true;
  for (uint16_t i = od; i != rtcNvsFailLogTotal; i++) {
    uint16_t slot = i % NVS_FAILLOG_SLOTS;
    if (!pierwszy) out += ",";
    pierwszy = false;
    out += "{\"ts\":" + String(rtcNvsFailLogTs[slot]) +
           ",\"klucz\":\"" + jsonEscape(String(rtcNvsFailLogKey[slot])) + "\"}";
  }
  out += "]}";
  return out;
}

void nvsFailLogOznaczWyslany() { rtcNvsFailLogSent = rtcNvsFailLogTotal; }

/* Wywolywane przy KAZDYM wybudzeniu, przed reszta logiki.
   Zwraca true, jesli wlasnie zabrzmial sygnal "zostawiles mnie otwarte". */
bool trackBoxOpen() {
  uint32_t now = rtcTimeValid ? (uint32_t)time(nullptr) : 0;

  /* --- wieczko zamkniete: kasujemy caly stan alarmowy --- */
  if (!boxIsOpen()) {
    if (rtcOpenSinceTs || rtcOpenWarnCount) {
      LOG("[OPN] pudelko zamkniete (bylo otwarte, %u sygnalow)\n", rtcOpenWarnCount);
      if (rtcOpenReported) rtcOpenClearPend = true;   // aplikacja czeka na odwolanie
      lidLogAdd(false);                               // przejscie otwarte -> zamkniete
    }
    rtcOpenSinceTs = 0; rtcNextWarnTs = 0; rtcOpenWarnCount = 0;
    rtcOpenReported = false;
    return false;
  }

  /* --- pierwsze wykrycie otwarcia: tylko zapisujemy moment --- */
  if (rtcOpenSinceTs == 0) {
    rtcOpenSinceTs = now ? now : 1;                   // 1 = "otwarte, czas nieznany"
    rtcNextWarnTs  = now ? now + OPEN_WARN_FIRST_S : 0;
    LOG("[OPN] pudelko otwarte - przypomne za %d min\n", OPEN_WARN_FIRST_S / 60);
    lidLogAdd(true);                                  // przejscie zamkniete -> otwarte
    return false;
  }

  /* --- czy wypada juz sygnal? --- */
  bool due = (rtcTimeValid && rtcNextWarnTs)
               ? (now >= rtcNextWarnTs)
               : (wakeReason == WAKE_TIMER);   // sen byl przyciety do interwalu
  if (!due) return false;

  rtcOpenWarnCount++;
  rtcNextWarnTs = now ? now + OPEN_WARN_REPEAT_S : 0;

  /* Po OPEN_WARN_MAX sygnalach milkniemy. Jesli pudelko jest otwarte od
     kilkunastu godzin, to prawdopodobnie nie zapomnialem wieczka, tylko
     magnes sie przesunal - dalsze pikanie niczego nie naprawi, a bateria
     jest potrzebniejsza na przypomnienia o leku.                        */
  if (rtcOpenWarnCount > OPEN_WARN_MAX) {
    LOG("[OPN] otwarte od bardzo dawna (%u) - dalej pilnuje, ale juz cicho\n",
        rtcOpenWarnCount);
    return false;
  }

  LOG("[OPN] nadal otwarte - sygnal %u z %d\n", rtcOpenWarnCount, OPEN_WARN_MAX);
  beepBoxOpen();
  return true;
}

/* Sekundy do najblizszego slotu w przyszlosci. */
uint32_t secondsToNextSlot(time_t utc) {
  if (!rtcTimeValid || slotCount == 0) return HOUSEKEEP_MAX_S;
  int now = localMinutesOfDay(utc);
  time_t local = utc + (time_t)rtcTzOffsetMin * 60;
  int sec = local % 60;
  uint32_t best = 24UL * 3600UL;
  for (int i = 0; i < slotCount; i++) {
    int32_t d = slotMinutes(i) - now;
    if (d <= 0) d += 1440;
    uint32_t s = (uint32_t)d * 60UL - sec;
    if (s < best) best = s;
  }
  return best;
}

/* =====================================================================
 *  5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)
 *      Rekord: "ts;type;batt;volt;slot"
 * ===================================================================== */
bool queuePush(const String& rec) {
  prefs.begin(NVS_NAMESPACE, false);
  uint16_t head  = prefs.getUShort("qh", 0);
  uint16_t count = prefs.getUShort("qc", 0);
  char key[8];
  snprintf(key, sizeof(key), "q%u", (unsigned)((head + count) % QUEUE_CAPACITY));
  /* Licznik podnosimy TYLKO wtedy, gdy tresc naprawde weszla. Inaczej w
     kolejce siedzialby wpis-widmo: queuePeek() nie ma czego odczytac,
     wysylka staje w miejscu, a licznik nigdy nie schodzi do zera.     */
  const bool ok = nvsPutStr(key, rec);
  if (ok) {
    if (count < QUEUE_CAPACITY) {
      nvsPutU16("qc", count + 1);
    } else {                                // pelno -> nadpisz najstarszy
      nvsPutU16("qh", (head + 1) % QUEUE_CAPACITY);
    }
  }
  prefs.end();
  if (ok) LOG("[QUE] zapisano offline: %s (w kolejce %u)\n", rec.c_str(), count + 1);
  else    LOG("[QUE] NIE UDALO SIE zapisac: %s\n", rec.c_str());
  return ok;
}

uint16_t queueCount() {
  prefs.begin(NVS_NAMESPACE, true);
  uint16_t c = prefs.getUShort("qc", 0);
  prefs.end();
  return c;
}

bool queuePeek(String& out) {
  prefs.begin(NVS_NAMESPACE, true);
  uint16_t head = prefs.getUShort("qh", 0);
  uint16_t count = prefs.getUShort("qc", 0);
  bool ok = false;
  if (count > 0) {
    char key[8];
    snprintf(key, sizeof(key), "q%u", (unsigned)head);
    out = prefs.getString(key, "");
    ok = out.length() > 0;
  }
  prefs.end();
  return ok;
}

void queuePop() {
  prefs.begin(NVS_NAMESPACE, false);
  uint16_t head = prefs.getUShort("qh", 0);
  uint16_t count = prefs.getUShort("qc", 0);
  if (count > 0) {
    nvsPutU16("qh", (head + 1) % QUEUE_CAPACITY);
    nvsPutU16("qc", count - 1);
  }
  prefs.end();
}

/* Zdejmuje wpis, ktorego NIGDY nie da sie wyslac.

   Swiadomie lamie zasade "nic nie kasujemy przed potwierdzonym 2xx".
   Zasada chroni przed utrata danych przy chwilowej awarii - a tu chodzi
   o wpis, ktorego baza nie przyjmie ani teraz, ani za tydzien. Zostawiony
   na czele blokuje WSZYSTKO za soba: dawki pietrza sie, az pierscien 120
   wpisow zacznie nadpisywac najstarsze. Tracimy jeden wpis zamiast calej
   kolejki - i tracimy go GLOSNO, bo licznik jedzie do statusu.        */
void queueDrop() {
  queuePop();
  if (rtcQueueDropped < 65535) rtcQueueDropped++;
}

/* --- Korekta dryfu zegara po dlugim okresie offline ---------------------
   W deep sleep czas liczy wewnetrzny oscylator RC, ktory po kilku dniach
   potrafi uciec o kilkadziesiat minut. Gdy po powrocie sieci NTP pokaze
   inny czas, przesuwamy o te sama roznice wszystkie zdarzenia czekajace
   w kolejce - inaczej trafilyby w bazie na zla godzine (a przy zdarzeniu
   tuz przed polnoca nawet na zly dzien).
   Korekta jest plaska, wiec dla najstarszych wpisow lekko przestrzelona,
   ale blad liczony jest w minutach, a nie godzinach.                     */
/* Korekta zegara dotyczy WSZYSTKICH znacznikow, nie tylko kolejki.

   TU BYL BLAD tej samej rodziny co token. queueShiftTimestamps() poprawialo
   zdarzenia czekajace na wyslanie - i tylko je. Ale na tym samym zegarze
   opiera sie jeszcze piec znacznikow w pamieci RTC. Najgrozniejszy jest
   rtcNextWarnTs: gdyby zegar przeskoczyl o dwie godziny do przodu, termin
   nastepnego sygnalu o otwartym wieczku wyladowalby w przeszlosci i pudelko
   zaczeloby pikac bez powodu. Skok do tylu dawalby odwrotnie - sygnal nie
   odezwalby sie wcale.

   Osobno kazdy z tych kawalkow byl poprawny.                            */
void przesunZnaczniki(int32_t delta) {
  auto przesun = [delta](uint32_t& t) {
    if (t == 0) return;                       // 0 znaczy "nie ustawiono"
    int64_t v = (int64_t)t + delta;
    t = v < 0 ? 0 : (uint32_t)v;
  };
  przesun(rtcLastOpenTs);
  przesun(rtcOpenSinceTs);
  przesun(rtcNextWarnTs);
  przesun(rtcLastPushTs);
  przesun(rtcTokenExp);
  przesun(rtcChargeSinceTs);
  LOG("[CLK] znaczniki przesuniete o %ld s\n", (long)delta);
}

void queueShiftTimestamps(int32_t delta) {
  if (delta == 0 || delta > 21600 || delta < -21600) return;   // max +/- 6 h

  prefs.begin(NVS_NAMESPACE, false);
  uint16_t head  = prefs.getUShort("qh", 0);
  uint16_t count = prefs.getUShort("qc", 0);
  int fixed = 0;
  for (uint16_t i = 0; i < count; i++) {
    char key[8];
    snprintf(key, sizeof(key), "q%u", (unsigned)((head + i) % QUEUE_CAPACITY));
    String rec = prefs.getString(key, "");
    int p = rec.indexOf(';');
    if (p <= 0) continue;
    uint32_t ts = (uint32_t)rec.substring(0, p).toInt();
    if (ts < 1700000000UL) continue;                 // brak czasu - nie ruszamy
    nvsPutStr(key, String((unsigned long)(ts + delta)) + rec.substring(p));
    fixed++;
  }
  prefs.end();
  if (fixed) LOG("[QUE] skorygowano czas %d zdarzen o %ld s\n", fixed, (long)delta);
}

/* =====================================================================
 *  6.  WiFi
 *
 *  LISTA SIECI, NIE JEDNA SIEC
 *  Wczesniej bylo tu samo `WiFi.begin()` bez argumentow - czyli jedna siec
 *  zapamietana przez sterownik, do zmiany wylacznie portalem przy otwartym
 *  pudelku. Teraz pamietamy do WIFI_SIECI_MAX sieci, dzieki czemu siec da
 *  sie DODAC Z APLIKACJI: na liscie stoi hotspot z telefonu, wiec na
 *  wyjezdzie wystarczy go wlaczyc, zeby pudelko odebralo siec hotelowa.
 *
 *  Kolejnosc prob zaczyna sie od tej, ktora zadzialala ostatnio, wiec
 *  w domu to nadal jedna proba i ani milisekundy radia wiecej.
 * ===================================================================== */

/* Klucze w NVS: "n0s"/"n0p", "n1s"/"n1p", ... Osobne klucze, a nie jeden
   sklejony napis, bo haslo do WiFi moze zawierac DOWOLNY znak - lacznie
   z tym, ktorego uzylibysmy jako separatora.                            */
static void netKlucz(char* buf, size_t n, int i, char rodzaj) {
  snprintf(buf, n, "n%d%c", i, rodzaj);
}

int wifiSieciCount() {
  prefs.begin(NVS_NAMESPACE, true);
  int n = prefs.getUChar("netN", 0);
  prefs.end();
  return (n > WIFI_SIECI_MAX) ? WIFI_SIECI_MAX : n;
}

String wifiSiecSsid(int i) {
  if (i < 0 || i >= WIFI_SIECI_MAX) return "";
  char k[8]; netKlucz(k, sizeof(k), i, 's');
  prefs.begin(NVS_NAMESPACE, true);
  String s = prefs.getString(k, "");
  prefs.end();
  return s;
}

static String wifiSiecPass(int i) {
  if (i < 0 || i >= WIFI_SIECI_MAX) return "";
  char k[8]; netKlucz(k, sizeof(k), i, 'p');
  prefs.begin(NVS_NAMESPACE, true);
  String s = prefs.getString(k, "");
  prefs.end();
  return s;
}

/* Dopisuje siec na POCZATEK listy: ostatnio dodana jest najpewniej ta,
   ktorej wlasnie potrzebujemy. Siec o tej samej nazwie zastepuje starszy
   wpis zamiast go dublowac - inaczej zmiana hasla do domowego WiFi
   zajmowalaby dwa miejsca z czterech.

   Zwraca false, gdy zapis do pamieci trwalej sie nie udal. Wolajacy MUSI
   to sprawdzic przed skasowaniem hasla z bazy (zasada 6).              */
/* Zapisuje CALA liste naraz. Wydzielone, bo dotykaja jej trzy operacje
   (dodanie, usuniecie, zmiana kolejnosci) i kazda wlasna kopia tego kodu
   bylaby trzecia okazja do pomylki w tym samym miejscu.

   Klucze POWYZEJ nowej dlugosci kasujemy, zeby ODZYSKAC MIEJSCE W NVS.
   Odczytu to nie zmienia - `netN` i tak ogranicza petle, a kazdy zapis
   nadpisuje wszystkie czytane klucze, wiec skasowana siec nie wrocilaby
   tak czy inaczej. Ale cala pamiec trwala pudelka to jedna partycja ~20 kB
   na kolejke, dziennik wieczka, czarna skrzynke i token (D25), a haslo do
   WiFi potrafi miec 63 znaki. Zostawianie w niej martwych wpisow to
   oddawanie miejsca, ktorego gdzie indziej brakuje.                    */
static bool wifiListeZapisz(const String* ss, const String* pp, int n) {
  bool ok = true;
  prefs.begin(NVS_NAMESPACE, false);
  for (int i = 0; i < n; i++) {
    char ks[8], kp[8];
    netKlucz(ks, sizeof(ks), i, 's');
    netKlucz(kp, sizeof(kp), i, 'p');
    if (!nvsPutStr(ks, ss[i])) ok = false;
    if (!nvsPutStr(kp, pp[i])) ok = false;
  }
  for (int i = n; i < WIFI_SIECI_MAX; i++) {
    char ks[8], kp[8];
    netKlucz(ks, sizeof(ks), i, 's');
    netKlucz(kp, sizeof(kp), i, 'p');
    prefs.remove(ks);
    prefs.remove(kp);
  }
  if (!prefs.putUChar("netN", (uint8_t)n)) ok = false;
  prefs.end();
  return ok;
}

/* Czyta cala liste do tablic. Osobno od zapisu, bo prefs.begin() nie moze
   sie zagniezdzac (B22).                                               */
static int wifiListeCzytaj(String* ss, String* pp) {
  int n = wifiSieciCount();
  for (int i = 0; i < n; i++) { ss[i] = wifiSiecSsid(i); pp[i] = wifiSiecPass(i); }
  return n;
}

/* Dopisuje siec na POCZATEK listy: ostatnio dodana jest najpewniej ta,
   ktorej wlasnie potrzebujemy. Siec o tej samej nazwie zastepuje starszy
   wpis zamiast go dublowac - inaczej zmiana hasla do domowego WiFi
   zajmowalaby dwa miejsca z czterech.

   Zwraca false, gdy zapis do pamieci trwalej sie nie udal. Wolajacy MUSI
   to sprawdzic przed skasowaniem hasla z bazy (zasada 6).              */
bool wifiSiecDodaj(const String& ssid, const String& pass) {
  if (!ssid.length() || ssid.length() > 32) return false;

  String ss[WIFI_SIECI_MAX], pp[WIFI_SIECI_MAX];
  int stare = wifiListeCzytaj(ss, pp);

  String nowe_s[WIFI_SIECI_MAX], nowe_p[WIFI_SIECI_MAX];
  nowe_s[0] = ssid; nowe_p[0] = pass;
  int n = 1;
  for (int i = 0; i < stare && n < WIFI_SIECI_MAX; i++) {
    if (ss[i] == ssid || !ss[i].length()) continue;      // duplikat albo pusty
    nowe_s[n] = ss[i]; nowe_p[n] = pp[i]; n++;
  }

  bool ok = wifiListeZapisz(nowe_s, nowe_p, n);
  rtcNetOstatnia = 0;                       // nowa siec idzie na pierwszy ogien
  LOG("[NET] siec '%s' na liscie (%d z %d)%s\n", ssid.c_str(), n, WIFI_SIECI_MAX,
      ok ? "" : "  UWAGA: zapis do pamieci NIEUDANY");
  return ok;
}

/* Usuwa siec o podanej nazwie.

   ZABEZPIECZENIE, ktorego nie wolno zdejmowac: nie pozwalamy usunac sieci,
   przez ktora pudelko jest WLASNIE polaczone, jesli jest OSTATNIA na liscie.
   Zostalyby wtedy tylko poswiadczenia sterownika - a te wskazuja dokladnie
   te sama siec, wiec efekt bylby zerowy albo, po ich nadpisaniu, katastro-
   falny: urzadzenie bez zadnej drogi powrotu poza portalem fizycznym.

   Usuniecie sieci, przez ktora akurat NIE jestesmy polaczeni, albo takiej,
   po ktorej zostaja inne - jest bezpieczne i przechodzi.               */
int wifiSiecUsun(const String& ssid) {
  if (!ssid.length()) return WIFI_USUN_BRAK;

  String ss[WIFI_SIECI_MAX], pp[WIFI_SIECI_MAX];
  int stare = wifiListeCzytaj(ss, pp);

  int znaleziona = -1;
  for (int i = 0; i < stare; i++) if (ss[i] == ssid) { znaleziona = i; break; }
  if (znaleziona < 0) return WIFI_USUN_BRAK;

  if (stare == 1 && WiFi.status() == WL_CONNECTED && WiFi.SSID() == ssid)
    return WIFI_USUN_OSTATNIA;

  String nowe_s[WIFI_SIECI_MAX], nowe_p[WIFI_SIECI_MAX];
  int n = 0;
  for (int i = 0; i < stare; i++) {
    if (i == znaleziona) continue;
    nowe_s[n] = ss[i]; nowe_p[n] = pp[i]; n++;
  }

  bool ok = wifiListeZapisz(nowe_s, nowe_p, n);
  rtcNetOstatnia = 0;                       // indeksy sie przesunely
  LOG("[NET] siec '%s' usunieta, zostaje %d%s\n", ssid.c_str(), n,
      ok ? "" : "  UWAGA: zapis do pamieci NIEUDANY");
  return ok ? WIFI_USUN_OK : WIFI_USUN_BLAD;
}

/* Przesuwa siec na poczatek listy - pudelko sprobuje jej PIERWSZEJ.

   To jest "przelacz na te siec" w wersji, ktora nie moze urwac lacznosci:
   nie rozlaczamy niczego tu i teraz, tylko zmieniamy kolejnosc prob. Gdyby
   nowa siec okazala sie nieosiagalna, pudelko zejdzie po liscie do starej
   i nic sie nie stanie.                                                */
bool wifiSiecPriorytet(const String& ssid) {
  if (!ssid.length()) return false;

  String ss[WIFI_SIECI_MAX], pp[WIFI_SIECI_MAX];
  int stare = wifiListeCzytaj(ss, pp);

  int znaleziona = -1;
  for (int i = 0; i < stare; i++) if (ss[i] == ssid) { znaleziona = i; break; }
  if (znaleziona < 0) return false;
  if (znaleziona == 0) { rtcNetOstatnia = 0; return true; }   // juz pierwsza

  String nowe_s[WIFI_SIECI_MAX], nowe_p[WIFI_SIECI_MAX];
  nowe_s[0] = ss[znaleziona]; nowe_p[0] = pp[znaleziona];
  int n = 1;
  for (int i = 0; i < stare; i++) {
    if (i == znaleziona) continue;
    nowe_s[n] = ss[i]; nowe_p[n] = pp[i]; n++;
  }

  bool ok = wifiListeZapisz(nowe_s, nowe_p, n);
  rtcNetOstatnia = 0;
  LOG("[NET] siec '%s' na czele listy%s\n", ssid.c_str(),
      ok ? "" : "  UWAGA: zapis do pamieci NIEUDANY");
  return ok;
}

/* Jedna proba polaczenia. Pusty ssid = poswiadczenia zapamietane przez
   sterownik (tak dzialalo pudelko, zanim pojawila sie lista).          */
static bool wifiSprobuj(const String& ssid, const String& pass, uint32_t limitMs) {
  /* Rozlaczenie przed kazda proba. Bez tego druga i kolejne WiFi.begin()
     trafiaja w sterownik, ktory wciaz probuje poprzedniej sieci, i potrafia
     zostac po cichu zignorowane - a wtedy caly przeglad listy jest tylko
     czekaniem na to samo niepowodzenie.                                 */
  if (WiFi.status() != WL_IDLE_STATUS) {
    WiFi.disconnect(false, false);
    delay(80);
  }
  if (ssid.length()) WiFi.begin(ssid.c_str(), pass.c_str());
  else               WiFi.begin();

  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < limitMs && !awakeTooLong())
    delay(200);
  return WiFi.status() == WL_CONNECTED;
}

bool wifiConnect() {
  if (batterySaver) {
    LOGLN("[NET] tryb oszczedzania baterii - radio pozostaje wylaczone");
    return false;
  }
  if (WiFi.status() == WL_CONNECTED) return true;
  WiFi.persistent(true);
  WiFi.mode(WIFI_STA);
  /* Chwila na dojscie sterownika do siebie. Bez tego polaczenie tuz po
     uspieniu radia (WIFI_OFF) potrafi sie nie udac za pierwszym razem -
     a to wlasnie ta sciezka wysyla stan po zamknieciu wieczka.        */
  delay(60);
  WiFi.setSleep(true);                      // modem-sleep w trakcie pracy

  /* Czekamy najwyzej WIFI_TIMEOUT_MS na PIERWSZA probe. Chwilowa awaria
     routera nie moze zatrzymac urzadzenia na dluzej - zdarzenie laduje
     w kolejce, a planNextSleep() ustawia wczesniejsze wybudzenie.

     Kolejne sieci dostaja juz tylko WIFI_ALT_TIMEOUT_MS: sa tam na wypadek
     wyjazdu, a nie po to, zeby przy kazdej awarii routera pudelko trzymalo
     radio wlaczone przez minute.                                       */
  int n = wifiSieciCount();
  bool ok = false;

  for (int k = 0; k < n && !ok && !awakeTooLong(); k++) {
    int i = (rtcNetOstatnia + k) % n;
    String ssid = wifiSiecSsid(i);
    if (!ssid.length()) continue;
    ok = wifiSprobuj(ssid, wifiSiecPass(i), k == 0 ? WIFI_TIMEOUT_MS
                                                   : WIFI_ALT_TIMEOUT_MS);
    if (ok) rtcNetOstatnia = (uint8_t)i;
    else LOG("[NET] '%s' nie odpowiada\n", ssid.c_str());
  }

  /* OSTATNIA DESKA RATUNKU: poswiadczenia zapamietane przez sterownik.

     Tak dzialalo pudelko przez cale miesiace, zanim pojawila sie lista -
     i wlasnie dlatego ta sciezka MUSI zostac. Pierwsza wersja tej funkcji
     probowala jej wylacznie przy pustej liscie, czyli nowa funkcja odcinala
     jedyna dzialajaca droge powrotu: wystarczylo, ze lista zawierala jeden
     bledny wpis, a pudelko przestawalo sie laczyc CALKIEM - mimo poprawnych
     poswiadczen lezacych obok w tej samej pamieci. Objawialo sie to
     najgorzej jak mozna: aplikacja pokazywala stan zamrozony w chwili
     wgrania firmware i nic nie mowilo, ze cokolwiek jest nie tak.

     Zasada, ta sama co przy portalu fizycznym: nowa droga nie moze zabierac
     starej, dopoki nie udowodni, ze dziala.                             */
  if (!ok && !awakeTooLong()) {
    if (n) LOGLN("[NET] zadna siec z listy nie odpowiada - probuje zapamietanej");
    ok = wifiSprobuj("", "", n ? WIFI_ALT_TIMEOUT_MS : WIFI_TIMEOUT_MS);
    if (ok && n) LOGLN("[NET] polaczono poswiadczeniami sterownika, nie z listy");
  }

  ok = WiFi.status() == WL_CONNECTED;
  LOG("[NET] %s%s\n", ok ? "polaczono, IP=" : "BRAK POLACZENIA",
      ok ? WiFi.localIP().toString().c_str() : "");

  /* ZASADA: kazde udane polaczenie = natychmiastowe sprawdzenie godziny.
     Zegar RC dryfuje w deep sleepie, wiec nie ufamy mu ani chwili dluzej
     niz to konieczne. Koszt: jeden pakiet UDP.                          */
  if (ok && !timeSyncedThisWake) {
    syncTimeNTP();
    timeSyncedThisWake = true;
  }
  return ok;
}

/* Twarde wylaczenie radia - tylko przed snem, bo esp_wifi_stop() zatrzymuje
   caly sterownik i powrot z tego stanu bywa zawodny.                    */
void wifiOff() {
  WiFi.disconnect(true, false);
  WiFi.mode(WIFI_OFF);
  esp_wifi_stop();
}

/* Uspienie radia na czas czuwania przy otwartym wieczku.

   Tu NIE wolno uzyc wifiOff(): esp_wifi_stop() ubija sterownik, a chwile
   pozniej - gdy zamkniesz wieczko - trzeba sie natychmiast polaczyc i
   wyslac stan. Po twardym zatrzymaniu to potrafi sie nie udac i wtedy
   aplikacja zostaje z banerem "pudelko otwarte" mimo zamknietego wieczka.
   Samo przelaczenie trybu na WIFI_OFF gasi nadajnik, a sterownik zostaje
   gotowy do pracy.                                                      */
void wifiUspij() {
  WiFi.disconnect(false, false);
  WiFi.mode(WIFI_OFF);
}

bool syncTimeNTP() {
  time_t before = time(nullptr);
  bool hadTime = rtcTimeValid && before > 1700000000;

  configTime(0, 0, "pool.ntp.org", "time.google.com", "time.cloudflare.com");
  uint32_t t0 = millis();
  while (time(nullptr) < 1700000000 && millis() - t0 < 8000 && !awakeTooLong()) delay(200);
  const bool bylZegar = hadTime;
  rtcTimeValid = time(nullptr) > 1700000000;
  LOG("[NTP] %s (epoch=%lu)\n", rtcTimeValid ? "OK" : "FAIL", (unsigned long)time(nullptr));

  /* Zegar wlasnie stal sie wiarygodny, a dawka zostala zapisana WCZESNIEJ,
     na slepo. Wiemy dokladnie, ile czasu temu to bylo (roznica na starej,
     nieprawdziwej skali) - i wiemy juz, ktora jest godzina. Wystarczy
     odjac jedno od drugiego, zeby dawka trafila na wlasciwa dobe.

     Bez tego pudelko po odzyskaniu sieci uznaloby, ze dawki dzis nie bylo,
     i przy nastepnym otwarciu zamilkloby dokladnie wtedy, gdy powinno
     ostrzec.                                                          */
  if (rtcTimeValid && !bylZegar && rtcTakenTs != 0 && before >= (time_t)rtcTakenTs) {
    uint32_t temu = (uint32_t)(before - (time_t)rtcTakenTs);
    if (temu < (uint32_t)ONE_DOSE_WINDOW_S) {
      time_t kiedy = time(nullptr) - (time_t)temu;
      rtcTakenTs = (uint32_t)kiedy;
      setTakenDay(localDayNumber(kiedy));
      LOG("[NTP] dawka sprzed %lu s przypisana do doby %lu\n",
          (unsigned long)temu, (unsigned long)rtcTakenDay);
    } else {
      rtcTakenTs = 0;                        // za dawno, nie ma czego ratowac
    }
  }

  /* Zegar sie przesunal w trakcie dlugiego offline'u? Popraw zaleglosci. */
  if (rtcTimeValid && hadTime) {
    int32_t delta = (int32_t)(time(nullptr) - before);
    if (delta > 60 || delta < -60) {
      queueShiftTimestamps(delta);
      przesunZnaczniki(delta);
    }
  }
  return rtcTimeValid;
}

/* =====================================================================
 *  7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)
 * ===================================================================== */
String idToken;

/* Token odlozony w pamieci nieulotnej.

   TU BYL BLAD WSPOLDZIALANIA, niewidoczny w zadnym tescie osobno.
   idToken jest zwykla zmienna, wiec deep sleep go kasuje - kazde
   wybudzenie logowalo sie haslem od nowa. Przy kilku wybudzeniach dziennie
   to nie ma znaczenia. Ale podglad ladowania budzi pudelko CO MINUTE, wiec
   zrobiloby sie z tego 1440 logowan haslem na dobe. Google limituje
   signInWithPassword i przy takim natezeniu potrafi czasowo zablokowac
   konto za podejrzana aktywnosc - czyli nocne ladowanie odcieloby pudelko
   od bazy. Kazdy z tych dwoch kawalkow byl poprawny osobno.

   Token zyje godzine, wiec trzymamy go w NVS razem z czasem waznosci.  */
bool tokenZPamieci() {
  if (!rtcTimeValid || rtcTokenExp == 0) return false;
  if ((uint32_t)time(nullptr) + TOKEN_MARGIN_S >= rtcTokenExp) return false;
  prefs.begin(NVS_NAMESPACE, true);
  idToken = prefs.getString("tok", "");
  prefs.end();
  return idToken.length() > 20;
}

void zapomnijToken() {
  idToken = "";
  rtcTokenExp = 0;
  prefs.begin(NVS_NAMESPACE, false);
  prefs.remove("tok");
  prefs.end();
}

/* --- HASLO URZADZENIA: pamiec trwala przed config.h  (D59) -------------

   PO CO TO ISTNIEJE. Binarke aktualizacji buduje automat z tego repo,
   a w repo stoi wylacznie placeholder - prawdziwe haslo zyje tylko na
   dysku Kuby. Gdyby haslo bylo WYLACZNIE w config.h, pierwsza
   aktualizacja przez WiFi wgralaby program, ktory nie umie sie zalogowac
   do bazy. Pudelko straciloby jedyna droge, ktora mozna je naprawic
   zdalnie - zostalby kabel.

   Dlatego haslo przeprowadza sie RAZ do pamieci trwalej i od tej pory
   config.h jest juz tylko ziarnem. Kuba wgrywa kablem dokladnie tak jak
   dotad, ze swoim haslem w config.h; roznica jest taka, ze robi to
   ostatni raz.

   KOLEJNOSC JAK PRZY SIECI WIFI (zasada 9): zapisujemy, potem czytamy
   z powrotem i dopiero zgodny odczyt znaczy "pudelko jest samodzielne".
   NVS w tym urzadzeniu potrafi zawiesc (D46, D47), a haslo zapisane
   "na wiare" byloby dokladnie tym rodzajem cichej straty, ktora wychodzi
   na jaw dopiero przy pierwszej aktualizacji - czyli najpozniej jak sie
   da. Od tego samego odczytu zalezy zgoda na OTA (`otaWolno`).         */
/* @extract-begin */
/* Tekst, ktory znaczy "tu nie ma hasla".

   STOI TUTAJ, A NIE W config.h - i to jest naprawa ergonomii, nie
   kosmetyka. Wczesniej w config.h byly DWIE linie z tym samym napisem
   "TUTAJ_WPISZ_HASLO": jedna do wypelnienia, druga do zostawienia.
   Kuba zapytal, czy wpisac haslo takze w tej drugiej - i dobrze, ze
   zapytal, bo wpisanie zepsuloby wszystko po cichu: oba napisy stalyby
   sie identyczne, `hasloJestPrawdziwe()` uznaloby prawdziwe haslo za
   placeholder i pudelko nie zalogowaloby sie w ogole.

   Wzorzec nie jest ustawieniem uzytkownika, tylko sposobem, w jaki
   firmware rozpoznaje binarke zbudowana przez automat z repo. Miejsce
   na haslo w config.h jest teraz dokladnie jedno.                     */
#define PASSWORD_PLACEHOLDER "TUTAJ_WPISZ_HASLO"
/* @extract-end */

bool hasloJestPrawdziwe(const String& h) {
  return h.length() > 0 && h != String(PASSWORD_PLACEHOLDER);
}

/* Haslo z pamieci trwalej. Puste = jeszcze go tam nie ma. */
String hasloZPamieci() {
  prefs.begin(NVS_NAMESPACE, true);
  String z = prefs.getString("fbpass", "");
  prefs.end();
  return hasloJestPrawdziwe(z) ? z : String("");
}

/* Czy pudelko jest juz samodzielne - czyli czy przezyje aktualizacje
   binarka zbudowana z repo, w ktorym hasla nie ma.                    */
bool hasloWPamieci() { return hasloZPamieci().length() > 0; }

/* Zapis z odczytem kontrolnym. Sam wynik nvsPutStr() nie wystarczy:
   to od tej wartosci zalezy, czy wolno wgrac binarke bez hasla.      */
bool hasloUtrwal(const String& h) {
  if (!hasloJestPrawdziwe(h)) return false;
  prefs.begin(NVS_NAMESPACE, false);
  const bool zapis = nvsPutStr("fbpass", h);
  const String kontrola = zapis ? prefs.getString("fbpass", "") : String("");
  prefs.end();
  return zapis && kontrola == h;
}

/* Ktorego hasla uzyc do logowania: pamiec trwala ma pierwszenstwo przed
   config.h. Binarka z automatu ma tam placeholder, wiec bez pamieci nie
   ma czym sie zalogowac - i to jest ten moment, w ktorym `otaWolno()`
   musialo wczesniej powiedziec "nie".                                 */
String hasloDoLogowania() {
  const String zPamieci = hasloZPamieci();
  if (zPamieci.length()) return zPamieci;
  return String(DEVICE_PASSWORD);
}

/* --- BOT TELEGRAM: token i czat w pamieci trwalej  (D67) ---------------

   TEN SAM WZOR CO HASLO DO BAZY, i to nie przypadek. Token bota jest
   sekretem dokladnie tej samej klasy: kto go ma, ten pisze w imieniu bota
   i czyta wszystko, co ktos do niego napisze. W config.h stac nie moze,
   bo ten plik jest w repo, a binarke buduje automat - wkompilowany token
   bylby tokenem opublikowanym przy pierwszej aktualizacji.

   Przychodzi wiec z aplikacji przez baze, ta sama droga co haslo do WiFi,
   i podlega tej samej zasadzie 9: zapis, ODCZYT KONTROLNY, i dopiero
   potem kasowanie z bazy. NVS w tym urzadzeniu potrafi zawiesc (D46,
   D47), a token zapisany "na wiare" dalby stan najgorszy z mozliwych -
   aplikacja pokazywalaby "bot podlaczony", a wiadomosci nie przyszlyby
   nigdy i nikt by nie wiedzial dlaczego.                               */
String tgTokenZPamieci() {
  prefs.begin(NVS_NAMESPACE, true);
  String t = prefs.getString("tgTok", "");
  prefs.end();
  return t;
}

String tgChatZPamieci() {
  prefs.begin(NVS_NAMESPACE, true);
  String c = prefs.getString("tgChat", "");
  prefs.end();
  return c;
}

/* Bot jest podlaczony dopiero wtedy, gdy sa OBA. Sam token nie ma do kogo
   napisac, samo id czatu nie ma czym.                                  */
bool tgSkonfigurowany() {
  return tgTokenZPamieci().length() > 0 && tgChatZPamieci().length() > 0;
}

/* Zapis obu wartosci naraz, potwierdzony odczytem. Zwraca false takze
   wtedy, gdy zapisala sie tylko jedna polowa - polowiczna konfiguracja
   wygladalaby jak dzialajaca, a nie byla.                              */
bool tgUtrwal(const String& token, const String& chat) {
  if (!token.length() || token.length() > TG_TOKEN_MAX) return false;
  if (!chat.length()  || chat.length()  > TG_CHAT_MAX)  return false;
  prefs.begin(NVS_NAMESPACE, false);
  const bool zapisT = nvsPutStr("tgTok", token);
  const bool zapisC = zapisT ? nvsPutStr("tgChat", chat) : false;
  const String kontrolaT = zapisC ? prefs.getString("tgTok", "")  : String("");
  const String kontrolaC = zapisC ? prefs.getString("tgChat", "") : String("");
  prefs.end();
  return zapisC && kontrolaT == token && kontrolaC == chat;
}

void tgZapomnij() {
  prefs.begin(NVS_NAMESPACE, false);
  prefs.remove("tgTok");
  prefs.remove("tgChat");
  prefs.end();
}

/* --- Czy w ogole wysylac - i dlaczego nie ----------------------------
   Wydzielone z `tgWyslijZalegle()` wylacznie po to, zeby dalo sie to
   przetestowac bez sieci i bez pamieci trwalej, tak samo jak
   `alarmPotwierdzony()` (D15b) i `otaDecyzja()` (D59). Sama wysylka to
   czyste we/wy; decyzja da sie sprawdzic na argumentach.

   KOLEJNOSC PYTAN JEST TRESCIA. Najpierw "czy jest o czym pisac" - bo
   przy pustej skrzynce nie wolno wlaczyc radia ani na sekunde, a to
   sprawdzenie kosztuje jedno porownanie. Potem "czy jest komu" i dopiero
   na koncu "czy nie za pozno".

   Nieznany czas nie jest powodem do milczenia. `teraz == 0` znaczy, ze
   pudelko nie ma wiarygodnego zegara, a `tsPowstania == 0`, ze nie mialo
   go w chwili zdarzenia - w obu razach nie umiemy zmierzyc wieku
   wiadomosci i wysylamy ja. To ta sama zasada, co przy dniach bez leku:
   milkniemy wylacznie wtedy, gdy wiemy na pewno.                       */
/* @extract-begin */
TgDecyzja tgDecyzja(bool botPodlaczony, bool cosCzeka,
                    uint32_t tsPowstania, uint32_t teraz) {
  if (!cosCzeka)      return TG_NIC;
  if (!botPodlaczony) return TG_BRAK_BOTA;
  if (tsPowstania && teraz && teraz > tsPowstania &&
      teraz - tsPowstania > (uint32_t)TG_MAX_WIEK_S) return TG_ZA_STARE;
  return TG_WYSLIJ;
}
/* @extract-end */

bool firebaseSignIn() {
  if (idToken.length() > 20) {
    /* Zwykle wybudzenie trwa sekundy, wiec token nie zdazy wygasnac.
       Ale na ladowarce pudelko czuwa GODZINAMI, a token zyje godzine -
       wiec waznosc trzeba sprawdzac takze tutaj, nie tylko przy starcie. */
    if (!rtcTimeValid || rtcTokenExp == 0) return true;
    if ((uint32_t)time(nullptr) + TOKEN_MARGIN_S < rtcTokenExp) return true;
    LOGLN("[FB ] token dobiega konca - odswiezam");
    idToken = "";
  }
  if (tokenZPamieci()) { LOGLN("[FB ] token z pamieci - bez logowania haslem"); return true; }

  WiFiClientSecure client;
  client.setInsecure();                     // prototyp; produkcyjnie: setCACert()
  client.setTimeout(15);

  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(12000);
  String url = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key="
               WEB_API_KEY;
  if (!http.begin(client, url)) { LOGLN("[FB ] nie mozna otworzyc polaczenia"); return false; }
  http.addHeader("Content-Type", "application/json");

  /* Pamiec trwala przed config.h - patrz hasloDoLogowania() i D59. */
  const String haslo = hasloDoLogowania();
  if (!hasloJestPrawdziwe(haslo)) {
    LOGLN("[FB ] BRAK HASLA: pamiec trwala pusta, a config.h ma placeholder");
    LOGLN("[FB ] wgraj firmware kablem z wpisanym haslem albo podaj je w portalu");
    return false;
  }

  JsonDocument req;
  req["email"] = DEVICE_EMAIL;
  req["password"] = haslo;
  req["returnSecureToken"] = true;
  String body;
  serializeJson(req, body);

  int code = http.POST(body);

  /* Odpowiedz wczytujemy W CALOSCI do pamieci, a dopiero potem parsujemy.
     Czytanie JSON-a prosto ze strumienia TLS bywa zawodne przy dluzszych
     odpowiedziach, a token Firebase ma ~1000 znakow - dokladnie ten
     przypadek konczyl sie komunikatem "HTTP 200 -> FAIL".               */
  String payload = (code > 0) ? http.getString() : String();
  http.end();

  if (code != 200) {
    LOG("[FB ] signIn HTTP %d\n", code);
    if (payload.length())
      LOG("[FB ] serwer odpowiedzial: %s\n", payload.substring(0, 250).c_str());
    return false;
  }

  JsonDocument res;
  DeserializationError err = deserializeJson(res, payload);
  if (err) {
    LOG("[FB ] blad parsowania JSON: %s (odpowiedz ma %d znakow)\n",
        err.c_str(), payload.length());
    return false;
  }

  idToken = res["idToken"].as<String>();
  if (idToken.length() < 20) {
    LOG("[FB ] brak idToken w odpowiedzi: %s\n", payload.substring(0, 250).c_str());
    return false;
  }

  /* Zapamietujemy token do nastepnych wybudzen. Bez wiarygodnego zegara
     nie ma jak sprawdzic waznosci, wiec wtedy nie zapisujemy.          */
  uint32_t zyje = res["expiresIn"].as<uint32_t>();
  if (zyje < 60 || zyje > 86400) zyje = 3600;
  if (rtcTimeValid) {
    rtcTokenExp = (uint32_t)time(nullptr) + zyje;
    prefs.begin(NVS_NAMESPACE, false);
    nvsPutStr("tok", idToken);
    prefs.end();
  }
  /* Haslo WLASNIE sie sprawdzilo - to jedyny moment, w ktorym wiemy na
     pewno, ze jest dobre. Przepisujemy je do pamieci trwalej, o ile
     jeszcze go tam nie ma. Robimy to PO udanym logowaniu, nigdy przed:
     zapisane wczesniej bledne haslo zostaloby na stale i przykrylo to
     poprawne z config.h przy nastepnym wgraniu kablem.                */
  if (!hasloWPamieci()) {
    if (hasloUtrwal(haslo))
      LOGLN("[FB ] haslo przeniesione do pamieci trwalej - pudelko jest samodzielne");
    else
      LOGLN("[FB ] NIE UDALO SIE utrwalic hasla - aktualizacje przez WiFi pozostaja zablokowane");
  }

  LOG("[FB ] zalogowano, token ma %d znakow\n", idToken.length());
  return true;
}

String rtdbUrl(const String& path) {
  return String("https://") + RTDB_HOST + path + "?auth=" + idToken;
}

/* JEDNO polaczenie TLS na cale wybudzenie, nie jedno na zapytanie.

   TU BYLA GLOWNA PRZYCZYNA POWOLNEJ SYNCHRONIZACJI.

   Kazde wywolanie tworzylo wlasnego klienta i zestawialo szyfrowane
   polaczenie od zera. Uzgodnienie TLS na tym procesorze to sekunda,
   czasem dwie - a jedno otwarcie pudelka to nie jedno zapytanie, tylko
   ciag: pobranie ustawien, oproznienie kolejki, zapis zdarzenia, stan
   wieczka, pelny status, czarna skrzynka. Szesc uzgodnien po kolei, i to
   zanim doliczymy logowanie do Firebase. Stad kilkanascie sekund tam,
   gdzie same dane zajmuja ulamek tego czasu.

   Teraz klient jest jeden i zyje przez cale wybudzenie, a HTTPClient
   dostaje setReuse(true), wiec kolejne zapytania do tego samego hosta
   ida juz gotowym kanalem. Placimy za uzgodnienie RAZ.                */
WiFiClientSecure rtdbClient;
bool rtdbClientGotowy = false;

int rtdbSend(const char* method, const String& path, const String& body, String* resp = nullptr) {
  if (!rtdbClientGotowy) {
    rtdbClient.setInsecure();
    rtdbClient.setTimeout(10);
    rtdbClientGotowy = true;
  }

  HTTPClient http;
  http.setReuse(true);                      // nie zrywaj kanalu po zapytaniu
  http.setConnectTimeout(8000);
  http.setTimeout(10000);
  if (!http.begin(rtdbClient, rtdbUrl(path))) return -1;
  http.addHeader("Content-Type", "application/json");
  int code = http.sendRequest(method, body);
  if (resp && code > 0) *resp = http.getString();
  http.end();
  /* Baza odmowila z powodu tokenu - kasujemy go, zeby nastepna proba
     zalogowala sie od nowa zamiast uparcie uzywac nieaktualnego.      */
  if (code == 401 || code == 403) {
    LOG("[FB ] baza odrzucila token (HTTP %d) - kasuje go\n", code);
    zapomnijToken();
  }
  return code;
}

/* Czy rekord ma komplet pieciu pol? Bez tego nie ma czego wyslac, a
   ponawianie go w nieskonczonosc niczego nie naprawi.                  */
bool rekordKompletny(const String& rec) {
  int p = -1;
  for (int i = 0; i < 4; i++) { p = rec.indexOf(';', p + 1); if (p < 0) return false; }
  return true;
}

/* Wysyla pojedyncze zdarzenie. rec = "ts;type;batt;volt;slot"
   Zwraca kod HTTP, a nie samo tak/nie - dzwoniacy musi umiec odroznic
   "sprobuj pozniej" od "to nie przejdzie nigdy".                       */
int pushEventRecord(const String& rec) {
  /* Uszkodzony rekord melduje sie tak samo jak odrzucenie przez baze. */
  if (!rekordKompletny(rec)) return 400;
  int p1 = rec.indexOf(';');
  int p2 = rec.indexOf(';', p1 + 1);
  int p3 = rec.indexOf(';', p2 + 1);
  int p4 = rec.indexOf(';', p3 + 1);

  JsonDocument doc;
  doc["ts"]      = (uint32_t)rec.substring(0, p1).toInt();
  doc["type"]    = rec.substring(p1 + 1, p2);
  doc["battery"] = rec.substring(p2 + 1, p3).toInt();
  doc["volt"]    = rec.substring(p3 + 1, p4).toFloat();
  doc["slot"]    = rec.substring(p4 + 1).toInt();
  doc["fw"]      = FW_VERSION;

  String body;
  serializeJson(doc, body);
  /* Odpowiedz bazy zbieramy TYLKO po to, zeby przy niepowodzeniu bylo
     wiadomo, co sie stalo.

     Cala decyzja D13 stoi na zalozeniu, ze wpis odrzucony przez reguly
     wraca kodem 400 - bo tylko wtedy trwaleOdrzucony() zdejmie go
     z kolejki. Tego zalozenia nikt nigdy nie zmierzyl. Jesli baza
     odpowiada tu 401 albo 403, wpis nie zostanie zdjety NIGDY, a przy
     okazji kazda proba skasuje token i wymusi ponowne logowanie.
     Log z pudelka rozstrzyga to jednym zdaniem, zamiast kolejnej
     hipotezy w dokumentacji.                                          */
  String odp;
  int code = rtdbSend("POST", "/devices/" DEVICE_ID "/events.json", body, &odp);
  if (code == 200) {
    LOG("[FB ] push event HTTP 200 : %s\n", body.c_str());
  } else {
    LOG("[FB ] push event HTTP %d : %s\n     odpowiedz bazy: %s\n",
        code, body.c_str(), odp.c_str());
  }
  return code;
}

/* Sam stan wieczka, wysylany natychmiast.

   Pelny status ciagnie za soba cala czarna skrzynke, a przy otwarciu
   pudelko robi jeszcze pobranie ustawien i oproznienie kolejki. Kazda z
   tych rzeczy to osobna runda po sieci, wiec informacja "otwarte"
   docierala do telefonu jako OSTATNIA - stad odczuwalne opoznienie.

   PATCH zmienia tylko trzy pola i nie rusza reszty statusu, wiec mozemy
   go wyslac od razu po zalogowaniu, przed cala reszta roboty.        */
bool pushLidState() {
  char body[96];
  snprintf(body, sizeof(body), "{\"boxOpen\":%s,\"openSince\":%lu,\"lastSeen\":%lu}",
           boxIsOpen() ? "true" : "false",
           (unsigned long)(boxIsOpen() ? rtcOpenSinceTs : 0),
           (unsigned long)time(nullptr));
  const bool stan = boxIsOpen();
  int code = rtdbSend("PATCH", "/devices/" DEVICE_ID "/status.json", String(body));
  const bool ok = (code >= 200 && code < 300);
  if (ok) { rtcOpenReported = stan; rtcStatusDirty = false; }
  else    { rtcStatusDirty = true; }
  LOG("[LID] stan wieczka (%s) %s\n", stan ? "OTWARTE" : "zamkniete",
      ok ? "wyslany" : "NIE doszedl");
  return ok;
}

/* --- Suma wgranego programu (uzywa jej i status, i decyzja o OTA) ---
   Stoi TUTAJ, a nie przy reszcie kodu aktualizacji, bo `pushStatus()`
   jest zdefiniowane wyzej i musi ja widziec. Kolejnosci definicji
   pilnuje audyt - w .ino ma ona znaczenie takze dla generatora
   prototypow (B21/D26).                                            */
String otaSumaZPamieci(const char* klucz) {
  prefs.begin(NVS_NAMESPACE, true);
  String s = prefs.getString(klucz, "");
  prefs.end();
  return s;
}

/* Suma programu, ktory NAPRAWDE siedzi w pudelku - albo pusty napis.

   TU BYL BLAD, i to taki, ktory naprawilem tylko w polowie. Po wgraniu
   kablem `otaMd5` zostaje z poprzedniej wersji, wiec `pushStatus()`
   dostal juz warunek "wysylaj tylko, gdy `otaFw` zgadza sie z biezaca
   wersja". Ale `otaDecyzja()` czytala sume WPROST z pamieci, z pominieciem
   tego warunku - i wychodzila z tego dwie rozne prawdy naraz: aplikacji
   pudelko meldowalo "nie znam wlasnej sumy", a samo sobie liczylo po
   sumie programu, ktorego juz w nim nie ma.

   Skutek widzialny u Kuby: swiezo wgrane 1.39.4 nie uznalo sie za
   aktualne, wiec zlecenie z bazy nie zostalo skasowane i wisialo dalej
   z czerwonym ostrzezeniem.

   Jedno zrodlo prawdy dla obu stron - stad ta funkcja.               */
String otaSumaWgranej() {
  const String dlaWersji = otaSumaZPamieci("otaFw");
  if (dlaWersji != String(FW_VERSION)) return String("");
  return otaSumaZPamieci("otaMd5");
}

bool pushStatus() {
  JsonDocument doc;
  doc["battery"]  = batteryPercentage;
  doc["battRaw"]  = batteryRawPercentage;   // przed wygladzeniem - do diagnostyki
  doc["volt"]     = realBatteryVoltage;
  doc["lastSeen"] = (uint32_t)time(nullptr);
  doc["rssi"]     = WiFi.RSSI();
  doc["ssid"]     = WiFi.SSID();
  /* Nazwy znanych sieci - SAME NAZWY, nigdy hasla. Bez tego aplikacja nie
     ma jak pokazac, czy siec dodana z telefonu w ogole doszla.          */
  {
    String znane = "";
    int ile = wifiSieciCount();
    for (int i = 0; i < ile; i++) {
      String s = wifiSiecSsid(i);
      if (!s.length()) continue;
      if (znane.length()) znane += "|";
      znane += s;
    }
    doc["nets"] = znane;
  }
  /* Wynik ostatniej proby przyjecia sieci z aplikacji. Puste, dopoki
     zadnej nie bylo - i to tez jest informacja.                        */
  doc["netMsg"] = rtcNetMsg;
  doc["fw"]       = FW_VERSION;
#if TG_ENABLED
  /* Bot Telegram (D67). Dwa pola, bo "bot nie pisze" ma inaczej trzy
     nieodroznialne z zewnatrz przyczyny: pudelko nie ma tokenu, ma go
     i Telegram odmawia, albo nie bylo jeszcze o czym pisac. Pierwsza
     kaze wrocic do ekranu parowania, druga zalozyc bota od nowa,
     trzecia nie kaze robic nic. Ta sama lekcja co `otaMsg` i `netMsg`.

     Samego tokenu nie wysylamy NIGDY - ani w calosci, ani w skrocie.
     Aplikacja go zna, bo sama go wpisala; do niczego nie potrzebuje go
     z powrotem, a status jest jedynym miejscem, ktore pudelko nadpisuje
     w bazie przy kazdym wybudzeniu.                                   */
  doc["tg"]     = tgSkonfigurowany();
  doc["tgMsg"]  = rtcTgMsg;
#endif
#if OTA_ENABLED
  /* Aktualizacja przez WiFi (D59). `otaMsg` to odpowiedz na pytanie
     "dlaczego jeszcze nie" - bez niej przycisk w aplikacji wygladalby
     na zepsuty za kazdym razem, gdy pudelko na cos czeka.

     `otaHaslo` mowi, czy pudelko przezyje aktualizacje: binarka z automatu
     nie zna hasla do bazy, wiec bez hasla w pamieci trwalej wgranie jej
     odcieloby pudelko od aplikacji. Aplikacja ma o tym krzyczec ZANIM
     ktos nacisnie przycisk, a nie po fakcie.                          */
  doc["otaMsg"]    = rtcOtaMsg;
  doc["otaWersja"] = rtcOtaWersja;       // co lezy na serwerze
  doc["otaHaslo"]  = hasloWPamieci();
  /* Czy pudelko W OGOLE widzi zlecenie z aplikacji.

     Bez tego pola "nic sie nie dzieje" mialo dwie zupelnie rozne
     przyczyny nie do odroznienia z zewnatrz: polecenie nie doszlo do
     pudelka, albo doszlo i cos je zatrzymalo. Pierwszy raz kosztowalo
     to dedukcje z licznika prob; drugi raz ma wystarczyc spojrzenie. */
  doc["otaProsba"] = rtcOtaProsba;
  {
    prefs.begin(NVS_NAMESPACE, true);
    doc["otaFail"] = prefs.getUShort("otaFail", 0);
    /* Wersja, ktora sie nie uruchomila i zostala cofnieta. Puste = nigdy
       sie to nie zdarzylo, i to tez jest informacja.                  */
    doc["otaBad"]  = prefs.getString("otaBad", "");
    /* Suma programu, ktory naprawde siedzi w pudelku. Aplikacja porownuje
       ja z opisem na serwerze i dzieki temu wie NA PEWNO, czy jest co
       wgrywac. Po numerze wersji tego nie widac: numer pisze czlowiek
       w config.h i da sie go zapomniec podbic. Puste do pierwszej
       aktualizacji przez WiFi - wtedy aplikacja wraca do numeru.

       Wysylamy ja WYLACZNIE wtedy, gdy powstala dla TEGO programu.
       Po wgraniu kablem w pamieci zostaje suma poprzedniej wersji, a
       podanie jej dalej znaczyloby "w pudelku siedzi binarka o tej
       sumie" - czyli klamstwo, na ktorym aplikacja opiera decyzje
       "jest nowa wersja". Niezgodnosc traktujemy jak brak sumy:
       porownanie spada wtedy na numer wersji, ktory po kablu jest
       prawdziwy.

       Liczymy ja PO `prefs.end()`, nie w srodku bloku: `otaSumaWgranej()`
       sama otwiera pamiec, a Preferences w rdzeniu ESP32 to jeden globalny
       uchwyt - zagniezdzone `begin()` nic nie robi, za to `end()` tej
       wewnetrznej funkcji zamyka uchwyt tej zewnetrznej i wszystko, co
       nastapiloby dalej, przepada. Tak zamilkla kiedys czarna skrzynka
       (B22). Audyt tego pilnuje.                                      */
    prefs.end();
  }
  doc["otaMd5"] = otaSumaWgranej();
#endif
  doc["boots"]    = rtcBootCount;
  doc["queued"]   = queueCount();
  /* Ciche straty przestaja byc ciche. Aplikacja krzyczy, gdy > 0.      */
  doc["dropped"]  = rtcQueueDropped;
  doc["nvsFail"]  = rtcNvsFail;
  /* Ktory klucz nie zapisal sie OSTATNIO - zeby "cos przepadlo" nie konczylo
     sie zgadywaniem, czy to byla dawka, czy diagnostyczny drobiazg.      */
  doc["nvsFailKey"] = rtcNvsFailKey;
  /* Wolne miejsce w pamieci trwalej. rtcNvsFail mowi, ze juz jest zle;
     to pole pozwala aplikacji ostrzec, ZANIM bedzie.                   */
  doc["nvsFree"]  = nvsWolneWpisy();
  doc["charging"] = rtcCharging;            // zywy podglad ladowania w aplikacji
  /* Na tych dwoch polach aplikacja opiera szacunek "ile jeszcze". Sam
     procent podczas ladowania nic nie mowi - napiecie pokazuje wtedy
     ladowarke - wiec liczymy z czasu i punktu startowego.            */
  doc["chargeSince"]   = rtcCharging ? rtcChargeSinceTs : 0;
  doc["chargeFromPct"] = (rtcCharging && rtcChargeFromPct != 255)
                         ? (int)rtcChargeFromPct : -1;
  /* Dwie ostatnie daty ladowania - aplikacja policzy z nich, ile realnie
     starczylo na jednym ladowaniu. Liczba dni z zycia, nie z katalogu. */
  prefs.begin(NVS_NAMESPACE, true);
  doc["lastCharge"] = prefs.getUInt("chgEnd", 0);
  doc["prevCharge"] = prefs.getUInt("chgPrev", 0);
  prefs.end();
  /* Aplikacja pokazuje ostrzezenie, gdy wieczko zostalo otwarte.
     openSince pozwala jej napisac, od jak dawna - bez zgadywania.     */
  doc["boxOpen"]   = boxIsOpen();
  doc["openSince"] = boxIsOpen() ? rtcOpenSinceTs : 0;
  const bool stanWyslany = boxIsOpen();
  String body;
  serializeJson(doc, body);
  int code = rtdbSend("PUT", "/devices/" DEVICE_ID "/status.json", body);
  const bool ok = (code >= 200 && code < 300);

  /* TU BYL BLAD, i to grozny.

     Wczesniej rtcOpenReported ustawialo sie PRZED wyslaniem, bez patrzenia
     na wynik. Gdy pakiet nie doszedl - zerwane WiFi, wygasly token, blad
     serwera - pudelko i tak zapisywalo sobie "aplikacja juz wie". Warunek
     rtcOpenReported != boxIsOpen() przestawal byc prawda, wiec ponownej
     proby NIE BYLO. Nigdy. Aplikacja zostawala z nieaktualnym stanem az
     do przypadkowej wysylki przy nastepnej dawce.

     Teraz zapisujemy to dopiero po potwierdzeniu, a nieudana proba zostawia
     slad w rtcStatusDirty - przezywa deep sleep i wymusza ponowienie.  */
  if (ok) {
    rtcOpenReported = stanWyslany;
    rtcStatusDirty  = false;
    rtcLastPushPct  = (int16_t)batteryPercentage;
    if (rtcTimeValid) rtcLastPushTs = (uint32_t)time(nullptr);
  } else {
    rtcStatusDirty  = true;
    LOG("[PSH] status NIE doszedl (kod %d) - ponowie\n", code);
  }

  /* Historia wybudzen - zeby dalo sie z telefonu sprawdzic, co pudelko
     robilo w nocy, zamiast zgadywac.

     Wysylamy ja TYLKO gdy przybylo wpisow. Wczesniej lecial caly dziennik
     przy kazdym statusie - a przy ladowaniu status idzie co minute, wiec
     te same kilka kilobajtow szly w kolko. To osobne zapytanie, wiec
     osobna zwloka przy kazdej synchronizacji.                        */
  prefs.begin(NVS_NAMESPACE, true);
  uint16_t idx = prefs.getUShort("lbIdx", 0);
  prefs.end();
  if (ok && idx != rtcLogWyslanyIdx) {
    if (rtdbSend("PUT", "/devices/" DEVICE_ID "/log.json", logbookJson()) / 100 == 2)
      rtcLogWyslanyIdx = idx;
  }

  /* Dziennik wieczka - test terenowy. Wysylamy CALOSC i dopiero po
     potwierdzeniu kasujemy z pamieci pudelka, zeby nie zapychac NVS.
     Kolejnosc jest tu istotna: gdyby kasowanie szlo przed potwierdzeniem,
     jedna nieudana wysylka niszczylaby caly pomiar bezpowrotnie.       */
  if (ok && lidLogCount() > 0) {
    /* KAZDA paczka ma wlasny klucz, a nie wspolny wezel.
       Pudelko kasuje dziennik po wyslaniu, wiec przy zwyklym PUT druga
       synchronizacja nadpisalaby pierwsza - po kilku dniach wyjazdu
       zostalby tylko ostatni dzien. Osobne klucze sie sumuja.         */
    String sciezka = "/devices/" DEVICE_ID "/lidlog/";
    sciezka += rtcTimeValid ? String((uint32_t)time(nullptr)) : String("b" + String(rtcBootCount));
    sciezka += ".json";
    if (rtdbSend("PUT", sciezka, lidLogJson()) / 100 == 2)
      lidLogClear();
    else
      LOGLN("[LID] dziennik NIE doszedl - zostaje w pamieci do nastepnego razu");
  }

  /* Historia nieudanych zapisow NVS (D47, prosba Kuby: "może w bazie damy
     jakiś zapis, żeby te nieudane tam wrzucał"). Ten sam wzor co dziennik
     wieczka - wlasny klucz na paczke, kasowanie/oznaczenie dopiero po
     potwierdzeniu. "Kasowanie" jest tu przesuwaniem znacznika `sent`, bo
     dane siedza w RTC, nie w NVS - zeby nie dokladac kolejnych zapisow do
     pamieci, ktora akurat moze zawodzic.                                */
  if (ok && nvsFailLogDoWyslania()) {
    String sciezka = "/devices/" DEVICE_ID "/nvsfaillog/";
    sciezka += rtcTimeValid ? String((uint32_t)time(nullptr)) : String("b" + String(rtcBootCount));
    sciezka += ".json";
    if (rtdbSend("PUT", sciezka, nvsFailLogJson()) / 100 == 2)
      nvsFailLogOznaczWyslany();
    else
      LOGLN("[NVS] dziennik niepowodzen NIE doszedl - zostaje do nastepnego razu");
  }
  return ok;
}

/* Pobiera harmonogram ustawiony w aplikacji. */
void fetchConfig() {
  String resp;
  int code = rtdbSend("GET", "/devices/" DEVICE_ID "/config.json", "", &resp);
  if (code != 200 || resp.length() < 5 || resp == "null") {
    LOG("[FB ] brak config (HTTP %d) - zostaje lokalny\n", code);
    return;
  }
  JsonDocument doc;
  if (deserializeJson(doc, resp) != DeserializationError::Ok) return;

  String s = "";
  JsonArray arr = doc["schedule"].as<JsonArray>();
  for (JsonVariant v : arr) {
    if (s.length()) s += "|";
    s += v.as<String>();
  }
  int16_t tz = doc["tzOffsetMin"] | DEFAULT_TZ_OFFSET;
  if (s.length() >= 5 && (s != String(rtcSchedule) || tz != rtcTzOffsetMin)) {
    saveSchedule(s, tz);
    LOG("[FB ] nowy harmonogram: %s\n", s.c_str());
  }

  /* Stan opakowania liczy aplikacja - plytka tylko go czyta, zeby moc
     ostrzec dzwiekiem. Dzieki temu nie ma konfliktu zapisow.           */
  if (!doc["pillsLeft"].isNull()) {
    rtcPillsLeft = doc["pillsLeft"].as<int>();
    LOG("[FB ] zostalo tabletek: %d\n", rtcPillsLeft);
  }

  /* --- Nowa siec WiFi dodana z aplikacji ------------------------------
     `wifiNowa` to skrzynka nadawcza na JEDNA siec, nie lista. Pudelko
     pamieta swoja liste lokalnie, wiec aplikacja ma tylko powiedziec
     "dolóz te" - i tyle.

     KOLEJNOSC JEST TU CALA TRESCIA (zasada 6): najpierw zapis do pamieci
     trwalej, potem sprawdzenie, czy sie udal, i DOPIERO wtedy kasowanie
     hasla z bazy. Odwrotnie stracilibysmy siec, ktorej nikt juz nie zna -
     a wraz z nia jedyna droge do pudelka poza portalem.

     Haslo kasujemy, bo nie ma powodu, zeby lezalo w bazie dluzej niz do
     najblizszego polaczenia. Reguly i tak pilnuja dostepu, ale krotszy
     czas zycia jest po prostu tansza obrona niz kazda inna.            */
  JsonObject nowa = doc["wifiNowa"].as<JsonObject>();
  if (!nowa.isNull()) {
    String ssid = nowa["ssid"] | "";
    String pass = nowa["pass"] | "";
    if (!ssid.length() || ssid.length() > 32) {
      snprintf(rtcNetMsg, sizeof(rtcNetMsg), "odrzucona: zla nazwa sieci");
    } else {
      if (wifiSiecDodaj(ssid, pass)) {
        int kod = rtdbSend("DELETE", "/devices/" DEVICE_ID "/config/wifiNowa.json", "");
        snprintf(rtcNetMsg, sizeof(rtcNetMsg), "przyjeta, kasowanie hasla HTTP %d", kod);
        LOG("[NET] siec '%s' przyjeta z aplikacji, kasowanie hasla z bazy: HTTP %d\n",
            ssid.c_str(), kod);
        /* Zostajemy przy sieci, ktora WLASNIE dziala. Nowa dostanie swoja
           szanse dopiero wtedy, gdy ta przestanie odpowiadac - inaczej
           kazde dodanie sieci kosztowaloby jedno nieudane laczenie.    */
        int ile = wifiSieciCount();
        for (int i = 0; i < ile; i++)
          if (wifiSiecSsid(i) == WiFi.SSID()) { rtcNetOstatnia = (uint8_t)i; break; }
      } else {
        snprintf(rtcNetMsg, sizeof(rtcNetMsg), "zapis do pamieci NIEUDANY");
        LOGLN("[NET] nie udalo sie zapisac sieci - haslo ZOSTAJE w bazie do nastepnej proby");
      }
    }
  }

  /* --- Polecenie na liscie sieci: usun / uzywaj tej --------------------
     Ta sama skrzynka nadawcza co `wifiNowa`, tylko bez hasla: aplikacja
     mowi CO zrobic, a pudelko odpowiada przez `netMsg`. Wpis kasujemy po
     wykonaniu, zeby polecenie nie powtarzalo sie w kolko - takze wtedy,
     gdy sie NIE UDALO. Nieusuwalne polecenie probowaloby sie wykonac przy
     kazdym polaczeniu i nigdy nie dalo sie go odwolac.                  */
  JsonObject cmd = doc["wifiCmd"].as<JsonObject>();
  if (!cmd.isNull()) {
    String akcja = cmd["akcja"] | "";
    String cel   = cmd["ssid"]  | "";

    if (akcja == "usun") {
      int wynik = wifiSiecUsun(cel);
      switch (wynik) {
        case WIFI_USUN_OK:
          snprintf(rtcNetMsg, sizeof(rtcNetMsg), "usunieto siec");
          break;
        case WIFI_USUN_BRAK:
          snprintf(rtcNetMsg, sizeof(rtcNetMsg), "nie znam tej sieci");
          break;
        case WIFI_USUN_OSTATNIA:
          snprintf(rtcNetMsg, sizeof(rtcNetMsg), "to jedyna siec - nie usuwam");
          break;
        default:
          snprintf(rtcNetMsg, sizeof(rtcNetMsg), "usuwanie: blad zapisu");
          break;
      }
    } else if (akcja == "priorytet") {
      bool ok2 = wifiSiecPriorytet(cel);
      snprintf(rtcNetMsg, sizeof(rtcNetMsg),
               ok2 ? "ta siec bedzie probowana pierwsza" : "nie znam tej sieci");
    } else {
      snprintf(rtcNetMsg, sizeof(rtcNetMsg), "nieznane polecenie sieci");
    }

    int kod = rtdbSend("DELETE", "/devices/" DEVICE_ID "/config/wifiCmd.json", "");
    LOG("[NET] polecenie '%s' na '%s' -> %s (kasowanie HTTP %d)\n",
        akcja.c_str(), cel.c_str(), rtcNetMsg, kod);
  }

#if OTA_ENABLED
  /* --- Prosba o aktualizacje programu (D59) ---------------------------
     Ta sama skrzynka nadawcza co `wifiCmd`: aplikacja stawia znacznik,
     pudelko go widzi przy najblizszym polaczeniu. Kasuje go dopiero
     `otaSprobuj()` - i to po wykonaniu, nie tutaj, bo dopiero tam wiadomo,
     czy sprawa jest zamknieta, czy pudelko na cos jeszcze czeka.

     Samego pobierania NIE robimy w tym miejscu. Trwa ono kilkadziesiat
     sekund, a `fetchConfig()` bywa wolane w srodku obslugi otwartego
     wieczka - czyli dokladnie wtedy, gdy pudelko ma zapisac dawke
     Warfinu. Aktualizacja czeka na swoja kolej w `goToSleep()`.       */
  JsonObject ota = doc["otaCmd"].as<JsonObject>();
  if (!ota.isNull()) {
    rtcOtaProsba = true;
    /* `md5` w zleceniu ignorujemy - patrz komentarz w otaDecyzja(). Liczy
       sie moment zlozenia: swiezsze niz ostatnia proba znaczy "czlowiek
       prosi teraz" i znosi blokade po serii nieudanych prob.           */
    rtcOtaTs = ota["ts"] | 0UL;
    LOG("[OTA] aplikacja prosi o aktualizacje (zlecenie z %lu) - zrobie to przed snem\n",
        (unsigned long)rtcOtaTs);
  }
#endif

#if TG_ENABLED
  /* --- Bot Telegram przyslany z aplikacji (D67) ------------------------
     Ta sama skrzynka nadawcza co `wifiNowa` i DOKLADNIE TA SAMA KOLEJNOSC
     (zasada 9): najpierw zapis do pamieci trwalej, potem sprawdzenie, czy
     sie udal, i dopiero wtedy kasowanie sekretu z bazy.

     Token bota jest sekretem tej samej klasy co haslo do WiFi: kto go ma,
     pisze w imieniu bota i czyta wszystko, co ktos do niego napisze.
     Odwrotna kolejnosc dawalaby stan, w ktorym aplikacja pokazuje "bot
     podlaczony", pudelko go nie ma, a token zniknal z bazy - czyli trzeba
     zakladac nowego u BotFathera. Reguly bazy i tak pilnuja dostepu, ale
     krotszy czas zycia jest tansza obrona niz kazda inna.              */
  JsonObject tg = doc["tgNowy"].as<JsonObject>();
  if (!tg.isNull()) {
    String tok  = tg["token"] | "";
    String chat = tg["chat"]  | "";
    if (!tok.length() || !chat.length()) {
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "odrzucony: brak tokenu albo czatu");
    } else if (tgUtrwal(tok, chat)) {
      int kod = rtdbSend("DELETE", "/devices/" DEVICE_ID "/config/tgNowy.json", "");
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "bot przyjety, kasowanie tokenu HTTP %d", kod);
      LOG("[TG ] bot przyjety z aplikacji, kasowanie tokenu z bazy: HTTP %d\n", kod);
    } else {
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "zapis bota do pamieci NIEUDANY");
      LOGLN("[TG ] nie udalo sie zapisac bota - token ZOSTAJE w bazie do nastepnej proby");
    }
  }

  /* --- Polecenie w sprawie bota: odlacz / napisz probna ---------------
     Bez tokenu, wiec kasujemy je po wykonaniu tak samo jak `wifiCmd` -
     takze wtedy, gdy sie nie udalo. Niekasowalne polecenie probowaloby
     sie wykonac przy kazdym polaczeniu i nie dalo sie odwolac.

     WYJATEK: `test` kasuje sie w `tgWyslijZalegle()`, i to dopiero po
     udanej wysylce. Powod jest ten sam, dla ktorego istnieje ten przycisk:
     ma rozstrzygnac, czy bot dziala. Skasowanie zlecenia tutaj znaczyloby,
     ze proba przepada przy pierwszym braku sieci - a czlowiek widzi cisze
     i nie wie, czy to bot, czy siec.                                    */
  JsonObject tgc = doc["tgCmd"].as<JsonObject>();
  if (!tgc.isNull()) {
    String akcja = tgc["akcja"] | "";
    if (akcja == "usun") {
      tgZapomnij();
      rtcTgSlot = -1;
      rtcTgSlotTs = 0;
      rtcTgBattCzeka = false;
      rtcTgTestProsba = false;
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "bot odlaczony");
      int kod = rtdbSend("DELETE", "/devices/" DEVICE_ID "/config/tgCmd.json", "");
      LOG("[TG ] bot odlaczony na zadanie aplikacji (kasowanie HTTP %d)\n", kod);
    } else if (akcja == "test") {
      rtcTgTestProsba = true;
      LOGLN("[TG ] aplikacja prosi o wiadomosc probna - wysle przed snem");
    } else {
      snprintf(rtcTgMsg, sizeof(rtcTgMsg), "nieznane polecenie bota");
      rtdbSend("DELETE", "/devices/" DEVICE_ID "/config/tgCmd.json", "");
    }
  }
#endif

  /* --- Prosba o rozejrzenie sie za sieciami WiFi ----------------------
     Aplikacja nie ma jak tego zrobic sama: Safari nie daje zadnej stronie
     dostepu do skanowania sieci. A i tak wazniejsza jest lista widziana
     PRZEZ PUDELKO - to ono ma sie polaczyc, a stoi gdzie indziej niz
     telefon. Siec swietnie widoczna z kanapy potrafi w ogole nie docierac
     do pudelka; wpisana z listy telefonu wygladalaby na dobra i nic by
     nie dzialalo.

     Sam skan robimy przed snem, nie tutaj - z tego samego powodu co przy
     aktualizacji: `fetchConfig()` bywa wolane w srodku obslugi otwartego
     wieczka, a skanowanie zabiera radio na kilka sekund i potrafi zerwac
     polaczenie. Dawka Warfinu ma pierwszenstwo przed wygoda.          */
  JsonObject sc = doc["wifiScan"].as<JsonObject>();
  if (!sc.isNull()) {
    rtcScanProsba = true;
    LOG("[SCN] aplikacja prosi o liste sieci (zlecenie z %lu)\n",
        (unsigned long)(sc["ts"] | 0UL));
  }

  /* --- Rozpisanie tygodniowe -----------------------------------------
     Do stringa ida DZIESIATE czesci tabletki, zeby polowka zmiescila sie
     w bajcie. Cokolwiek podejrzanego - choc jeden dzien brakujacy, nie
     bedacy liczba albo poza zakresem regul bazy - kasuje CALE rozpisanie
     i wracamy do dzwonienia codziennie. Nie wiemy = dzwonimy.          */
  String dw = "";
  JsonVariant weekV = doc["doseWeek"];
  if (!weekV.isNull()) {
    for (int i = 0; i < 7; i++) {
      /* Firebase oddaje tablice jako tablice tylko przy kompletnych
         kluczach 0..6; niekompletna wraca jako obiekt. Czytamy obie
         postacie, tak samo jak aplikacja.                             */
      char klucz[2] = { (char)('0' + i), 0 };
      JsonVariant v = weekV[i];
      if (v.isNull()) v = weekV[klucz];
      if (!v.is<float>() && !v.is<int>()) { dw = ""; break; }
      float f = v.as<float>();
      if (!(f >= 0.0f && f <= 10.0f)) { dw = ""; break; }
      if (dw.length()) dw += "|";
      dw += String((int)lroundf(f * 10.0f));
    }
  }

  /* --- Wyjatki na konkretne daty --------------------------------------
     Bierzemy wylacznie dni OD DZIS w przod i najwyzej DOSE_EX_MAX
     najblizszych: przeszle sa juz historia (rozlicza je aplikacja), a
     pamiec RTC jest za cenna, zeby trzymac w niej caly rok wstecz.    */
  uint32_t exDni[DOSE_EX_MAX];
  uint8_t  exWar[DOSE_EX_MAX];
  int      exN = 0;
  JsonObject exObj = doc["doseDays"].as<JsonObject>();
  if (!exObj.isNull()) {
    uint32_t dzis = rtcTimeValid ? localDayNumber(time(nullptr)) : 0;
    for (JsonPair kv : exObj) {
      uint32_t d = dateKeyToNum(kv.key().c_str());
      if (!d || d < dzis) continue;
      JsonVariant v = kv.value();
      if (!v.is<float>() && !v.is<int>()) continue;
      float f = v.as<float>();
      if (!(f >= 0.0f && f <= 10.0f)) continue;
      uint8_t w = (uint8_t)lroundf(f * 10.0f);
      /* Wstawianie z sortowaniem rosnaco po dacie: gdy wyjatkow jest
         wiecej niz miejsca, maja zostac te NAJBLIZSZE.                */
      int poz = 0;
      for (int j = 0; j < exN && j < DOSE_EX_MAX; j++) {
        if (exDni[j] >= d) break;
        poz = j + 1;
      }
      if (poz >= DOSE_EX_MAX) continue;
      for (int j = (exN < DOSE_EX_MAX ? exN : DOSE_EX_MAX - 1); j > poz; j--) {
        exDni[j] = exDni[j-1];
        exWar[j] = exWar[j-1];
      }
      exDni[poz] = d;
      exWar[poz] = w;
      if (exN < DOSE_EX_MAX) exN++;
    }
  }
  String dex = "";
  for (int i = 0; i < exN; i++) {
    if (dex.length()) dex += "|";
    dex += String((unsigned long)exDni[i]) + ":" + String((int)exWar[i]);
  }

  /* Zapis do pamieci trwalej tylko przy zmianie - NVS ma skonczona liczbe
     cykli, a config pobieramy przy kazdym polaczeniu.                  */
  String terazW = "", terazE = "";
  for (int i = 0; i < 7; i++) {
    if (terazW.length()) terazW += "|";
    terazW += String((int)rtcDoseWeek[i]);
  }
  bool brakRozpisania = true;
  for (int i = 0; i < 7; i++) if (rtcDoseWeek[i] != DOSE_NIEZNANA) brakRozpisania = false;
  if (brakRozpisania) terazW = "";
  for (uint8_t i = 0; i < rtcDoseExCount; i++) {
    if (terazE.length()) terazE += "|";
    terazE += String((unsigned long)rtcDoseExDay[i]) + ":" + String((int)rtcDoseExVal[i]);
  }
  if (dw != terazW || dex != terazE) {
    saveDosing(dw, dex);
    LOG("[DOS] nowe rozpisanie: %s  wyjatki: %s\n",
        dw.length()  ? dw.c_str()  : "(brak)",
        dex.length() ? dex.c_str() : "(brak)");
  }
}

/* Wysyla zaleglosci z kolejki - choćby z 10 dni bez internetu.
   Kazdy wpis kasujemy dopiero po potwierdzeniu HTTP 200, wiec zerwane
   polaczenie w polowie nie gubi danych. Przerwiemy tez, gdy czuwanie
   trwa juz za dlugo - reszta poleci przy nastepnym wybudzeniu.        */
/* Czy ten kod HTTP znaczy "ten wpis nie zostanie przyjety NIGDY"?
   400 - reguly odrzucily tresc, 413 - pakiet za duzy. Swiadomie NIE ma
   tu 401/403 (wygasly token - po odswiezeniu przejdzie), ani 404 (zly
   adres bazy odrzucalby CALA kolejke), ani 5xx i bledow sieci.        */
bool trwaleOdrzucony(int code) {
  return code == 400 || code == 413;
}

bool flushQueue() {
  String rec;
  int guard = QUEUE_CAPACITY + 2;
  while (guard-- > 0) {
    if (!queuePeek(rec)) {
      if (queueCount() == 0) return true;
      /* Licznik mowi, ze cos w kolejce jest, ale nie da sie tego
         odczytac. Nieczytelny wpis zostawiony na czele zatrzymalby
         wysylke na zawsze - a licznik nigdy nie zszedlby do zera.    */
      LOGLN("[QUE] wpis nieczytelny - zdejmuje");
      queueDrop();
      continue;
    }
    if (awakeTooLong()) {
      LOGLN("[QUE] limit czasu czuwania - dokoncze przy nastepnym polaczeniu");
      return false;
    }
    int code = pushEventRecord(rec);
    /* Kazdy wyslany wpis to dowod, ze siec dziala - wiec backoff wraca do
       zera. TU BYL BLAD (B19): rtcRetryCount zerowal sie WYLACZNIE w
       reportEvent(), czyli tylko przy otwarciu pudelka. Po kilku dniach bez
       internetu licznik stal na maksimum, a wtedy przerwa miedzy probami to
       cztery godziny. Gdy zaleglosci nie miescily sie w jednym czuwaniu
       (limit AWAKE_LIMIT_MS), reszta czekala kolejne cztery godziny mimo
       sprawnej sieci - i tak w kolko. Powrot do domu nie przyspieszal
       niczego, bo zadne z tych wybudzen nie przechodzilo przez
       reportEvent().                                                     */
    if (code == 200) { rtcRetryCount = 0; queuePop(); continue; }
    if (trwaleOdrzucony(code)) {
      LOG("[QUE] odrzucony na stale (HTTP %d) - zdejmuje: %s\n", code, rec.c_str());
      rtcRetryCount = 0;          // baza ODPOWIEDZIALA - siec dziala, tylko wpis byl zly
      queueDrop();
      continue;
    }
    return false;                 // chwilowa awaria - sprobujemy pozniej
  }
  return true;
}

/* =====================================================================
 *  8.  ZDARZENIA
 * ===================================================================== */
String makeRecordAt(const char* type, int slot, uint32_t ts) {
  char buf[80];
  snprintf(buf, sizeof(buf), "%lu;%s;%d;%.3f;%d",
           (unsigned long)ts, type, batteryPercentage, realBatteryVoltage, slot);
  return String(buf);
}

String makeRecord(const char* type, int slot) {
  return makeRecordAt(type, slot, rtcTimeValid ? (uint32_t)time(nullptr) : 0);
}

/* --- Znaczniki dobowe przezywajace RESET --------------------------------
   Pamiec RTC przezywa deep sleep, ale NIE przezywa resetu ani odlaczenia
   baterii. Gdyby "dawka juz wzieta dzisiaj" siedziala wylacznie tam, po
   kazdym restarcie pudelko zapominaloby o dzisiejszej dawce i pozwolilo
   zapisac ja drugi raz - a to jest wlasnie to, przed czym ma chronic.
   Dlatego oba znaczniki dublujemy w pamieci nieulotnej.                 */
void loadDayMarkers() {
  if (rtcTakenDay != 0 && rtcRolloverDay != 0) return;   // RTC ma komplet
  prefs.begin(NVS_NAMESPACE, true);
  if (rtcTakenDay == 0)    rtcTakenDay    = prefs.getULong("takenDay", 0);
  if (rtcRolloverDay == 0) rtcRolloverDay = prefs.getULong("rollDay", 0);
  if (rtcAlarmDoneDay == 0) {
    rtcAlarmDoneDay  = prefs.getULong("almDay", 0);
    /* Maska idzie ZAWSZE razem z dniem, nigdy osobno: dzien bez maski
       znaczylby "cos juz odzwoniono, ale nie wiadomo co", a wtedy nie da
       sie odroznic wyciszonego przypomnienia od czekajacego. */
    rtcAlarmDoneMask = prefs.getUShort("almMask", 0);
  }
  prefs.end();
  LOG("[DAY] odtworzono z pamieci: wzieta=%lu, rozliczona=%lu\n",
      (unsigned long)rtcTakenDay, (unsigned long)rtcRolloverDay);
}

/* Kasuje znaczniki doby - do testowania. Po tym pudelko znowu potraktuje
   najblizsze otwarcie jako pierwsza dzisiejsza dawke.                    */
void clearDayMarkers() {
  rtcTakenDay = 0;
  rtcRolloverDay = 0;
  rtcAlarmDoneDay = 0;
  rtcAlarmDoneMask = 0;
  prefs.begin(NVS_NAMESPACE, false);
  prefs.remove("takenDay");
  prefs.remove("rollDay");
  prefs.remove("almDay");
  prefs.remove("almMask");
  prefs.end();
  LOGLN("[DAY] znaczniki doby wyczyszczone (dawka, rozliczenie, odzwonione alarmy)");
}

void setTakenDay(uint32_t day) {
  if (rtcTakenDay == day) return;
  rtcTakenDay = day;
  prefs.begin(NVS_NAMESPACE, false);
  prefs.putULong("takenDay", day);
  prefs.end();
}

void setRolloverDay(uint32_t day) {
  if (rtcRolloverDay == day) return;
  rtcRolloverDay = day;
  prefs.begin(NVS_NAMESPACE, false);
  prefs.putULong("rollDay", day);
  prefs.end();
}

/* --- Rolowanie doby ----------------------------------------------------
   Wywolywane przy kazdym wybudzeniu z timera. Gdy zmienil sie dzien, a
   dawka z poprzedniej doby nigdy nie zostala zapisana, do kolejki trafia
   zdarzenie "missed" ze znacznikiem 23:59 tamtego dnia. NIE wlaczamy tu
   radia - wpis poleci przy najblizszym polaczeniu, a kalendarz w apce
   dostanie twarda informacje zamiast domyslania sie z braku danych.    */
/* Odnotowuje wziecie dawki. Dziala TAKZE bez wiarygodnego zegara.

   TU BYLA DZIURA, zglosil ja Kuba z wyjazdu.

   Wczesniej stalo tu `if (rtcTimeValid) setTakenDay(...)`, wiec pudelko
   bez internetu NIE ZAPISYWALO w ogole, ze dawka zostala wzieta - a co
   za tym idzie, przy ponownym otwarciu nie mialo na czym oprzec
   ostrzezenia. rtcTimeValid ustawia wylacznie syncTimeNTP(), wiec po
   resecie (wgranie programu przed wyjazdem, zanik zasilania) i bez sieci
   ochrona przed druga dawka NIE ISTNIALA. Objaw dokladnie taki, jak
   opisal: zamykasz wieczko, otwierasz ponownie i cisza.

   Znacznik czasu zapisujemy zawsze. Bez NTP nie jest to prawdziwa data,
   tylko licznik od startu plytki - ale ROZNICA miedzy dwoma odczytami
   jest poprawna, bo zegar RTC tyka takze w deep sleepie. A do pytania
   "czy brales to niedawno" wystarcza wlasnie roznica.               */
void zapiszDawke() {
  uint32_t teraz = (uint32_t)time(nullptr);
  rtcTakenTs = teraz ? teraz : 1;             // 0 znaczy "nie bylo dawki"
  if (rtcTimeValid) setTakenDay(localDayNumber((time_t)teraz));
}

/* Czy dawka na te dobe zostala juz zapisana.

   Dwie drogi, bo dwie sytuacje:
     - zegar wiarygodny -> porownujemy NUMER DOBY. Pewna odpowiedz.
     - zegar nieznany   -> patrzymy, ile czasu minelo od ostatniej dawki.
       To tylko podejrzenie: nie wiemy, gdzie przebiega granica doby,
       wiec przy braniu o 23:00 i nastepnym o 10:00 wyjdzie "juz brane",
       choc to nowa doba. Dlatego wolajacy MUSI to rozroznic - sygnal
       ostrzegawczy tak, ale wyrzucenie otwarcia juz nie.              */
/* Czy alarm dla tej doby zostal juz odegrany do konca.

   TU BYL BLAD, zglosil go Kuba z wyjazdu: "pika 3 raz od 18:30, o 19:30
   tez, a tabletke juz wzialem".

   Po wyczerpaniu MAX_ALARM_RETRIES prob alarm zapisywal "missed" i czyscil
   rtcPendingSlot - ale NIE zostawial sladu, ze dla tej doby juz zadzwonil.
   Tymczasem matchSlot() dopasowuje pore leku w oknie +/-MATCH_WINDOW_MIN,
   czyli dla przypomnienia o 20:00 zwraca slot 0 przez CALE trzy godziny,
   od 18:30 do 21:30. Kazde wybudzenie w tym oknie zaczynalo wiec caly
   alarm od nowa.

   A wybudzen w tym oknie jest sporo, gdy pudelko nie ma sieci:
   planNextSleep() budzi je do ponawiania wysylki kolejki po 15, 30, 60,
   120 i 240 minutach. Stad dzwonienie o 18:30, potem o 19:30 i dalej.

   Z SIECIA TEGO NIE WIDAC: kolejka jest pusta, wiec nie ma po co budzic
   pudelka w srodku okna i alarm odzywa sie doklandie raz. Blad ujawnia sie
   wylacznie po dluzszym czasie offline - czyli wtedy, gdy pudelko ma byc
   najbardziej samodzielne.

   NAPRAWA D23 MIALA SKUTEK UBOCZNY, ktory znalazl sie dopiero teraz (D56):
   znacznik byl na CALA DOBE, wiec pierwsze przypomnienie, poddajac sie,
   wyciszalo takze wszystkie nastepne. Druga pozycja w harmonogramie znaczy
   "przypomnij jeszcze raz o 23:00" (CLAUDE.md 4b) i nie odzywala sie
   dokladnie wtedy, kiedy jest potrzebna - gdy pierwsze przypomnienie
   przeszlo bez odzewu. Dlatego znacznik jest teraz MASKA BITOWA slotow:
   ten sam slot nie wraca (D23 zachowane co do joty), ale kolejny - owszem. */
void oznaczAlarmObsluzony(int slot) {
  uint32_t teraz = (uint32_t)time(nullptr);
  rtcAlarmDoneTs = teraz ? teraz : 1;
  if (rtcTimeValid) {
    uint32_t dzis = localDayNumber((time_t)teraz);
    /* Nowa doba kasuje maske. Bez tego wczorajsze bity wyciszalyby
       dzisiejsze przypomnienia o tych samych numerach.                */
    if (rtcAlarmDoneDay != dzis) { rtcAlarmDoneDay = dzis; rtcAlarmDoneMask = 0; }
    if (slot >= 0 && slot < 16) rtcAlarmDoneMask |= (uint16_t)(1u << slot);
    else                        rtcAlarmDoneMask = 0xFFFF;   // slot nieznany: cisza na cala dobe
    prefs.begin(NVS_NAMESPACE, false);
    prefs.putULong("almDay", rtcAlarmDoneDay);   // jak setTakenDay obok
    prefs.end();
    nvsPutU16("almMask", rtcAlarmDoneMask);
  }
}

bool alarmJuzObsluzony(int slot) {
  if (rtcTimeValid) {
    if (rtcAlarmDoneDay == 0 || rtcAlarmDoneDay != localDayNumber(time(nullptr))) return false;
    if (slot < 0 || slot >= 16) return rtcAlarmDoneMask != 0;
    return (rtcAlarmDoneMask & (uint16_t)(1u << slot)) != 0;
  }
  /* Bez zegara nie wiemy nawet, ktora jest godzina - tym bardziej, ktore
     to przypomnienie. Zostaje stara, czasowa ochrona przed powtorka:
     ostrozniej znaczy tu ciszej, bo drugie dzwonienie po omacku byloby
     zgadywaniem, a matchSlot() bez zegara i tak zwraca -1.            */
  if (rtcAlarmDoneTs == 0) return false;
  uint32_t teraz = (uint32_t)time(nullptr);
  return teraz >= rtcAlarmDoneTs && (teraz - rtcAlarmDoneTs) < (uint32_t)ONE_DOSE_WINDOW_S;
}

/* Ktore przypomnienie jest OSTATNIA szansa w tej dobie lekowej.

   Potrzebne, bo "missed" wolno zglosic dopiero wtedy, gdy dzis nie ma juz
   nic po nim. Zglaszane po pierwszym przypomnieniu malowaloby dzien na
   czerwono o 20:00, choc o 23:00 pudelko ma jeszcze raz zapytac - i gdyby
   tabletka poszla o 22:00, aplikacja musialaby ten wpis odkrecac (rodzina
   B27/B29: wpisy "missed" scigajace sie z prawdziwa dawka).

   Kolejnosc liczona w RAMIE DOBY LEKOWEJ, nie na zegarze: przy godzinach
   ["23:00","07:30"] ostatnia szansa to 23:00, bo 07:30 nastepnego dnia
   nalezy juz do kolejnej doby.                                        */
int ostatniSlotDoby() {
  if (slotCount <= 0) return -1;
  int best = 0, bestRama = -1;
  for (int i = 0; i < slotCount; i++) {
    int rama = ((slotMinutes(i) - DAY_START_HOUR * 60) % 1440 + 1440) % 1440;
    if (rama > bestRama) { bestRama = rama; best = i; }
  }
  return best;
}

bool juzDzisBrane() {
  if (rtcTimeValid) return rtcTakenDay != 0 && rtcTakenDay == localDayNumber(time(nullptr));
  if (rtcTakenTs == 0) return false;
  uint32_t teraz = (uint32_t)time(nullptr);
  return teraz >= rtcTakenTs && (teraz - rtcTakenTs) < (uint32_t)ONE_DOSE_WINDOW_S;
}

void checkDayRollover() {
  if (!rtcTimeValid) return;
  uint32_t today = localDayNumber(time(nullptr));

  if (rtcRolloverDay == 0) { setRolloverDay(today); return; }    // pierwszy raz
  if (rtcRolloverDay == today) return;                           // ta sama doba

  /* TU GINELY CALE DNI.

     Wczesniej powstawal DOKLADNIE JEDEN wpis "missed", ze znacznikiem
     wczoraj 23:59, po czym rtcRolloverDay skakal od razu na dzis. Gdy
     pudelko stalo kilka dni - rozladowane ogniwo, wyjazd, wyjete z
     ladowarki - dni posrednie nie dostawaly nic. W kalendarzu wygladaly
     jak "brak danych", a nie jak "nie wziete", czyli dokladnie odwrotnie
     niz w rzeczywistosci.

     Teraz domykamy KAZDA otwarta dobe od rtcRolloverDay do wczoraj.
     Kolejnosc jest od najstarszej, zeby kolejka zostala FIFO.

     Limit MAX_ROLLOVER_DNI istnieje po to, zeby pudelko wracajace po
     miesiacu nie wystrzelilo 30 wpisow naraz w 120-elementowa kolejke,
     wypychajac z niej prawdziwe dawki. Dluzsza przerwa i tak znaczy, ze
     urzadzenie bylo martwe, a nie ze ktos nie wzial leku.            */
  time_t now = time(nullptr);
  time_t polnocDzis = now - (time_t)localMinutesOfDay(now) * 60;

  int doZamkniecia = 0;
  for (int wstecz = 1; wstecz <= MAX_ROLLOVER_DNI; wstecz++) {
    uint32_t ts = (uint32_t)(polnocDzis - (time_t)(wstecz - 1) * 86400 - 60);
    if (localDayNumber((time_t)ts) < rtcRolloverDay) break;
    doZamkniecia = wstecz;
    if (localDayNumber((time_t)ts) == rtcRolloverDay) break;
  }

  for (int wstecz = doZamkniecia; wstecz >= 1; wstecz--) {
    uint32_t ts = (uint32_t)(polnocDzis - (time_t)(wstecz - 1) * 86400 - 60);
    uint32_t doba = localDayNumber((time_t)ts);
    if (doba < rtcRolloverDay) continue;
    if (doba == rtcTakenDay) {
      LOG("[DAY] doba %lu zamknieta poprawnie\n", (unsigned long)doba);
      continue;
    }
    /* Doba rozpisana BEZ leku nie jest doba pominieta. Bez tego kazdy dzien
       odstawienia przed zabiegiem trafialby do kalendarza jako czerwony,
       a stamtad do raportu dla lekarza jako zaniżona skutecznosc.
       dawkaNaDobe() zwraca zero wylacznie wtedy, gdy wiemy to na pewno.  */
    if (dawkaNaDobe(doba, localWeekday((time_t)ts)) == 0) {
      LOG("[DAY] doba %lu rozpisana bez leku - nie zglaszam\n", (unsigned long)doba);
      continue;
    }
    queuePush(makeRecordAt("missed", slotCount == 1 ? 0 : -1, ts));
    LOG("[DAY] doba %lu zamknieta bez dawki -> missed\n", (unsigned long)doba);
  }
  setRolloverDay(today);
}

/* Probuje wyslac online; jesli sie nie uda - do NVS. */
void reportEvent(const char* type, int slot) {
  String rec = makeRecord(type, slot);

  if (wifiConnect()) {
    /* wifiConnect() sam synchronizuje zegar. Jesli zdarzenie powstalo, zanim
       znalismy czas (ts=0), odtwarzamy je teraz z prawidlowym znacznikiem. */
    if (rtcTimeValid && rec.startsWith("0;")) rec = makeRecord(type, slot);

    if (firebaseSignIn()) {
      /* Najpierw sam stan wieczka - to jedno male zapytanie, ktore
         zapala wskaznik w telefonie. Reszta moze poczekac.          */
      pushLidState();
      fetchConfig();
      flushQueue();
      if (pushEventRecord(rec) == 200) {
        note("wyslane");
        pushStatus();
        rtcRetryCount = 0;                 // sukces - kasujemy backoff
        beepAck();
        if (strcmp(type, "open") == 0) {
          if (rtcPillsLeft >= 0 && rtcPillsLeft < LOW_STOCK_WARN) {
            delay(300); beepLowStock();    // "konczy sie opakowanie"
          }
          if (batteryPercentage <= BATT_WARN_PCT) {
            delay(300);
            beepLowBattery(batteryPercentage <= BATT_CRIT_PCT);
          }
        }
        return;
      }
    }
  }
  /* Brak sieci albo blad serwera: zdarzenie ląduje w pamieci nieulotnej,
     a rtcRetryCount steruje tym, jak szybko sprobujemy ponownie.       */
  /* WYNIK queuePush() DECYDUJE O DZWIEKU. Nie wolno go zignorowac.

     TU BYL BLAD B24, i zglosil go Kuba jednym zdaniem: "za kazdym razem
     byl charakterystyczny dzwiek, jakby probowal wyslac, ale mu sie nie
     udalo". Ten dzwiek - beepQueued() - znaczy "zapisalem u siebie, wysle
     pozniej". Grał ZAWSZE, bo wynik queuePush() leciał do kosza.

     A queuePush() od naprawy B5 potrafi zwrocic false: gdy zapis do NVS
     nie przeszedl, tresc NIE zostaje nigdzie, tylko licznik sie nie
     podnosi. Pudelko meldowalo wtedy "mam to u siebie" o dawce, ktorej
     nie mialo. To najgorszy mozliwy rodzaj klamstwa w tym urzadzeniu:
     czlowiek slyszy potwierdzenie, wychodzi z domu i jest spokojny.

     Teraz nieudany zapis ma WLASNY sygnal - podwojny blad, wyraznie
     inny od pojedynczego potwierdzenia kolejki. "Nie mam tej dawki
     nigdzie, zapisz ja recznie."                                      */
  const bool zapisane = queuePush(rec);
  if (rtcRetryCount < 200) rtcRetryCount++;
  /* Otwarcie pudelka jest zawsze potwierdzane dzwiekiem - takze wtedy, gdy
     nie udalo sie go wyslac. Inaczej brak sieci wyglada dokladnie tak samo
     jak zepsute urzadzenie, a to dwie zupelnie rozne sytuacje.           */
  if (!zapisane)                        note("DAWKA NIEZAPISANA");
  else if (WiFi.status() == WL_CONNECTED) note("baza nie odpowiada");
  else                                    note("brak wifi");

  if (!zapisane)                      { beepErr(); delay(250); beepErr(); }
  else if (strcmp(type, "open") == 0) beepQueued();
  else                                beepErr();

  if (zapisane)
    LOG("[EVT] '%s' zapisane w pamieci, w kolejce %u zdarzen\n", type, queueCount());
  else
    LOG("[EVT] '%s' NIE ZAPISANE - pamiec odmowila, dawka przepadla\n", type);
}

/* =====================================================================
 *  9.  ALARM
 * ===================================================================== */
/* Dzwoni przez ALARM_WINDOW_S i sprawdza czy pudelko zostalo otwarte.
   Zwraca true jesli otwarto.                                          */
/* Czy stan wieczka w oknie alarmu liczy sie jako POTWIERDZENIE dawki.

   Wydzielone z runAlarmWindow() wylacznie po to, zeby dalo sie to
   przetestowac. Sama petla to czyste we/wy - budzik, delay() i odczyt
   pinu - wiec nie da sie jej uruchomic w tescie. Decyzja da sie.

   ZACHOWANIE JEST DZISIAJ NIEZMIENIONE: wystarczy, ze wieczko JEST
   otwarte. I to jest dokladnie sedno bledu B1 - pytamy "czy jest
   otwarte", a nie "czy ktos je otworzyl". Przy przesunietym magnesie
   pudelko codziennie zapisuje dawke, ktorej nie bylo, i nie dzwoni.

   Trzy warianty naprawy (DECYZJE, B1) sa teraz zmiana JEDNEJ linii
   w tym miejscu, z gotowym zestawem testow obok:
     A: return otwarteTeraz && !byloOtwarteNaStarcie;
     B: potwierdzeniem jest zamkniecie, gdy bylo otwarte na starcie
     C: nie ufaj wieczku otwartemu od dawna (rtcOpenSinceTs)
   Ktory wariant - o tym decyduje pomiar z dziennika wieczka, nie ten
   komentarz. Do tego czasu zostaje stan obecny.                      */
bool alarmPotwierdzony(bool otwarteTeraz, bool byloOtwarteNaStarcie) {
  (void)byloOtwarteNaStarcie;     // uzywaja go warianty A i B naprawy B1
  return otwarteTeraz;
}

bool runAlarmWindow() {
  /* Krytycznie niska bateria: zew ladowania poprzedza alarm o leku, zebys
     dowiedzial sie o tym takze wtedy, gdy akurat nie otwierasz pudelka. */
  if (batteryPercentage <= BATT_CRIT_PCT) { beepLowBattery(true); delay(400); }

  const bool byloOtwarteNaStarcie = boxIsOpen();

  buzzerInit();
  uint32_t deadline = millis() + (uint32_t)ALARM_WINDOW_S * 1000UL;

  while (millis() < deadline) {
    for (int b = 0; b < BEEPS_PER_BURST; b++) {
      buzzerTone(BUZZER_FREQ_HZ);
      delay(BEEP_MS);
      buzzerTone(0);
      delay(120);
      if (alarmPotwierdzony(boxIsOpen(), byloOtwarteNaStarcie)) { buzzerOff(); return true; }
    }
    uint32_t gapEnd = millis() + BURST_GAP_MS;
    while (millis() < gapEnd) {
      if (alarmPotwierdzony(boxIsOpen(), byloOtwarteNaStarcie)) { buzzerOff(); return true; }
      delay(50);
    }
  }
  buzzerOff();
  return false;
}

/* =====================================================================
 * 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)
 *
 *      Pudelko tworzy wlasna siec WiFi. Laczysz sie z nia telefonem,
 *      automatycznie otwiera sie strona, wybierasz swoja siec z listy
 *      i wpisujesz haslo. Zadnej dodatkowej aplikacji.
 *
 *      DLACZEGO NIE BLUETOOTH: kontroler BLE w rdzeniu arduino-esp32 3.3.x
 *      wywala sie na ESP32-C3 juz przy inicjalizacji (HLI Magic mismatch,
 *      Memory protection fault) i restartuje plytke w petli. Portal przez
 *      WiFi nie ma tej zaleznosci, dziala z kazdego telefonu i zajmuje
 *      mniej pamieci.
 * ===================================================================== */
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

static String portalPage(int found) {
  String o = F("<!doctype html><html lang=pl><meta charset=utf-8>"
      "<meta name=viewport content='width=device-width,initial-scale=1'>"
      "<title>PillBox</title><style>"
      "body{font-family:-apple-system,BlinkMacSystemFont,sans-serif;background:#0b1220;"
      "color:#e8eefc;padding:26px 20px;max-width:420px;margin:0 auto}"
      "h2{font-size:20px;margin:0 0 4px}p{color:#8ea0c0;font-size:14px;margin:0 0 20px}"
      "label{font-size:12px;color:#61748f;text-transform:uppercase;letter-spacing:.06em}"
      "select,input{width:100%;padding:13px;margin:6px 0 16px;border-radius:11px;"
      "border:1px solid #26344f;background:#1d2942;color:#e8eefc;font-size:16px;"
      "box-sizing:border-box;-webkit-appearance:none}"
      "button{width:100%;padding:15px;border:0;border-radius:11px;background:#2dd4bf;"
      "color:#06322c;font-size:16px;font-weight:600}</style>"
      "<h2>PillBox</h2><p>Wybierz swoja siec WiFi.</p>"
      "<form action='/save' method='POST'><label>Siec</label><select name='s'>");
  for (int i = 0; i < found; i++) {
    String e = htmlEscape(WiFi.SSID(i));
    o += "<option value=\"" + e + "\">" + e + "  (" + String((int)WiFi.RSSI(i)) + " dBm)</option>";
  }
  o += F("</select><label>Haslo</label><input name='p' type='password' autocomplete='off'>");

  /* Haslo urzadzenia do Firebase - pole pojawia sie TYLKO wtedy, gdy
     pamiec trwala go nie ma (D59).

     Po co: od 1.38.0 binarke aktualizacji buduje automat z repo, wiec nie
     ma w niej hasla - ono siedzi w pamieci pudelka. Gdyby ta pamiec
     kiedys przepadla (awaria NVS, "Erase All Flash" przy wgrywaniu),
     pudelko nie mialoby czym zalogowac sie do bazy, a bez bazy nie ma
     zdalnej drogi, zeby mu to haslo podac. Zostawalby kabel.

     Portal fizyczny zostaje na zawsze (zasada 9), wiec to wlasnie on jest
     wlasciwym miejscem na taka droge powrotna. Pole jest ukryte, dopoki
     haslo siedzi w pamieci - zeby nie kusilo do wpisywania czegokolwiek
     przy zwyklej zmianie sieci.                                        */
  if (!hasloWPamieci())
    o += F("<label>Haslo urzadzenia (Firebase)</label>"
           "<input name='d' type='password' autocomplete='off'>"
           "<p style='margin:-8px 0 16px;font-size:12px'>Pudelko nie ma zapisanego "
           "hasla do bazy. Bez niego polaczy sie z WiFi, ale nie z aplikacja.</p>");

  o += F("<button type=submit>Polacz</button></form>");
  return o;
}

void startWifiPortal() {
  /* Trzy pikniecia = "jestem w trybie konfiguracji" */
  buzzerInit();
  for (int i = 0; i < 3; i++) { buzzerTone(2200); delay(90); buzzerTone(0); delay(90); }
  buzzerOff();

  WiFi.persistent(true);
  WiFi.disconnect(true, false);
  WiFi.mode(WIFI_AP_STA);

  /* Skan robimy PRZED uruchomieniem punktu dostepowego - pozniej bylby
     wolniejszy i potrafi zrywac polaczenie telefonu.                   */
  int found = WiFi.scanNetworks();
  if (found < 0) found = 0;
  LOG("[AP ] znaleziono %d sieci\n", found);

  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress ip = WiFi.softAPIP();
  LOG("[AP ] siec '%s', otworz http://%s\n", AP_SSID, ip.toString().c_str());

  DNSServer dns;
  dns.start(53, "*", ip);            // kazda domena -> nasza strona (captive portal)
  WebServer server(80);

  bool done = false;
  String pendingSsid, pendingPass, pendingDevPass;

  server.on("/", [&]() { server.send(200, "text/html; charset=utf-8", portalPage(found)); });

  server.on("/save", HTTP_POST, [&]() {
    pendingSsid = server.arg("s");
    pendingPass = server.arg("p");
    pendingDevPass = server.arg("d");   // puste, gdy pole bylo ukryte
    server.send(200, "text/html; charset=utf-8",
      F("<!doctype html><meta charset=utf-8><meta name=viewport content='width=device-width'>"
        "<body style='font-family:-apple-system,sans-serif;background:#0b1220;color:#e8eefc;"
        "padding:40px 24px;text-align:center'>"
        "<h2>Lacze sie...</h2><p style='color:#8ea0c0'>Mozesz zamknac to okno.<br>"
        "Pudelko potwierdzi dzwiekiem.</p>"));
    done = true;
  });

  /* iOS sprawdza polaczenie pod losowymi adresami - kazdy odsylamy na strone. */
  server.onNotFound([&]() {
    server.sendHeader("Location", String("http://") + ip.toString(), true);
    server.send(302, "text/plain", "");
  });

  server.begin();

  /* Zamkniecie wieczka konczy parowanie.

     Portal wlacza sie po otwarciu pudelka, wiec zamkniecie go jest
     naturalnym "juz koniec" - nie trzeba nic pamietac ani odliczac pieciu
     minut z punktem dostepowym swiecacym na pol mieszkania. Punkt dostepowy
     to najdrozszy tryb pracy tego ukladu, wiec kazda sekunda krocej to
     realna oszczednosc.

     Warunek jest napisany tak, zeby dzialal takze wtedy, gdy portal
     wystartowal przy ZAMKNIETYM pudelku (np. brak zapisanego WiFi po
     pierwszym wlaczeniu). Wtedy przejscia otwarte->zamkniete po prostu
     nie ma i zostaje zwykly limit czasu.                               */
  const bool bylOtwarty = boxIsOpen();
  bool zamknietoWieczko = false;

  uint32_t t0 = millis();
  while (!done && millis() - t0 < (uint32_t)PORTAL_TIMEOUT_S * 1000UL) {
    dns.processNextRequest();
    server.handleClient();

    if (bylOtwarty && !boxIsOpen()) {
      /* Krotkie odbicie styku nie moze przerwac wpisywania hasla. */
      delay(120);
      if (!boxIsOpen()) { zamknietoWieczko = true; break; }
    }
    delay(5);
  }

  if (zamknietoWieczko) {
    LOGLN("[AP ] wieczko zamkniete - koncze parowanie");
    note("parowanie przerwane");
    /* Dwa opadajace tony: "zamykam sklep". Latwe do odroznienia od
       trzech wznoszacych, ktore oznaczaly wejscie w tryb konfiguracji. */
    buzzerInit();
    buzzerTone(3000); delay(120);
    buzzerTone(2300); delay(200);
    buzzerOff();
  }

  server.stop();
  dns.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);

  if (!done) {
    LOGLN("[AP ] timeout - nikt sie nie polaczyl");
    beepErr();
    return;
  }

  /* Konfiguracja moglaby pochlonac caly limit czuwania - dajemy zapas. */
  extendAwake(90000);
  LOG("[AP ] laczenie z siecia '%s'...\n", pendingSsid.c_str());
  WiFi.begin(pendingSsid.c_str(), pendingPass.c_str());

  uint32_t t1 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t1 < WIFI_TIMEOUT_MS + 10000UL) delay(200);

  if (WiFi.status() != WL_CONNECTED) {
    LOGLN("[AP ] nie udalo sie - zle haslo albo siec 5 GHz");
    beepErr(); delay(300); beepErr();
    return;
  }

  LOG("[AP ] polaczono, IP=%s\n", WiFi.localIP().toString().c_str());
  /* Siec z portalu ida na liste tak samo jak siec dodana z aplikacji -
     jedno zrodlo prawdy. Wczesniej portal zostawial ja wylacznie
     w pamieci sterownika, wiec kazda nastepna siec kasowala poprzednia. */
  wifiSiecDodaj(pendingSsid, pendingPass);
  syncTimeNTP();
  timeSyncedThisWake = true;

  /* Haslo urzadzenia podane w portalu jest KANDYDATEM, nie prawda (D59).
     Zapisujemy je, probujemy zalogowac - i jesli baza je odrzuci,
     kasujemy z powrotem. Zostawione blokowaloby na stale to poprawne
     z config.h przy nastepnym wgraniu kablem, czyli literowka w portalu
     kosztowalaby cala droge powrotna.                                  */
  bool haslozPortalu = false;
  if (pendingDevPass.length() && !hasloWPamieci()) {
    if (hasloUtrwal(pendingDevPass)) {
      haslozPortalu = true;
      LOGLN("[AP ] haslo urzadzenia zapisane - sprawdzam je w bazie");
    } else {
      LOGLN("[AP ] haslo urzadzenia NIE zapisalo sie do pamieci");
    }
  }

  if (firebaseSignIn()) { fetchConfig(); flushQueue(); pushStatus(); }
  else if (haslozPortalu) {
    prefs.begin(NVS_NAMESPACE, false);
    prefs.remove("fbpass");
    prefs.end();
    LOGLN("[AP ] baza odrzucila to haslo - skasowane, sprobuj jeszcze raz");
    beepErr();
  }
  beepAck();
}

/* =====================================================================
 * 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59
 *
 *  Binarke buduje automat na GitHubie i kladzie ja obok aplikacji na
 *  GitHub Pages. Pudelko pobiera najpierw MALY plik z opisem, a caly
 *  program dopiero wtedy, gdy suma kontrolna rozni sie od tej, ktora juz
 *  ma. To jest odpowiedz na "zeby nie pobierala sie ta sama wersja":
 *  rozstrzyga SUMA, nie numer wersji. Numer stoi w config.h i pisze go
 *  czlowiek, wiec da sie go zapomniec podbic; suma liczy sie z pliku
 *  i skłamac nie umie.
 *
 *  CZEGO SIE TU PILNUJE, PO KOLEI:
 *
 *  1. Dawka jest wazniejsza od aktualizacji. OTA rusza dopiero po tym,
 *     jak wszystko inne zostalo zrobione i potwierdzone - i nigdy przy
 *     niepustej kolejce. Zaleglosci maja dojechac pierwsze.
 *  2. Jedna proba, nie "az sie uda". Niedostepny plik przy zasadzie
 *     "probuj do skutku" znaczylby wlaczone radio przy KAZDYM otwarciu
 *     wieczka, codziennie. Licznik w NVS i doba przerwy to zamykaja.
 *  3. Wersja, ktora sie nie uruchomila, trafia na czarna liste i nie
 *     jest juz nigdy pobierana. Bez tego pudelko sciagaloby ten sam
 *     zepsuty program w kolko.
 *  4. Bez hasla w pamieci trwalej nie ma aktualizacji. Binarka z automatu
 *     nie zna hasla, wiec wgranie jej pudelku, ktore tez go nie zna,
 *     odcieloby je od bazy - a to jedyna droga naprawy bez kabla.
 * ===================================================================== */
#if OTA_ENABLED

/* --- CALA DECYZJA W JEDNYM MIEJSCU, NA SAMYCH ARGUMENTACH -------------
   Bez odczytow z NVS, bez WiFi, bez zegara sprzetowego - dzieki temu
   testy potrafia ja URUCHOMIC, a nie tylko przeczytac wyrazeniem
   regularnym. Ta sama lekcja co przy `alarmPotwierdzony()` (D15b):
   decyzja wyjeta z petli daje sie sprawdzic, petla nie.

   `sumaZdalna` to suma z opisu na serwerze, `sumaLokalna` - suma tego,
   co siedzi w pudelku teraz, `sumaZla` - wersja z czarnej listy.       */
OtaDecyzja otaDecyzja(bool hasloJest, int wKolejce, int battPct, bool naLadowarce,
                      uint8_t nieudane, uint32_t teraz, uint32_t ostatniaProba,
                      uint32_t rozmiar, uint32_t tsZlecenia,
                      const String& sumaZdalna, const String& sumaLokalna,
                      const String& sumaZla) {
  /* Opis pobrany ze smieci albo ze strony bledu 404 zapisanej jako plik.
     Suma MD5 ma dokladnie 32 znaki - krotsza nie jest suma.            */
  if (sumaZdalna.length() != 32) return OTA_ZLY_OPIS;
  if (rozmiar < OTA_MIN_BIN_SIZE || rozmiar > OTA_MAX_BIN_SIZE) return OTA_ZLY_OPIS;

  /* SUMY Z BAZY TU JUZ NIE MA - i to jest swiadome cofniecie (D59).

     Stalo tu porownanie sumy z opisu z suma przyslana przez aplikacje,
     opisane jako "dwa kanaly musza powiedziec to samo". Mialo bronic przed
     podmiana pliku przy `setInsecure()`.

     TA OBRONA BYLA ILUZORYCZNA. Pudelko laczy sie BEZ weryfikacji
     certyfikatu takze z Firebase (`rtdbClient.setInsecure()`), wiec kto
     potrafi podmienic plik z GitHuba, potrafi rownie dobrze podmienic
     odpowiedz bazy. Dwa kanaly, ta sama dziura - zero bezpieczenstwa
     w zamian za realny koszt.

     A koszt byl codzienny: suma w zleceniu dotyczy KONKRETNEJ wersji,
     wiec kazda nowa publikacja uniewazniala zlecenie zlozone chwile
     wczesniej. Kuba spedzil na tym caly wieczor.

     Przed uszkodzonym pobraniem nadal chroni `Update.setMD5()` z sumy
     podanej w opisie - to lapie zerwany transfer, czyli ryzyko, ktore
     naprawde wystepuje. Prawdziwa obrona przed podmiana to `setCACert()`
     i to jest jedyny uczciwy sposob; zostaje jako znany dlug, wpisany
     w "Czego nie zweryfikowano".                                      */

  /* Najpierw to, co konczy sprawe bez zadnego kosztu. "Nic nowego" jest
     pierwsze, bo to najczestsza odpowiedz w codziennej pracy - pudelko
     pyta o opis przy kazdej okazji i prawie zawsze slyszy "masz aktualne". */
  if (sumaZdalna == sumaLokalna) return OTA_NIC_NOWEGO;
  if (sumaZla.length() == 32 && sumaZdalna == sumaZla) return OTA_ZEPSUTA;

  if (!hasloJest)   return OTA_BEZ_HASLA;
  if (wKolejce > 0) return OTA_KOLEJKA;

  /* Na ladowarce prad jest za darmo (D8), wiec prog baterii nie ma tam
     sensu. Poza ladowarka pilnujemy go, bo aktualizacja to okolo 1-2 mAh
     - grosze, ale na wyczerpanym ogniwie kazdy grosz jest pozyczka.    */
  if (!naLadowarce && battPct >= 0 && battPct < OTA_MIN_BATT_PCT) return OTA_BATERIA;

  /* SWIEZE ZLECENIE ZNOSI "PODDALEM SIE".

     TU BYLA LUKA, ktora Kuba zobaczyl wprost: aplikacja pisala "Nacisnij
     przycisk jeszcze raz, zeby zlecic od nowa", on naciskal - i pudelko
     dalej odmawialo, bo licznik nieudanych prob zostawal na trzech.
     Rada na ekranie byla wiec nieprawdziwa: nacisniecie NIC nie zmienialo.

     Licznik chroni przed pudelkiem probujacym w kolko SAMO z siebie.
     Kiedy czlowiek prosi PONOWNIE - juz po tych nieudanych probach -
     to nie jest petla, tylko swiadoma decyzja, i ma prawo dostac swoja
     szanse. `tsZlecenia > ostatniaProba` odroznia te dwie sytuacje
     dokladnie: zlecenie zlozone PO ostatniej probie jest nowe.        */
  const bool swiezeZlecenie = tsZlecenia && tsZlecenia > ostatniaProba;
  if (!swiezeZlecenie && nieudane >= OTA_MAX_FAILS) return OTA_PODDANO;

  /* Odstep liczymy tylko z wiarygodnym zegarem. Bez niego `teraz` jest
     zerem i wtedy WOLNO probowac - inaczej pudelko bez zsynchronizowanego
     czasu nie zaktualizowaloby sie nigdy.                              */
  /* DOBOWEGO ODSTEPU JUZ NIE MA - zniesiony na zadanie Kuby (D59):
     "znies to ze trzeba dobe czekac, czy widziales gdzies zeby jakies
     urzadzenie tak mialo".

     Mial racje, i to z dwoch powodow. Po pierwsze zaden sprzet tak sie nie
     zachowuje: gdy czlowiek prosi o aktualizacje, dostaje ja albo dostaje
     blad - nie "wroc jutro". Po drugie odstep miał chronic przed petla
     ponowien, ale ta petla i tak nie moze powstac: aktualizacja rusza
     WYLACZNIE na jawne zlecenie z aplikacji, zlecenie kasuje sie po
     wykonaniu, a czlowiek moze je odwolac przyciskiem w kazdej chwili.
     Przed pudelkiem probujacym w kolko chroni licznik OTA_MAX_FAILS -
     i to on jest wlasciwym narzedziem, bo liczy NIEPOWODZENIA, a nie
     czas.

     `ostatniaProba` i `tsZlecenia` zostaly w sygnaturze i nadal cos znacza,
     ale juz tylko WPUSZCZAJA - porownanie tych dwoch wartosci wyzej znosi
     "poddalem sie" po swiezym zleceniu. Zadna z nich niczego nie blokuje.
     `teraz` nie sluzy juz do niczego procz zapisu chwili proby.        */
  (void)teraz;

  return OTA_ROB;
}

/* Krotki opis decyzji dla aplikacji i logu. */
const char* otaOpisDecyzji(OtaDecyzja d) {
  switch (d) {
    case OTA_ROB:         return "pobieram";
    case OTA_NIC_NOWEGO:  return "aktualne";
    case OTA_BEZ_HASLA:   return "brak hasla w pamieci pudelka";
    case OTA_KOLEJKA:     return "czekam na wyslanie zaleglych dawek";
    case OTA_BATERIA:     return "za malo baterii - postaw na ladowarke";
    case OTA_PODDANO:     return "trzy proby bez skutku - poddalem sie";
    case OTA_ZEPSUTA:     return "ta wersja juz raz nie wstala";
    case OTA_ZLY_OPIS:    return "opis wersji na serwerze jest niepoprawny";
  }
  return "nieznany stan";
}

/* --- Pamiec trwala aktualizacji --------------------------------------
   "otaMd5" - suma programu, ktory dziala teraz. "otaBad" - suma wersji,
   ktora sie nie uruchomila. "otaFail" - ile prob z rzedu spalilo. "otaTs"
   - kiedy byla ostatnia. "otaPend" - suma wersji wgranej, ale jeszcze
   niepotwierdzonej. "otaBoot" - ile razy taka wersja startowala.       */
/* Zapis proby PRZED pobraniem, nie po nim.

   To jest ta sama ostroznosc co przy kolejce (zasada 6), tylko odwrocona:
   tam nie kasujemy przed potwierdzeniem, tu LICZYMY probe, zanim cokolwiek
   zrobimy. Gdyby licznik rosl dopiero po nieudanej probie, aktualizacja
   ktora zawiesza pudelko w polowie nie zwiekszylaby go nigdy - i pudelko
   wchodziloby w to samo zawieszenie przy kazdym otwarciu wieczka.      */
void otaZanotujProbe(uint32_t teraz) {
  prefs.begin(NVS_NAMESPACE, false);
  uint16_t ile = prefs.getUShort("otaFail", 0);
  nvsPutU16("otaFail", ile + 1);
  if (teraz) prefs.putUInt("otaTs", teraz);
  prefs.end();
}

void otaWyzerujLicznik() {
  prefs.begin(NVS_NAMESPACE, false);
  nvsPutU16("otaFail", 0);
  prefs.end();
}

/* CZY W BAZIE STOI ZLECENIE - pytamy TU, tuz przed proba.

   TU BYLA DZIURA, przez ktora "kliknij i pudelko przyjmie" nie dzialalo,
   i to niezaleznie od tego, co robilo samo pobieranie.

   Wygladalo to tak: `rtcOtaProsba` ustawia sie WYLACZNIE w `fetchConfig()`,
   a `fetchConfig()` leci na POCZATKU wybudzenia. Aktualizacja rusza na
   KONCU, z `goToSleep()`. Miedzy jednym a drugim mija cale wybudzenie.
   Jesli czlowiek nacisnal przycisk w tej wlasnie chwili - albo po prostu
   ulamek sekundy po tym, jak pudelko odczytalo ustawienia - to zlecenie
   bylo juz w bazie, pudelko meldowalo sie ze swiezym `lastSeen`, a o
   zleceniu NIE WIEDZIALO. Wychodzilo z `otaSprobuj()` pierwsza linijka,
   po cichu, bez powodu.

   Aplikacja pokazywala wtedy najgorszy z mozliwych ekranow: "pudelko
   laczylo sie po zleceniu i aktualizacji NIE ZROBILO - nie podalo powodu".
   Czyli oskarzenie o awarie tam, gdzie pudelko po prostu nie dostalo
   wiadomosci. Kuba widzial to kilka razy pod rzad i mial racje, ze "cos
   jest nie tak z calym procesem".

   Naprawa jest jedna i zamyka cala te klase bledow: nie ufamy temu, co
   pamietamy z poczatku wybudzenia, tylko pytamy baze JESZCZE RAZ, w chwili
   gdy naprawde mamy zamiar cos zrobic. Kosztuje to jedno male zapytanie
   przy wlaczonym juz radiu - kilkaset bajtow.                          */
bool otaZlecenieWBazie(uint32_t& tsZlecenia) {
  String resp;
  const int code = rtdbSend("GET", "/devices/" DEVICE_ID "/config/otaCmd.json",
                            "", &resp);
  if (code != 200 || resp.length() < 2 || resp == "null") return false;

  JsonDocument doc;
  if (deserializeJson(doc, resp) != DeserializationError::Ok) return false;
  if (!doc["ts"].isNull()) tsZlecenia = doc["ts"].as<uint32_t>();
  return true;
}

/* Pobiera SAM OPIS - kilkaset bajtow zamiast 1,2 MB. Dzieki temu
   codzienne sprawdzenie "czy jest cos nowego" kosztuje tyle, co zwykly
   zapis do bazy, a nie cala aktualizacje.                             */
bool otaPobierzOpis(String& wersja, String& md5, uint32_t& rozmiar) {
  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(15);

  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(12000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  String url = String(OTA_BASE_URL) + OTA_JSON_FILE;
  if (!http.begin(client, url)) { LOGLN("[OTA] nie moge otworzyc polaczenia"); return false; }

  int code = http.GET();
  String payload = (code > 0) ? http.getString() : String();
  http.end();

  if (code != 200) {
    LOG("[OTA] opis wersji: HTTP %d\n", code);
    return false;
  }

  JsonDocument doc;
  if (deserializeJson(doc, payload) != DeserializationError::Ok) {
    LOGLN("[OTA] opis wersji nie jest poprawnym JSON-em");
    return false;
  }
  wersja  = doc["wersja"] | "";
  md5     = doc["md5"]    | "";
  rozmiar = doc["rozmiar"] | 0UL;
  md5.toLowerCase();
  LOG("[OTA] serwer ma wersje %s (%lu B, %s)\n",
      wersja.c_str(), (unsigned long)rozmiar, md5.c_str());
  return true;
}

/* Samo wgranie. Suma sprawdzana jest PRZEZ ESP32 podczas zapisu
   (`Update.setMD5`), wiec uszkodzony transfer odpada sam i nigdy nie
   dochodzi do przelaczenia partycji.

   Suma pochodzi Z BAZY, a plik z GitHub Pages - dwa rozne kanaly. Przy
   `setInsecure()` to jedyna realna przeszkoda dla kogos, kto siedzi w tej
   samej sieci: podmiana samego pliku nie wystarczy, trzeba trafic i w
   baze. Pelna obrona to `setCACert()` i to jest znany dlug.           */
bool otaWgraj(const String& md5, uint32_t rozmiar) {
  /* ZWALNIAMY POLACZENIE Z BAZA, ZANIM OTWORZYMY DRUGIE.

     TU BYLA NAJPRAWDOPODOBNIEJSZA PRZYCZYNA restartu w trakcie pobierania,
     ktory zglosil Kuba ("zapikalo, ale nie bylo fanfar", wybudzen: 1,
     wersja bez zmian, zero wpisow w czarnej skrzynce).

     `rtdbClient` jest GLOBALNY i trzyma otwarty kanal TLS do Firebase przez
     cale wybudzenie - to swiadoma optymalizacja, bo jedno polaczenie na
     wybudzenie zamiast jednego na zapytanie bylo kiedys glowna przyczyna
     powolnej synchronizacji. Ale przy aktualizacji otwieramy DRUGI kanal
     TLS, na plik z GitHuba. Dwa naraz to okolo 100 kB samych buforow
     mbedTLS na ukladzie, ktory ma 400 kB - a rownolegle leci jeszcze zapis
     do flasha i stos WiFi. Brakuje pamieci i plytka sie restartuje, zanim
     zdazy cokolwiek zapisac.

     Zamkniecie jest bezpieczne: `rtdbSend()` odbudowuje polaczenie samo,
     gdy bedzie potrzebne (kasowanie polecenia i meldunek ida juz po
     pobraniu). Kosztuje jedno zestawienie TLS, a daje pamiec, bez ktorej
     pobranie nie ma szans.                                             */
  const uint32_t wolnePrzed = ESP.getFreeHeap();
  rtdbClient.stop();
  LOG("[OTA] pamiec przed pobieraniem: %u B (po zamknieciu bazy: %u B)\n",
      (unsigned)wolnePrzed, (unsigned)ESP.getFreeHeap());

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(20);

  HTTPClient http;
  http.setConnectTimeout(10000);
  /* NIE `OTA_HTTP_TIMEOUT_MS` - to bylo po cichu klamstwo.

     `HTTPClient::setTimeout()` bierze **uint16_t** (rdzen 3.3.11,
     HTTPClient.h:217). Podane tu 90000 nie powodowalo bledu kompilacji,
     tylko obcinalo sie do 24464 ms - limit, ktorego nikt nie wybral
     i ktorego nie bylo widac w kodzie. Ten sam rodzaj cichej wpadki co
     B21: kompilator milczy, a program robi cos innego, niz mowi zrodlo.

     `OTA_HTTP_TIMEOUT_MS` zostaje tam, gdzie ma sens - jako budzet czasu
     czuwania na cala operacje. Tutaj idzie limit na POJEDYNCZY odczyt,
     wartosc mieszczaca sie w uint16_t w calosci.                       */
  http.setTimeout(OTA_HTTP_READ_MS);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  /* HTTP/1.0 - i to jest wlasciwa naprawa, nie obejscie.

     Tryb "chunked" (serwer wysyla plik kawalkami, bez podania dlugosci
     z gory) istnieje dopiero w HTTP/1.1. Zapis firmware go NIE OBSLUGUJE -
     z tego samego powodu oficjalny mechanizm aktualizacji w bibliotece
     ESP32 tez schodzi tutaj do HTTP/1.0. W tej wersji protokolu serwer
     nie ma jak wybrac chunked, wiec zawsze poda `Content-Length`
     i strumien dochodzi do `Update` w postaci, ktora ten rozumie.

     Bez tej linii GitHub Pages potrafi odpowiedziec chunkiem, `getSize()`
     zwraca wtedy -1, a pobranie konczy sie, zanim ruszy. Dokladnie to
     widzial Kuba: dwa pikniecia i "pobieranie nie doszlo do konca", trzy
     razy pod rzad.                                                     */
  http.useHTTP10(true);

  String url = String(OTA_BASE_URL) + OTA_BIN_FILE;
  if (!http.begin(client, url)) return false;

  int code = http.GET();
  if (code != 200) {
    LOG("[OTA] pobieranie programu: HTTP %d\n", code);
    http.end();
    return false;
  }

  /* BRAK DLUGOSCI W NAGLOWKU TO NIE JEST BLAD.

     TU BYLA PRZYCZYNA, przez ktora aktualizacja "nie chciala sie zrobic"
     przy KAZDEJ probie - Kuba slyszal dwa pikniecia i po chwili dostawal
     "pobieranie nie doszlo do konca", trzy razy pod rzad, bez restartu.

     Stalo tu `if (len <= 0 || ...) return false;`. Gdy serwer wysyla plik
     w trybie chunked - bez `Content-Length` z gory, a tak potrafi
     odpowiadac GitHub Pages - `getSize()` zwraca -1. Kod odrzucal wiec
     pobranie w pierwszej sekundzie, ZANIM sciagnal choc bajt. Objaw
     wygladal jak zerwany transfer, a transfer w ogole nie ruszal.

     Dlugosci z naglowka NIE POTRZEBUJEMY: znamy ja z opisu wersji, ktory
     pobralismy chwile wczesniej, i to on jest zrodlem prawdy. Gdy serwer
     dlugosc poda, sprawdzamy zgodnosc; gdy nie poda - po prostu ufamy
     opisowi. Suma kontrolna i tak zweryfikuje calosc przy zapisie.     */
  const int len = http.getSize();
  rtcOtaNagl = (int32_t)len;          // pomiar - trafi do czarnej skrzynki
  if (len > 0 && (uint32_t)len != rozmiar) {
    LOG("[OTA] rozmiar sie nie zgadza: opis %lu, naglowek %d\n",
        (unsigned long)rozmiar, len);
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "plik na serwerze ma inny rozmiar");
    http.end();
    return false;
  }
  LOG("[OTA] naglowek podaje %d B, opis %lu B - pobieram\n",
      len, (unsigned long)rozmiar);

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
    /* Najczestszy realny blad przy tym rodzaju OTA - potwierdzaja to
       zgloszenia do biblioteki ESP32 (arduino-esp32 #10213, #6254):
       przy wiekszych plikach transfer potrafi sie urwac w polowie.
       U nas dochodzi do tego slaby sygnal (-76..-87 dBm u Kuby).

       Podajemy ILE udalo sie pobrac, bo to rozroznia dwie zupelnie rozne
       sytuacje: "0 z 1,2 MB" znaczy, ze polaczenie w ogole nie ruszylo,
       a "900 kB z 1,2 MB" - ze urwalo sie w trakcie i warto sprobowac
       blizej routera. Bez tej liczby obie wygladaly tak samo.        */
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg),
             "pobrano %u kB z %lu kB - podejdz blizej routera",
             (unsigned)(zapisane / 1024), (unsigned long)(rozmiar / 1024));
    LOG("[OTA] przerwane po %u B z %lu\n",
        (unsigned)zapisane, (unsigned long)rozmiar);
    Update.abort();
    return false;
  }
  if (!Update.end(true)) {
    /* Plik doszedl w calosci, ale ESP32 go odrzucil. Dwa powody warte
       rozroznienia: niezgodna suma (plik dojechal uszkodzony) albo cos
       innego z zapisem. Pierwszy znaczy "sprobuj jeszcze raz", drugi -
       "cos jest nie tak z partycja" i wymaga kabla.                  */
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
   nie mozemy na nim polegac - i to jest wazniejsze, niz sie wydaje: bez
   wlasnego mechanizmu program, ktory sie wysypuje przy starcie, zamienia
   pudelko w cegle do czasu kabla.

   Robimy wiec wlasny licznik. Kazdy start z niepotwierdzona wersja go
   podnosi; dojscie do `goToSleep()` - czyli przejscie calej normalnej
   drogi programu - kasuje. Gdy licznik przekroczy OTA_BOOT_TRIES, wersja
   ladzie na czarnej liscie i wracamy na poprzednia partycje.

   Wywolywane na samym poczatku setup(), zanim cokolwiek innego zdazy
   sie wysypac.                                                        */
void otaSprawdzPoStarcie() {
  const String pend = otaSumaZPamieci("otaPend");
  if (pend.length() != 32) return;

  prefs.begin(NVS_NAMESPACE, false);
  uint16_t proby = prefs.getUShort("otaBoot", 0) + 1;
  nvsPutU16("otaBoot", proby);
  prefs.end();

  LOG("[OTA] start %u z niepotwierdzona wersja %s\n", proby, pend.c_str());
  if (proby <= OTA_BOOT_TRIES) return;

  /* Tyle prob wystarczy. Wersja idzie na czarna liste, zeby nie pobrac
     jej ponownie, i wracamy na partycje, z ktorej przyszlismy.        */
  LOGLN("[OTA] ta wersja nie dochodzi do konca - wracam do poprzedniej");
  prefs.begin(NVS_NAMESPACE, false);
  nvsPutStr("otaBad", pend);
  prefs.remove("otaPend");
  nvsPutU16("otaBoot", 0);
  prefs.end();

  const esp_partition_t* poprzednia = esp_ota_get_next_update_partition(nullptr);
  if (poprzednia && esp_ota_set_boot_partition(poprzednia) == ESP_OK) {
    LOGLN("[OTA] przelaczono - restart");
    delay(100);
    esp_restart();
  }
  LOGLN("[OTA] nie udalo sie przelaczyc partycji - zostaje jak jest");
}

/* Program przeszedl cala swoja droge i zasypia normalnie. To jest dowod,
   ze wersja dziala - mocniejszy niz "wstala", a nie wymagajacy zasiegu
   WiFi (pudelko na wyjezdzie tez musi moc potwierdzic).               */
void otaPotwierdzDzialanie() {
  const String pend = otaSumaZPamieci("otaPend");
  if (pend.length() != 32) return;

  prefs.begin(NVS_NAMESPACE, false);
  nvsPutStr("otaMd5", pend);
  /* DLA KTOREJ WERSJI ta suma obowiazuje.

     TU BYL BLAD, zglosil go Kuba: "jest najnowsza wersja, a i tak pozwala
     mi wgrac". Suma zapisuje sie wylacznie po udanej aktualizacji przez
     WiFi - ale program da sie zmienic TAKZE KABLEM, a wgranie kablem tedy
     nie przechodzi. W pamieci zostawala wiec suma POPRZEDNIEGO programu,
     podczas gdy w pudelku siedzial juz inny. Aplikacja porownywala ja
     z plikiem na serwerze, widziala roznice i w kolko proponowala
     aktualizacje do wersji, ktora juz byla wgrana.

     Numer wersji wystarczy, zeby to rozstrzygnac: jesli zapisana wersja
     nie zgadza sie z biezaca, suma dotyczy czegos innego i nie wolno jej
     uzywac. Czytamy ja w pushStatus() dopiero po tym sprawdzeniu.     */
  nvsPutStr("otaFw", FW_VERSION);
  prefs.remove("otaPend");
  nvsPutU16("otaBoot", 0);
  nvsPutU16("otaFail", 0);
  /* Znacznik ostatniej proby tez idzie do kosza.

     TU BYL BLAD, zglosil go Kuba: "Powod: nastepna proba za dobe" - tuz po
     tym, jak poprzednia aktualizacja SIE UDALA. Doba przerwy istnieje po to,
     zeby niedostepny plik nie znaczyl wlaczonego radia przy kazdym otwarciu
     wieczka. Ale `otaZanotujProbe()` zapisuje czas przed KAZDA proba, takze
     przed ta, ktora sie powiedzie - wiec sukces blokowal nastepna
     aktualizacje na 24 godziny. Kara za wygrana.

     Tutaj docieramy wylacznie wtedy, gdy nowa wersja wstala i przeszla cala
     swoja droge. Zadnej pętli nie ma czego przerywac, wiec licznik prob
     i zegar startuja od zera.                                          */
  prefs.remove("otaTs");
  prefs.end();

  /* Nieszkodliwe, gdy rdzen zbudowano bez rollbacku - wtedy po prostu
     nic nie robi. Gdyby kiedys byl wlaczony, ta linia jest tym, co
     powstrzymuje bootloader przed cofnieciem dzialajacej wersji.      */
  esp_ota_mark_app_valid_cancel_rollback();
  LOG("[OTA] wersja %s potwierdzona jako dzialajaca\n", FW_VERSION);

  /* Slyszalny dowod, ze aktualizacja przez WiFi doszla do konca. Gra raz
     w zyciu kazdej wersji - dokladnie tutaj, bo dopiero w tym miejscu
     wiadomo, ze nowy program nie tylko sie zapisal, ale i przezyl cala
     swoja pierwsza droge. Wersja cofnieta przez licznik startow nigdy
     tu nie dojdzie, wiec cisza tez cos znaczy.                        */
  beepNowaWersja();
}

#endif  /* OTA_ENABLED */

/* =====================================================================
 * 10b. CZARNA SKRZYNKA
 *
 *  Port USB jest martwy przez caly czas normalnej pracy - deep sleep go
 *  odlacza. Kiedy wiec pudelko "przestaje nagle dzialac", nie ma zadnego
 *  sladu, po ktorym mozna dojsc, co sie stalo. Zgadywanie kosztowalo nas
 *  juz kilka rund.
 *
 *  Dlatego kazde wybudzenie zostawia po sobie jedna linijke w pamieci
 *  nieulotnej: kiedy, z jakiego powodu, jak sie skonczylo, ile bylo
 *  baterii i ile zdarzen czekalo w kolejce. Trzydziesci dwie ostatnie
 *  linijki wypisuja sie po resecie i lecą do aplikacji, wiec historie
 *  mozna przeczytac z telefonu, bez komputera.
 * ===================================================================== */
/* @extract-begin */
#define LOGBOOK_SLOTS 32
/* @extract-end */

RTC_DATA_ATTR char rtcNote[24] = {0};    // co sie wydarzylo w tym wybudzeniu

void note(const char* co) { strncpy(rtcNote, co, sizeof(rtcNote) - 1); }

/* Czy ten wpis w ogole wart jest miejsca w historii?

   Historia ma 32 pozycje. Jesli beda w niej rutynowe meldunki, to jedna
   noc bez sieci wypchnie z niej wszystko, co bylo wczesniej - a wlasnie
   wtedy historia jest najbardziej potrzebna. Dlatego rutyna wypada.    */
bool wartoZapisac(const char* co) {
#if LOGBOOK_VERBOSE
  return true;
#else
  if (!strcmp(co, "-"))                 return false;  // nic sie nie stalo
  if (!strcmp(co, "meldunek OK"))       return false;  // widac po "ostatniej aktualizacji"
  if (!strcmp(co, "wieczko zamkniete")) return false;  // wynika z otwarcia
  if (!strcmp(co, "drgniecie styku"))   return false;  // odfiltrowane drganie
  return true;
#endif
}

void logbookAdd(const char* co) {
  if (!wartoZapisac(co)) return;

  /* queueCount() SAM otwiera i zamyka prefs, wiec musi zostac wywolane
     PRZED naszym begin() - inaczej jego end() zamknie NASZ uchwyt.

     TU BYL BLAD B22, i przez niego czarna skrzynka nie zapisala ani jednej
     linijki. Preferences to jeden globalny obiekt z jednym uchwytem:
     begin() na juz otwartym zwraca false i nic nie robi, ale end()
     zagniezdzonego wywolania zamyka uchwyt funkcji nadrzednej. Wywolanie
     queueCount() siedzialo w argumentach snprintf ponizej, wiec kazdy
     nvsPutStr() po nim trafial w zamkniety uchwyt i zawodzil - za kazdym
     razem, przy kazdym wybudzeniu.

     Objaw u uzytkownika: "Historia pudelka" pusta, a licznik nvsFail
     rosnacy co wybudzenie. Do tego pushStatus() nadpisywal historie
     w bazie pusta tablica, wiec ginela takze ta zapisana wczesniej.  */
  const uint16_t wKolejce = queueCount();

  prefs.begin(NVS_NAMESPACE, false);
  uint16_t idx = prefs.getUShort("lbIdx", 0);
  char klucz[8];
  snprintf(klucz, sizeof(klucz), "lb%u", idx % LOGBOOK_SLOTS);

  /* Format: czas|powod|co sie stalo|bateria|kolejka
     Krotko, bo NVS to nie miejsce na powiesci.                        */
  /* 96, nie 72: od 1.43.2 wpisy o aktualizacji nosza NUMER WERSJI, bo
     "wgrane" bez wersji nie odpowiada na jedyne pytanie, ktore sie
     wtedy zadaje - CO sie wgralo. Przy dlugiej nazwie wybudzenia
     ("zamkniecie wieczka") stary bufor obcinal koncowke wpisu.       */
  char linia[96];
  snprintf(linia, sizeof(linia), "%lu|%s|%s|%d|%u",
           (unsigned long)(rtcTimeValid ? time(nullptr) : 0),
           wakeName(wakeReason), co, batteryPercentage, wKolejce);
  if (nvsPutStr(klucz, linia)) nvsPutU16("lbIdx", (idx + 1) % (LOGBOOK_SLOTS * 8));
  prefs.end();
}

/* Wypisuje historie od najstarszej do najnowszej. */
void logbookPrint() {
  prefs.begin(NVS_NAMESPACE, true);
  uint16_t idx = prefs.getUShort("lbIdx", 0);
  LOGLN("[HIST] ostatnie wybudzenia (czas | powod | wynik | bateria | kolejka):");
  bool cokolwiek = false;
  for (int i = 0; i < LOGBOOK_SLOTS; i++) {
    char klucz[8];
    snprintf(klucz, sizeof(klucz), "lb%u", (idx + i) % LOGBOOK_SLOTS);
    String l = prefs.getString(klucz, "");
    if (l.length()) { LOG("[HIST]   %s\n", l.c_str()); cokolwiek = true; }
  }
  if (!cokolwiek) LOGLN("[HIST]   (pusto - pudelko jeszcze nic nie zapisalo)");
  prefs.end();
}

/* Ucieczka znakow, ktore lamia JSON.

   Dziennik byl dotad sklejany bez tego. Dzialalo, bo zaden z tekstow nie ma
   cudzyslowu ani backslasha - ale to nie jest zabezpieczenie, tylko szczescie.
   Jeden taki znak w notatce zamienia caly dziennik w niepoprawny JSON, baza
   odrzuca zapis i historia znika CALA, nie tylko ten jeden wpis. Awaria cicha:
   w aplikacji wyglada jak "pudelko nic nie przyslalo".                      */

/* Cala historia jako tablica JSON - do wyslania do aplikacji. */
String logbookJson() {
  prefs.begin(NVS_NAMESPACE, true);
  uint16_t idx = prefs.getUShort("lbIdx", 0);
  String out = "[";
  bool pierwszy = true;
  for (int i = 0; i < LOGBOOK_SLOTS; i++) {
    char klucz[8];
    snprintf(klucz, sizeof(klucz), "lb%u", (idx + i) % LOGBOOK_SLOTS);
    String l = prefs.getString(klucz, "");
    if (!l.length()) continue;
    if (!pierwszy) out += ",";
    pierwszy = false;
    out += "\"" + jsonEscape(l) + "\"";
  }
  prefs.end();
  return out + "]";
}

/* =====================================================================
 * 10c. GESTY SERWISOWE I AUTOTEST
 *
 *  Przycisk nie budzi ukladu ze snu - fizycznie sie nie da, bo maska
 *  wybudzania ma jeden wspolny poziom, a kontaktron i przycisk potrzebuja
 *  przeciwnych. Ale przycisk wcale nie musi budzic: po otwarciu wieczka
 *  plytka i tak czuwa, czekajac az je zamkniesz. Wystarczy w tym czasie
 *  patrzec na przycisk - i nagle mamy pelnowartosciowy interfejs bez
 *  jednego dodatkowego kabla.
 * ===================================================================== */
/* Czeka, az wieczko zostanie zamkniete, i po drodze liczy nacisniecia
   przycisku. Zastepuje dawne "poczekaj, az zamkna" - ten sam czas, ta sama
   bateria, tylko teraz cos jeszcze przy okazji robi.                    */
Gest czekajNaZamkniecieIGest(uint32_t limitMs) {
  uint32_t t0 = millis();
  bool byl = buttonPressed();
  uint32_t wcisnietyOd = byl ? t0 : 0;
  int klikniec = 0;
  bool radioZgaszone = false;
  uint32_t ostatniPing = millis();
  if (boxIsOpen()) byloOtwarte = true;

  /* Powiedz aplikacji, ze wieczko jest otwarte - TERAZ, nie po zamknieciu.

     TU BYL BLAD i tlumaczy on cala "zepsuta synchronizacje".

     Sciezka "juz dzis brales" nigdy nie wlaczala radia: piknela i od razu
     szla czekac na zamkniecie. Pelny status leci dopiero PO powrocie z
     tego czekania, czyli po zamknieciu wieczka. Efekt: otwierasz pudelko
     drugi raz w ciagu dnia, patrzysz w telefon przez dwie minuty i nie ma
     tam nic - bo pudelko przez ten czas w ogole sie nie odzywalo. Nie
     bylo wolne. Bylo CICHE.

     Ta sama luka dotyczyla czekania po alarmie. Teraz stan wieczka idzie
     z kazdej sciezki, ktora zaczyna czekac przy otwartym pudelku.

     Warunek na rtcOpenReported pilnuje, zebysmy nie wysylali drugi raz
     tego, co przy zwyklym otwarciu poszlo juz w reportEvent().        */
  if (boxIsOpen() && !rtcOpenReported && !batterySaver) {
    if (wifiConnect() && firebaseSignIn()) pushLidState();
    else rtcStatusDirty = true;
  }

  /* Czuwanie moze trwac dlugo, wiec bezpiecznik czasowy trzeba odsunac -
     inaczej przerwalby czekanie po dwoch i pol minuty.                  */
  extendAwake(limitMs + 30000);

  while (millis() - t0 < limitMs && !awakeTooLong()) {
    /* Przez pierwsze RADIO_OTWARTE_S radio zostaje wlaczone. Kosztuje to
       trzy setne procenta baterii, a daje polaczenie gotowe do uzycia:
       zamykasz wieczko i stan idzie do aplikacji natychmiast, bez rundy
       logowania do sieci. Dopiero gdy pudelko stoi otwarte dluzej, radio
       gasnie - bo wtedy to ono odpowiada za wiekszosc poboru. Po
       zamknieciu wraca samo, patrz koniec setup().                     */
    /* Podtrzymanie polaczenia.

       Router potrafi rozlaczyc stacje, ktora nic nie nadaje, a plytka
       pracuje w modem-sleep. Gdy to sie stanie, wysylka po zamknieciu
       wieczka musi najpierw odbudowac lacze - a to przy nieudanej
       pierwszej probie oznacza kilkanascie sekund ciszy zamiast
       natychmiastowej aktualizacji w telefonie.

       Sprawdzamy wiec co 5 s i odbudowujemy lacze OD RAZU, w czasie
       gdy i tak czekamy. W chwili zamkniecia polaczenie jest gotowe. */
    if (!radioZgaszone && millis() - ostatniPing > 5000) {
      ostatniPing = millis();
      if (WiFi.status() != WL_CONNECTED) {
        LOGLN("[NET] lacze padlo podczas czekania - odbudowuje");
        WiFi.reconnect();
      }
    }

    if (!radioZgaszone && millis() - t0 > RADIO_OTWARTE_S * 1000UL) {
      LOG("[NET] wieczko otwarte %lu s - usypiam radio\n",
          (unsigned long)((millis() - t0) / 1000));
      wifiUspij();
      radioZgaszone = true;
    }
    bool teraz = buttonPressed();

    if (teraz != byl) {
      if (teraz) {                       // wcisniety
        wcisnietyOd = millis();
      } else {                           // zwolniony - liczymy klikniecie
        if (millis() - wcisnietyOd < GEST_PRZYTRZYM_MS) {
          klikniec++;
          LOG("[GST] klikniecie %d z %d\n", klikniec, GEST_KLIKNIEC);
          buzzerInit(); buzzerTone(3000); delay(35); buzzerOff();
        }
        wcisnietyOd = 0;
      }
      byl = teraz;
    }

    /* Przytrzymanie - portal WiFi. Reagujemy juz w trakcie trzymania,
       zeby bylo wiadomo, ze gest zostal przyjety.                      */
    if (teraz && wcisnietyOd && millis() - wcisnietyOd >= GEST_PRZYTRZYM_MS) {
      LOGLN("[GST] przytrzymanie -> portal WiFi");
      return GEST_PORTAL;
    }
    if (klikniec >= GEST_KLIKNIEC) {
      LOGLN("[GST] trzy klikniecia -> autotest");
      return GEST_TEST;
    }

    /* Wieczko zamkniete i nikt nie bawi sie przyciskiem - koniec czekania.
       Gdy trwa gest, dajemy dokonczyc mimo zamknietego wieczka.        */
    if (!boxIsOpen() && !teraz && klikniec == 0 && millis() - t0 > 1200) {
      LOG("[REED] zamkniete po %lu s czuwania\n", (unsigned long)((millis() - t0) / 1000));
      msZamkniecia = millis();          // do zmierzenia opoznienia wysylki
      break;
    }
    delay(25);
  }
  return GEST_BRAK;
}

/* --- Sygnaly autotestu ----------------------------------------------
   ZASADA: pikniecia mowia, KTORY etap sie zaczyna, a nie czy zdany.
   Jedno pikniecie = etap 1, dwa = etap 2 i tak dalej. Dzieki temu przy
   sklejonym pudelku wiadomo, na czym stoimy i czy trzeba cos zrobic -
   a wyniki i tak widac pozniej w aplikacji, wiec nie ma sensu meczyc
   uszu potwierdzaniem kazdego kroku.                                 */
void pikNumer(int n) {
  buzzerInit();
  for (int i = 0; i < n; i++) {
    buzzerTone(2700); delay(85);
    buzzerTone(0);    delay(170);
  }
  buzzerOff();
  delay(250);                            // cisza, zeby dalo sie policzyc
}

/* Koniec testu - celowo NIE seria piknięć, zeby nie pomylic z numerem
   etapu. Krotka opadajaca sygnatura.                                  */
void pikKoniecTestu() {
  buzzerInit();
  const uint16_t f[3] = {3200, 2800, 2400};
  for (int i = 0; i < 3; i++) { buzzerTone(f[i]); delay(150); }
  buzzerOff();
}

/* JEDYNY sygnal mowiacy o bledzie - i tylko o tym jednym, ktory ma
   znaczenie: bez sieci wynik testu NIE DOTRZE do aplikacji, wiec nie ma
   gdzie go sprawdzic. Piec sekund lagodnego pulsowania na malym
   wypelnieniu: slychac, ale nie wierci.                               */
void pikBrakSieci() {
  buzzerInit();
  for (int i = 0; i < 5; i++) {
    buzzerTonCicho(2700, 90);            // ok. 9% wypelnienia
    delay(420);
    buzzerTone(0);
    delay(580);
  }
  buzzerOff();
}

/* Zapisuje wynik etapu do historii, zeby dalo sie go przeczytac w telefonie. */
void wynikEtapu(const char* nazwa, bool ok) {
  char buf[24];
  snprintf(buf, sizeof(buf), "test:%s %s", nazwa, ok ? "OK" : "BLAD");
  logbookAdd(buf);
  LOG("[TST] %s: %s\n", nazwa, ok ? "OK" : "BLAD");
}

/* --- Autotest -------------------------------------------------------
 *  Siedem etapow. Przed KAZDYM pudelko podaje jego numer piknieciami:
 *  raz, dwa, trzy... Tylko dwa etapy wymagaja Twojej reakcji - drugi
 *  (rusz wieczkiem) i trzeci (nacisnij przycisk). Reszta leci sama.
 *  Wyniki ogladasz w aplikacji; jedyny sygnal o bledzie to brak sieci,
 *  bo wtedy w aplikacji nic byś nie zobaczyl.
 * ------------------------------------------------------------------ */
/* Kazdy etap melduje sie z chwila startu i odswieza bezpiecznik czasowy.

   Bez tego "test sie przerwal" jest nie do zdiagnozowania: nie wiadomo,
   czy zabraklo czasu, czy zawiesila sie siec, czy w ogole nie doszedl do
   danego etapu. Z tym - log mowi to wprost.                          */
static void etapTestu(const char* nazwa) {
  extendAwake(120000);                   // zaden etap nie moze paść przez limit
  LOG("[TST] --- etap: %s (t=%lu ms) ---\n", nazwa, (unsigned long)millis());
}

void autoTest() {
  LOGLN("[TST] ===== AUTOTEST =====");
  note("autotest");
  logbookAdd("test:START");
  extendAwake(150000);                   // test ma prawo potrwac
  int bledy = 0;

  /* --- 1. Kontaktron: RUSZ WIECZKIEM ---------------------------------
     Jedyny etap, w ktorym jestes potrzebny - i dlatego stoi pierwszy.
     Zaczyna sie od JEDNEGO pikniecia, ktore samo w sobie jest dowodem,
     ze buzzer dziala; osobnego etapu na to nie trzeba.

     Przycisku nie sprawdzamy wcale. Skoro uruchomiles ten test trzema
     klknieciami, to przycisk dziala - testowanie tego byloby pytaniem
     o rzecz, ktora wlasnie udowodniles.                              */
  etapTestu("kontaktron");
  pikNumer(1);
  {
    bool start = boxIsOpen(), zmiana = false;
    uint32_t t0 = millis();
    LOGLN("[TST] rusz wieczkiem - czekam 20 s");
    while (millis() - t0 < 20000) {
      if (boxIsOpen() != start) { zmiana = true; break; }
      delay(30);
    }
    wynikEtapu("kontaktron", zmiana);
    if (!zmiana) bledy++;
  }

  /* Dwa pikniecia = koniec czesci z Toba. Od tej chwili pudelko
     sprawdza sie samo i mozesz je odlozyc.                          */
  delay(250);
  pikNumer(2);

  /* --- 2. Zasilanie i pomiar baterii --- */
  etapTestu("bateria");
  {
    readBattery();
    int mn = 4095, mx = 0;
    for (int i = 0; i < 8; i++) { int r = readBatteryRaw();
      if (r < mn) mn = r; if (r > mx) mx = r; delay(15); }
    bool ok = realBatteryVoltage > 3.0f && realBatteryVoltage < 4.35f && (mx - mn) < 250;
    LOG("[TST] bateria %.2f V, rozrzut %d\n", realBatteryVoltage, mx - mn);
    wynikEtapu("bateria", ok);
    if (!ok) bledy++;
  }

  /* --- 3. Pamiec nieulotna --- */
  etapTestu("pamiec");
  {
    bool ok = prefs.begin("pbtest", false);
    if (ok) {
      prefs.putULong("p", 0xC0FFEE01);
      ok = prefs.getULong("p", 0) == 0xC0FFEE01;
      prefs.remove("p");
      prefs.end();
    }
    wynikEtapu("pamiec", ok);
    if (!ok) bledy++;
  }

  /* --- 4. WiFi --- */
  etapTestu("wifi");
  bool wifi = wifiConnect();
  wynikEtapu("wifi", wifi);
  if (!wifi) bledy++;

  /* --- 5. Zegar --- */
  etapTestu("zegar");
  bool zegar = wifi && rtcTimeValid;
  wynikEtapu("zegar", zegar);
  if (!zegar) bledy++;

  /* --- 6. Zapis do bazy --- */
  etapTestu("baza");
  bool baza = wifi && firebaseSignIn();
  if (baza) { fetchConfig(); flushQueue(); }
  wynikEtapu("baza", baza);
  if (!baza) bledy++;

  char pod[24];
  snprintf(pod, sizeof(pod), "test:KONIEC %d bledow", bledy);
  logbookAdd(pod);
  LOG("[TST] ===== KONIEC: %d bledow =====\n", bledy);

  etapTestu("wysylka wyniku");
  delay(300);
  pikKoniecTestu();

  /* Wynik ma trafic do telefonu OD RAZU.

     rtcLogWyslanyIdx zerujemy celowo. Normalnie dziennik idzie tylko
     wtedy, gdy przybylo wpisow - ale tu wpisy powstaly przed chwila i
     to wlasnie one sa calym sensem tej operacji. Bez tego wyniki testu
     czekalyby w pudelku do nastepnej okazji, a w aplikacji bylaby
     pustka - dokladnie to, co widziales.                            */
  bool wyslano = false;
  if (baza) {
    rtcLogWyslanyIdx = 0xFFFF;
    wyslano = pushStatus();
  }

  /* Bez sieci wyniku NIE MA gdzie zobaczyc - i tylko o tym warto
     poinformowac dzwiekiem. Reszta jest w aplikacji.                  */
  if (!wyslano) {
    LOGLN("[TST] brak sieci - wynik zostaje w pamieci pudelka");
    rtcStatusDirty = true;               // dosle przy najblizszej okazji
    delay(600);
    pikBrakSieci();
  }
}

/* =====================================================================
 * 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)
 *
 *      Dzwonek slychac w domu. Wiadomosc dociera wszedzie - i o to tu
 *      chodzi. Pudelko dzwoni przez dwie minuty do pustego mieszkania,
 *      a czlowiek dowiaduje sie o pominietej dawce Warfinu dopiero
 *      wieczorem, gdy wroci i otworzy aplikacje.
 *
 *      DLACZEGO WYSYLA PUDELKO. Telefon spi razem z wlascicielem, a iOS
 *      nie budzi stron dodanych do ekranu glownego - aplikacja fizycznie
 *      nie ma jak niczego przypomniec o 20:00. Pudelko w tej chwili jest
 *      wybudzone, bo wlasnie skonczylo dzwonic. To jedyne miejsce w calym
 *      ukladzie, ktore wtedy zyje.
 *
 *      CENA, i mowimy o niej wprost takze w aplikacji: powiadomienie
 *      wymaga, zeby PUDELKO mialo internet. Bez sieci nie przyjdzie nic.
 *      To ta sama granica, ktora obowiazuje dawki jadace do kalendarza -
 *      nie nowa slabosc, tylko ta sama.
 * ===================================================================== */
#if TG_ENABLED
/* Samo zapytanie. Zwraca true wylacznie przy HTTP 200 od Telegrama -
   od tego zalezy, czy skasujemy czekajace powiadomienie (zasada 6).

   TOKEN IDZIE W ADRESIE, WIEC ADRESU NIE LOGUJEMY. Telegram nie zna
   innej drogi; nasza jest nie wpisac go do niczego, co Kuba moglby
   wkleic w zgloszeniu. W logu zostaje sam kod odpowiedzi.

   ZWALNIAMY KANAL DO BAZY, ZANIM OTWORZYMY DRUGI. Ten sam powod co przy
   OTA: `rtdbClient` jest globalny i trzyma otwarte TLS przez cale
   wybudzenie, a dwa konteksty mbedTLS naraz to okolo 100 kB na ukladzie,
   ktory ma 400 kB. Pobranie firmware wywracalo sie na tym (D60). Tutaj
   przesylamy kilkaset bajtow, wiec restart jest znacznie mniej prawdo-
   podobny - ale stawka jest wiadomosc o pominietej dawce leku
   przeciwzakrzepowego, a cena to jedno dodatkowe uzgodnienie TLS przy
   nastepnym zapytaniu do bazy. `rtdbSend()` odbudowuje kanal sam.    */
bool tgWyslijTekst(const String& tekst) {
  const String token = tgTokenZPamieci();
  const String chat  = tgChatZPamieci();
  if (!token.length() || !chat.length()) return false;

  rtdbClient.stop();

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(15);

  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(12000);

  String url = String("https://") + TG_HOST + "/bot" + token + "/sendMessage";
  if (!http.begin(client, url)) {
    LOGLN("[TG ] nie moge otworzyc polaczenia");
    return false;
  }

  /* Tresc budujemy ArduinoJsonem, a nie skladaniem napisow - cudzyslow
     albo znak nowej linii w tekscie zepsulby caly pakiet. Wiadomosci sa
     dzisiaj nasze wlasne, ale ta funkcja nie ma prawa zalezec od tego,
     ze nikt jej nigdy nie poda cudzego napisu.                       */
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
   Rozdzielenie zamiaru od wysylki jest tu celowe i wazne. Alarm konczy
   sie w `case WAKE_TIMER`, czyli w polowie wybudzenia - a radio zabrane
   w tym miejscu weszloby miedzy nieodebrane przypomnienie a zapis
   zdarzenia "missed" do kolejki. Dane o leku ida pierwsze; wiadomosc
   czeka na `goToSleep()`, tak samo jak aktualizacja i skan sieci
   (zasada 11).                                                       */
void tgZglosNieodebrane(int slot) {
  rtcTgSlot   = (int8_t)slot;
  rtcTgSlotTs = rtcTimeValid ? (uint32_t)time(nullptr) : 0;
  LOG("[TG ] przypomnienie %d bez odzewu - napisze przed snem\n", slot);
}

/* Bateria: jedna wiadomosc na rozladowanie, nie jedna na wybudzenie.

   Kuba poprosil o dokladnie to ("tylko jeden jak pudelko ma malo
   bateri"), i bez znacznika byloby inaczej: ponizej progu pudelko
   przechodzi tedy przy KAZDYM otwarciu wieczka, czyli codziennie.

   Znacznik zdejmuje sie po naladowaniu powyzej TG_BATT_RESET_PCT.
   Gdyby zostawal na stale, druga wiadomosc nie przyszlaby nigdy - a
   ogniwo rozladuje sie jeszcze wiele razy.

   Zero procent oznacza tu takze "nie ma baterii, zasilanie z USB"
   (odczyty ponizej 2 V, patrz readBattery), wiec ten przypadek
   pomijamy - inaczej kazde podlaczenie kabla do programowania konczylo
   by sie wiadomoscia o rozladowanym ogniwie.                         */
void tgSprawdzBaterie() {
  if (batteryPercentage >= TG_BATT_RESET_PCT) {
    rtcTgBattZgloszona = false;
    return;
  }
  if (batteryPercentage <= 0 || batteryPercentage > BATT_WARN_PCT) return;
  if (rtcTgBattZgloszona || rtcTgBattCzeka) return;
  rtcTgBattCzeka = true;
  LOG("[TG ] bateria %d%% - napisze przed snem\n", batteryPercentage);
}

/* Zdanie, ktore czlowiek przeczyta na telefonie.

   Godzina bierze sie z harmonogramu, nie z zegara: `slots[]` to pora
   PRZYPOMNIENIA (zasada 4b) i wlasnie ona ma stac w wiadomosci. Przy
   dwoch przypomnieniach w ciagu wieczora inaczej przyszlyby dwa
   identyczne zdania.

   Zdanie NIE mowi "nie wziales" jako fakt, tylko opisuje to, co pudelko
   naprawde wie: dzwonilo i nikt go nie otworzyl. Tabletke da sie wziac
   z blistra lezacego obok - to nie jest czeste, ale klamstwo w tym
   miejscu podkopalo by zaufanie do wszystkich pozostalych wiadomosci. */
String tgTekstNieodebrane(int slot) {
  String godz = (slot >= 0 && slot < slotCount) ? slots[slot] : String("");
  String s = "⏰ PillBox: tabletka nieodebrana\n\n";
  if (godz.length()) s += "Przypomnienie " + godz + " — pudełko dzwoniło i nikt go nie otworzył.";
  else               s += "Pudełko dzwoniło i nikt go nie otworzył.";
  s += "\n\nJeśli wziąłeś ją bez otwierania pudełka, zaznacz dawkę ręcznie w aplikacji.";
  return s;
}

String tgTekstBateria() {
  String s = "🔋 PillBox: słaba bateria\n\n";
  s += "Zostało " + String(batteryPercentage) + "% — naładuj pudełko.";
  s += "\n\nPrzy pustym ogniwie nie zadzwoni i nie przyśle powiadomienia.";
  return s;
}

/* --- Wysylka: TU I TYLKO TU  ------------------------------------------
   Wolane z `goToSleep()`, czyli po zapisie dawki, wyslaniu statusu
   i oproznieniu kolejki - dokladnie z tego samego powodu, dla ktorego
   stad rusza aktualizacja i skan sieci (zasada 11). W tym miejscu nie ma
   juz czego opoznic.

   PRZED skanem sieci i przed aktualizacja, i to jest wazne w tej
   kolejnosci: skan potrafi zerwac lacze, a udana aktualizacja konczy sie
   restartem - wiadomosc wyslana za nimi nie poszlaby wcale.

   Przy pustej skrzynce funkcja wychodzi PRZED wlaczeniem radia. Cisza nie
   kosztuje tu nic - ani miliampera, ani sekundy czuwania.             */
void tgWyslijZalegle() {
  tgSprawdzBaterie();

  const bool cosCzeka = (rtcTgSlot >= 0) || rtcTgBattCzeka || rtcTgTestProsba;
  const uint32_t teraz = rtcTimeValid ? (uint32_t)time(nullptr) : 0;
  /* Wiek liczymy dla nieodebranego przypomnienia - ono jedno traci sens
     ze starosci. Ostrzezenie o baterii jest prawdziwe tak dlugo, jak
     ogniwo jest slabe, a wiadomosc probna wysyla sie na zadanie i wtedy
     "za pozno" nie ma znaczenia.                                      */
  const uint32_t tsWieku = (rtcTgSlot >= 0) ? rtcTgSlotTs : 0;

  const TgDecyzja d = tgDecyzja(tgSkonfigurowany(), cosCzeka, tsWieku, teraz);

  if (d == TG_NIC) return;

  if (d == TG_BRAK_BOTA) {
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "bot niepodlaczony - nie mam komu pisac");
    LOGLN("[TG ] jest o czym napisac, ale bot nie jest podlaczony");
    /* Znacznikow NIE kasujemy: bot moze dojechac z aplikacji w ciagu
       najblizszych minut, a wtedy wiadomosc jeszcze ma sens. Po
       TG_MAX_WIEK_S zdejmie ja gałąź TG_ZA_STARE ponizej.            */
    return;
  }

  if (d == TG_ZA_STARE) {
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "przypomnienie za stare - nie wyslalem");
    LOGLN("[TG ] czekajace powiadomienie jest starsze niz 3 h - kasuje je");
    rtcTgSlot   = -1;
    rtcTgSlotTs = 0;
    return;
  }

  if (WiFi.status() != WL_CONNECTED && !wifiConnect()) {
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "brak sieci - wiadomosc czeka");
    LOGLN("[TG ] brak sieci - wiadomosc poczeka do nastepnego wybudzenia");
    return;
  }
  extendAwake(20000);                    // uzgodnienie TLS z zapasem

  /* KAZDA rzecz kasuje sie osobno i dopiero po swoim wlasnym HTTP 200
     (zasada 6). Wspolny warunek na koncu gubilby wiadomosc, ktora
     przeszla, razem z ta, ktora nie przeszla.                        */
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
      rtcTgBattZgloszona = true;         // do naladowania juz o tym nie piszemy
      wyslane++;
    } else nieudane++;
  }

  if (rtcTgTestProsba) {
    const bool ok = tgWyslijTekst(
        "✅ PillBox: wiadomość próbna\n\n"
        "Bot działa. Tak wyglądają powiadomienia z pudełka.");
    if (ok) { rtcTgTestProsba = false; wyslane++; }
    else    nieudane++;
    /* Zlecenie z bazy kasujemy TYLKO po udanej probie. Nieudana ma
       wrocic przy nastepnym wybudzeniu - inaczej "wyslij probna"
       konczylo by sie cisza, ktorej nie da sie odroznic od zepsutego
       bota.                                                          */
    if (ok && firebaseSignIn())
      rtdbSend("DELETE", "/devices/" DEVICE_ID "/config/tgCmd.json", "");
  }

  if (nieudane)
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "Telegram odmowil (%d z %d)",
             nieudane, wyslane + nieudane);
  else
    snprintf(rtcTgMsg, sizeof(rtcTgMsg), "wyslane: %d", wyslane);

  /* Powod dojezdza OD RAZU, nie przy nastepnym wybudzeniu - ta sama
     lekcja co przy `otaZglos()` (D59). Radio jeszcze zyje, wiec meldunek
     nic nie kosztuje, a bez niego ekran przez kilka godzin twierdzilby,
     ze wszystko w porzadku.                                          */
  if (nieudane || wyslane) {
    if (!pushStatus()) rtcStatusDirty = true;
  }
}
#endif  /* TG_ENABLED */

/* =====================================================================
 * 11.  DEEP SLEEP
 * ===================================================================== */
/* --- Aktualizacja: wykonanie ---------------------------------------
   Stoi TUTAJ, a nie przy reszcie kodu OTA, bo zapisuje slad w czarnej
   skrzynce (`logbookAdd`), a ta jest zdefiniowana nizej. Kolejnosci
   definicji pilnuje audyt - w .ino ma ona znaczenie takze dla
   generatora prototypow (B21/D26).                                 */
#if OTA_ENABLED
/* Powod odmowy albo niepowodzenia idzie do aplikacji OD RAZU, a nie przy
   najblizszym wybudzeniu.

   Roznica jest cala tresc tego ekranu. `otaSprobuj()` chodzi w
   `goToSleep()`, czyli JUZ PO wyslaniu zwyklego statusu - samo ustawienie
   `rtcStatusDirty` znaczyloby, ze aplikacja pozna powod dopiero za
   kilka godzin. Przez ten czas pokazywalaby pogodne "zlecone, czekaj",
   podczas gdy pudelko wlasnie odmowilo i nie zamierza nic robic.
   Radio w tym momencie jeszcze zyje, wiec meldunek nic nie kosztuje.  */
void otaZglos() {
  if (!pushStatus()) rtcStatusDirty = true;   // nie doszlo - dosle pozniej
}

/* --- CALOSC: sprawdz, zdecyduj, ewentualnie wgraj --------------------
   Wolane z jednego miejsca - z `goToSleep()`, czyli po tym, jak pudelko
   zrobilo juz wszystko, po co wstalo. Dawka jest wtedy zapisana
   i potwierdzona, status wyslany, kolejka opróżniona.

   Nie wraca, jesli aktualizacja sie powiodla - konczy restartem.      */
/* JAKIE SIECI WIDZI PUDELKO - na wyrazna prosbe z aplikacji.

   Powod, dla ktorego to robi PUDELKO, a nie telefon: Safari nie daje
   zadnej stronie dostepu do skanowania WiFi, wiec aplikacja fizycznie
   nie ma jak pokazac listy. Ale nawet gdyby miala, wazniejsza jest lista
   widziana STAD: to pudelko ma sie polaczyc, a stoi w innym miejscu niz
   telefon. Siec swietnie widoczna z kanapy potrafi nie docierac do
   pudelka - i wtedy nazwa wybrana z listy telefonu wygladalaby na dobra,
   a polaczenie nie powstaloby nigdy. Dlatego razem z nazwa idzie SILA
   SYGNALU: czlowiek od razu widzi, czy pudelko w tym miejscu ma szanse.

   Skan kosztuje kilka sekund radia i potrafi zerwac polaczenie, wiec
   robimy go dopiero przed snem - po zapisie dawki i wyslaniu statusu,
   z tego samego powodu co aktualizacje (zasada 11 w duchu).

   Zlecenie w bazie kasujemy DOPIERO po potwierdzonym zapisie listy
   (zasada 6). Nieudana wysylka zostawia prosbe na miejscu i wraca do
   niej przy nastepnym wybudzeniu.                                     */
void skanujSieci() {
  if (!rtcScanProsba) return;

  if (WiFi.status() != WL_CONNECTED && !wifiConnect()) {
    LOGLN("[SCN] brak sieci - lista poczeka do nastepnego wybudzenia");
    return;
  }

  extendAwake(30000);                 // skan + wysylka, z zapasem
  const int found = WiFi.scanNetworks();
  LOG("[SCN] widze %d sieci\n", found);

  JsonDocument doc;
  doc["ts"] = (uint32_t)(rtcTimeValid ? time(nullptr) : 0);
  JsonArray arr = doc["nets"].to<JsonArray>();
  for (int i = 0; i < found && (int)arr.size() < SCAN_MAX_NETS; i++) {
    String s = WiFi.SSID(i);
    /* Siec ukryta (pusta nazwa) i nazwa dluzsza, niz dopuszczaja reguly
       bazy, odpadaja tutaj. Jeden nieznany element odrzucilby CALY wpis,
       a z nim liste, po ktora ktos wlasnie siegnal.                    */
    if (s.length() == 0 || s.length() > 32) continue;
    JsonObject o = arr.add<JsonObject>();
    o["s"] = s;
    o["r"] = (int)WiFi.RSSI(i);
  }
  WiFi.scanDelete();                  // pamiec po skanie oddajemy od razu

  /* Skan potrafi zerwac lacze - odbudowujemy je przed wysylka. */
  if (WiFi.status() != WL_CONNECTED && !wifiConnect()) {
    LOGLN("[SCN] po skanie nie ma sieci - lista poczeka");
    return;
  }
  if (!firebaseSignIn()) {
    LOGLN("[SCN] brak logowania - lista poczeka");
    return;
  }

  String body;
  serializeJson(doc, body);
  const int code = rtdbSend("PUT", "/devices/" DEVICE_ID "/scan.json", body);
  if (code == 200) {
    rtdbSend("DELETE", "/devices/" DEVICE_ID "/config/wifiScan.json", "");
    rtcScanProsba = false;
    LOGLN("[SCN] lista sieci wyslana");
  } else {
    LOG("[SCN] nie udalo sie wyslac listy (HTTP %d) - sprobuje pozniej\n", code);
  }
}

void otaSprobuj() {
  /* Pamiec z poczatku wybudzenia NIE jest tu warunkiem, tylko przyspieszaczem.
     Gdy `fetchConfig()` widzialo zlecenie - wiemy od razu. Gdy nie widzialo,
     a radio nadal stoi, PYTAMY BAZE JESZCZE RAZ: zlecenie mogło dojechać
     w trakcie tego wybudzenia. Patrz `otaZlecenieWBazie()` - to jest ta
     dziura, przez ktora "kliknij i pudelko przyjmie" nie dzialalo.

     Bez radia nie ma jak zapytac i nie ma jak pobrac, wiec wtedy - i tylko
     wtedy - wychodzimy po cichu: nic sie nie stalo i nic nie zginelo,
     zlecenie poczeka do nastepnego wybudzenia.                          */
  if (!rtcOtaProsba && WiFi.status() != WL_CONNECTED) return;

  /* TU BYL BLAD - i objaw byl dokladnie taki, jak opisal Kuba: "otworzylem
     dwa razy i nie wiem dlaczego aktualizacja sie nie zrobila", a ekran
     mowil "nie podalo powodu".

     Stalo tu `if (WiFi.status() != WL_CONNECTED) return;` - CICHE wyjscie.
     Do tego miejsca dochodzimy z `goToSleep()`, czyli po calej reszcie
     wybudzenia: po zapisie dawki, wyslaniu statusu i czekaniu na zamkniecie
     wieczka. Przy slabym sygnale (u Kuby -90 dBm, granica zasiegu) router
     zdazy w tym czasie rozlaczyc stacje, ktora nic nie nadaje. Radio bylo
     wiec sprawne, siec w zasiegu, a aktualizacja rezygnowala bez slowa
     i bez sladu - licznik prob tez nie rosl, bo do niego nie dochodzilo.

     Dwie naprawy naraz. Po pierwsze: nie rezygnujemy, tylko ODBUDOWUJEMY
     lacze - pudelko i tak nie spi, a zlecenie czeka. Po drugie: kazde
     wyjscie zostawia powod, bo "nie podalo powodu" jest najgorsza z
     mozliwych odpowiedzi dla kogos, kto stoi nad pudelkiem.           */
  if (WiFi.status() != WL_CONNECTED && !wifiConnect()) {
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "nie zlapalem sieci przed snem");
    rtcStatusDirty = true;                      // radia nie ma, powod dosle pozniej
    LOGLN("[OTA] brak sieci przy zasypianiu - sprobuje przy nastepnym wybudzeniu");
    return;
  }
  /* Token zyje godzine, a od ostatniego logowania mogla minac cala doba
     czuwania na ladowarce. Bez waznego tokenu nie da sie ani skasowac
     polecenia, ani zameldowac powodu.                                 */
  if (!firebaseSignIn()) {
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "nie moge sie zalogowac do bazy");
    rtcStatusDirty = true;
    LOGLN("[OTA] brak logowania - aktualizacja czeka");
    return;
  }

  /* DOPYTANIE O ZLECENIE - dopiero tutaj, bo dopiero tu mamy pewny token.
     Gdy `fetchConfig()` widzialo zlecenie na poczatku wybudzenia, ten krok
     sie nie wykonuje. Gdy nie widzialo, pytamy baze jeszcze raz - i to
     zamyka dziure opisana przy `otaZlecenieWBazie()`.                   */
  if (!rtcOtaProsba) {
    uint32_t tsSwieze = 0;
    if (!otaZlecenieWBazie(tsSwieze)) return;   // aplikacja naprawde o nic nie prosi
    rtcOtaProsba = true;
    rtcOtaTs     = tsSwieze;
    LOG("[OTA] zlecenie dojechalo w trakcie wybudzenia (z %lu) - biore je teraz\n",
        (unsigned long)tsSwieze);
  }

  const uint32_t teraz = rtcTimeValid ? (uint32_t)time(nullptr) : 0;

  String wersja, md5;
  uint32_t rozmiar = 0;
  if (!otaPobierzOpis(wersja, md5, rozmiar)) {
    snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "nie moge pobrac opisu wersji");
    otaZanotujProbe(teraz);
    otaZglos();
    return;
  }

  prefs.begin(NVS_NAMESPACE, true);
  const uint16_t nieudane = prefs.getUShort("otaFail", 0);
  const uint32_t ostatnia = prefs.getUInt("otaTs", 0);
  prefs.end();

  const OtaDecyzja d = otaDecyzja(
      hasloWPamieci(), queueCount(), batteryPercentage, rtcCharging,
      (uint8_t)(nieudane > 255 ? 255 : nieudane), teraz, ostatnia, rozmiar,
      rtcOtaTs, md5, otaSumaWgranej(), otaSumaZPamieci("otaBad"));

  snprintf(rtcOtaWersja, sizeof(rtcOtaWersja), "%s", wersja.c_str());
  snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "%s", otaOpisDecyzji(d));
  LOG("[OTA] decyzja: %s\n", otaOpisDecyzji(d));

  /* "Nic nowego" znaczy, ze prosba zostala spelniona - kasujemy ja,
     zeby przycisk w aplikacji nie zostal wcisniety na zawsze. To samo
     przy wersji z czarnej listy i przy poddaniu sie: dalsze proby nic
     nie dadza, a niekasowalne polecenie probowaloby w kolko (ta sama
     lekcja co `wifiCmd`).                                            */
  if (d == OTA_NIC_NOWEGO || d == OTA_ZEPSUTA || d == OTA_PODDANO) {
    rtdbSend("DELETE", "/devices/" DEVICE_ID "/config/otaCmd.json", "");
    rtcOtaProsba = false;
    if (d == OTA_NIC_NOWEGO) otaWyzerujLicznik();
    otaZglos();
    return;
  }
  if (d != OTA_ROB) { otaZglos(); return; }   // powod minie sam

  /* Nowe zlecenie po serii niepowodzen zaczyna liczenie od zera - inaczej
     pierwsza kolejna porazka od razu wracalaby do "poddalem sie", a ekran
     nadal pokazywalby stary licznik.                                   */
  if (nieudane >= OTA_MAX_FAILS) {
    otaWyzerujLicznik();
    LOGLN("[OTA] nowe zlecenie po serii niepowodzen - licznik od zera");
  }

  /* Od tego miejsca leci ~1,2 MB przez radio. Limit czuwania trzeba
     podniesc, inaczej `awakeTooLong()` przerwie pobieranie w polowie. */
  extendAwake(OTA_HTTP_TIMEOUT_MS + 30000);
  otaZanotujProbe(teraz);

  /* SLAD W CZARNEJ SKRZYNCE, ZANIM COKOLWIEK ZACZNIEMY.

     TU BYLA LUKA: `logbookAdd()` stoi na koncu `goToSleep()`, czyli ZA
     aktualizacja. Gdy pobieranie konczylo sie restartem - udanym albo
     awaryjnym - wpis nie powstawal NIGDY. Akurat ta operacja, ktora
     trwa najdluzej i jako jedyna moze wywalic pudelko, nie zostawiala
     po sobie ani slowa. Kuba zglosil "zapikalo, ale nie bylo fanfar"
     i nie bylo czego przeczytac.

     Zapisujemy wiec od razu, wlasnym wywolaniem. Jesli pobieranie
     wywali plytke, po restarcie ta linijka bedzie jedynym dowodem, ze
     w ogole ruszylo.                                                 */
  {
    /* Wolna pamiec w slad - jesli plytka znowu padnie, ta liczba powie,
       czy zabraklo RAM-u, czy przyczyna byla inna. Bez niej zostaje
       zgadywanie, a to juz raz kosztowalo caly wieczor.               */
    char m[56];
    snprintf(m, sizeof(m), "ota:pobieram %s, wolne %u kB",
             rtcOtaWersja[0] ? rtcOtaWersja : "?",
             (unsigned)(ESP.getFreeHeap() / 1024));
    logbookAdd(m);
  }

  /* Dwa pikniecia: "zaczynam, zaraz zamilkne na minute". Bez tego
     pudelko wyglada na zawieszone - a brzeczyk jest jedynym kanalem,
     ktorym mowi cokolwiek bez telefonu (D29).                        */
  beepAck(); delay(150); beepAck();

  if (!otaWgraj(md5, rozmiar)) {
    /* `otaWgraj()` ustawilo juz konkretny powod - nadpisanie go ogolnym
       "nie doszlo do konca" zabieraloby jedyna informacje, ktora naprawde
       cos mowi. Ogolny tekst zostaje wylacznie wtedy, gdy nie zdazyl
       powstac zaden inny (np. samo polaczenie nie ruszylo).          */
    if (!rtcOtaMsg[0] || strstr(rtcOtaMsg, "pobieram"))
      snprintf(rtcOtaMsg, sizeof(rtcOtaMsg), "pobieranie nie doszlo do konca");
    logbookAdd(rtcOtaMsg[0] ? rtcOtaMsg : "ota:nieudane");
    /* DRUGA LINIJKA TO POMIAR, nie powtorzenie. Powod z `rtcOtaMsg` mowi,
       CO sie stalo; ta mowi, co serwer powiedzial o dlugosci pliku i ile
       zostalo RAM-u. Dopoki tych dwoch liczb nie widac z zewnatrz,
       "chunked" i "zabraklo pamieci" pozostaja domyslami - a domyslow
       w tej sprawie bylo juz dosc.                                     */
    {
      char m[40];
      snprintf(m, sizeof(m), "ota:naglowek %ld, wolne %u kB",
               (long)rtcOtaNagl, (unsigned)(ESP.getFreeHeap() / 1024));
      logbookAdd(m);
    }
    LOGLN("[OTA] nieudane - stary program zostaje bez zmian");
    beepErr();
    otaZglos();
    return;
  }

  /* Wgrane. Zapisujemy sume jako NIEPOTWIERDZONA - potwierdzi ja dopiero
     nowy program, gdy dojdzie do konca swojej pierwszej drogi.        */
  prefs.begin(NVS_NAMESPACE, false);
  nvsPutStr("otaPend", md5);
  nvsPutU16("otaBoot", 0);
  prefs.end();

  /* Polecenie kasujemy PRZED restartem - po nim nie wrocimy juz tutaj.
     Gdyby kasowanie nie doszlo, nowy program zobaczy te sama prosbe,
     policzy sume jako "aktualne" i skasuje ja wtedy. Samo sie naprawia. */
  rtdbSend("DELETE", "/devices/" DEVICE_ID "/config/otaCmd.json", "");

  /* Ostatnia chwila na zapis: za `esp_restart()` nie ma juz nic.     */
  {
    /* Z NUMEREM WERSJI. "ota:wgrane, restart" mowilo, ze cos sie wgralo -
       a pytanie, ktore sie wtedy zadaje, brzmi: CO sie wgralo. Historia
       ma na to odpowiadac sama, bez zestawiania jej z niczym innym.   */
    char m[48];
    snprintf(m, sizeof(m), "ota:wgrane %s, restart",
             rtcOtaWersja[0] ? rtcOtaWersja : "?");
    logbookAdd(m);
  }
  LOGLN("[OTA] wgrane - restart na nowa wersje");
  beepAck();
  delay(200);
  esp_restart();
}

#endif  /* OTA_ENABLED */

void goToSleep(uint32_t seconds) {
  buzzerOff();

#if TG_ENABLED
  /* --- Powiadomienie na telefon: PIERWSZE z trzech rzeczy przed snem ---
     Kolejnosc nie jest dowolna. Skan sieci potrafi zerwac lacze, a udana
     aktualizacja konczy sie restartem - wiadomosc puszczona za nimi nie
     poszlaby wcale albo poszlaby dopiero nastepnego dnia. Z trzech rzeczy,
     ktore pudelko robi przed zasnieciem, ta jedna dotyczy leku.

     Przy pustej skrzynce `tgWyslijZalegle()` wychodzi przed wlaczeniem
     radia, wiec zwykle wybudzenie nie placi za to nic.                */
  tgWyslijZalegle();
#endif

  /* Lista sieci PRZED aktualizacja, i celowo poza `#if OTA_ENABLED`.

     Przed - bo udana aktualizacja konczy sie restartem, wiec wszystko za
     nia i tak by nie doszlo; czlowiek czekajacy na liste dostalby zamiast
     niej ciszę. Kilka sekund skanu przed minuta pobierania nie robi
     roznicy, a poza blokiem - bo rozgladanie sie za WiFi nie ma nic
     wspolnego z tym, czy pudelko umie sie aktualizowac.               */
  skanujSieci();

#if OTA_ENABLED
  /* --- Aktualizacja programu: TU I TYLKO TU  (D59) --------------------
     To jest jedyne miejsce w calym programie, przez ktore przechodzi
     KAZDA sciezka wybudzenia - otwarcie wieczka, alarm, ladowarka,
     zwykle sprawdzenie. Jedno wywolanie pokrywa wiec wszystkie okazje
     naraz i nie trzeba pamietac o dopisaniu go w nowym miejscu.

     Rownie wazne jest, KIEDY tutaj docieramy: po tym, jak pudelko zrobilo
     juz wszystko, po co wstalo. Dawka jest zapisana i potwierdzona,
     status wyslany, kolejka opróżniona. Aktualizacja nie ma wiec jak
     wejsc przed obowiazkami ani ich opoznic - a przy niepustej kolejce
     `otaDecyzja()` i tak powie "nie" (zasada 6 w duchu: najpierw dane
     o leku, potem wygoda).

     `otaPotwierdzDzialanie()` idzie PIERWSZE i bezwarunkowo. Dojscie do
     tego miejsca jest wlasnie dowodem, ze program przeszedl cala swoja
     droge i nie wysypal sie po drodze - czyli ze swiezo wgrana wersja
     nadaje sie do uzytku. Bez tego wlasny licznik startow cofnalby ja
     po trzech wybudzeniach mimo tego, ze dziala.                      */
  otaPotwierdzDzialanie();
  otaSprobuj();                 // nie wraca, jesli wgralo nowa wersje
#endif

  wifiOff();

  /* Slad po tym wybudzeniu. Zapisujemy tuz przed snem, zeby zawierał
     takze to, co wydarzylo sie na samym koncu.                        */
  logbookAdd(rtcNote[0] ? rtcNote : "-");
  rtcNote[0] = 0;

  /* Jawne ustawienie podciagniec tuz przed snem - pinMode() bywa gubiony
     przy przechodzeniu w deep sleep.                                    */
#if USE_INTERNAL_PULLS
  gpio_pullup_en((gpio_num_t)GPIO_REED);
  gpio_pulldown_dis((gpio_num_t)GPIO_REED);
#else
  gpio_pullup_dis((gpio_num_t)GPIO_REED);     // zewnetrzny 1M do 3V3
  gpio_pulldown_dis((gpio_num_t)GPIO_REED);
#endif
  gpio_pullup_en((gpio_num_t)GPIO_BUTTON);    // przycisk rozwarty = 0 uA
  gpio_pulldown_dis((gpio_num_t)GPIO_BUTTON);

  /* =================================================================
     UZBRAJAMY WYLACZNIE KONTAKTRON - i to jednym wywolaniem.

     esp_deep_sleep_enable_gpio_wakeup() NIE SUMUJE kolejnych wywolan.
     Kazde z nich nadpisuje maske i poziom ustawione poprzednio. Przez to
     uzbrojenie przycisku (wywolywane jako drugie) kasowalo uzbrojenie
     kontaktronu i pudelko nigdy - ani razu - nie obudzilo sie na otwarcie.
     Widac to wprost w logu: "uzbrojone zrodla: kontaktron ... | przycisk
     ... ", a po wybudzeniu maska 0x00000020, czyli sam GPIO5. Bit
     kontaktronu nie zapalil sie nigdy.

     Poziomu tez nie da sie pogodzic: maska ma JEDEN wspolny poziom, a nasze
     piny potrzebuja przeciwnych - kontaktron budzi stanem WYSOKIM (magnes
     odsuniety), przycisk NISKIM (zwarcie do masy). Przy stykach idacych do
     GND nie ma na to sposobu bez lutowania.

     Wybor jest wiec prosty: kontaktron pracuje codziennie i jest calym
     sensem urzadzenia, a przycisk sluzy do jednorazowej konfiguracji WiFi.
     Przycisk dostaje wlasny gest, ktory nie potrzebuje deep sleep:
     PRZYTRZYMAJ GO PODCZAS RESETU.
     ================================================================= */
  rtcArmedForClose = false;
  esp_deepsleep_gpio_wake_up_mode_t reedMode;

  if (boxIsOpen()) {
    /* Pin jest juz w stanie "otwarte", wiec uzbrojenie go na ten sam poziom
       obudziloby uklad natychmiast, w petli. Odwracamy poziom i czekamy na
       ZAMKNIECIE - dzieki temu zamkniecie wieczka od razu konczy stan
       alarmowy, bez czekania na timer.                                  */
    reedMode = (REED_OPEN_LEVEL == HIGH) ? ESP_GPIO_WAKEUP_GPIO_LOW
                                         : ESP_GPIO_WAKEUP_GPIO_HIGH;
    rtcArmedForClose = true;

    uint32_t cap = openWarnSecondsLeft();
    if (seconds > cap) seconds = cap;
    LOG("[SLP] pudelko otwarte - obudze sie za %lu s albo przy zamknieciu\n",
        (unsigned long)cap);
  } else {
    reedMode = (REED_OPEN_LEVEL == HIGH) ? ESP_GPIO_WAKEUP_GPIO_HIGH
                                         : ESP_GPIO_WAKEUP_GPIO_LOW;
  }

  if (seconds < 5) seconds = 5;
  if (seconds > HOUSEKEEP_MAX_S) seconds = HOUSEKEEP_MAX_S;

  /* JEDNO wywolanie, jeden pin - nie ma czego nadpisac. */
  esp_deep_sleep_enable_gpio_wakeup(1ULL << GPIO_REED, reedMode);

  esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);

  /* =================================================================
     TO SA DWIE LINIE, PRZEZ KTORE KONTAKTRON W OGOLE DZIALA.

     esp_deep_sleep_start() tuz przed zasnieciem SAM przestawia
     podciagniecia na pinach wybudzania - i robi to ODWROTNIE do
     ustawionego poziomu. Dla pinu czekajacego na stan WYSOKI wlacza
     wewnetrzny rezystor sciagajacy do masy (~10 kOhm), zeby pin sam z
     siebie nie wywolal wybudzenia. W dokumentacji stoi to wprost:
     "you don't need to care to pull-up or pull-down before using this
     function, because this will be set internally in
     esp_deep_sleep_start based on the wakeup mode".

     Przy naszym ukladzie to jest zabojcze. Kontaktron zwiera pin do masy,
     gdy pudelko jest ZAMKNIETE, a stan wysoki daje dopiero rezystor
     podciagajacy 100 kOhm po otwarciu. Wewnetrzne 10 kOhm do masy wygrywa
     z zewnetrznymi 100 kOhm w stosunku 10:1 - na pinie zostaje jakies
     0,4 V zamiast 3,3 V. Prog stanu wysokiego nigdy nie zostaje
     przekroczony, wiec pudelko NIE MA JAK sie obudzic, choc wszystko
     inne jest ustawione poprawnie.

     Dokladnie ten przypadek, z tym samym rezystorem 100 kOhm i
     stykiem do masy, opisuje zgloszenie espressif/esp-idf#12183.
     Zatwierdzone tam obejscie to zatrzasniecie konfiguracji pinu przed
     snem: gpio_hold_en() utrwala podciagniecie, przez co pozniejsze
     wlaczenie rezystora sciagajacego nie ma juz zadnego skutku.

     Naprawa w rdzeniu trafila dopiero do ESP-IDF 5.2+, a i tam
     domyslne zachowanie zostalo bez zmian - wiec to obejscie zostaje.
     ================================================================= */
  gpio_hold_en((gpio_num_t)GPIO_REED);
  gpio_deep_sleep_hold_en();

  /* Wypisujemy wprost, co ma nas obudzic. Gdy pudelko "nie budzi sie na
     kontaktron", to jest pierwsza linia, ktora trzeba sprawdzic.        */
  LOG("[SLP] uzbrojony kontaktron GPIO%d: budzi przy stanie %s (%s) | timer=%lu s\n",
      GPIO_REED,
      reedMode == ESP_GPIO_WAKEUP_GPIO_HIGH ? "WYSOKIM" : "NISKIM",
      rtcArmedForClose ? "czekam na ZAMKNIECIE" : "czekam na OTWARCIE",
      (unsigned long)seconds);
  LOGLN("[SLP] przycisk nie budzi ze snu - portal WiFi: przytrzymaj go przy RESECIE");
  LOGLN("[SLP] podciagniecie kontaktronu ZATRZASNIETE (bez tego rdzen wlacza"
        "\n[SLP] rezystor do masy i pin nigdy nie osiaga stanu wysokiego)");
  LOG("[SLP] stan pinow teraz: kontaktron=%d, przycisk=%d\n",
      digitalRead(PIN_REED), digitalRead(PIN_BUTTON));
  LOG("[SLP] deep sleep na %lu s\n", (unsigned long)seconds);
#if DEBUG_SERIAL
  Serial.flush();
#endif
  esp_deep_sleep_start();
}

/* Ile spac po obsluzeniu biezacego zdarzenia.
   Wybieramy NAJWCZESNIEJSZY z powodow do wybudzenia.                  */
uint32_t planNextSleep() {
  time_t now = time(nullptr);

  /* 1. Alarm czeka na ponowna probe - drzemka ma pierwszenstwo. */
  if (rtcPendingSlot >= 0 && rtcAlarmRetries < MAX_ALARM_RETRIES) return SNOOZE_S;

  /* 2. Nastepna zaplanowana dawka (30 s zapasu na dryf zegara). */
  uint32_t s = secondsToNextSlot(now);
  if (s > 30) s -= 30;

  /* 3. Zaleglosci w kolejce -> ponow probe z narastajaca przerwa:
        15 min, 30, 60, 120, 240 min. Chwilowa awaria routera zalatwia
        sie sama, a stale zerwana siec nie zjada baterii.             */
  uint16_t q = queueCount();
  if (q > 0) {
    uint8_t shift = rtcRetryCount > 4 ? 4 : rtcRetryCount;
    uint32_t r = (uint32_t)RETRY_BASE_S << shift;
    if (r > RETRY_MAX_S) r = RETRY_MAX_S;
    LOG("[SLP] %u zdarzen w kolejce, ponowna proba za %lu s\n", q, (unsigned long)r);
    if (r < s) s = r;
  }

  /* 3b. Status nie dotarl do aplikacji - ponawiamy z tym samym backoffem.

        Bez tego punktu nieudana wysylka czekala na najblizsza dawke albo na
        HOUSEKEEP_MAX_S, czyli nawet 12 godzin. Aplikacja pokazywalaby przez
        ten czas nieprawde - a caly sens tego pudelka polega na tym, ze temu
        co widac na telefonie mozna wierzyc.                             */
  if (rtcStatusDirty || rtcOpenClearPend || rtcOpenReported != boxIsOpen()) {
    uint8_t shift = rtcRetryCount > 4 ? 4 : rtcRetryCount;
    uint32_t r = (uint32_t)RETRY_BASE_S << shift;
    if (r > RETRY_MAX_S) r = RETRY_MAX_S;
    LOGLN("[SLP] aplikacja nie ma aktualnego stanu - ponowna proba");
    if (r < s) s = r;
  }

  /* 3c. Ladowarka = prad za darmo. Meldujemy sie co minute, zeby w
         aplikacji bylo widac rosnacy procent na zywo, bez klikania. */
  if (rtcCharging && CHARGE_POLL_S < s) s = CHARGE_POLL_S;

  /* 4. Rolowanie doby minute po jej koncu (DAY_START_HOUR). */
#if MIDNIGHT_CHECK
  uint32_t m = secondsToDayBoundary(now);
  if (m < s) s = m;
#endif

  /* 5. Pudelko zostawione otwarte - nie spimy dluzej niz do sygnalu. */
  if (boxIsOpen()) {
    uint32_t w = openWarnSecondsLeft();
    if (w < s) s = w;
  }

  /* 6. Nie spimy dluzej niz HOUSEKEEP_MAX_S - okazja do sync czasu. */
  if (s > HOUSEKEEP_MAX_S) s = HOUSEKEEP_MAX_S;
  return s;
}

/* =====================================================================
 * 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)
 * ===================================================================== */
/* Czuwanie na ladowarce.

   Wychodzimy na trzy sposoby: wyciagniety kabel, otwarte wieczko albo
   bezpiecznik czasowy. Otwarcie wieczka jest tu wazniejsze niz wyglada -
   plytka JUZ czuwa, wiec kontaktron nie wywola zadnego przerwania i
   otwarcie przeszloby niezauwazone. Dlatego pilnujemy go sami, a obsluge
   oddajemy normalnej sciezce przez krotki sen: kontaktron stoi wtedy w
   stanie wybudzenia, wiec plytka wstaje natychmiast jako WAKE_REED i
   dawka zapisuje sie dokladnie tak samo jak zawsze.                   */
void petlaLadowania() {
  if (!rtcCharging || batterySaver) return;
  if (!wifiConnect() || !firebaseSignIn()) {
    LOGLN("[CHG] brak sieci - zostaje przy zwyklym cyklu snu");
    return;
  }
  LOGLN("[CHG] ladowanie - zostaje na lacza, bez usypiania");
  pushStatus();

  const uint32_t t0 = millis();
  uint32_t ostatniPomiar = millis(), ostatniMeldunek = millis();
  float poprzednieV = realBatteryVoltage;
  int   ostatniPct  = batteryPercentage;
  bool  bylOtwarty  = boxIsOpen();

  while (rtcCharging && millis() - t0 < CHARGE_AWAKE_MAX_S * 1000UL) {
    extendAwake(120000);                 // bezpiecznik czuwania nie moze przerwac

    /* Czuwanie moze trwac godzinami, wiec w jego trakcie MOZE wypasc pora
       dawki albo koniec doby lekowej. Spiace pudelko obudzilby timer -
       czuwajace nie dostanie niczego. Bez tego ladowanie od 19 do 23
       polknieloby przypomnienie o 20:00 bez sladu.

       Nie powielamy tu obslugi alarmu. Oddajemy sterowanie normalnej
       sciezce przez sekundowy sen - dokladnie jak przy wieczku.        */
    if (rtcTimeValid) {
      time_t ter = time(nullptr);
      uint32_t doSlotu = secondsToNextSlot(ter);
      uint32_t doDoby  = secondsToDayBoundary(ter);
      if (doSlotu <= 2 || doDoby <= 2) {
        LOGLN("[CHG] pora dawki lub koniec doby - oddaje normalnej obsludze");
        note("termin przy ladowaniu");
        goToSleep(1);
      }
    }

    const bool teraz = boxIsOpen();
    if (teraz && !bylOtwarty) {
      LOGLN("[CHG] wieczko otwarte podczas ladowania - oddaje normalnej obsludze");
      note("otwarte przy ladowaniu");
      goToSleep(1);                      // wstanie natychmiast jako WAKE_REED
    }
    bylOtwarty = teraz;

    if (millis() - ostatniPomiar >= CHARGE_SAMPLE_S * 1000UL) {
      ostatniPomiar = millis();

      /* Skoro juz wisimy na lacza, to sluchamy w OBIE strony. Zmiana
         godziny przypomnienia w telefonie wchodzi w zycie od razu,
         zamiast czekac na nastepne wybudzenie. Poza ladowarka takiej
         mozliwosci nie ma - tam pudelko spi i nikt go nie dobudzi.  */
      if (wifiConnect() && firebaseSignIn()) { fetchConfig(); flushQueue(); }

      readBattery();
      trackCharging(realBatteryVoltage, poprzednieV);
      poprzednieV = realBatteryVoltage;
      /* Procentu podczas ladowania NIE ruszamy - mierzylibysmy napiecie
         ladowarki, nie ogniwa. Zostaje ostatni wiarygodny odczyt.     */
      if (rtcBattPct != 255) batteryPercentage = (int)rtcBattPct;

      const bool zmiana = (batteryPercentage != ostatniPct);
      if (zmiana || millis() - ostatniMeldunek >= CHARGE_PUSH_MAX_S * 1000UL) {
        if (wifiConnect() && firebaseSignIn()) {
          pushStatus();
          ostatniPct = batteryPercentage;
          ostatniMeldunek = millis();
        }
      }
    }
    delay(200);
  }

  /* Kabel wyciagniety - teraz mozna zmierzyc PRAWDE. Napiecie ogniwa
     opada do swojej rzeczywistej wartosci dopiero po kilkunastu
     sekundach od odlaczenia, wiec dajemy mu czas i dopiero wtedy
     kasujemy filtr i przyjmujemy swiezy odczyt jako punkt wyjscia.  */
  if (!rtcCharging) {
    LOGLN("[CHG] kabel odlaczony - czekam na ustabilizowanie napiecia");
    uint32_t tS = millis();
    while (millis() - tS < CHARGE_SETTLE_S * 1000UL) { extendAwake(60000); delay(250); }
    readBattery();
    resetBatteryFilter();
    batteryPercentage = batteryRawPercentage;
    rtcBattPct = (uint8_t)batteryRawPercentage;
    LOG("[CHG] stan po ladowaniu: %d%% (%.2f V)\n",
        batteryPercentage, realBatteryVoltage);
  }

  /* Ostatni meldunek - zeby aplikacja zgasila wskaznik ladowania od razu,
     a nie dopiero przy nastepnym wybudzeniu.                          */
  LOG("[CHG] koniec czuwania na ladowarce po %lu s (%s)\n",
      (unsigned long)((millis() - t0) / 1000),
      rtcCharging ? "bezpiecznik czasowy" : "kabel odlaczony");
  if (wifiConnect() && firebaseSignIn()) pushStatus();
  else rtcStatusDirty = true;
}


void setup() {

  /* --- Okno na wgranie programu -------------------------------------
     Po resecie trzymamy sie wybudzeni przez chwile, zeby port USB zdazyl
     sie zglosic i mozna bylo wgrac nowy program. Bez tego plytka zasypia
     szybciej, niz komputer ja zauwazy.                                 */
  /* Zwalniamy zatrzask zalozony przed snem (patrz goToSleep). Bez tego pin
     kontaktronu zostalby zamrozony w konfiguracji sprzed uspienia i
     pinMode() nie mialby na niego wplywu.                              */
  gpio_deep_sleep_hold_dis();
  gpio_hold_dis((gpio_num_t)GPIO_REED);

  rtcBootCount++;
  configureInputs();          // MUSI byc przed oknem testowym ponizej
  buzzerOff();

#if OTA_ENABLED
  /* Licznik startow swiezo wgranej wersji - jak najwczesniej, zanim
     cokolwiek innego zdazy sie wysypac. Jesli nowy program nie dochodzi
     do `goToSleep()`, po OTA_BOOT_TRIES probach wracamy na poprzednia
     partycje. To nasz wlasny rollback: Arduino buduje ESP32 bez tego
     bootloaderowego, wiec bez tej linii zepsuta wersja zamienialaby
     pudelko w cegle do czasu kabla (D59).                             */
  otaSprawdzPoStarcie();
#endif

  /* --- Ostrzezenie "juz dzis brales" - NATYCHMIAST -------------------
     Ten sygnal ma jedno zadanie: zatrzymac Twoja reke, zanim wysypiesz
     druga dawke. Kazda dziesiata sekundy opoznienia jest tu realna, bo
     odkrecasz wieczko szybciej, niz plytka konczy rozruch.

     Wczesniej lecial dopiero po starcie portu szeregowego (300 ms
     zwloki), pomiarze baterii i kilku odczytach z pamieci - razem grubo
     ponad pol sekundy po odkreceniu. Sygnal przychodzil, gdy tabletka
     byla juz w dloni.

     Teraz jest pierwszy. Wszystko, czego potrzebuje, siedzi w pamieci
     RTC i jest dostepne od razu: znacznik dnia i zegar. Reszta rozruchu
     - port, bateria, siec - dzieje sie PO nim.                        */
  wakeReason = detectWakeReason();

  /* Znacznik dawki MUSI byc odtworzony przed sygnalem.

     Po deep sleepie siedzi w pamieci RTC i jest od razu. Ale po kazdym
     resecie - wgraniu programu, zaniku zasilania, brownoucie - RTC jest
     wyzerowane, a prawda lezy w pamieci nieulotnej. Bez tego wywolania
     wczesny sygnal czytal ZERO i milczal dokladnie wtedy, gdy ostrzezenie
     przed druga dawka jest najbardziej potrzebne.                     */
  loadDayMarkers();

  bool juzOstrzezono = false;
#if ONE_DOSE_PER_DAY
  if (wakeReason == WAKE_REED && boxIsOpen() && juzDzisBrane()) {
    beepAlreadyTaken();
    juzOstrzezono = true;
  }
#endif

#if DEBUG_SERIAL
  Serial.begin(115200);
  delay(300);
#endif
  if (juzOstrzezono) LOGLN("[REED] ostrzezenie o powtorce dane natychmiast");

#if BOOT_HOLD_MS > 0
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
    /* --- Okno na wgranie programu, a przy okazji test stykow ----------
       Deep sleep odlacza port USB, wiec wybudzenia kontaktronem nie da
       sie zobaczyc w monitorze portu szeregowego. Tu, dopoki plytka
       jeszcze nie spi, kazda zmiana stanu jest wypisywana od razu.
       Dzieki temu wiadomo, czy styk w ogole dziala - a to rozdziela dwa
       zupelnie rozne problemy: zle okablowanie od zlego wybudzania.    */
    LOG("\n[USB] okno na wgrywanie: %d s (mozesz teraz kliknac Upload)\n",
        BOOT_HOLD_MS / 1000);
    LOGLN("[TEST] To jest takze test stykow. Przysun i odsun magnes od");
    LOGLN("[TEST] kontaktronu - kazda zmiana pojawi sie ponizej.");
    LOGLN("[TEST] TRZYMAJ przycisk do konca okna  -> portal WiFi.");
    LOGLN("[TEST] Nacisnij i PUSC przycisk        -> kasowanie znacznika dawki.");

    bool     lastReed = boxIsOpen(), lastBtn = buttonPressed(), btnHeld = false;
    uint16_t reedChanges = 0, btnChanges = 0;
    LOG("[TEST] stan poczatkowy: kontaktron=%d (%s), przycisk=%d (%s)\n",
        digitalRead(PIN_REED),   lastReed ? "OTWARTE"   : "zamkniete",
        digitalRead(PIN_BUTTON), lastBtn  ? "wcisniety" : "zwolniony");

    uint32_t hold = millis(), window = BOOT_HOLD_MS;
    while (millis() - hold < window) {
      bool r = boxIsOpen(), b = buttonPressed();
      if (r != lastReed) {
        lastReed = r; reedChanges++;
        LOG("\n[TEST] kontaktron -> %s (pin=%d)\n",
            r ? "OTWARTE" : "ZAMKNIETE", digitalRead(PIN_REED));
        /* Kazda zmiana przedluza okno, zeby dalo sie spokojnie pomachac
           magnesem kilka razy. Sufit chroni przed czuwaniem bez konca.  */
        if (window < BOOT_TEST_MAX_MS) window = (millis() - hold) + BOOT_HOLD_MS;
      }
      if (b != lastBtn) {
        lastBtn = b; btnChanges++;
        if (b) btnHeld = true;
        LOG("\n[TEST] przycisk -> %s\n", b ? "WCISNIETY" : "zwolniony");
        if (window < BOOT_TEST_MAX_MS) window = (millis() - hold) + BOOT_HOLD_MS;
      }
#if DEBUG_SERIAL
      if ((millis() - hold) % 1000 < 20) { Serial.print("."); Serial.flush(); }
#endif
      delay(20);
    }
    LOGLN("");
    LOG("[TEST] kontaktron zmienil stan %u raz(y), przycisk %u raz(y)\n",
        reedChanges, btnChanges);

    /* Historia z pamieci nieulotnej - jedyny sposob, zeby dowiedziec sie,
       co pudelko robilo przez ostatnie dni, skoro port USB spi razem z nim. */
    logbookPrint();
    if (reedChanges == 0)
      LOGLN("[TEST] Jesli ruszales magnesem, a tu jest zero - winne jest\n"
            "[TEST] okablowanie lub sam kontaktron, nie oprogramowanie.");
    else
      LOGLN("[TEST] Styk dziala. Jesli mimo to pudelko nie budzi sie ze snu,\n"
            "[TEST] problem lezy w wybudzaniu, nie w polaczeniu.");

    /* Przycisk nie budzi juz ze snu (nie da sie go pogodzic z kontaktronem
       w jednej masce wybudzania), wiec portal WiFi ma teraz wlasny gest:
       przytrzymaj przycisk i nacisnij RESET.                            */
    if (buttonPressed()) {
      LOGLN("[BTN] przycisk trzymany przy resecie -> tryb konfiguracji WiFi");
      portalRequested = true;
    } else if (btnHeld) {
      clearDayMarkers();
      LOGLN("[TEST] znacznik dzisiejszej dawki skasowany - mozesz testowac otwieranie");
      beepAck();
    }
    extendAwake(20000);                    // nie zjadaj limitu czuwania
  }
#endif

  readBattery();
  loadSchedule();
  loadDosing();
  /* loadDayMarkers() wywolane juz na samym poczatku setup() - musi byc
     przed sygnalem o powtorce, a nie dopiero tutaj.                   */

  /* Przycisk trzymany podczas resetu zastepuje dawne wybudzanie przyciskiem. */
  if (portalRequested) {
    rtcStuckButton = 0;                     // gest jest swiadomy, nie zacisniety styk
    wakeReason = WAKE_BUTTON;
  }

  /* --- Ochrona ogniwa i wykrywanie ladowarki --------------------------
     Odczyt ponizej 2,0 V = brak baterii (zasilanie z USB) - ignorujemy.  */
  bool haveCell = realBatteryVoltage > 2.0f;

  /* Napiecie wyraznie wyzsze niz przy poprzednim wybudzeniu oznacza, ze
     ktos wlasnie podlaczyl ladowarke. To najlepszy moment, zeby zlapac
     siec i ustawic zegar - zwlaszcza gdy pudelko wczesniej padlo.       */
  bool chargingDetected = haveCell && rtcLastVoltage > 2.0f &&
                          realBatteryVoltage > rtcLastVoltage + CHARGE_RISE_V;
  LOG("[CHG] U=%.3f  poprz=%.3f  laduje=%d  wysokich=%u  blok=%d\n",
      realBatteryVoltage, rtcLastVoltage, rtcCharging ? 1 : 0,
      rtcWysokieZRzedu, rtcBlokWysokie ? 1 : 0);

  /* Stan ladowania liczymy PRZED nadpisaniem poprzedniego napiecia. */
  if (trackCharging(realBatteryVoltage, rtcLastVoltage)) {
    /* NIE aktualizujemy tu procentu i to jest swiadoma decyzja.

       Uklad ladujacy trzyma na ogniwie swoje napiecie - okolo 4,2 V -
       niezaleznie od tego, ile energii naprawde w nim jest. Dzielnik
       mierzy wiec napiecie LADOWARKI, nie stanu naladowania, i kazdy
       odczyt wychodzi 100%. Pokazywanie tego w aplikacji byloby po
       prostu klamstwem, i to takim, ktore znika dopiero po odlaczeniu.

       Zamiast tego zostawiamy ostatni wiarygodny odczyt sprzed
       ladowania, a aplikacja jawnie pisze, ze pomiar bedzie po kablu.
       Prawdziwy stan mierzymy po odlaczeniu - patrz petlaLadowania.  */
    batteryPercentage = rtcBattPct == 255 ? batteryRawPercentage : (int)rtcBattPct;
  }
  rtcLastVoltage = realBatteryVoltage;

  if (rtcCutoff) {
    /* Bylismy odciete. Wracamy do zycia dopiero po realnym doladowaniu. */
    if (haveCell && realBatteryVoltage < BATT_RECOVER_V) {
      LOG("[BAT] %.2f V - nadal za malo, sprawdze za %d s\n",
          realBatteryVoltage, CUTOFF_RECHECK_S);
      goToSleep(CUTOFF_RECHECK_S);
    }
    LOGLN("[BAT] ladowanie wykryte - wracam do pracy");
    rtcCutoff = false;
    rtcTimeValid = false;                   // po padzie zegar jest bez wartosci
    resetBatteryFilter();                   // wskazanie ma znow moc rosnac
    batteryPercentage = batteryRawPercentage;
    rtcBattPct = (uint8_t)batteryRawPercentage;
    beepCharging();
    if (wifiConnect() && firebaseSignIn()) { fetchConfig(); flushQueue(); pushStatus(); }
    goToSleep(planNextSleep());
  }

  if (haveCell && realBatteryVoltage < BATT_CUTOFF_V) {
    LOG("[BAT] %.2f V - odcinam sie dla ochrony ogniwa\n", realBatteryVoltage);
    note("bateria odcieta");
    beepLowBattery(true);
    rtcCutoff = true;
    /* NIE spimy bezterminowo - budzimy sie co 15 min, zeby wykryc moment
       podlaczenia ladowarki i od razu zsynchronizowac zegar.            */
    goToSleep(CUTOFF_RECHECK_S);
  }

  batterySaver = haveCell && realBatteryVoltage < BATT_SAFE_V;
  if (batterySaver) LOGLN("[BAT] niskie napiecie - loguje offline, bez WiFi");

  if (chargingDetected) {
    LOGLN("[BAT] wykryto ladowanie - synchronizuje czas");
    /* Filtr baterii z zasady nie pozwala wskazaniu rosnac. Ladowanie to
       jedyny moment, w ktorym rosnac powinno - wiec kasujemy historie
       i przyjmujemy swiezy odczyt bez inercji.                          */
    resetBatteryFilter();
    batteryPercentage = batteryRawPercentage;
    rtcBattPct = (uint8_t)batteryRawPercentage;
    LOG("[BAT] filtr wyzerowany, wskazanie %d%%\n", batteryPercentage);
    beepCharging();
    batterySaver = false;                   // ladujemy sie, mozna wlaczyc radio
    if (wifiConnect() && firebaseSignIn()) { fetchConfig(); flushQueue(); pushStatus(); }
  }

  LOG("\n===== boot #%lu  powod: %s  bat=%d%% =====\n",
      (unsigned long)rtcBootCount, wakeName(wakeReason), batteryPercentage);
  LOG("[GPIO] kontaktron=%d (pudelko %s), przycisk=%d (%s)\n",
      digitalRead(PIN_REED),   boxIsOpen()     ? "OTWARTE"  : "zamkniete",
      digitalRead(PIN_BUTTON), buttonPressed() ? "wcisniety" : "zwolniony");

  /* Pilnowanie otwartego wieczka dziala niezaleznie od powodu wybudzenia -
     stad przed switchem, a nie w ktorymkolwiek z przypadkow.            */
  boxOpenWarned = trackBoxOpen();

  switch (wakeReason) {

    /* ---------- Z. Wieczko wlasnie zamkniete ----------
       Nic wiecej nie trzeba robic - trackBoxOpen() juz wyczyscil stan
       alarmowy, a ewentualne powiadomienie aplikacji poleci nizej.    */
    case WAKE_CLOSED:
      LOGLN("[OPN] wybudzenie: ktos zamknal pudelko");
      note("wieczko zamkniete");
      break;

    /* ---------- A. Pierwsze wlaczenie / reset ---------- */
    case WAKE_BOOT: {
      bool haveCreds = false;
      { wifi_config_t wc;
        WiFi.mode(WIFI_STA);
        delay(50);                                   // czas na inicjalizacje sterownika
        if (esp_wifi_get_config(WIFI_IF_STA, &wc) == ESP_OK)
          haveCreds = strlen((const char*)wc.sta.ssid) > 0;
        if (!haveCreds && WiFi.SSID().length() > 0) haveCreds = true;   // zapasowa sciezka
      }
      if (!haveCreds) {
        LOGLN("[SYS] brak zapisanego WiFi -> portal konfiguracyjny");
        note("brak zapisanego WiFi");
        startWifiPortal();
      } else {
        note("uruchomienie");
        if (wifiConnect()) {
          if (firebaseSignIn()) { fetchConfig(); flushQueue(); pushStatus(); }
#if REPORT_BOOT_EVENT
          reportEvent("boot", -1);
#endif
        } else {
          note("uruchomienie bez sieci");
#if REPORT_BOOT_EVENT
          queuePush(makeRecord("boot", -1));
#endif
        }
      }
      break;
    }

    /* ---------- B. Kontaktron: ktos otworzyl pudelko ---------- */
    case WAKE_REED: {
      /* Otwarcie pudelka to najwazniejsze zdarzenie w calym urzadzeniu -
         dajemy mu z zapasem czasu na sygnal, polaczenie i wyslanie.     */
      extendAwake(60000);
#if ONE_DOSE_PER_DAY
      /* --- Bierzesz raz dziennie. Liczy sie TYLKO pierwsze otwarcie. ---
         Ta kontrola musi byc PRZED antydrganiami. Wczesniej bylo odwrotnie
         i drugie otwarcie w ciagu minuty konczylo sie cisza zamiast
         ostrzezenia - a to wlasnie wtedy najbardziej grozi podwojna dawka. */
      if (juzDzisBrane()) {
        LOGLN("[REED] dawka juz zapisana -> OSTRZEZENIE");
        note("juz dzis brane");
        /* Sygnal zwykle poszedl juz na samym poczatku setup(). Tutaj
           zostaje wylacznie dla przypadku, w ktorym tam sie nie udal
           - np. wieczko drgnelo i w tamtej chwili bylo zamkniete.  */
        if (!juzOstrzezono) beepAlreadyTaken();

        /* Z WIARYGODNYM ZEGAREM mamy pewnosc, ze to powtorka - konczymy
           i nie zapisujemy niczego.

           BEZ ZEGARA to tylko podejrzenie oparte na czasie od poprzedniej
           dawki. Ostrzec trzeba (od tego ten sygnal jest), ale otwarcia
           NIE WOLNO wyrzucic: gdyby to jednak byla nowa doba, prawdziwa
           dawka przepadlaby bez sladu. Falszywe pikniecie kosztuje
           sekunde, zgubiona dawka - dzien w kalendarzu.              */
        if (rtcTimeValid) {
          gestPoOtwarciu = czekajNaZamkniecieIGest(CZEKAJ_ZAMKNIECIE_MS);
          break;
        }
        LOGLN("[REED] ...ale bez zegara to tylko podejrzenie - zapisuje mimo to");
      }
#endif

      /* Antydrgania: dwa drgniecia styku w ciagu 60 s to jedno otwarcie.
         Dotyczy juz tylko sytuacji, gdy dawki jeszcze dzis nie bylo.     */
      if (rtcTimeValid) {
        uint32_t now = (uint32_t)time(nullptr);
        if (rtcLastOpenTs && now > rtcLastOpenTs && now - rtcLastOpenTs < 60) {
          LOGLN("[REED] drgniecie styku w ciagu 60 s - pomijam");
          note("drgniecie styku");
          break;
        }
        rtcLastOpenTs = now;
      }

#if ONE_DOSE_PER_DAY
      zapiszDawke();
#endif

      int slot = (rtcPendingSlot >= 0) ? rtcPendingSlot : matchSlot(time(nullptr));
      if (slot < 0 && slotCount == 1) slot = 0;   // raz dziennie: zawsze slot 0
      LOG("[REED] otwarcie zapisane, slot=%d%s\n", slot,
          rtcTimeValid ? "" : "  (zegar jeszcze nieznany)");

      rtcPendingSlot  = -1;                 // alarm obsluzony
      rtcAlarmRetries = 0;
      reportEvent("open", slot);

#if ONE_DOSE_PER_DAY
      /* Jesli w chwili otwarcia nie znalismy jeszcze daty, znacznik nie
         mogl powstac. reportEvent() laczy sie z siecia i ustawia zegar,
         wiec dopiero teraz mozemy go domknac - inaczej kolejne otwarcie
         tego samego dnia nie zostaloby rozpoznane.                      */
      if (rtcTimeValid) {
        uint32_t today = localDayNumber(time(nullptr));
        if (rtcTakenDay != today) {
          setTakenDay(today);
          LOG("[DAY] znacznik dawki uzupelniony po synchronizacji: %lu\n",
              (unsigned long)today);
        }
      }
#endif

      /* Poczekaj az pudelko zostanie zamkniete, inaczej wysoki stan na
         pinie natychmiast wybudzi uklad ponownie.                      */
      /* To samo czekanie co dawniej, tylko po drodze obserwujemy przycisk.
         Jesli w tym czasie zamknales wieczko, aplikacja dowie sie o tym
         od razu - blok na koncu setup() zauwazy rozjazd i wysle status. */
      gestPoOtwarciu = czekajNaZamkniecieIGest(CZEKAJ_ZAMKNIECIE_MS);
      break;
    }

    /* ---------- C. Przycisk: portal konfiguracji WiFi ---------- */
    case WAKE_BUTTON:
      /* Przycisk zwarty na stale wybudzalby uklad w kolko i pudelko
         piszczaloby bez konca. Po kilku takich wybudzeniach z rzedu
         przestajemy reagowac, dopoki styk sie nie rozewrze.            */
      if (rtcStuckButton >= 3) {
        LOG("[BTN] przycisk zwarty od %u wybudzen - ignoruje\n", rtcStuckButton);
        break;
      }
      LOGLN("[BTN] tryb konfiguracji WiFi (portal)");
      note("portal WiFi");
      startWifiPortal();
      break;

    /* ---------- D. Timer: pora na lek albo rutynowy meldunek ---------- */
    case WAKE_TIMER: {
      if (!rtcTimeValid) {                  // bez czasu nie wiemy co robic
        if (wifiConnect() && firebaseSignIn()) fetchConfig();
      }
#if MIDNIGHT_CHECK
      checkDayRollover();                   // czy poprzednia doba sie domknela
#endif
      int slot = matchSlot(time(nullptr));

      if (rtcPendingSlot >= 0) slot = rtcPendingSlot;   // kontynuacja alarmu

#if ONE_DOSE_PER_DAY
      /* Jesli dawka na dzis juz zostala zapisana - nie dzwon w ogole. */
      if (slot >= 0 && (juzDzisBrane() || alarmJuzObsluzony(slot))) {
        LOGLN("[ALM] dawka na dzis zalatwiona (wzieta albo odzwoniona) - cisza");
        rtcPendingSlot = -1;
        rtcAlarmRetries = 0;
        slot = -1;
      }
#endif

      /* Dzien rozpisany BEZ leku - nie ma o czym przypominac.

         To nie jest oszczednosc baterii, tylko bezpieczenstwo: dzwonek
         w dniu odstawienia przed zabiegiem zacheca do wziecia tabletki,
         ktorej brac nie wolno. Nie zglaszamy tez "missed" - dzien bez leku
         nie jest dniem pominietym i aplikacja liczy go tak samo.

         dzisBezLeku() zwraca true WYLACZNIE przy pewnym zerze: bez zegara
         albo bez rozpisania dzwonimy normalnie.                          */
      if (slot >= 0 && dzisBezLeku()) {
        LOGLN("[ALM] dzien rozpisany bez leku - cisza");
        note("dzien bez leku");
        rtcPendingSlot = -1;
        rtcAlarmRetries = 0;
        oznaczAlarmObsluzony(slot);
        slot = -1;
      }

      if (slot >= 0) {
        rtcPendingSlot = slot;
        LOG("[ALM] slot %d (%s), proba %u/%u\n",
            slot, slots[slot].c_str(), rtcAlarmRetries + 1, MAX_ALARM_RETRIES);

        bool opened = runAlarmWindow();
        /* Dzwonienie moglo zjesc caly limit - zostawiamy czas na wyslanie. */
        extendAwake(60000);

        if (opened) {                             // otwarto w trakcie dzwonienia
          rtcPendingSlot = -1;
          rtcAlarmRetries = 0;
          rtcLastOpenTs = (uint32_t)time(nullptr);
          zapiszDawke();
          oznaczAlarmObsluzony(slot);
          reportEvent("open", slot);
#if ONE_DOSE_PER_DAY
          if (rtcTimeValid && rtcTakenDay != localDayNumber(time(nullptr)))
            setTakenDay(localDayNumber(time(nullptr)));
#endif
          gestPoOtwarciu = czekajNaZamkniecieIGest(CZEKAJ_ZAMKNIECIE_MS);
        } else {
          rtcAlarmRetries++;
          if (rtcAlarmRetries >= MAX_ALARM_RETRIES) {
            rtcPendingSlot = -1;
            rtcAlarmRetries = 0;
            /* Odzwonione. Bez tego kazde kolejne wybudzenie w oknie
               +/-90 min zaczynaloby alarm od nowa (D23) - ale znacznik
               dotyczy TEGO przypomnienia, nie calej doby (D56).       */
            oznaczAlarmObsluzony(slot);

            /* "missed" dopiero, gdy dzis nie ma juz nastepnej szansy.
               Po pierwszym z dwoch przypomnien dzien nie jest jeszcze
               pominiety - a taki wpis malowalby go na czerwono o 20:00
               i aplikacja musialaby go odkrecac, gdyby tabletka poszla
               o 22:00. Doby, ktore skoncza sie bez dawki, i tak domyka
               checkDayRollover().                                     */
            /* --- Powiadomienie na telefon: po KAZDYM nieodebranym ------
               Tu rozchodzi sie ono ze zdarzeniem "missed", i to swiadomie.

               Wpis "missed" powstaje dopiero przy ostatnim przypomnieniu
               doby, bo wcześniejszy malowalby dzien na czerwono o 20:00,
               a tabletka moze pojsc o 22:00 (D64). To dotyczy DANYCH.

               Wiadomosc na telefon jest czyms innym: ma dotrzec wtedy, gdy
               jeszcze da sie cos z tym zrobic. O 23:00 na przypominanie
               jest po prostu pozno. Kuba wybral wprost "po kazdym
               nieodebranym", i to jest wlasciwy wybor - przypomnienie nie
               jest zapisem w kalendarzu i nie musi czekac na rozstrzygniecie
               doby.                                                      */
#if TG_ENABLED
            tgZglosNieodebrane(slot);
#endif

            if (slot == ostatniSlotDoby()) {
              LOGLN("[ALM] pominieta dawka");
              note("dawka pominieta");
              reportEvent("missed", slot);
            } else {
              LOGLN("[ALM] brak odzewu - czekamy na kolejne przypomnienie");
              note("bez odzewu, bedzie kolejne");
            }
          }
        }
      } else {
        /* Rutynowe wybudzenie: meldunek o baterii, sync czasu, config. */
        LOGLN("[SYS] housekeeping");
        if (wifiConnect() && firebaseSignIn()) { fetchConfig(); flushQueue(); pushStatus();
                                                 note("meldunek OK"); }
        else note("meldunek bez sieci");
      }
      break;
    }
  }

  /* --- Gest wykonany po otwarciu wieczka -----------------------------
     Obsluzony PO glownej galezi, zeby dawka zdazyla sie zapisac zanim
     zaczniemy cokolwiek testowac.                                     */
  if (gestPoOtwarciu == GEST_TEST) {
    autoTest();
  } else if (gestPoOtwarciu == GEST_PORTAL) {
    LOGLN("[GST] wchodze w tryb konfiguracji WiFi");
    note("portal WiFi");
    extendAwake((uint32_t)PORTAL_TIMEOUT_S * 1000UL + 30000UL);
    startWifiPortal();
  }
  gestPoOtwarciu = GEST_BRAK;

  /* --- Powiadomienie aplikacji o otwartym wieczku ----------------------
     Wysylamy tylko na zmianie stanu: gdy wlasnie zabrzmial pierwszy (lub
     kolejny) sygnal, albo gdy pudelko zostalo zamkniete, a aplikacja wciaz
     pokazuje alarm. Nie robimy tego przy zwyklym, krotkim otwarciu na lek -
     tam radio i tak sie wlacza, a alarm nie ma jeszcze racji bytu.       */
#if OPEN_REPORT_APP
  /* Trzeci warunek jest tu najwazniejszy.

     Przy zwyklym otwarciu na lek pudelko wysyla status, gdy wieczko jest
     JESZCZE OTWARTE - bo trzymasz je w reku. Do bazy szlo wiec boxOpen=true
     i na tym sie konczylo: zamkniecie wieczka nie bylo zadnym "ostrzezeniem"
     ani "odwolaniem alarmu", wiec nikt aplikacji nie prostowal. Baner
     "pudelko jest otwarte" wisial do nastepnej synchronizacji, czasem
     godzinami.

     Teraz porownujemy stan faktyczny z tym, co aplikacja ma zapisane.
     Rozjazd = jedno dodatkowe wyslanie statusu. Kosztuje ulamek mAh,
     bo plytka i tak jest w tym momencie wybudzona.                    */
  /* Czwarty warunek: jesli wieczko bylo w tym wybudzeniu otwarte, to po
     jego zamknieciu ZAWSZE wysylamy stan - nie polegajac na tym, ze
     porownanie z rtcOpenReported wypadnie akurat po naszej mysli. To jest
     dokladnie ten moment, w ktorym aplikacja ma sie dowiedziec prawdy,
     wiec nie ma sensu go warunkowac czymkolwiek.                       */
  /* Przy ladowaniu budzimy sie co minute, ale nie ma sensu wysylac
     w kolko tego samego. Meldunek idzie, gdy procent sie zmienil albo
     minelo CHARGE_PUSH_MAX_S - reszte wybudzen konczymy po cichu.    */
  bool meldunekLadowania = false;
  if (rtcCharging) {
    uint32_t teraz = rtcTimeValid ? (uint32_t)time(nullptr) : 0;
    meldunekLadowania = (rtcLastPushPct != batteryPercentage)
                     || rtcLastPushTs == 0 || teraz == 0
                     || (teraz - rtcLastPushTs) >= CHARGE_PUSH_MAX_S;
  }
  if (boxOpenWarned || rtcOpenClearPend || byloOtwarte || rtcStatusDirty
      || meldunekLadowania || rtcOpenReported != boxIsOpen()) {
    if (batterySaver) {
      LOGLN("[OPN] niskie napiecie - nie wlaczam radia dla samego statusu");
    } else {
      /* Radio zdazylo zasnac podczas czuwania - budzimy je i dajemy
         uczciwy zapas czasu na polaczenie oraz wyslanie.               */
      extendAwake(60000);
      LOG("[OPN] wysylam stan koncowy: pudelko %s\n",
          boxIsOpen() ? "OTWARTE" : "zamkniete");
      /* Zamkniecie wieczka: najpierw krotki PATCH, zeby aplikacja
         zareagowala natychmiast, a dopiero potem pelny status.      */
      /* USTAWIENIA CZYTAMY TAKZE TUTAJ - i to jest naprawa prawdziwej
         przyczyny tego, ze "kliknij i pudelko przyjmie" nie dzialalo.

         `fetchConfig()` wisialo WYLACZNIE na `reportEvent()`, czyli na
         sciezce zapisu dawki. A drugie otwarcie wieczka w ciagu doby
         (`juz dzis brane`) oraz odfiltrowane drgniecie styku wychodza
         z obslugi kontaktronu WCZESNIEJ - `reportEvent()` sie tam nie
         wykonuje. Pudelko wlaczalo wiec radio, meldowalo stan wieczka
         i status - czyli aktualizowalo `lastSeen` - a ustawien NIE
         czytalo. Zlecenia aktualizacji nie mialo prawa zobaczyc.

         Objaw byl dokladnie taki, jak opisal Kuba: otwiera wieczko raz
         za razem, pudelko nie pika, w historii ani jednego `ota:pobieram`,
         a aplikacja pisze "laczylo sie po zleceniu i nie zrobilo". Bo
         laczylo sie naprawde - tylko nie po to.

         Wieczorem, po wzieciu tabletki, KAZDE otwarcie idzie ta wlasnie
         sciezka. Czyli w praktyce: aktualizacja nie mogla ruszyc z
         otwarcia wieczka przez wiekszosc doby.

         Radio i tak tu stoi, wiec odczyt ustawien kosztuje jedno male
         zapytanie. Przy okazji naprawia to takze zwykla nieswiezosc:
         zmiana harmonogramu albo rozpisania z aplikacji dojezdzala do
         pudelka dopiero nastepnego dnia.                              */
      const bool bazaGotowa = wifiConnect() && firebaseSignIn();
      if (bazaGotowa) fetchConfig();
      if (bazaGotowa && pushLidState() && pushStatus()) {
        /* Ile naprawde uplynelo od zamkniecia wieczka do potwierdzenia
           z bazy. Bez tej liczby "za wolno" jest nie do zdiagnozowania:
           nie wiadomo, czy zwleka pudelko, czy telefon.               */
        if (msZamkniecia) {
          char m[40];
          snprintf(m, sizeof(m), "zamkn->wyslane %lu ms",
                   (unsigned long)(millis() - msZamkniecia));
          note(m);
          LOG("[LID] od zamkniecia do potwierdzenia: %lu ms\n",
              (unsigned long)(millis() - msZamkniecia));
        }
        rtcOpenClearPend = false;
        LOG("[OPN] aplikacja powiadomiona: pudelko %s\n",
            boxIsOpen() ? "OTWARTE" : "zamkniete");
      } else {
        /* Nie udalo sie TERAZ - ale to nie znaczy, ze sie nie uda.
           planNextSleep() widzi rtcStatusDirty i ustawi wczesniejsze
           wybudzenie zamiast czekac do nastepnej dawki.               */
        rtcStatusDirty = true;
        LOGLN("[OPN] nie doszlo - ponowie przy najblizszym wybudzeniu");
      }
    }
  }
#endif

  /* Ladowarka: zostajemy na lacza zamiast usypiac ---------------------
     Prad jest za darmo, wiec cykl "obudz sie - polacz - wyslij - spij"
     nie ma tu sensu. Zostajemy z jednym polaczeniem i meldujemy zmiany
     procentu na biezaco. Konczymy, gdy kabel zostanie wyciagniety.    */
  petlaLadowania();

  goToSleep(planNextSleep());
}

void loop() {
  /* nieosiagalne - urzadzenie zawsze konczy w deep sleep */
}
