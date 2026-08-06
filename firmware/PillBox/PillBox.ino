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
 *    Partition Scheme : Huge APP (3MB No OTA/1MB SPIFFS)
 *                       (domyslny podzial jest ZA MALY - program ma ~1,5 MB)
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
#include <time.h>

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
RTC_DATA_ATTR uint32_t rtcLastPushTs    = 0;    // kiedy ostatnio poszedl status
RTC_DATA_ATTR int16_t  rtcLastPushPct   = -1;   // z jakim procentem baterii
RTC_DATA_ATTR uint16_t rtcLogWyslanyIdx = 0xFFFF;// stan czarnej skrzynki przy ostatniej wysylce

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

/* =====================================================================
 *  STAN GLOBALNY
 * ===================================================================== */
Preferences prefs;

int    batteryPercentage    = 0;    // po wygladzeniu - to trafia do aplikacji
int    batteryRawPercentage = 0;    // prosto z krzywej, do diagnostyki
float  realBatteryVoltage   = 0.0f;

String slots[12];          // godziny "HH:MM"
int    slotCount = 0;

enum WakeReason { WAKE_BOOT, WAKE_REED, WAKE_BUTTON, WAKE_TIMER, WAKE_CLOSED };
WakeReason wakeReason = WAKE_BOOT;

/* Co uzytkownik pokazal przyciskiem po otwarciu wieczka. Typ musi byc
   zdefiniowany TUTAJ, przed zmienna gestPoOtwarciu ponizej - kompilator
   czyta plik z gory na dol i nie zna typu, ktory poznaje pozniej.     */
enum Gest { GEST_BRAK, GEST_TEST, GEST_PORTAL };

bool boxOpenWarned  = false;    // czy w tym wybudzeniu zabrzmial sygnal "otwarte"
bool byloOtwarte    = false;    // czy w tym wybudzeniu wieczko bylo otwarte
uint32_t msZamkniecia = 0;      // millis() w chwili wykrycia zamkniecia
bool portalRequested = false;   // przycisk trzymany przy resecie -> portal WiFi
Gest gestPoOtwarciu  = GEST_BRAK;  // co uzytkownik pokazal po otwarciu wieczka

bool batterySaver = false;      // true = za niskie napiecie, nie wlaczamy radia
bool timeSyncedThisWake = false;

bool syncTimeNTP();             // deklaracja - wifiConnect() wola ja od razu
String logbookJson();           // czarna skrzynka - definicje nizej
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
  prefs.putString("sched", s);
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
    prefs.putString(klucz, linia);
    prefs.putUShort("llCnt", cnt + 1);
    LOG("[LID] %s (wpis %u/%d)\n", otwarte ? "OTWARTE" : "zamkniete",
        cnt + 1, LIDLOG_SLOTS);
  } else {
    uint16_t zgubione = prefs.getUShort("llLost", 0);
    if (zgubione < 65000) prefs.putUShort("llLost", zgubione + 1);
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
    out += "\"";
    out += l;
    out += "\"";
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
  prefs.putUShort("llCnt", 0);
  prefs.putUShort("llLost", 0);
  prefs.end();
  LOG("[LID] dziennik wyslany i skasowany (%u wpisow)\n", cnt);
}

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
void queuePush(const String& rec) {
  prefs.begin(NVS_NAMESPACE, false);
  uint16_t head  = prefs.getUShort("qh", 0);
  uint16_t count = prefs.getUShort("qc", 0);
  char key[8];
  snprintf(key, sizeof(key), "q%u", (unsigned)((head + count) % QUEUE_CAPACITY));
  prefs.putString(key, rec);
  if (count < QUEUE_CAPACITY) {
    prefs.putUShort("qc", count + 1);
  } else {                                  // pelno -> nadpisz najstarszy
    prefs.putUShort("qh", (head + 1) % QUEUE_CAPACITY);
  }
  prefs.end();
  LOG("[QUE] zapisano offline: %s (w kolejce %u)\n", rec.c_str(), count + 1);
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
    prefs.putUShort("qh", (head + 1) % QUEUE_CAPACITY);
    prefs.putUShort("qc", count - 1);
  }
  prefs.end();
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
    prefs.putString(key, String((unsigned long)(ts + delta)) + rec.substring(p));
    fixed++;
  }
  prefs.end();
  if (fixed) LOG("[QUE] skorygowano czas %d zdarzen o %ld s\n", fixed, (long)delta);
}

/* =====================================================================
 *  6.  WiFi
 * ===================================================================== */
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
  WiFi.begin();                             // uzyj poswiadczen zapisanych w NVS

  /* Czekamy najwyzej WIFI_TIMEOUT_MS. Chwilowa awaria routera nie moze
     zatrzymac urzadzenia na dluzej - zdarzenie laduje w kolejce, a
     planNextSleep() ustawia wczesniejsze wybudzenie na ponowna probe.  */
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < WIFI_TIMEOUT_MS
         && !awakeTooLong()) {
    delay(200);
  }
  bool ok = WiFi.status() == WL_CONNECTED;
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
  rtcTimeValid = time(nullptr) > 1700000000;
  LOG("[NTP] %s (epoch=%lu)\n", rtcTimeValid ? "OK" : "FAIL", (unsigned long)time(nullptr));

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

  JsonDocument req;
  req["email"] = DEVICE_EMAIL;
  req["password"] = DEVICE_PASSWORD;
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
    prefs.putString("tok", idToken);
    prefs.end();
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

