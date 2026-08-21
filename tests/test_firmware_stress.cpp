/* =====================================================================
 *  TESTY ODPORNOSCI FIRMWARE
 *
 *  Nie sprawdzaja poprawnego dnia pracy - od tego jest test_firmware.cpp.
 *  Tutaj celowo doprowadzamy pudelko do sytuacji granicznych: dziesiec dni
 *  bez sieci, przepelniona kolejka, zegar cofniety o godzine, rok pracy
 *  bez przerwy, tysiac wybudzen z zaklocanym pomiarem baterii.
 *
 *  Wszystko dziala na PRAWDZIWYCH funkcjach wycietych z PillBox.ino.
 *      python3 extract.py && g++ -O0 -std=c++17 test_firmware_stress.cpp -o s && ./s
 * ===================================================================== */
#include "arduino_shim.h"
#include "../firmware/PillBox/config.h"
#include <cstring>
#include <vector>
#include <set>

time_t FAKE_NOW = 0;
int    FAKE_ADC = 0;
unsigned long FAKE_MILLIS = 0;
FakeSerial Serial;

Preferences prefs;
String  slots[12];
String  idToken;          /* token Firebase - potrzebny przez tokenZPamieci() */
int     slotCount = 0;
int     batteryPercentage = 0;
int     batteryRawPercentage = 0;
float   realBatteryVoltage = 0.0f;
bool    rtcTimeValid = true;
/* Znaczniki powiadomien o zapasie i terminie INR (D83) - test odpornosci
   ich nie uzywa, ale logic.inc wciaga funkcje, ktore je czytaja.      */
int16_t rtcPillsLeft = -1;
bool    rtcTgStockCzeka = false;
bool    rtcTgStockZgloszony = false;
char    rtcInrDue[11] = "";
char    rtcTgInrDzien[11] = "";
uint8_t rtcTgInrMaska = 0;
int8_t  rtcTgInrIdx = -1;
bool    rtcTgInrCzeka = false;
int16_t rtcTzOffsetMin = 120;
uint32_t rtcTakenDay = 0;
uint32_t rtcRolloverDay = 0;
uint32_t awakeDeadlineMs = AWAKE_LIMIT_MS;
uint8_t  rtcBattPct = 255;
uint8_t  rtcBattUp  = 0;
bool     rtcCharging   = false;
float    rtcVoltMax    = 0.0f;
uint8_t  rtcChargeIdle = 0;
uint8_t  rtcWysokieZRzedu = 0;
bool     rtcBlokWysokie   = false;
uint32_t rtcChargeSinceTs = 0;
uint8_t  rtcChargeFromPct = 255;

uint32_t rtcLastOpenTs    = 0;
uint32_t rtcLastPushTs    = 0;
uint32_t rtcTokenExp      = 0;
uint32_t rtcTakenTs       = 0;
uint32_t rtcAlarmDoneDay  = 0;
uint16_t rtcAlarmDoneMask = 0;
uint32_t rtcAlarmDoneTs   = 0;
uint32_t rtcOpenSinceTs   = 0;
uint32_t rtcNextWarnTs    = 0;
uint16_t rtcOpenWarnCount = 0;
bool     rtcOpenReported  = false;
bool     rtcOpenClearPend = false;
bool     rtcStatusDirty   = false;
int8_t   rtcPendingSlot   = -1;
uint8_t  rtcAlarmRetries  = 0;
uint8_t  rtcRetryCount    = 0;
enum WakeReason { WAKE_BOOT, WAKE_REED, WAKE_BUTTON, WAKE_TIMER, WAKE_CLOSED };
WakeReason wakeReason = WAKE_TIMER;
bool FAKE_BOX_OPEN = false;
int  FAKE_BEEPS    = 0;
bool boxIsOpen()   { return FAKE_BOX_OPEN; }
void beepBoxOpen() { FAKE_BEEPS++; }

uint16_t rtcNvsFail      = 0;
char     rtcNvsFailKey[10] = "";  // ktory klucz nie zapisal sie ostatnio (D46)
#define NVS_FAILLOG_SLOTS 8
uint32_t rtcNvsFailLogTs[NVS_FAILLOG_SLOTS]     = {0};
char     rtcNvsFailLogKey[NVS_FAILLOG_SLOTS][10] = {{0}};
uint16_t rtcNvsFailLogTotal = 0;
uint16_t rtcNvsFailLogSent  = 0;
uint16_t rtcQueueDropped = 0;

/* Rozpisanie dawek. Domyslnie "nic nie wiem" - w tym stanie pudelko ma
   dzwonic normalnie i zglaszac pominiete dawki.                        */
uint8_t  rtcDoseWeek[7]   = { DOSE_NIEZNANA, DOSE_NIEZNANA, DOSE_NIEZNANA,
                              DOSE_NIEZNANA, DOSE_NIEZNANA, DOSE_NIEZNANA,
                              DOSE_NIEZNANA };
uint32_t rtcDoseExDay[DOSE_EX_MAX] = { 0 };
uint8_t  rtcDoseExVal[DOSE_EX_MAX] = { 0 };
uint8_t  rtcDoseExCount   = 0;
bool     rtcDosingLoaded  = false;
uint8_t  rtcNetOstatnia   = 0;   // ktora siec z listy zadzialala ostatnio

/* flushQueue() jest teraz wyciagane do testow, wiec potrzebuje atrapy
   wysylki. Tu nikt jej nie wola - wystarczy, ze istnieje.            */
int pushEventRecord(const String&) { return 200; }

#include "logic.inc"

int PASS = 0, FAIL = 0;
#define CHECK(cond, ...) do{ if(cond){PASS++;} else {FAIL++; \
  printf("  FAIL  %s:%d  ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } }while(0)
void head(const char* t){ printf("\n=== %s ===\n", t); }

time_t utc(int y,int mo,int d,int h,int mi,int s=0){
  struct tm t{}; t.tm_year=y-1900; t.tm_mon=mo-1; t.tm_mday=d;
  t.tm_hour=h; t.tm_min=mi; t.tm_sec=s;
  return timegm(&t);
}
time_t local(int y,int mo,int d,int h,int mi,int s=0){
  return utc(y,mo,d,h,mi,s) - (time_t)rtcTzOffsetMin*60;
}

int main(){

/* ================= 1. KOLEJKA PONAD POJEMNOSC ================= */
head("Kolejka offline: wiecej zdarzen niz miejsca");
prefs.wipe();
rtcTimeValid = true;
parseSchedule(String("20:00"));

/* Dziesiec dni bez sieci przy jednej dawce dziennie to 10 wpisow - miesci
   sie spokojnie. Ale awaria routera plus pudelko budzone kilkanascie razy
   dziennie potrafi wypelnic kolejke. Sprawdzamy, co sie wtedy dzieje.   */
for (int i = 0; i < QUEUE_CAPACITY + 80; i++) {
  FAKE_NOW = local(2026, 8, 1, 12, 0) + i * 60;
  batteryPercentage = 50 + (i % 40);
  realBatteryVoltage = 3.8f;
  queuePush(makeRecord("open", 0));
}
uint16_t ile = queueCount();
CHECK(ile <= QUEUE_CAPACITY, "kolejka nie przekracza pojemnosci (%u z %d)",
      ile, QUEUE_CAPACITY);
CHECK(ile == QUEUE_CAPACITY, "i jest wypelniona do konca (%u)", ile);

/* Najstarsze wypadaja, najnowsze zostaja - czyli tracimy historie,
   a nie ostatnia dawke. To wazniejsze niz sie wydaje.                  */
String pierwszy;
CHECK(queuePeek(pierwszy), "da sie odczytac najstarszy wpis");
unsigned long tsPierwszy = (unsigned long)pierwszy.substring(0, pierwszy.indexOf(';')).toInt();
unsigned long tsNajstarszyMozliwy = (unsigned long)(local(2026, 8, 1, 12, 0) + 80 * 60);
CHECK(tsPierwszy >= tsNajstarszyMozliwy,
      "po przepelnieniu zostaly NAJNOWSZE wpisy (%lu >= %lu)",
      tsPierwszy, tsNajstarszyMozliwy);

/* Oproznianie do zera nie moze sie zapetlic ani zostawic smieci. */
int zdjete = 0;
String tmp;
while (queuePeek(tmp) && zdjete < QUEUE_CAPACITY * 3) { queuePop(); zdjete++; }
CHECK(zdjete == QUEUE_CAPACITY, "zdjeto dokladnie tyle, ile bylo (%d)", zdjete);
CHECK(queueCount() == 0, "kolejka pusta (%u)", queueCount());
CHECK(!queuePeek(tmp), "pusta kolejka nic nie zwraca");
queuePop();                                   // pop na pustej nie moze zaszkodzic
CHECK(queueCount() == 0, "pop na pustej kolejce jest nieszkodliwy");

/* ================= 2. ZEGAR SKACZE ================= */
head("Korekta czasu po dlugim braku sieci");
prefs.wipe();
FAKE_NOW = local(2026, 8, 1, 20, 0);
for (int i = 0; i < 5; i++) { FAKE_NOW += 3600; queuePush(makeRecord("open", 0)); }

String przed;
queuePeek(przed);
long tsPrzed = przed.substring(0, przed.indexOf(';')).toInt();

/* Zegar byl spozniony o 2 godziny - NTP przesuwa go do przodu. */
queueShiftTimestamps(7200);
String po;
queuePeek(po);
long tsPo = po.substring(0, po.indexOf(';')).toInt();
CHECK(tsPo - tsPrzed == 7200, "znaczniki przesuniete do przodu o 7200 s (%ld)", tsPo - tsPrzed);

/* I w druga strone - zegar spieszyl sie o godzine. */
queueShiftTimestamps(-3600);
queuePeek(po);
long tsPo2 = po.substring(0, po.indexOf(';')).toInt();
CHECK(tsPo2 - tsPrzed == 3600, "i cofniete o 3600 s (%ld)", tsPo2 - tsPrzed);

/* Absurdalne cofniecie nie moze dac ujemnego znacznika. */
queueShiftTimestamps(-4000000000LL > 0 ? 0 : -2000000000);
queuePeek(po);
long tsPo3 = po.substring(0, po.indexOf(';')).toInt();
CHECK(tsPo3 >= 0, "znacznik nie schodzi ponizej zera (%ld)", tsPo3);
CHECK(queueCount() == 5, "korekta nie gubi wpisow (%u)", queueCount());

/* ================= 3. ROK PRACY BEZ PRZERWY ================= */
head("Rok rolowania doby bez ani jednego duplikatu");
prefs.wipe();
rtcRolloverDay = 0; rtcTakenDay = 0;
rtcTimeValid = true;
parseSchedule(String("20:00"));

int dniWziete = 0, dniPominiete = 0;
for (int d = 0; d < 365; d++) {
  /* Wieczorem bierzemy lek w 4 dni na 5. */
  bool wzial = (d % 5 != 0);
  if (wzial) {
    FAKE_NOW = local(2026, 1, 1, 20, 0) + (time_t)d * 86400;
    setTakenDay(localDayNumber(FAKE_NOW));
    dniWziete++;
  } else {
    dniPominiete++;
  }
  /* Nad ranem nastepnego dnia nastepuje rozliczenie doby. */
  FAKE_NOW = local(2026, 1, 1, DAY_START_HOUR, 1) + (time_t)(d + 1) * 86400;
  checkDayRollover();
  checkDayRollover();          // drugie wywolanie nie moze nic dodac
  checkDayRollover();
}
uint16_t wKolejce = queueCount();
CHECK(wKolejce <= QUEUE_CAPACITY, "kolejka nie pekla (%u)", wKolejce);
CHECK(wKolejce > 0, "pominiete dawki trafily do kolejki (%u)", wKolejce);

/* Kazde zdarzenie 'missed' musi dotyczyc INNEJ doby - potrojne wywolanie
   rozliczenia nie ma prawa zdublowac ani jednego dnia.                  */
std::set<unsigned long> doby;
int wpisow = 0;
String rec;
while (queuePeek(rec)) {
  unsigned long ts = (unsigned long)rec.substring(0, rec.indexOf(';')).toInt();
  doby.insert((unsigned long)localDayNumber((time_t)ts));
  wpisow++;
  queuePop();
  if (wpisow > QUEUE_CAPACITY + 10) break;
}
CHECK((int)doby.size() == wpisow, "zero duplikatow: %d wpisow, %zu roznych dob",
      wpisow, doby.size());

/* ================= 4. TYSIAC POMIAROW BATERII Z SZUMEM ================= */
head("Tysiac wybudzen z zaklocanym pomiarem");
resetBatteryFilter();
{
  int poprzedni = -1, wzrostow = 0, przekroczen = 0, skokowWDol = 0;
  int zmian = 0, maxOdchylenie = 0;
  /* Ogniwo powoli sie rozladowuje, ale kazdy odczyt ma losowy blad -
     dokladnie tak, jak w rzeczywistosci.                               */
  unsigned seed = 12345;
  for (int i = 0; i < 1000; i++) {
    seed = seed * 1103515245u + 12345u;
    int szum = (int)((seed >> 16) % 9) - 4;              // +/- 4 punkty
    int prawda = 100 - i / 12;                            // spadek z czasem
    if (prawda < 0) prawda = 0;
    int surowy = prawda + szum;
    if (surowy < 0) surowy = 0;
    if (surowy > 100) surowy = 100;

    int pokazany = battSmooth(surowy);
    if (pokazany < 0 || pokazany > 100) przekroczen++;
    int odchyl = pokazany > prawda ? pokazany - prawda : prawda - pokazany;
    if (odchyl > maxOdchylenie) maxOdchylenie = odchyl;
    if (poprzedni >= 0) {
      if (pokazany != poprzedni) zmian++;
      if (pokazany > poprzedni) wzrostow++;
      if (poprzedni - pokazany > BATT_STEP_DOWN) skokowWDol++;
    }
    poprzedni = pokazany;
  }
  CHECK(przekroczen == 0, "wskazanie nigdy nie wyszlo poza 0-100 (%d)", przekroczen);
  CHECK(skokowWDol == 0, "zaden skok w dol nie przekroczyl limitu (%d)", skokowWDol);
  CHECK(poprzedni <= 25, "po tysiacu pomiarow wskazanie doszlo w dol (%d%%)", poprzedni);

  /* To sa liczby, ktore widzi uzytkownik. Stabilnosc: przy ilu wybudzeniach
     procent w ogole drgnal. Dokladnosc: jak daleko potrafil odejsc od prawdy.
     Jedno bez drugiego nic nie znaczy - filtr, ktory zawsze pokazuje 50%,
     jest idealnie stabilny i zupelnie bezuzyteczny.                       */
  CHECK(zmian < 200, "wskazanie stoi w miejscu przy wiekszosci wybudzen "
        "(zmian: %d na 1000)", zmian);
  CHECK(maxOdchylenie <= 8, "i mimo to nie odbiega od prawdy (max %d pkt)",
        maxOdchylenie);
  CHECK(wzrostow < zmian, "spadkow jest wiecej niz wzrostow (%d wzrostow z %d zmian)",
        wzrostow, zmian);
}

/* Ladowanie od zera do pelna: filtr musi puscic wskazanie w gore od razu
   po wyzerowaniu, a nie pelznac po jednym punkcie.                      */
resetBatteryFilter();
battSmooth(4);
resetBatteryFilter();
CHECK(battSmooth(100) == 100, "po wykryciu ladowarki wskazanie skacze od razu");

/* ================= 5. WIECZKO OTWARTE PRZEZ DOBE ================= */
head("Wieczko zostawione otwarte na cala dobe");
{
  rtcTimeValid = true;
  FAKE_NOW = local(2026, 8, 1, 22, 0);
  rtcOpenSinceTs = 0; rtcNextWarnTs = 0; rtcOpenWarnCount = 0;
  rtcOpenReported = false; rtcOpenClearPend = false;
  FAKE_BOX_OPEN = true; FAKE_BEEPS = 0;
  wakeReason = WAKE_TIMER;

  trackBoxOpen();                              // wykrycie otwarcia
  FAKE_NOW += OPEN_WARN_FIRST_S;
  trackBoxOpen();                              // pierwszy sygnal

  /* Dalej przez cala dobe, co pol godziny. */
  int sygnalow = FAKE_BEEPS;
  for (int i = 0; i < 48; i++) {
    FAKE_NOW += OPEN_WARN_REPEAT_S;
    trackBoxOpen();
  }
  CHECK(FAKE_BEEPS <= OPEN_WARN_MAX,
        "po dobie liczba sygnalow nie przekroczyla limitu (%d z %d)",
        FAKE_BEEPS, OPEN_WARN_MAX);
  CHECK(FAKE_BEEPS >= sygnalow, "sygnaly nie ubywaja (%d)", FAKE_BEEPS);
  CHECK(rtcOpenSinceTs != 0, "stan alarmowy trwa, dopoki wieczko otwarte");

  /* Zamkniecie konczy wszystko i nie zostawia smieci. */
  FAKE_BOX_OPEN = false;
  trackBoxOpen();
  CHECK(rtcOpenSinceTs == 0 && rtcOpenWarnCount == 0 && rtcNextWarnTs == 0,
        "zamkniecie czysci caly stan");

  /* I mozna zaczac od nowa. */
  FAKE_BOX_OPEN = true; FAKE_BEEPS = 0;
  trackBoxOpen();
  FAKE_NOW += OPEN_WARN_FIRST_S;
  CHECK(trackBoxOpen(), "kolejne otwarcie znowu ostrzega");
}

/* ================= 6. SMIECIOWY HARMONOGRAM ================= */
head("Harmonogram z bledami");
{
  struct { const char* wej; int oczek; } proby[] = {
    { "",                 0 },
    { "|||",              0 },
    { "aa:bb",            1 },   // dlugosc i dwukropek sie zgadzaja
    { "25:99",            1 },   // format ok, wartosc absurdalna
    { "8:00",             0 },   // za krotkie
    { "08:00",            1 },
    { "08:00|20:00",      2 },
    { "08:00|08:00",      2 },   // duplikaty odsiewa aplikacja
    { "08:00|zle|20:00",  2 },
    { "  08:00  ",        1 },   // spacje obcinane
  };
  for (auto& p : proby) {
    parseSchedule(String(p.wej));
    CHECK(slotCount == p.oczek, "'%s' -> %d slotow (oczekiwano %d)",
          p.wej, slotCount, p.oczek);
  }

  /* Wiecej pozycji niz miejsca w tablicy - nie moze byc wyjscia poza zakres. */
  String duzo;
  for (int i = 0; i < 40; i++) duzo += String(i < 10 ? "0" : "") + String(i % 24) + ":00|";
  parseSchedule(duzo);
  CHECK(slotCount <= 12, "nie wiecej niz 12 godzin (%d)", slotCount);

  /* Godziny absurdalne nie moga dac ujemnych minut. */
  parseSchedule(String("25:99"));
  if (slotCount > 0) CHECK(slotMinutes(0) >= 0, "minuty nieujemne (%d)", slotMinutes(0));
}

/* ================= 7. PRACA BEZ ZEGARA ================= */
head("Pudelko bez wiarygodnego zegara");
{
  prefs.wipe();
  rtcTimeValid = false;
  FAKE_NOW = 0;

  /* Zdarzenie powstale przed synchronizacja ma znacznik 0 - i tak MUSI
     trafic do kolejki, zeby dawka nie przepadla.                       */
  batteryPercentage = 70; realBatteryVoltage = 3.9f;
  queuePush(makeRecord("open", 0));
  CHECK(queueCount() == 1, "dawka bez zegara tez trafia do kolejki");

  String r;
  queuePeek(r);
  CHECK(r.startsWith("0;"), "znacznik czasu to zero: %s", r.substring(0, 12).c_str());

  /* Rolowanie doby bez zegara nie ma prawa niczego zglaszac. */
  uint16_t przedRol = queueCount();
  checkDayRollover();
  CHECK(queueCount() == przedRol, "bez zegara nie zglaszamy pominietych dawek");

  /* Sen tez musi byc sensowny, a nie zerowy albo nieskonczony. */
  uint32_t s = secondsToDayBoundary(0);
  CHECK(s > 0 && s <= HOUSEKEEP_MAX_S, "sen bez zegara w rozsadnych granicach (%lu s)",
        (unsigned long)s);
  rtcTimeValid = true;
}

/* ================= 8. SPOJNOSC USTAWIEN ================= */
head("Ustawienia nie moga sie wykluczac");
CHECK(QUEUE_CAPACITY >= 30,
      "kolejka mieści co najmniej miesiac dawek (%d)", QUEUE_CAPACITY);
CHECK((uint32_t)MAX_ALARM_RETRIES * SNOOZE_S < 86400,
      "wszystkie proby alarmu mieszcza sie w dobie");
CHECK(MATCH_WINDOW_MIN * 2 < 1440,
      "okno dopasowania nie obejmuje calej doby");
CHECK(OPEN_WARN_FIRST_S < OPEN_WARN_REPEAT_S * 4,
      "pierwsze ostrzezenie nie jest absurdalnie pozniej niz kolejne");
CHECK(BATT_STEP_DOWN * 4 <= 100,
      "przejscie calej skali wymaga co najmniej kilku wybudzen");
CHECK(BATT_DEADBAND > 0 && BATT_DEADBAND < 10,
      "martwa strefa tlumi szum, ale nie chowa prawdziwych zmian");
CHECK(BATT_RISE_STREAK >= 2,
      "pojedynczy wyzszy odczyt nie podnosi wskazania");
CHECK(DAY_START_HOUR * 60 + 60 < MATCH_WINDOW_MIN + 1440,
      "granica doby nie koliduje z oknem dopasowania dawki");

printf("\n──────────────────────────────────────\n");
printf("  ZALICZONE: %d    BLEDY: %d\n", PASS, FAIL);
printf("──────────────────────────────────────\n");
return FAIL ? 1 : 0;
}