/* Wysyla pojedyncze zdarzenie. rec = "ts;type;batt;volt;slot" */
bool pushEventRecord(const String& rec) {
  int p1 = rec.indexOf(';');
  int p2 = rec.indexOf(';', p1 + 1);
  int p3 = rec.indexOf(';', p2 + 1);
  int p4 = rec.indexOf(';', p3 + 1);
  if (p4 < 0) return false;

  JsonDocument doc;
  doc["ts"]      = (uint32_t)rec.substring(0, p1).toInt();
  doc["type"]    = rec.substring(p1 + 1, p2);
  doc["battery"] = rec.substring(p2 + 1, p3).toInt();
  doc["volt"]    = rec.substring(p3 + 1, p4).toFloat();
  doc["slot"]    = rec.substring(p4 + 1).toInt();
  doc["fw"]      = FW_VERSION;

  String body;
  serializeJson(doc, body);
  int code = rtdbSend("POST", "/devices/" DEVICE_ID "/events.json", body);
  LOG("[FB ] push event HTTP %d : %s\n", code, body.c_str());
  return code == 200;
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

bool pushStatus() {
  JsonDocument doc;
  doc["battery"]  = batteryPercentage;
  doc["battRaw"]  = batteryRawPercentage;   // przed wygladzeniem - do diagnostyki
  doc["volt"]     = realBatteryVoltage;
  doc["lastSeen"] = (uint32_t)time(nullptr);
  doc["rssi"]     = WiFi.RSSI();
  doc["ssid"]     = WiFi.SSID();
  doc["fw"]       = FW_VERSION;
  doc["boots"]    = rtcBootCount;
  doc["queued"]   = queueCount();
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
}

/* Wysyla zaleglosci z kolejki - choćby z 10 dni bez internetu.
   Kazdy wpis kasujemy dopiero po potwierdzeniu HTTP 200, wiec zerwane
   polaczenie w polowie nie gubi danych. Przerwiemy tez, gdy czuwanie
   trwa juz za dlugo - reszta poleci przy nastepnym wybudzeniu.        */
bool flushQueue() {
  String rec;
  int guard = QUEUE_CAPACITY + 2;
  while (queuePeek(rec) && guard-- > 0) {
    if (awakeTooLong()) {
      LOGLN("[QUE] limit czasu czuwania - dokoncze przy nastepnym polaczeniu");
      return false;
    }
    if (!pushEventRecord(rec)) return false;
    queuePop();
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
  prefs.end();
  LOG("[DAY] odtworzono z pamieci: wzieta=%lu, rozliczona=%lu\n",
      (unsigned long)rtcTakenDay, (unsigned long)rtcRolloverDay);
}

/* Kasuje znaczniki doby - do testowania. Po tym pudelko znowu potraktuje
   najblizsze otwarcie jako pierwsza dzisiejsza dawke.                    */
void clearDayMarkers() {
  rtcTakenDay = 0;
  rtcRolloverDay = 0;
  prefs.begin(NVS_NAMESPACE, false);
  prefs.remove("takenDay");
  prefs.remove("rollDay");
  prefs.end();
  LOGLN("[DAY] znaczniki doby wyczyszczone");
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
void checkDayRollover() {
  if (!rtcTimeValid) return;
  uint32_t today = localDayNumber(time(nullptr));

  if (rtcRolloverDay == 0) { setRolloverDay(today); return; }    // pierwszy raz
  if (rtcRolloverDay == today) return;                           // ta sama doba

  if (rtcTakenDay != rtcRolloverDay) {
    /* 23:59 poprzedniej doby = teraz minus tyle, ile uplynelo dzis, minus minuta */
    time_t now = time(nullptr);
    uint32_t ts = (uint32_t)(now - (localMinutesOfDay(now) * 60) - 60);
    queuePush(makeRecordAt("missed", slotCount == 1 ? 0 : -1, ts));
    LOG("[DAY] doba %lu zamknieta bez dawki -> missed\n", (unsigned long)rtcRolloverDay);
  } else {
    LOG("[DAY] doba %lu zamknieta poprawnie\n", (unsigned long)rtcRolloverDay);
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
      if (pushEventRecord(rec)) {
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
  queuePush(rec);
  if (rtcRetryCount < 200) rtcRetryCount++;
  /* Otwarcie pudelka jest zawsze potwierdzane dzwiekiem - takze wtedy, gdy
     nie udalo sie go wyslac. Inaczej brak sieci wyglada dokladnie tak samo
     jak zepsute urzadzenie, a to dwie zupelnie rozne sytuacje.           */
  note(WiFi.status() == WL_CONNECTED ? "baza nie odpowiada" : "brak wifi");
  if (strcmp(type, "open") == 0) beepQueued();
  else                           beepErr();
  LOG("[EVT] '%s' zapisane w pamieci, w kolejce %u zdarzen\n", type, queueCount());
}

/* =====================================================================
 *  9.  ALARM
 * ===================================================================== */
/* Dzwoni przez ALARM_WINDOW_S i sprawdza czy pudelko zostalo otwarte.
   Zwraca true jesli otwarto.                                          */
bool runAlarmWindow() {
  /* Krytycznie niska bateria: zew ladowania poprzedza alarm o leku, zebys
     dowiedzial sie o tym takze wtedy, gdy akurat nie otwierasz pudelka. */
  if (batteryPercentage <= BATT_CRIT_PCT) { beepLowBattery(true); delay(400); }

  buzzerInit();
  uint32_t deadline = millis() + (uint32_t)ALARM_WINDOW_S * 1000UL;

  while (millis() < deadline) {
    for (int b = 0; b < BEEPS_PER_BURST; b++) {
      buzzerTone(BUZZER_FREQ_HZ);
      delay(BEEP_MS);
      buzzerTone(0);
      delay(120);
      if (boxIsOpen()) { buzzerOff(); return true; }
    }
    uint32_t gapEnd = millis() + BURST_GAP_MS;
    while (millis() < gapEnd) {
      if (boxIsOpen()) { buzzerOff(); return true; }
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
  o += F("</select><label>Haslo</label><input name='p' type='password' autocomplete='off'>"
         "<button type=submit>Polacz</button></form>");
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
  String pendingSsid, pendingPass;

  server.on("/", [&]() { server.send(200, "text/html; charset=utf-8", portalPage(found)); });

  server.on("/save", HTTP_POST, [&]() {
    pendingSsid = server.arg("s");
    pendingPass = server.arg("p");
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
  syncTimeNTP();
  timeSyncedThisWake = true;
  if (firebaseSignIn()) { fetchConfig(); flushQueue(); pushStatus(); }
  beepAck();
}

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
  prefs.begin(NVS_NAMESPACE, false);
  uint16_t idx = prefs.getUShort("lbIdx", 0);
  char klucz[8];
  snprintf(klucz, sizeof(klucz), "lb%u", idx % LOGBOOK_SLOTS);

  /* Format: czas|powod|co sie stalo|bateria|kolejka
     Krotko, bo NVS to nie miejsce na powiesci.                        */
  char linia[72];
  snprintf(linia, sizeof(linia), "%lu|%s|%s|%d|%u",
           (unsigned long)(rtcTimeValid ? time(nullptr) : 0),
           wakeName(wakeReason), co, batteryPercentage, queueCount());
  prefs.putString(klucz, linia);
  prefs.putUShort("lbIdx", (idx + 1) % (LOGBOOK_SLOTS * 8));
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
 * 11.  DEEP SLEEP
 * ===================================================================== */
void goToSleep(uint32_t seconds) {
  buzzerOff();
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
  if (wakeReason == WAKE_REED && rtcTimeValid && boxIsOpen()
      && rtcTakenDay == localDayNumber(time(nullptr))) {
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
      if (rtcTimeValid) {
        uint32_t today = localDayNumber(time(nullptr));
        if (rtcTakenDay == today) {
          LOG("[REED] dawka na dzien %lu juz zapisana -> OSTRZEZENIE\n",
              (unsigned long)today);
          note("juz dzis brane");
          /* Sygnal zwykle poszedl juz na samym poczatku setup(). Tutaj
             zostaje wylacznie dla przypadku, w ktorym tam sie nie udal
             - np. wieczko drgnelo i w tamtej chwili bylo zamkniete.  */
          if (!juzOstrzezono) beepAlreadyTaken();
          gestPoOtwarciu = czekajNaZamkniecieIGest(CZEKAJ_ZAMKNIECIE_MS);
          break;
        }
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
      if (rtcTimeValid) setTakenDay(localDayNumber(time(nullptr)));
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
      if (slot >= 0 && rtcTimeValid && rtcTakenDay == localDayNumber(time(nullptr))) {
        LOGLN("[ALM] dawka na dzis juz wzieta - alarm pominiety");
        rtcPendingSlot = -1;
        rtcAlarmRetries = 0;
        slot = -1;
      }
#endif

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
          if (rtcTimeValid) setTakenDay(localDayNumber(time(nullptr)));
          reportEvent("open", slot);
#if ONE_DOSE_PER_DAY
          if (rtcTimeValid && rtcTakenDay != localDayNumber(time(nullptr)))
            setTakenDay(localDayNumber(time(nullptr)));
#endif
          gestPoOtwarciu = czekajNaZamkniecieIGest(CZEKAJ_ZAMKNIECIE_MS);
        } else {
          rtcAlarmRetries++;
          if (rtcAlarmRetries >= MAX_ALARM_RETRIES) {
            LOGLN("[ALM] pominieta dawka");
            note("dawka pominieta");
            reportEvent("missed", slot);
            rtcPendingSlot = -1;
            rtcAlarmRetries = 0;
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
      if (wifiConnect() && firebaseSignIn() && pushLidState() && pushStatus()) {
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
