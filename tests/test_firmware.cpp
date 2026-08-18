/* =====================================================================
 *  Testy logiki firmware. Kompiluja PRAWDZIWE funkcje wyciete
 *  z PillBox.ino (logic.inc) na atrapach Arduino.
 *      python3 extract.py && g++ -O0 -std=c++17 test_firmware.cpp -o t && ./t
 * ===================================================================== */
#include "arduino_shim.h"
#include <cstring>
#include "../firmware/PillBox/config.h"

#include <vector>
#include <set>
#include <string>
#include <utility>

time_t FAKE_NOW = 0;
int    FAKE_ADC = 0;
unsigned long FAKE_MILLIS = 0;
FakeSerial Serial;

/* globalne, ktorych uzywa wycieta logika */
Preferences prefs;
String  slots[12];
String  idToken;          /* token Firebase - potrzebny przez tokenZPamieci() */
int     slotCount = 0;
int     batteryPercentage = 0;
int     batteryRawPercentage = 0;
float   realBatteryVoltage = 0.0f;
uint8_t rtcBattPct = 255;
uint8_t rtcBattUp  = 0;
bool     rtcCharging   = false;
float    rtcVoltMax    = 0.0f;
uint8_t  rtcChargeIdle = 0;
uint8_t  rtcWysokieZRzedu = 0;
bool     rtcBlokWysokie   = false;
uint32_t rtcChargeSinceTs = 0;
uint8_t  rtcChargeFromPct = 255;
bool    rtcTimeValid = true;
int16_t rtcTzOffsetMin = 120;
uint32_t rtcTakenDay = 0;
uint32_t rtcRolloverDay = 0;
uint32_t awakeDeadlineMs = AWAKE_LIMIT_MS;
uint16_t rtcNvsFail      = 0;
char     rtcNvsFailKey[10] = "";  // ktory klucz nie zapisal sie ostatnio (D46)
#define NVS_FAILLOG_SLOTS 8
uint32_t rtcNvsFailLogTs[NVS_FAILLOG_SLOTS]     = {0};
char     rtcNvsFailLogKey[NVS_FAILLOG_SLOTS][10] = {{0}};
uint16_t rtcNvsFailLogTotal = 0;
uint16_t rtcNvsFailLogSent  = 0;
uint16_t rtcQueueDropped = 0;

/* --- atrapy dla dni bez leku --- */
uint8_t  rtcDoseWeek[7]   = { DOSE_NIEZNANA, DOSE_NIEZNANA, DOSE_NIEZNANA,
                              DOSE_NIEZNANA, DOSE_NIEZNANA, DOSE_NIEZNANA,
                              DOSE_NIEZNANA };
uint32_t rtcDoseExDay[DOSE_EX_MAX] = { 0 };
uint8_t  rtcDoseExVal[DOSE_EX_MAX] = { 0 };
uint8_t  rtcDoseExCount   = 0;
bool     rtcDosingLoaded  = false;
uint8_t  rtcNetOstatnia   = 0;   // ktora siec z listy zadzialala ostatnio
/* Zdejmuje rozpisanie - stan "nic nie wiem", w ktorym pudelko ma dzwonic. */
void resetDosing(){
  for (int i = 0; i < 7; i++) rtcDoseWeek[i] = DOSE_NIEZNANA;
  rtcDoseExCount = 0;
}

/* --- atrapy dla pilnowania otwartego wieczka --- */
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

/* --- atrapy dla planowania snu --- */
int8_t   rtcPendingSlot  = -1;
uint8_t  rtcAlarmRetries = 0;
uint8_t  rtcRetryCount   = 0;
enum WakeReason { WAKE_BOOT, WAKE_REED, WAKE_BUTTON, WAKE_TIMER, WAKE_CLOSED };
WakeReason wakeReason = WAKE_TIMER;

bool FAKE_BOX_OPEN = false;
int  FAKE_BEEPS    = 0;
bool boxIsOpen()   { return FAKE_BOX_OPEN; }
void beepBoxOpen() { FAKE_BEEPS++; }

/* Ustawia stan wyjsciowy dla testow otwartego pudelka. */
void resetOpenState(bool open){
  rtcOpenSinceTs = 0; rtcNextWarnTs = 0; rtcOpenWarnCount = 0;
  rtcOpenReported = false; rtcOpenClearPend = false;
  FAKE_BOX_OPEN = open; FAKE_BEEPS = 0; wakeReason = WAKE_TIMER;
}

/* --- atrapa wysylki, zeby dalo sie testowac flushQueue() bez sieci ---
   FAKE_HTTP steruje odpowiedzia serwera, FAKE_SENT zbiera to, co poszlo. */
int FAKE_HTTP = 200;
std::vector<std::string> FAKE_SENT;
int pushEventRecord(const String& rec) {
  FAKE_SENT.push_back(rec.s);
  return FAKE_HTTP;
}

#include "logic.inc"

/* ---------- mikro-framework ---------- */
int PASS = 0, FAIL = 0;
#define CHECK(cond, ...) do{ if(cond){PASS++;} else {FAIL++; \
  printf("  FAIL  %s:%d  ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } }while(0)
void head(const char* t){ printf("\n=== %s ===\n", t); }

/* UTC epoch dla podanej daty/godziny */
time_t utc(int y,int mo,int d,int h,int mi,int s=0){
  struct tm t{}; t.tm_year=y-1900; t.tm_mon=mo-1; t.tm_mday=d;
  t.tm_hour=h; t.tm_min=mi; t.tm_sec=s;
  return timegm(&t);
}
/* czas lokalny (PL lato = UTC+2) -> epoch UTC */
time_t local(int y,int mo,int d,int h,int mi,int s=0){
  return utc(y,mo,d,h,mi,s) - (time_t)rtcTzOffsetMin*60;
}

int main(){

/* ================= 1. HARMONOGRAM ================= */
head("Parsowanie harmonogramu");
parseSchedule(String("08:00"));
CHECK(slotCount==1, "jedna godzina -> slotCount=%d", slotCount);
CHECK(slots[0]==String("08:00"), "slots[0]=%s", slots[0].c_str());
CHECK(slotMinutes(0)==480, "08:00 = %d min", slotMinutes(0));

parseSchedule(String("08:00|20:30"));
CHECK(slotCount==2, "dwie godziny -> %d", slotCount);
CHECK(slotMinutes(1)==1230, "20:30 = %d min", slotMinutes(1));

parseSchedule(String("8:0|zle|20:00|"));
CHECK(slotCount==1 && slots[0]==String("20:00"), "smieci odrzucone, zostalo %d", slotCount);

parseSchedule(String(""));
CHECK(slotCount==0, "pusty harmonogram -> %d", slotCount);

/* ================= 2. CZAS LOKALNY ================= */
head("Konwersja czasu (strefa +120 min)");
rtcTzOffsetMin = 120;
FAKE_NOW = local(2026,7,31,8,15);
CHECK(localMinutesOfDay(FAKE_NOW)==495, "08:15 lokalnie = %d min", localMinutesOfDay(FAKE_NOW));
CHECK(localDayNumber(FAKE_NOW)==20260731u, "dzien = %u", localDayNumber(FAKE_NOW));

/* 00:30 lokalnie to jeszcze doba POPRZEDNIA - patrz DAY_START_HOUR.
   localMinutesOfDay pokazuje przy tym prawdziwa godzine zegarowa,
   bo alarmy maja dzwonic o 20:00 niezaleznie od granicy doby.        */
FAKE_NOW = local(2026,8,1,0,30);
CHECK(localDayNumber(FAKE_NOW)==20260731u,
      "po polnocy trwa jeszcze doba z 31 lipca = %u", localDayNumber(FAKE_NOW));
CHECK(localMinutesOfDay(FAKE_NOW)==30, "00:30 = %d min", localMinutesOfDay(FAKE_NOW));

/* 23:30 lokalnie = 21:30 UTC tego samego dnia */
FAKE_NOW = local(2026,7,31,23,30);
CHECK(localDayNumber(FAKE_NOW)==20260731u, "23:30 lokalnie dzien = %u", localDayNumber(FAKE_NOW));

/* ================= 3. DOPASOWANIE DAWKI ================= */
head("Dopasowanie otwarcia do dawki (okno +/-90 min)");
parseSchedule(String("08:00"));
rtcTimeValid = true;

FAKE_NOW = local(2026,7,31,8,5);
CHECK(matchSlot(FAKE_NOW)==0, "08:05 -> slot %d", matchSlot(FAKE_NOW));
FAKE_NOW = local(2026,7,31,6,31);
CHECK(matchSlot(FAKE_NOW)==0, "06:31 (na granicy) -> slot %d", matchSlot(FAKE_NOW));
FAKE_NOW = local(2026,7,31,9,29);
CHECK(matchSlot(FAKE_NOW)==0, "09:29 -> slot %d", matchSlot(FAKE_NOW));
FAKE_NOW = local(2026,7,31,14,0);
CHECK(matchSlot(FAKE_NOW)==-1, "14:00 poza oknem -> slot %d", matchSlot(FAKE_NOW));

/* przejscie przez polnoc: dawka o 23:30, otwarcie o 00:20 */
parseSchedule(String("23:30"));
FAKE_NOW = local(2026,8,1,0,20);
CHECK(matchSlot(FAKE_NOW)==0, "dawka 23:30, otwarcie 00:20 -> slot %d", matchSlot(FAKE_NOW));

rtcTimeValid = false;
CHECK(matchSlot(FAKE_NOW)==-1, "bez zegara nie zgadujemy slotu");
rtcTimeValid = true;

/* ================= 4. PLANOWANIE SNU ================= */
head("Czas do nastepnej dawki");
parseSchedule(String("08:00"));
FAKE_NOW = local(2026,7,31,8,0,0);
CHECK(secondsToNextSlot(FAKE_NOW)==86400u, "dokladnie o 08:00 -> %u s (doba)", secondsToNextSlot(FAKE_NOW));

FAKE_NOW = local(2026,7,31,7,0,0);
CHECK(secondsToNextSlot(FAKE_NOW)==3600u, "07:00 -> %u s", secondsToNextSlot(FAKE_NOW));

FAKE_NOW = local(2026,7,31,22,0,0);
CHECK(secondsToNextSlot(FAKE_NOW)==36000u, "22:00 -> %u s (10 h)", secondsToNextSlot(FAKE_NOW));

FAKE_NOW = local(2026,7,31,7,59,30);
CHECK(secondsToNextSlot(FAKE_NOW)==30u, "07:59:30 -> %u s", secondsToNextSlot(FAKE_NOW));

parseSchedule(String("08:00|20:00"));
FAKE_NOW = local(2026,7,31,12,0,0);
CHECK(secondsToNextSlot(FAKE_NOW)==28800u, "dwie dawki, 12:00 -> %u s (8 h)", secondsToNextSlot(FAKE_NOW));

head("Czas do konca doby lekowej");
/* Doba konczy sie o DAY_START_HOUR, nie o polnocy. */
FAKE_NOW = local(2026,7,31,2,0,0);
CHECK(secondsToDayBoundary(FAKE_NOW)==3660u,
      "02:00 -> %u s (godzina do granicy + minuta)", secondsToDayBoundary(FAKE_NOW));
FAKE_NOW = local(2026,7,31,3,0,0);
CHECK(secondsToDayBoundary(FAKE_NOW)==86460u,
      "dokladnie o granicy -> %u s (pelna doba)", secondsToDayBoundary(FAKE_NOW));
FAKE_NOW = local(2026,7,31,23,0,0);
CHECK(secondsToDayBoundary(FAKE_NOW)==14460u,
      "23:00 -> %u s (4 h do granicy + minuta)", secondsToDayBoundary(FAKE_NOW));

/* ================= 5. KOLEJKA OFFLINE ================= */
head("Kolejka offline (pierscien w NVS)");
prefs.wipe();
batteryPercentage = 80; realBatteryVoltage = 4.02f;
FAKE_NOW = local(2026,7,20,8,0);

CHECK(queueCount()==0, "start pusty");
for (int i = 0; i < 10; i++) queuePush(makeRecordAt("open", 0, 1750000000u + i));
CHECK(queueCount()==10, "po 10 wpisach: %u", queueCount());

String r;
CHECK(queuePeek(r) && r.startsWith(String("1750000000;open")), "najstarszy pierwszy: %s", r.c_str());
queuePop();
CHECK(queueCount()==9, "po zdjeciu: %u", queueCount());
CHECK(queuePeek(r) && r.startsWith(String("1750000001;")), "kolejnosc FIFO: %s", r.c_str());

/* przepelnienie - nadpisuje najstarsze, nie rosnie w nieskonczonosc */
prefs.wipe();
for (int i = 0; i < QUEUE_CAPACITY + 25; i++) queuePush(makeRecordAt("open", 0, 1750000000u + i));
CHECK(queueCount()==QUEUE_CAPACITY, "przepelnienie -> %u (limit %d)", queueCount(), QUEUE_CAPACITY);
queuePeek(r);
unsigned long oldest = (unsigned long)r.substring(0, r.indexOf(';')).toInt();
CHECK(oldest==1750000000UL+25, "najstarszy po nadpisaniu: %lu (oczekiwano %lu)", oldest, 1750000000UL+25);

/* opróznienie do zera */
int guard = 0;
while (queueCount() > 0 && guard++ < 500) queuePop();
CHECK(queueCount()==0, "kolejka opróżniona: %u", queueCount());

/* ================= 5b. ODRZUCONY WPIS NIE ZATYKA KOLEJKI (B3) ======= */
head("Odrzucony wpis nie zatyka kolejki");
prefs.wipe();
awakeDeadlineMs = AWAKE_LIMIT_MS; FAKE_MILLIS = 0;
rtcQueueDropped = 0; FAKE_SENT.clear(); FAKE_HTTP = 200;

CHECK(trwaleOdrzucony(400), "HTTP 400 - reguly odrzucily, nigdy nie przejdzie");
CHECK(trwaleOdrzucony(413), "HTTP 413 - pakiet za duzy, nigdy nie przejdzie");
CHECK(!trwaleOdrzucony(401), "HTTP 401 - wygasly token, ponawiamy");
CHECK(!trwaleOdrzucony(403), "HTTP 403 - brak uprawnien, ponawiamy");
CHECK(!trwaleOdrzucony(404), "HTTP 404 - zly adres bazy, NIE kasujemy calej kolejki");
CHECK(!trwaleOdrzucony(500), "HTTP 500 - awaria serwera, ponawiamy");
CHECK(!trwaleOdrzucony(-1),  "brak sieci - ponawiamy");

for (int i = 0; i < 3; i++) queuePush(makeRecordAt("open", 0, 1750000000u + i));
CHECK(flushQueue() && queueCount()==0, "wszystko wyslane: %u zostalo", queueCount());
CHECK(FAKE_SENT.size()==3, "poszly 3 wpisy (%d)", (int)FAKE_SENT.size());

/* Sedno bledu: przy HTTP 400 stary kod wychodzil NIE zdejmujac wpisu,
   wiec ten sam rekord blokowal wszystkie dawki za soba - na zawsze.  */
prefs.wipe(); FAKE_SENT.clear(); rtcQueueDropped = 0;
for (int i = 0; i < 3; i++) queuePush(makeRecordAt("open", 0, 1750000000u + i));
FAKE_HTTP = 400;
CHECK(flushQueue(), "odrzucenie nie melduje sie jako awaria sieci");
CHECK(queueCount()==0, "zatkany wpis zdjety, kolejka drozna (%u)", queueCount());
CHECK(rtcQueueDropped==3, "strata policzona i widoczna: %u", rtcQueueDropped);

/* Awaria chwilowa dziala odwrotnie: NIC nie wolno wyrzucic. */
prefs.wipe(); FAKE_SENT.clear(); rtcQueueDropped = 0;
for (int i = 0; i < 3; i++) queuePush(makeRecordAt("open", 0, 1750000000u + i));
FAKE_HTTP = 500;
CHECK(!flushQueue(), "awaria serwera melduje niepowodzenie");
CHECK(queueCount()==3, "przy awarii nie tracimy nic (%u)", queueCount());
CHECK(rtcQueueDropped==0, "licznik strat nietkniety: %u", rtcQueueDropped);

/* --- B19: backoff musi wracac do zera, gdy siec sie odezwie ------------
   rtcRetryCount rosnie przy kazdej nieudanej wysylce i steruje przerwa
   miedzy probami: 15 min, 30, 60, 120, 240 - i tyle juz zostaje.

   TU BYL BLAD. Zerowanie stalo WYLACZNIE w reportEvent(), czyli tylko na
   sciezce "otwarto pudelko". Po kilku dniach bez internetu licznik stal
   na maksimum, a wybudzenia rutynowe (fetchConfig(); flushQueue();
   pushStatus();) nie ruszaly go wcale. Powrot do domu niczego wiec nie
   przyspieszal: jesli zaleglosci nie zmiescily sie w jednym czuwaniu,
   reszta czekala kolejne CZTERY GODZINY mimo sprawnej sieci.          */
prefs.wipe(); FAKE_SENT.clear(); FAKE_HTTP = 200;
queuePush(makeRecordAt("open", 0, 1750000000u));
rtcRetryCount = 4;                                  // stan po dobie bez sieci
CHECK(flushQueue(), "wysylka po powrocie sieci przechodzi");
CHECK(rtcRetryCount == 0, "udana wysylka kasuje backoff (%u)", rtcRetryCount);

prefs.wipe(); FAKE_SENT.clear(); FAKE_HTTP = 500;
queuePush(makeRecordAt("open", 0, 1750000000u));
rtcRetryCount = 4;
CHECK(!flushQueue(), "awaria serwera nadal melduje niepowodzenie");
CHECK(rtcRetryCount == 4,
      "awaria NIE kasuje backoffu - inaczej pudelko dobijaloby sie co 15 min "
      "do martwej bazy i zjadlo baterie (%u)", rtcRetryCount);

/* Odmowa na stale to co innego: baza ODPOWIEDZIALA, wiec siec dziala.
   Reszta kolejki ma prawo pojsc od razu, a nie za cztery godziny.     */
prefs.wipe(); FAKE_SENT.clear(); FAKE_HTTP = 400; rtcQueueDropped = 0;
queuePush(makeRecordAt("open", 0, 1750000000u));
rtcRetryCount = 4;
CHECK(flushQueue(), "odrzucenie nie jest awaria sieci");
CHECK(rtcRetryCount == 0, "odpowiedz bazy tez jest dowodem na dzialajaca siec (%u)",
      rtcRetryCount);
FAKE_HTTP = 200; rtcQueueDropped = 0; rtcRetryCount = 0;

/* Rekord uszkodzony tez nigdy nie przejdzie - nie moze blokowac. */
CHECK(rekordKompletny(makeRecordAt("open", 0, 1750000000u)), "pelny rekord przechodzi");
CHECK(!rekordKompletny(String("cos;bez;pieciu;pol")), "brak piatego pola wykryty");
CHECK(!rekordKompletny(String("")), "pusty rekord wykryty");

/* ================= 5c. NIEUDANY ZAPIS DO NVS (B5) ================= */
head("Nieudany zapis do pamieci nie udaje udanego");
prefs.wipe(); rtcNvsFail = 0; rtcNvsFailKey[0] = 0;
prefs.failKeys.insert("q0");
CHECK(!queuePush(makeRecordAt("open", 0, 1750000000u)), "queuePush melduje niepowodzenie");
CHECK(queueCount()==0, "licznik nie podniesiony - brak wpisu-widma (%u)", queueCount());
CHECK(rtcNvsFail==1, "awaria policzona: %u", rtcNvsFail);
/* D46: sam licznik mowi "cos przepadlo", nie mowi CO. Bez klucza kazdy
   incydent - wpis kolejki (realna strata dawki) czy token logowania
   (bez znaczenia) - wyglada w aplikacji identycznie.                   */
CHECK(String(rtcNvsFailKey)=="q0",
      "zapamietany klucz nieudanego zapisu: '%s'", rtcNvsFailKey);
prefs.failKeys.clear();
CHECK(queuePush(makeRecordAt("open", 0, 1750000001u)), "po ustaniu awarii zapis wchodzi");
CHECK(queueCount()==1, "i tym razem licznik rosnie (%u)", queueCount());

/* nvsPutU16 zapamietuje klucz tak samo jak nvsPutStr - to DWIE oddzielne
   funkcje i latwo poprawic jedna, zapominajac o drugiej.               */
prefs.wipe(); rtcNvsFail = 0; rtcNvsFailKey[0] = 0;
prefs.failKeys.insert("qc");
prefs.begin(NVS_NAMESPACE, false);
CHECK(!nvsPutU16("qc", 5), "nvsPutU16 tez melduje niepowodzenie");
prefs.end();
CHECK(String(rtcNvsFailKey)=="qc", "i tez zapamietuje klucz: '%s'", rtcNvsFailKey);
prefs.failKeys.clear();

/* Kolejny nieudany zapis NADPISUJE poprzedni klucz - trzymamy tylko
   OSTATNI, nie historie. Przy powtarzajacym sie problemie ostatni klucz
   i tak jest reprezentatywny; przy jednorazowym jest jedynym.          */
prefs.wipe(); rtcNvsFail = 0; rtcNvsFailKey[0] = 0;
prefs.failKeys.insert("dw"); prefs.failKeys.insert("tok");
prefs.begin(NVS_NAMESPACE, false);
nvsPutStr("dw", String("10|10|10|10|10|10|10"));
nvsPutStr("tok", String("abc.def.ghi"));
prefs.end();
CHECK(rtcNvsFail==2, "oba niepowodzenia policzone (%u)", rtcNvsFail);
CHECK(String(rtcNvsFailKey)=="tok", "a klucz to OSTATNI z nich: '%s'", rtcNvsFailKey);
prefs.failKeys.clear();
prefs.wipe(); rtcNvsFail = 0; rtcNvsFailKey[0] = 0;

/* ================= 5d. HISTORIA NIEUDANYCH ZAPISOW (D47) ================
   Kuba: "może w bazie damy jakiś zapis, żeby te nieudane tam wrzucał" - z
   samej liczby i ostatniego klucza nie da sie sprawdzic hipotezy typu
   "to sie dzieje po alarmie". Trzeba widziec CZAS kazdego incydentu.    */
head("Historia nieudanych zapisow - pierscien w RTC");
auto zresetujDziennik = [](){
  rtcNvsFailLogTotal = 0; rtcNvsFailLogSent = 0;
  for (int i = 0; i < NVS_FAILLOG_SLOTS; i++) { rtcNvsFailLogTs[i]=0; rtcNvsFailLogKey[i][0]=0; }
};
zresetujDziennik(); rtcTimeValid = true;
CHECK(!nvsFailLogDoWyslania(), "pusty dziennik - nic do wyslania");
CHECK(nvsFailLogJson()==String("{\"wpisy\":[]}"), "i pusta tablica w JSON");

prefs.wipe();
prefs.failKeys.insert("q5");
FAKE_NOW = local(2026, 8, 11, 20, 0);
prefs.begin(NVS_NAMESPACE, false);
nvsPutStr("q5", String("cokolwiek"));
prefs.end();
CHECK(nvsFailLogDoWyslania(), "po jednym niepowodzeniu jest co wyslac");
String j1 = nvsFailLogJson();
CHECK(j1.indexOf("\"klucz\":\"q5\"") >= 0, "JSON zawiera klucz: %s", j1.c_str());
CHECK(j1.indexOf(String((unsigned long)FAKE_NOW).c_str()) >= 0,
      "i prawdziwy czas niepowodzenia: %s", j1.c_str());
nvsFailLogOznaczWyslany();
CHECK(!nvsFailLogDoWyslania(), "po potwierdzeniu nie ma juz nic do wyslania");
CHECK(nvsFailLogJson()==String("{\"wpisy\":[]}"), "kolejny JSON znow pusty");
prefs.failKeys.clear();

/* Bez znanego zegara wpis i tak powstaje - z czasem 0, a nie zgubiony. */
zresetujDziennik(); prefs.wipe();
rtcTimeValid = false;
prefs.failKeys.insert("dw");
prefs.begin(NVS_NAMESPACE, false);
nvsPutStr("dw", String("cokolwiek"));
prefs.end();
CHECK(nvsFailLogJson().indexOf("\"ts\":0") >= 0,
      "brak zegara -> ts=0, nie zmyslony czas: %s", nvsFailLogJson().c_str());
rtcTimeValid = true;
prefs.failKeys.clear();

/* Wiecej niz NVS_FAILLOG_SLOTS niepowodzen: pierscien zachowuje NAJNOWSZE,
   a JSON nie probuje zrekonstruowac tego, co juz nadpisane.             */
zresetujDziennik(); prefs.wipe();
for (int i = 0; i < NVS_FAILLOG_SLOTS + 3; i++) {
  char klucz[8]; snprintf(klucz, sizeof(klucz), "k%d", i);
  prefs.failKeys.insert(klucz);
  prefs.begin(NVS_NAMESPACE, false);
  nvsPutStr(klucz, String("x"));
  prefs.end();
  prefs.failKeys.clear();
}
CHECK(rtcNvsFailLogTotal == NVS_FAILLOG_SLOTS + 3,
      "licznik rosnie ponad pojemnosc pierscienia (%u)", rtcNvsFailLogTotal);
{
  String j = nvsFailLogJson();
  CHECK(j.indexOf("\"klucz\":\"k2\"") < 0, "najstarsze (k2) juz nadpisane, nie ma go w JSON");
  CHECK(j.indexOf(String("\"klucz\":\"k" + String(NVS_FAILLOG_SLOTS+2) + "\"").c_str()) >= 0,
        "ale najnowszy wpis jest: %s", j.c_str());
  /* NAJWAZNIEJSZE W TYM TEScie: samo "k2 nie wystepuje" przechodzi takze
     bez obcinania zakresu w nvsFailLogJson() - fizyczne nadpisanie slotu
     w pierscieniu i tak je usuwa. Prawdziwa stawka to LICZBA wpisow: bez
     obciecia petla probuje odtworzyc wszystkie 11 (total-sent), z czego
     3 sa duplikatami danych z nadpisanych slotow. Liczymy wystapienia
     "klucz" wprost - to jedyny sposob odroznic te dwa przypadki.        */
  int ile = 0;
  for (int p = j.indexOf("\"klucz\""); p >= 0; p = j.indexOf("\"klucz\"", p+1)) ile++;
  CHECK(ile == NVS_FAILLOG_SLOTS,
        "dokladnie tyle wpisow, ile miejsc w pierscieniu, bez duplikatow (%d)", ile);
}
nvsFailLogOznaczWyslany();
CHECK(!nvsFailLogDoWyslania(), "po wyslaniu paczki znow nic nie czeka");
zresetujDziennik();

/* Wpis-widmo z czasow przed poprawka: licznik mowi "1", tresci brak.
   Nieczytelny wpis na czele blokowalby wysylke w nieskonczonosc.     */
prefs.wipe(); rtcQueueDropped = 0; FAKE_HTTP = 200; FAKE_SENT.clear();
queuePush(makeRecordAt("open", 0, 1750000000u));
prefs.str.erase("q0");                            /* tresc znika, licznik zostaje */
CHECK(queueCount()==1, "przygotowanie: licznik mowi 1 (%u)", queueCount());
CHECK(flushQueue(), "nieczytelny wpis nie zawiesza wysylki");
CHECK(queueCount()==0, "i zostaje zdjety (%u)", queueCount());
CHECK(rtcQueueDropped==1, "strata policzona: %u", rtcQueueDropped);

prefs.failKeys.clear(); rtcNvsFail = 0; rtcQueueDropped = 0; FAKE_HTTP = 200;

/* ================= 6. KOREKTA DRYFU ZEGARA ================= */
head("Korekta znacznikow czasu po synchronizacji NTP");
prefs.wipe();
queuePush(makeRecordAt("open", 0, 1750000000u));
queuePush(makeRecordAt("missed", 0, 1750086400u));
queueShiftTimestamps(+900);                     /* zegar spieszyl sie o 15 min */
queuePeek(r);
CHECK(r.startsWith(String("1750000900;")), "przesuniecie +900: %s", r.c_str());
queuePop(); queuePeek(r);
CHECK(r.startsWith(String("1750087300;")), "drugi wpis tez: %s", r.c_str());

prefs.wipe();
queuePush(makeRecordAt("open", 0, 1750000000u));
queueShiftTimestamps(+40000);                   /* powyzej 6 h -> ignorujemy */
queuePeek(r);
CHECK(r.startsWith(String("1750000000;")), "zbyt duza korekta odrzucona: %s", r.c_str());

prefs.wipe();
queuePush(makeRecordAt("open", 0, 0));          /* wpis bez czasu */
queueShiftTimestamps(+600);
queuePeek(r);
CHECK(r.startsWith(String("0;")), "wpis bez czasu nietkniety: %s", r.c_str());

/* ================= 7. FORMAT REKORDU ================= */
head("Format rekordu zdarzenia");
batteryPercentage = 73; realBatteryVoltage = 3.957f;
String rec = makeRecordAt("open", 0, 1750000123u);
CHECK(rec==String("1750000123;open;73;3.957;0"), "rekord: %s", rec.c_str());

int p1 = rec.indexOf(';'), p2 = rec.indexOf(';', p1+1),
    p3 = rec.indexOf(';', p2+1), p4 = rec.indexOf(';', p3+1);
CHECK(p4 > 0, "cztery separatory obecne");
CHECK(rec.substring(p1+1, p2)==String("open"), "typ=%s", rec.substring(p1+1,p2).c_str());
CHECK(rec.substring(p2+1, p3).toInt()==73, "bateria=%ld", rec.substring(p2+1,p3).toInt());
CHECK(fabs(rec.substring(p3+1, p4).toFloat() - 3.957f) < 0.001, "napiecie odczytane poprawnie");
CHECK(rec.substring(p4+1).toInt()==0, "slot=%ld", rec.substring(p4+1).toInt());

rtcTimeValid = false;
CHECK(makeRecord("boot", -1).startsWith(String("0;boot")), "bez zegara ts=0");
rtcTimeValid = true;

/* ================= 8. ROLOWANIE DOBY ================= */
head("Rolowanie doby lekowej (o DAY_START_HOUR, nie o polnocy)");
parseSchedule(String("08:00"));
prefs.wipe();

/* pierwszy raz: tylko zapamietuje dzien */
rtcRolloverDay = 0; rtcTakenDay = 0;
FAKE_NOW = local(2026,7,30,3,1);
checkDayRollover();
CHECK(rtcRolloverDay==20260730u && queueCount()==0, "pierwsze uruchomienie nie zglasza braku");

/* dawka wzieta 30 lipca -> przy rolowaniu 31-go nic sie nie dzieje */
rtcTakenDay = 20260730u;
FAKE_NOW = local(2026,7,31,3,1);
checkDayRollover();
CHECK(queueCount()==0, "doba zamknieta poprawnie -> brak zdarzenia (%u)", queueCount());
CHECK(rtcRolloverDay==20260731u, "znacznik przesuniety: %u", rtcRolloverDay);

/* dawka NIE wzieta 31 lipca -> nad ranem 1 sierpnia leci "missed".
   Kluczowe: NIE o 00:01, bo o tej porze wciaz mozna ja wziac.       */
FAKE_NOW = local(2026,8,1,3,1);
checkDayRollover();
CHECK(queueCount()==1, "pominieta dawka zapisana (%u)", queueCount());
queuePeek(r);
unsigned long ts = (unsigned long)r.substring(0, r.indexOf(';')).toInt();
CHECK(localDayNumber((time_t)ts)==20260731u,
      "znacznik nalezy do 31 lipca, a nie do 1 sierpnia (jest %u)", localDayNumber((time_t)ts));
CHECK(localMinutesOfDay((time_t)ts)==1439, "znacznik = 23:59 (%d min)", localMinutesOfDay((time_t)ts));
CHECK(r.indexOf(String("missed")) > 0, "typ zdarzenia: %s", r.c_str());

/* powtorne wywolanie tego samego dnia nie duplikuje */
checkDayRollover();
CHECK(queueCount()==1, "brak duplikatu przy ponownym wybudzeniu (%u)", queueCount());

/* ================= 9a2. DNI BEZ LEKU ================= */
head("Rozpisanie tygodniowe - czytanie i odrzucanie");
resetDosing();
parseDoseWeek(String("0|10|10|5|10|10|15"));
CHECK(rtcDoseWeek[0]==0 && rtcDoseWeek[3]==5 && rtcDoseWeek[6]==15,
      "siedem poprawnych liczb wchodzi w calosci");

/* NAJWAZNIEJSZY TEST W TEJ SEKCJI - ten sam, co po stronie aplikacji.
   Brakujacy dzien odczytany jako zero byl by cichym dniem bez leku
   przeciwzakrzepowego, wiec niepelne rozpisanie musi zniknac CALE.   */
parseDoseWeek(String("10|10|10|10|10|10"));
CHECK(rtcDoseWeek[0]==DOSE_NIEZNANA && rtcDoseWeek[5]==DOSE_NIEZNANA,
      "szesc dni zamiast siedmiu kasuje CALE rozpisanie, nie daje zera");
parseDoseWeek(String(""));
CHECK(rtcDoseWeek[2]==DOSE_NIEZNANA, "pusty napis tez nie zostawia zer");
parseDoseWeek(String("10|10|10|10|10|10|10"));
CHECK(rtcDoseWeek[4]==10, "poprawne rozpisanie wraca po odrzuceniu zlego");

head("Wyjatki na konkretne daty");
CHECK(dateKeyToNum("2026-08-14")==20260814u, "data z aplikacji na liczbe");
CHECK(dateKeyToNum("2026-8-14")==0,  "skrocona data odrzucona");
CHECK(dateKeyToNum("nie-data-xx")==0, "napis bez cyfr odrzucony");
CHECK(dateKeyToNum(nullptr)==0, "brak klucza nie wywraca parsera");

resetDosing();
parseDoseWeek(String("10|10|10|10|10|10|10"));
parseDoseEx(String("20260814:0|20260815:5"));
CHECK(rtcDoseExCount==2, "dwa wyjatki wczytane (%u)", rtcDoseExCount);
CHECK(dawkaNaDobe(20260814u, 1)==0, "wyjatek bije rozpisanie tygodniowe");
CHECK(dawkaNaDobe(20260815u, 1)==5, "polowka z wyjatku");
CHECK(dawkaNaDobe(20260816u, 1)==10, "dzien bez wyjatku bierze z tygodnia");

resetDosing();
CHECK(dawkaNaDobe(20260816u, 1)==DOSE_NIEZNANA,
      "bez rozpisania odpowiedz brzmi 'nie wiem', a nie 'zero'");
CHECK(dawkaNaDobe(20260816u, 9)==DOSE_NIEZNANA, "dzien tygodnia poza zakresem nie czyta poza tablice");

head("Dzien bez leku - kiedy pudelko milczy, a kiedy dzwoni");
resetDosing();
parseDoseWeek(String("0|10|10|10|10|10|10"));   // niedziela bez leku
{
  /* Szukamy prawdziwej niedzieli, zeby test nie zalezal od tego, na jaki
     dzien tygodnia wypada konkretna data w kalendarzu.               */
  time_t niedz = 0;
  for (int d = 1; d <= 7; d++) {
    time_t t = local(2026, 8, d, 12, 0);
    if (localWeekday(t) == 0) { niedz = t; break; }
  }
  CHECK(niedz != 0, "w pierwszym tygodniu sierpnia jest niedziela");
  rtcTimeValid = true;
  FAKE_NOW = niedz;
  CHECK(dzisBezLeku(), "w niedziele rozpisana na zero - cisza");
  FAKE_NOW = niedz + 86400;
  CHECK(!dzisBezLeku(), "nazajutrz juz nie");

  /* Bez zegara nie wiadomo, ktory jest dzien - wtedy DZWONIMY. */
  FAKE_NOW = niedz;
  rtcTimeValid = false;
  CHECK(!dzisBezLeku(), "bez znanego zegara nie wyciszamy sie NIGDY");
  rtcTimeValid = true;

  resetDosing();
  CHECK(!dzisBezLeku(), "bez rozpisania tez dzwonimy");
}

head("Doba bez leku nie jest doba pominieta");
prefs.wipe();
parseSchedule(String("08:00"));
{
  time_t ts31 = local(2026, 7, 31, 23, 59);
  int wd = localWeekday(ts31);

  /* Najpierw kontrola: przy zwyklym rozpisaniu 31 lipca MA sie zglosic. */
  resetDosing();
  parseDoseWeek(String("10|10|10|10|10|10|10"));
  rtcTakenDay = 0; rtcRolloverDay = 20260731u;
  FAKE_NOW = local(2026, 8, 1, 3, 1);
  checkDayRollover();
  CHECK(queueCount()==1, "dzien z lekiem nadal trafia do kolejki jako pominiety (%u)",
        queueCount());

  /* A teraz to samo, gdy ten dzien tygodnia jest rozpisany na zero. */
  prefs.wipe();
  resetDosing();
  parseDoseWeek(String("10|10|10|10|10|10|10"));
  rtcDoseWeek[wd] = 0;
  rtcTakenDay = 0; rtcRolloverDay = 20260731u;
  FAKE_NOW = local(2026, 8, 1, 3, 1);
  checkDayRollover();
  CHECK(queueCount()==0, "doba rozpisana bez leku nie zglasza pominietej dawki (%u)",
        queueCount());

  /* Wyjatek na date dziala tak samo - to nim robi sie odstawienie. */
  prefs.wipe();
  resetDosing();
  parseDoseWeek(String("10|10|10|10|10|10|10"));
  parseDoseEx(String("20260731:0"));
  rtcTakenDay = 0; rtcRolloverDay = 20260731u;
  FAKE_NOW = local(2026, 8, 1, 3, 1);
  checkDayRollover();
  CHECK(queueCount()==0, "odstawienie przed zabiegiem tez nie jest pominieciem (%u)",
        queueCount());
}
resetDosing();
prefs.wipe();

/* ================= 9a2b. PUSTY ZAPIS TO NIE AWARIA ================= */
head("Pusty zapis do pamieci nie jest utrata danych");
/* putString("") zwraca ZERO, czyli tyle samo, co zapis nieudany. Kosztowalo
   to falszywy alarm "pudelko zglasza utrate danych" i siec WiFi, ktora nie
   chciala sie przyjac, bo haslo bylo puste.                             */
prefs.wipe();
rtcNvsFail = 0;
prefs.begin(NVS_NAMESPACE, false);
CHECK(nvsPutStr("pusty", String("")), "zapis pustej wartosci MELDUJE SUKCES");
prefs.end();
CHECK(rtcNvsFail==0, "i nie podnosi licznika utraty danych (%u)", rtcNvsFail);
CHECK(prefs.getString("pusty", "nic")=="nic",
      "pusty klucz czyta sie jako wartosc domyslna");

/* Prawdziwa awaria nadal MUSI byc widoczna - inaczej naprawa zamiotlaby
   pod dywan cala rodzine bledu 3.5.                                     */
prefs.wipe(); rtcNvsFail = 0;
prefs.failKeys.insert("cos");
prefs.begin(NVS_NAMESPACE, false);
CHECK(!nvsPutStr("cos", String("tresc")), "prawdziwa awaria dalej melduje porazke");
prefs.end();
CHECK(rtcNvsFail==1, "i dalej podnosi licznik (%u)", rtcNvsFail);
prefs.failKeys.clear();

/* Siec OTWARTA - hotel, lotnisko - nie ma hasla i musi sie zapisac. */
prefs.wipe(); rtcNvsFail = 0;
CHECK(wifiSiecDodaj(String("hotel-wifi"), String("")),
      "siec bez hasla zapisuje sie poprawnie");
CHECK(wifiSieciCount()==1, "i jest na liscie (%d)", wifiSieciCount());
CHECK(wifiSiecSsid(0)=="hotel-wifi", "z wlasciwa nazwa");
CHECK(rtcNvsFail==0, "bez ani jednego falszywego alarmu (%u)", rtcNvsFail);

/* Rozpisanie bez zadnego wyjatku na date - `dex` jest wtedy pusty. */
prefs.wipe(); rtcNvsFail = 0;
saveDosing(String("10|10|10|10|10|10|10"), String(""));
CHECK(rtcNvsFail==0, "puste wyjatki dawkowania to tez nie awaria (%u)", rtcNvsFail);
CHECK(rtcDoseWeek[0]==10, "a rozpisanie i tak weszlo");
resetDosing();
prefs.wipe(); rtcNvsFail = 0;

/* ================= 9a3. LISTA ZNANYCH SIECI ================= */
head("Lista sieci WiFi");
prefs.wipe();
CHECK(wifiSieciCount()==0, "swieze pudelko nie zna zadnej sieci");
CHECK(wifiSiecSsid(0)=="", "i nie zmysla nazwy");

CHECK(wifiSiecDodaj(String("dom"), String("haslo1")), "pierwsza siec zapisana");
CHECK(wifiSieciCount()==1, "licznik sieci = 1 (%d)", wifiSieciCount());
CHECK(wifiSiecSsid(0)=="dom", "nazwa odczytana z pamieci");

/* Nowa siec idzie na POCZATEK - to zwykle ta, ktorej wlasnie potrzebujemy. */
wifiSiecDodaj(String("hotspot"), String("haslo2"));
CHECK(wifiSiecSsid(0)=="hotspot" && wifiSiecSsid(1)=="dom",
      "nowa siec ladu je na poczatku listy");
CHECK(wifiSieciCount()==2, "obie sieci na liscie (%d)", wifiSieciCount());

/* Ta sama nazwa ze zmienionym haslem ZASTEPUJE stary wpis. Inaczej zmiana
   hasla do domowego WiFi zjadalaby drugie miejsce z czterech.          */
wifiSiecDodaj(String("dom"), String("nowehaslo"));
CHECK(wifiSieciCount()==2, "duplikat nie zwieksza listy (%d)", wifiSieciCount());
CHECK(wifiSiecSsid(0)=="dom" && wifiSiecSsid(1)=="hotspot",
      "zaktualizowana siec jest teraz pierwsza");

/* Limit: piata siec wypycha najstarsza, a nie przepelnia tablicy. */
prefs.wipe();
for (int i = 0; i < WIFI_SIECI_MAX + 3; i++)
  wifiSiecDodaj(String("siec") + String(i), String("h"));
CHECK(wifiSieciCount()==WIFI_SIECI_MAX,
      "lista nie przekracza limitu (%d z %d)", wifiSieciCount(), WIFI_SIECI_MAX);
CHECK(wifiSiecSsid(0)==String("siec") + String(WIFI_SIECI_MAX + 2),
      "na czele stoi ostatnio dodana");

/* Nazwa pusta albo absurdalnie dluga nie moze wejsc na liste - bez tego
   pudelko probowaloby sie laczyc z niczym i tracilo na to radio.       */
prefs.wipe();
CHECK(!wifiSiecDodaj(String(""), String("h")), "pusta nazwa odrzucona");
CHECK(!wifiSiecDodaj(String(std::string(33, 'x').c_str()), String("h")),
      "nazwa ponad 32 znaki odrzucona");
CHECK(wifiSieciCount()==0, "i nic nie weszlo na liste (%d)", wifiSieciCount());

/* NAJWAZNIEJSZY TEST TEJ SEKCJI.
   Wynik false znaczy "siec NIE zostala zapisana", a na tej odpowiedzi opiera
   sie decyzja, czy wolno skasowac haslo z bazy (zasada 6). Gdyby zapis
   milczaco udawal sukces, haslo znikneloby z bazy, a pudelko nie znaloby
   sieci - i nie byloby juz sposobu, zeby mu ja podac inaczej niz portalem. */
prefs.wipe();
prefs.failKeys.insert("n0s");
CHECK(!wifiSiecDodaj(String("dom"), String("haslo")),
      "nieudany zapis do pamieci MELDUJE porazke, a nie udaje sukcesu");
prefs.failKeys.clear();
prefs.wipe();
prefs.failKeys.insert("netN");
CHECK(!wifiSiecDodaj(String("dom"), String("haslo")),
      "nieudany zapis licznika sieci tez jest porazka");
prefs.failKeys.clear();
prefs.wipe();

/* ================= 9a4. USUWANIE I KOLEJNOSC SIECI ================= */
head("Usuwanie sieci z listy");
prefs.wipe();
WiFi._rozlacz();
wifiSiecDodaj(String("dom"), String("h1"));
wifiSiecDodaj(String("hotspot"), String("h2"));
wifiSiecDodaj(String("praca"), String("h3"));
CHECK(wifiSieciCount()==3, "trzy sieci na starcie (%d)", wifiSieciCount());

CHECK(wifiSiecUsun(String("dom"))==WIFI_USUN_OK, "siec z srodka listy usuwa sie");
CHECK(wifiSieciCount()==2, "zostaja dwie (%d)", wifiSieciCount());
CHECK(wifiSiecSsid(0)=="praca" && wifiSiecSsid(1)=="hotspot",
      "a pozostale zachowuja kolejnosc");
CHECK(wifiSiecUsun(String("nie-ma-takiej"))==WIFI_USUN_BRAK,
      "nieznana siec to osobna odpowiedz, nie cicha zgoda");

/* NAJWAZNIEJSZE ZABEZPIECZENIE: nie wolno usunac jedynej sieci, przez ktora
   pudelko wlasnie rozmawia - zostaloby bez drogi powrotu poza portalem. */
prefs.wipe();
wifiSiecDodaj(String("dom"), String("h1"));
WiFi._polacz("dom");
CHECK(wifiSiecUsun(String("dom"))==WIFI_USUN_OSTATNIA,
      "jedyna siec, przez ktora jestesmy polaczeni, NIE daje sie usunac");
CHECK(wifiSieciCount()==1, "i naprawde zostaje na liscie (%d)", wifiSieciCount());

/* Ta sama siec, ale gdy jest DRUGA - usuwanie jest juz bezpieczne. */
wifiSiecDodaj(String("hotspot"), String("h2"));
WiFi._polacz("dom");
CHECK(wifiSiecUsun(String("dom"))==WIFI_USUN_OK,
      "gdy jest zapasowa, polaczona siec wolno usunac");
CHECK(wifiSieciCount()==1 && wifiSiecSsid(0)=="hotspot", "zostaje zapasowa");

/* Skrocenie listy oddaje miejsce w NVS. Odczytu to nie zmienia (netN i tak
   ogranicza petle), wiec sprawdzamy to, co naprawde jest stawka: czy martwe
   klucze zniknely z pamieci. Cala partycja to ~20 kB na wszystko (D25),
   a haslo do WiFi miewa 63 znaki.                                       */
prefs.wipe();
WiFi._rozlacz();
wifiSiecDodaj(String("a"), String("1"));
wifiSiecDodaj(String("b"), String("2"));
wifiSiecDodaj(String("c"), String("3"));
CHECK(prefs.str.count("n2s")==1, "trzecia siec faktycznie zajmuje miejsce w NVS");
wifiSiecUsun(String("a"));
wifiSiecUsun(String("b"));
CHECK(wifiSieciCount()==1, "zostala jedna (%d)", wifiSieciCount());
CHECK(prefs.str.count("n1s")==0 && prefs.str.count("n2s")==0,
      "a klucze po skroconej liscie sa skasowane, nie porzucone");
CHECK(prefs.str.count("n1p")==0 && prefs.str.count("n2p")==0,
      "razem z haslami - to one zajmuja najwiecej");
CHECK(prefs.str.count("n0s")==1 && wifiSiecSsid(0)=="c", "a to, co zostalo, dziala");

head("Kolejnosc prob: przelaczanie sieci");
prefs.wipe();
WiFi._rozlacz();
wifiSiecDodaj(String("dom"), String("h1"));
wifiSiecDodaj(String("hotspot"), String("h2"));
CHECK(wifiSiecSsid(0)=="hotspot", "ostatnio dodana jest pierwsza");
CHECK(wifiSiecPriorytet(String("dom")), "przelaczenie na 'dom' przyjete");
CHECK(wifiSiecSsid(0)=="dom" && wifiSiecSsid(1)=="hotspot",
      "wskazana siec staje na czele listy");
CHECK(wifiSieciCount()==2, "bez gubienia pozostalych (%d)", wifiSieciCount());
CHECK(!wifiSiecPriorytet(String("nie-ma-takiej")), "nieznanej sieci nie da sie wybrac");
CHECK(wifiSiecPriorytet(String("dom")), "wskazanie tej, ktora juz jest pierwsza, jest bezpieczne");
CHECK(wifiSieciCount()==2 && wifiSiecSsid(0)=="dom", "i niczego nie psuje");
/* Haslo musi wedrowac razem z nazwa - inaczej po przelaczeniu pudelko
   probowaloby sie laczyc z wlasciwa siecia i cudzym haslem. */
prefs.wipe();
wifiSiecDodaj(String("dom"), String("tajne-dom"));
wifiSiecDodaj(String("hotspot"), String("tajne-hot"));
wifiSiecPriorytet(String("dom"));
CHECK(wifiSiecPass(0)=="tajne-dom" && wifiSiecPass(1)=="tajne-hot",
      "hasla ida za nazwami przy zmianie kolejnosci");
prefs.wipe();
WiFi._rozlacz();

/* ================= 9b. ZNACZNIKI PRZEZYWAJACE RESET ================= */
head("Znacznik dawki przezywa reset plytki");
prefs.wipe();
rtcTakenDay = 0; rtcRolloverDay = 0;

setTakenDay(20260801u);
CHECK(rtcTakenDay==20260801u, "znacznik ustawiony w RAM");

/* symulujemy RESET: pamiec RTC sie zeruje, NVS zostaje */
rtcTakenDay = 0; rtcRolloverDay = 0;
loadDayMarkers();
CHECK(rtcTakenDay==20260801u,
      "po resecie znacznik odtworzony z pamieci nieulotnej (%u)", rtcTakenDay);

setRolloverDay(20260801u);
rtcTakenDay = 0; rtcRolloverDay = 0;
loadDayMarkers();
CHECK(rtcRolloverDay==20260801u, "znacznik doby tez przezywa reset (%u)", rtcRolloverDay);
CHECK(rtcTakenDay==20260801u, "oba znaczniki wracaja razem");

/* wybudzenie z deep sleep: RTC ma dane, NVS nie jest czytane */
prefs.wipe();
loadDayMarkers();
CHECK(rtcTakenDay==20260801u, "przy zywej pamieci RTC nie siegamy do NVS");

/* nowy dzien nadpisuje stary */
setTakenDay(20260802u);
rtcTakenDay = 0;
loadDayMarkers();
CHECK(rtcTakenDay==20260802u, "nowy dzien zapisany trwale (%u)", rtcTakenDay);

/* pelny scenariusz: dawka wzieta, reset, ponowne otwarcie tego samego dnia */
prefs.wipe();
rtcTakenDay = 0; rtcRolloverDay = 0;
FAKE_NOW = local(2026,8,1,19,3);
setTakenDay(localDayNumber(FAKE_NOW));          // pierwsze otwarcie
rtcTakenDay = 0;                                 // ktos wgrywa program -> RESET
loadDayMarkers();
FAKE_NOW = local(2026,8,1,19,12);
CHECK(rtcTakenDay == localDayNumber(FAKE_NOW),
      "drugie otwarcie po resecie rozpoznane jako TEN SAM dzien -> ostrzezenie zamiast zapisu");

/* ================= 9. BATERIA ================= */
head("Pomiar napiecia (mediana + kalibracja)");
/* raw = U / (3,3 * 2 * 0,921 / 4095).  Sama matematyka napiecia sie nie
   zmienila - zmienilo sie tylko przeliczenie napiecia na procent.      */
resetBatteryFilter();
FAKE_ADC = 2829;                 /* ogniwo naladowane, 4,20 V */
readBattery();
CHECK(fabs(realBatteryVoltage - 4.20f) < 0.01f, "pelne ogniwo -> %.3f V", realBatteryVoltage);
CHECK(batteryPercentage==100, "-> %d%% (musi byc rowne 100)", batteryPercentage);

resetBatteryFilter();
FAKE_ADC = 2594;                 /* 3,85 V */
readBattery();
CHECK(fabs(realBatteryVoltage - 3.85f) < 0.01f, "srodek -> %.3f V", realBatteryVoltage);

resetBatteryFilter();
FAKE_ADC = 2257;                 /* 3,35 V - prog wylaczenia radia */
readBattery();
CHECK(realBatteryVoltage >= BATT_SAFE_V - 0.01f && realBatteryVoltage < BATT_SAFE_V + 0.01f,
      "prog BATT_SAFE_V trafiony: %.3f V", realBatteryVoltage);

resetBatteryFilter();
FAKE_ADC = 2156;                 /* 3,20 V - prog odciecia */
readBattery();
CHECK(realBatteryVoltage < BATT_SAFE_V && realBatteryVoltage <= BATT_CUTOFF_V + 0.01f,
      "prog odciecia: %.3f V", realBatteryVoltage);
CHECK(realBatteryVoltage > 2.0f, "wciaz rozpoznawane jako obecna bateria (%.3f V)", realBatteryVoltage);

resetBatteryFilter();
FAKE_ADC = 0;
readBattery();
CHECK(batteryPercentage==0, "brak sygnalu -> %d%% (bez wartosci ujemnych)", batteryPercentage);
CHECK(realBatteryVoltage < 2.0f, "napiecie %.3f V < 2,0 V = 'brak baterii'", realBatteryVoltage);

resetBatteryFilter();
FAKE_ADC = 4095;
readBattery();
CHECK(batteryPercentage==100, "maksimum ADC -> %d%% (obciete do 100)", batteryPercentage);

/* ================= 10b. KRZYWA ROZLADOWANIA ================= */
head("Napiecie -> procent wedlug krzywej LiPo");
CHECK(battPercentFromCurve(4.20f)==100, "4,20 V -> %d%%", battPercentFromCurve(4.20f));
CHECK(battPercentFromCurve(4.50f)==100, "powyzej skali obciete -> %d%%", battPercentFromCurve(4.50f));
CHECK(battPercentFromCurve(3.27f)==0,   "3,27 V -> %d%%", battPercentFromCurve(3.27f));
CHECK(battPercentFromCurve(2.00f)==0,   "ponizej skali obciete -> %d%%", battPercentFromCurve(2.00f));
CHECK(battPercentFromCurve(3.85f)==55,  "3,85 V -> %d%%", battPercentFromCurve(3.85f));
CHECK(battPercentFromCurve(3.80f)==40,  "3,80 V -> %d%%", battPercentFromCurve(3.80f));
CHECK(battPercentFromCurve(3.75f)==25,  "3,75 V -> %d%%", battPercentFromCurve(3.75f));

/* Interpolacja miedzy punktami tabeli. */
CHECK(battPercentFromCurve(4.135f)>90 && battPercentFromCurve(4.135f)<95,
      "wartosc miedzy punktami interpolowana (%d%%)", battPercentFromCurve(4.135f));

/* Monotonicznosc: wyzsze napiecie nie moze dawac nizszego procentu. */
{
  int prev = -1; bool mono = true;
  for (float v = 3.20f; v <= 4.30f; v += 0.005f) {
    int p = battPercentFromCurve(v);
    if (p < prev) { mono = false; break; }
    prev = p;
  }
  CHECK(mono, "procent rosnie monotonicznie z napieciem");
}

/* Sedno zmiany: dawne liniowe przeliczenie zawyzalo dol skali.
   Przy 3,70 V pokazywalo 44%, choc to juz gleboka rezerwa.          */
{
  int lin = (int)lround(((3.70f - 3.3f) / (4.2f - 3.3f)) * 100.0f);
  CHECK(lin > 40, "stara skala dawala przy 3,70 V az %d%%", lin);
  CHECK(battPercentFromCurve(3.70f) < 20,
        "nowa skala mowi %d%% - i to jest prawda", battPercentFromCurve(3.70f));
}

/* Progi dzwiekowe musza teraz wypadac wyraznie nad napieciem odciecia,
   inaczej ostrzezenie odzywaloby sie, gdy jest juz za pozno.         */
{
  float vWarn = -1, vCrit = -1;
  for (float v = 3.20f; v <= 4.20f; v += 0.005f) {
    if (vCrit < 0 && battPercentFromCurve(v) >= BATT_CRIT_PCT) vCrit = v;
    if (vWarn < 0 && battPercentFromCurve(v) >= BATT_WARN_PCT) vWarn = v;
  }
  CHECK(vCrit > BATT_SAFE_V + 0.2f,
        "prog krytyczny (%d%%) wypada przy %.2f V, z zapasem nad %.2f V",
        BATT_CRIT_PCT, vCrit, BATT_SAFE_V);
  CHECK(vWarn > vCrit, "prog ostrzegawczy (%.2f V) powyzej krytycznego (%.2f V)", vWarn, vCrit);
}

/* ================= 10c. WYGLADZANIE WSKAZANIA ================= */
head("Filtr wskazania baterii");
/* Filtr ma dwa zadania naraz: nie klamac i nie migotac. Migotanie brzmi
   blaho, ale to wlasnie ono sprawia, ze czlowiek przestaje ufac liczbie -
   a wtedy caly wskaznik baterii traci sens.                             */
resetBatteryFilter();
CHECK(battSmooth(80)==80, "pierwszy odczyt przyjmowany wprost");

/* MARTWA STREFA: drobne wahania szumu nie ruszaja wskazania. */
CHECK(battSmooth(79)==80, "spadek o 1 to szum - wskazanie stoi");
CHECK(battSmooth(78)==80, "spadek o 2 tez");
CHECK(battSmooth(81)==80, "wzrost o 1 rowniez nie rusza");
CHECK(battSmooth(77)==77, "dopiero spadek o 3 jest traktowany powaznie");

/* Chwilowe zalamanie napiecia nie moze zrzucic wskazania o 20 punktow. */
resetBatteryFilter();
battSmooth(80);
int dip = battSmooth(55);
CHECK(dip == 80 - BATT_STEP_DOWN, "gwaltowny spadek przyciety do %d punktow (jest %d)",
      BATT_STEP_DOWN, 80 - dip);

/* POWROT W GORE wymaga potwierdzenia. Jeden wysoki odczyt to przypadek. */
int h1 = battSmooth(80);
CHECK(h1 == dip, "pierwszy wyzszy odczyt jeszcze nie podnosi (%d)", h1);
int h2 = battSmooth(80);
CHECK(h2 == dip, "drugi tez nie (%d)", h2);
int h3 = battSmooth(80);
CHECK(h3 == dip + BATT_STEP_UP, "dopiero %d z rzedu podnosi o %d (%d -> %d)",
      BATT_RISE_STREAK, BATT_STEP_UP, dip, h3);
CHECK(h3 < 80, "i nigdy skokiem (%d)", h3);

/* Przerwana seria zaczyna liczenie od nowa - inaczej pojedyncze zaklocenia
   rozrzucone w czasie sumowalyby sie w falszywy wzrost.                 */
resetBatteryFilter();
battSmooth(60);
battSmooth(70); battSmooth(70);      // dwa wyzsze
battSmooth(60);                      // przerwa serii
int po = battSmooth(70);
CHECK(po == 60, "przerwana seria nie podnosi wskazania (%d)", po);

/* Trwaly spadek jest nadganiany, tylko w kontrolowanym tempie. */
resetBatteryFilter();
battSmooth(90);
int v = 90;
for (int i = 0; i < 20; i++) v = battSmooth(50);
CHECK(v == 50, "po kilkunastu wybudzeniach wskazanie dochodzi do prawdy (%d)", v);

/* Ladowanie: bez wyzerowania filtra wskazanie pelzloby po punkcie. */
resetBatteryFilter();
battSmooth(20);
int pelzanie = 20;
for (int i = 0; i < BATT_RISE_STREAK; i++) pelzanie = battSmooth(100);
CHECK(pelzanie == 20 + BATT_STEP_UP,
      "bez zerowania wzrost ograniczony do %d punktu (%d)", BATT_STEP_UP, pelzanie);
resetBatteryFilter();
CHECK(battSmooth(100) == 100, "po wykryciu ladowania wskazanie skacze od razu");

/* Granice skali. */
resetBatteryFilter();
battSmooth(4);
CHECK(battSmooth(0) == 0, "nie schodzi ponizej zera");
resetBatteryFilter();
battSmooth(97);
CHECK(battSmooth(100) == 97, "male roznice przy pelnej baterii tez sa szumem");
resetBatteryFilter();
battSmooth(100);
CHECK(battSmooth(100) == 100, "nie przekracza stu");

/* progi ochronne z config.h musza byc spojne */
head("Spojnosc progow ochronnych");
CHECK(BATT_CUTOFF_V < BATT_SAFE_V, "cutoff %.2f < safe %.2f", BATT_CUTOFF_V, BATT_SAFE_V);
CHECK(BATT_SAFE_V < BATT_RECOVER_V, "safe %.2f < recover %.2f", BATT_SAFE_V, BATT_RECOVER_V);
CHECK(BATT_CRIT_PCT < BATT_WARN_PCT, "krytyczny %d%% < ostrzegawczy %d%%", BATT_CRIT_PCT, BATT_WARN_PCT);
CHECK(RETRY_BASE_S < RETRY_MAX_S, "backoff rosnie");
CHECK(SNOOZE_S * MAX_ALARM_RETRIES < 86400, "drzemki miesza sie w dobie");
CHECK(MATCH_WINDOW_MIN <= 720, "okno dopasowania nie przekracza pol doby");


/* ================= 10. BEZPIECZNIK CZASOWY ================= */
head("Ruchomy limit czuwania");
awakeDeadlineMs = AWAKE_LIMIT_MS;
FAKE_MILLIS = 0;
CHECK(!awakeTooLong(), "start: limit nieprzekroczony");
FAKE_MILLIS = AWAKE_LIMIT_MS - 1000;
CHECK(!awakeTooLong(), "tuz przed limitem jeszcze pracujemy");
FAKE_MILLIS = AWAKE_LIMIT_MS + 1;
CHECK(awakeTooLong(), "po limicie przerywamy");

/* Scenariusz: parowanie BLE trwalo dluzej niz limit czuwania. */
awakeDeadlineMs = AWAKE_LIMIT_MS;
FAKE_MILLIS = 170000;                       /* 170 s - uzytkownik wpisywal haslo */
CHECK(awakeTooLong(), "przed korekta: limit przekroczony (WiFi by odmowilo)");
extendAwake(90000);
CHECK(!awakeTooLong(), "po extendAwake jest czas na synchronizacje");
CHECK(awakeDeadlineMs == 260000u, "nowy limit = %lu ms", (unsigned long)awakeDeadlineMs);
FAKE_MILLIS = 261000;
CHECK(awakeTooLong(), "przedluzenie tez ma koniec");

/* Scenariusz: alarm dzwonil pelne 120 s, potem wysylka. */
awakeDeadlineMs = AWAKE_LIMIT_MS;
FAKE_MILLIS = 121000;
extendAwake(60000);
CHECK(!awakeTooLong(), "po alarmie zostaje czas na wyslanie dawki");
CHECK(awakeDeadlineMs == 181000u, "limit po alarmie = %lu ms", (unsigned long)awakeDeadlineMs);

/* extendAwake nigdy nie SKRACA juz przyznanego czasu. */
awakeDeadlineMs = 300000;
FAKE_MILLIS = 10000;
extendAwake(5000);
CHECK(awakeDeadlineMs == 300000u, "krotsze przedluzenie nie cofa limitu (%lu)",
      (unsigned long)awakeDeadlineMs);
FAKE_MILLIS = 0;

head("Spojnosc limitow czasowych");
CHECK((uint32_t)PORTAL_TIMEOUT_S * 1000UL > AWAKE_LIMIT_MS,
      "portal WiFi (%d s) przekracza bazowy limit (%d ms) - dlatego limit MUSI byc ruchomy",
      PORTAL_TIMEOUT_S, AWAKE_LIMIT_MS);
CHECK(strlen(AP_PASS) >= 8, "haslo punktu dostepowego ma min. 8 znakow (wymog WPA2): %d",
      (int)strlen(AP_PASS));
CHECK(strlen(AP_SSID) > 0 && strlen(AP_SSID) <= 32, "nazwa sieci miesci sie w limicie SSID");
CHECK((uint32_t)ALARM_WINDOW_S * 1000UL + WIFI_TIMEOUT_MS < AWAKE_LIMIT_MS + 60000UL,
      "alarm + polaczenie miesci sie w przedluzonym limicie");

/* ================= 12. DOBA LEKOWA KONCZY SIE NAD RANEM ================= */
head("Tabletka wzieta po polnocy liczy sie do dnia poprzedniego");
CHECK(DAY_START_HOUR >= 0 && DAY_START_HOUR <= 6,
      "granica doby w rozsadnym zakresie (%d)", DAY_START_HOUR);

CHECK(localDayNumber(local(2026,8,1,20,0)) == 20260801u,
      "20:00 -> %u (normalna pora)", localDayNumber(local(2026,8,1,20,0)));
CHECK(localDayNumber(local(2026,8,1,23,59)) == 20260801u,
      "23:59 -> %u (jeszcze ten sam dzien)", localDayNumber(local(2026,8,1,23,59)));
CHECK(localDayNumber(local(2026,8,2,0,10)) == 20260801u,
      "00:10 -> %u (to WCIAZ dawka z 1 sierpnia)", localDayNumber(local(2026,8,2,0,10)));
CHECK(localDayNumber(local(2026,8,2,2,0)) == 20260801u,
      "02:00 -> %u (przypadek z zycia: wziete przed snem)",
      localDayNumber(local(2026,8,2,2,0)));
CHECK(localDayNumber(local(2026,8,2,2,59)) == 20260801u,
      "02:59 -> %u (ostatnia minuta starej doby)", localDayNumber(local(2026,8,2,2,59)));
CHECK(localDayNumber(local(2026,8,2,3,0)) == 20260802u,
      "03:00 -> %u (nowa doba)", localDayNumber(local(2026,8,2,3,0)));
CHECK(localDayNumber(local(2026,8,2,8,0)) == 20260802u,
      "08:00 -> %u (rano to juz nowy dzien)", localDayNumber(local(2026,8,2,8,0)));

/* Najwazniejszy skutek: dwa otwarcia - o 20:00 i o 02:00 nastepnej doby -
   NIE moga wyladowac w dwoch roznych dniach, bo to ta sama dawka.        */
CHECK(localDayNumber(local(2026,8,1,20,0)) == localDayNumber(local(2026,8,2,1,0)),
      "20:00 i 01:00 nastepnego dnia to ta sama doba lekowa");

/* ...ale prawdziwie kolejna dawka juz sie rozroznia. */
CHECK(localDayNumber(local(2026,8,2,2,0)) != localDayNumber(local(2026,8,2,20,0)),
      "02:00 i 20:00 tego samego dnia to JUZ dwie rozne doby");

/* Granica przetrwa zmiane miesiaca i roku. */
CHECK(localDayNumber(local(2026,9,1,1,0)) == 20260831u,
      "1 wrzesnia 01:00 -> %u (koniec sierpnia)", localDayNumber(local(2026,9,1,1,0)));
CHECK(localDayNumber(local(2027,1,1,1,0)) == 20261231u,
      "Nowy Rok 01:00 -> %u (sylwester)", localDayNumber(local(2027,1,1,1,0)));

/* ================= 13. PUDELKO ZOSTAWIONE OTWARTE ================= */
head("Ostrzeganie o otwartym wieczku");
rtcTimeValid = true;
FAKE_NOW = local(2026,8,1,20,0);

/* Pierwsze wykrycie: cisza, tylko zapamietanie momentu. */
resetOpenState(true);
CHECK(trackBoxOpen() == false, "swieze otwarcie nie pika od razu");
CHECK(rtcOpenSinceTs == (uint32_t)FAKE_NOW, "zapamietany moment otwarcia");
CHECK(rtcNextWarnTs == (uint32_t)FAKE_NOW + OPEN_WARN_FIRST_S,
      "pierwszy sygnal zaplanowany za %d s", OPEN_WARN_FIRST_S);
CHECK(FAKE_BEEPS == 0, "brak sygnalu (%d)", FAKE_BEEPS);

/* Zamkniete w miedzyczasie - nic sie nie dzieje. */
FAKE_BOX_OPEN = false;
CHECK(trackBoxOpen() == false, "zamkniete w ciagu kwadransa: cisza");
CHECK(rtcOpenSinceTs == 0 && rtcOpenWarnCount == 0, "stan alarmowy wyczyszczony");
CHECK(FAKE_BEEPS == 0, "nadal bez sygnalu");

/* Kwadrans minal, wieczko nadal otwarte -> pierwszy sygnal. */
resetOpenState(true);
trackBoxOpen();                                    // wykrycie otwarcia
FAKE_NOW += OPEN_WARN_FIRST_S - 1;
CHECK(trackBoxOpen() == false, "sekunde przed czasem jeszcze cisza");
FAKE_NOW += 1;
CHECK(trackBoxOpen() == true, "po %d min pierwszy sygnal", OPEN_WARN_FIRST_S/60);
CHECK(FAKE_BEEPS == 1, "dokladnie jedno piknięcie (%d)", FAKE_BEEPS);
CHECK(rtcNextWarnTs == (uint32_t)FAKE_NOW + OPEN_WARN_REPEAT_S,
      "nastepne przypomnienie za %d min", OPEN_WARN_REPEAT_S/60);

/* Powtorki co 30 minut. */
FAKE_NOW += OPEN_WARN_REPEAT_S / 2;
CHECK(trackBoxOpen() == false, "w polowie przerwy nie zawraca glowy");
CHECK(FAKE_BEEPS == 1, "wciaz jedno piknięcie (%d)", FAKE_BEEPS);
FAKE_NOW += OPEN_WARN_REPEAT_S / 2;
CHECK(trackBoxOpen() == true, "po 30 min druga przypominajka");
CHECK(FAKE_BEEPS == 2, "dwa piknięcia (%d)", FAKE_BEEPS);

/* Zamkniecie po zgloszeniu do aplikacji ma zostawic slad do odwolania. */
rtcOpenReported = true;
FAKE_BOX_OPEN = false;
trackBoxOpen();
CHECK(rtcOpenClearPend == true, "po zamknieciu aplikacja dostanie odwolanie alarmu");
CHECK(rtcOpenSinceTs == 0 && rtcOpenWarnCount == 0 && rtcOpenReported == false,
      "reszta stanu wyzerowana");

/* Zaciety kontaktron: po OPEN_WARN_MAX sygnalach milkniemy. */
resetOpenState(true);
trackBoxOpen();
for (int i = 0; i < OPEN_WARN_MAX + 5; i++){
  FAKE_NOW += (i == 0 ? OPEN_WARN_FIRST_S : OPEN_WARN_REPEAT_S);
  trackBoxOpen();
}
CHECK(FAKE_BEEPS == OPEN_WARN_MAX,
      "po %d sygnalach cisza, mimo dalszych sprawdzen (%d)", OPEN_WARN_MAX, FAKE_BEEPS);
CHECK(rtcOpenWarnCount > OPEN_WARN_MAX, "ale licznik dalej rosnie (%u)", rtcOpenWarnCount);
FAKE_BOX_OPEN = false;
trackBoxOpen();
CHECK(rtcOpenWarnCount == 0, "zamkniecie kasuje takze tryb cichy");

/* Bez zegara (po padzie baterii) odmierzamy dlugoscia snu. */
resetOpenState(true);
rtcTimeValid = false;
CHECK(trackBoxOpen() == false, "bez zegara: pierwsze wykrycie nadal ciche");
CHECK(rtcOpenSinceTs == 1, "znacznik 'otwarte, czas nieznany'");
wakeReason = WAKE_TIMER;
CHECK(trackBoxOpen() == true, "kolejne wybudzenie z timera -> sygnal");
wakeReason = WAKE_REED;
CHECK(trackBoxOpen() == false, "wybudzenie z kontaktronu nie jest odmierzeniem czasu");
rtcTimeValid = true;

/* ═══════════ CZARNA SKRZYNKA — poprawnosc JSON ═══════════ */
head("Czarna skrzynka");

prefs.wipe();
FAKE_NOW = local(2026,8,6,12,0);
rtcTimeValid = true;
wakeReason = WAKE_REED;
batteryPercentage = 77;

CHECK(logbookJson() == "[]", "pusty dziennik to poprawna pusta tablica: %s",
      logbookJson().c_str());

/* --- B22: czarna skrzynka nie zapisala ANI JEDNEJ linijki --------------
   Zgloszone przez Kube po wgraniu 1.28.1: "Pudelko zglasza utrate danych,
   3 zapisow sie nie udalo", a Historia pudelka pusta.

   Preferences to JEDEN globalny obiekt z JEDNYM uchwytem. begin() na juz
   otwartym uchwycie zwraca false i nic nie robi, ale end() tego
   zagniezdzonego wywolania ZAMYKA uchwyt funkcji nadrzednej.

   logbookAdd() otwieral prefs, a potem w tej samej instrukcji snprintf
   wolal queueCount() - ktore samo robi begin()/end(). Po powrocie uchwyt
   byl juz zamkniety, wiec nvsPutStr() ponizej zawodzil ZA KAZDYM RAZEM.
   Efekt: dziennik zawsze pusty, licznik nvsFail rosnacy co wybudzenie,
   a pushStatus() nadpisywal historie w bazie pusta tablica.

   Blad istnial takze przed naprawa B5 - tylko byl wtedy CICHY, bo nikt
   nie patrzyl na wynik putString(). D14 zadzialalo dokladnie tak, jak
   mialo: zamienilo ciche znikanie danych w widoczny licznik.          */
{
  prefs.wipe();
  prefs.strict = true;                 // NVS: zapis bez uchwytu nie przechodzi
  rtcNvsFail = 0;
  queuePush(makeRecordAt("open", 0, 1750000000u));   // zeby queueCount() != 0
  rtcNvsFail = 0;
  prefs.zagniezdzoneBegin = 0;

  logbookAdd("wyslane");
  CHECK(prefs.zagniezdzoneBegin == 0,
        "logbookAdd() nie otwiera prefs drugi raz w srodku wlasnego bloku (%d)",
        prefs.zagniezdzoneBegin);
  CHECK(rtcNvsFail == 0, "i zaden zapis nie przepada (nvsFail=%u)", rtcNvsFail);
  CHECK(logbookJson().indexOf("wyslane") >= 0,
        "wpis NAPRAWDE trafia do dziennika: %s", logbookJson().c_str());
  /* Liczba w kolejce musi byc prawdziwa, a nie zerem "bo tak wyszlo". */
  CHECK(logbookJson().indexOf("|1") >= 0,
        "i niesie stan kolejki: %s", logbookJson().c_str());
  prefs.strict = false;
}

prefs.wipe();
FAKE_NOW = local(2026,8,6,12,0);
rtcTimeValid = true;
wakeReason = WAKE_REED;
batteryPercentage = 77;

logbookAdd("wyslane");
{
  String j = logbookJson();
  CHECK(j.indexOf("wyslane") >= 0, "wpis trafil do dziennika: %s", j.c_str());
  CHECK(j.charAt(0) == '[' && j.charAt(j.length()-1) == ']', "to tablica JSON");
}

/* Rutyna NIE moze zajmowac miejsca - inaczej jedna noc bez sieci wypycha
   z historii wszystko, co bylo wczesniej.                                */
{
  int przed = logbookJson().length();
  logbookAdd("-");
  logbookAdd("meldunek OK");
  logbookAdd("wieczko zamkniete");
  logbookAdd("drgniecie styku");
  CHECK(logbookJson().length() == przed, "rutynowe wpisy nie zajmuja miejsca");
}

/* TU BYLA MINA: sklejanie bez ucieczki znakow. Jeden cudzyslow w notatce
   zamienial CALY dziennik w niepoprawny JSON i historia znikala w calosci. */
prefs.wipe();
logbookAdd("ma \"cudzyslow\" i \\ backslash");
{
  String j = logbookJson();
  CHECK(j.indexOf("\\\"") >= 0, "cudzyslow uciekniety: %s", j.c_str());
  CHECK(j.indexOf("\\\\") >= 0, "backslash uciekniety");
  /* Policz cudzyslowy NIEpoprzedzone backslashem - musza byc dokladnie dwa,
     czyli otwierajacy i zamykajacy jedyny element tablicy.                */
  int gole = 0;
  for (int i = 0; i < j.length(); i++)
    if (j.charAt(i) == '"' && (i == 0 || j.charAt(i-1) != '\\')) gole++;
  CHECK(gole == 2, "dokladnie dwa gole cudzyslowy = poprawny JSON (%d)", gole);
}

prefs.wipe();
resetOpenState(false);
rtcTimeValid = true;

/* ═══════════ DZIENNIK WIECZKA (test terenowy) ═══════════ */
head("Dziennik wieczka");

prefs.wipe();
resetOpenState(false);
FAKE_NOW = local(2026,8,6,9,0);
CHECK(lidLogCount() == 0, "na start dziennik pusty");

/* Otwarcie i zamkniecie zapisuja sie jako dwa osobne przejscia. */
FAKE_BOX_OPEN = true;  trackBoxOpen();
CHECK(lidLogCount() == 1, "otwarcie zapisane (%u)", lidLogCount());
FAKE_BOX_OPEN = false; trackBoxOpen();
CHECK(lidLogCount() == 2, "zamkniecie zapisane (%u)", lidLogCount());

/* KLUCZOWE: kolejne wybudzenia BEZ zmiany stanu nie moga smiecic.
   Pudelko budzi sie co kilka minut - gdyby kazde wybudzenie dopisywalo
   wpis, dziennik zapelnilby sie w godzine i nie powiedzialby nic.     */
for (int i = 0; i < 10; i++) trackBoxOpen();
CHECK(lidLogCount() == 2, "10 wybudzen przy zamknietym wieczku nic nie dopisuje (%u)",
      lidLogCount());
FAKE_BOX_OPEN = true; trackBoxOpen();
for (int i = 0; i < 10; i++) { FAKE_NOW += 60; trackBoxOpen(); }
CHECK(lidLogCount() == 3, "przy otwartym wieczku tez tylko jedno przejscie (%u)",
      lidLogCount());

/* Dziennik NIE MOZE ruszac kolejki dawek - to byl caly powod, dla ktorego
   ma wlasny bufor zamiast korzystac z queuePush().                      */
prefs.wipe();
resetOpenState(false);
queuePush("100;open;80;4.0;0");
uint16_t dawekPrzed = queueCount();
for (int i = 0; i < 40; i++) {
  FAKE_BOX_OPEN = !FAKE_BOX_OPEN; FAKE_NOW += 30; trackBoxOpen();
}
CHECK(queueCount() == dawekPrzed,
      "40 ruchow wieczka nie tyka kolejki dawek (%u == %u)", queueCount(), dawekPrzed);
CHECK(lidLogCount() > 0, "ale dziennik wieczka je zapisal (%u)", lidLogCount());

/* Przepelnienie: zostawiamy najstarsze, liczymy zgubione. */
prefs.wipe();
resetOpenState(false);
for (int i = 0; i < LIDLOG_SLOTS + 20; i++) {
  FAKE_BOX_OPEN = !FAKE_BOX_OPEN; FAKE_NOW += 30; trackBoxOpen();
}
CHECK(lidLogCount() == LIDLOG_SLOTS,
      "dziennik zatrzymuje sie na %d wpisach (%u)", LIDLOG_SLOTS, lidLogCount());
{
  String j = lidLogJson();
  CHECK(j.indexOf("\"zgubione\":20") >= 0,
        "policzone dokladnie 20 zgubionych: %s", j.substring(0, 24).c_str());
  CHECK(j.indexOf("\"wpisy\":[") >= 0, "JSON ma liste wpisow");
}

/* Kasowanie po potwierdzonej wysylce. */
lidLogClear();
CHECK(lidLogCount() == 0, "po wyslaniu dziennik pusty (%u)", lidLogCount());
{
  String j = lidLogJson();
  CHECK(j.indexOf("\"zgubione\":0") >= 0, "licznik zgubionych tez wyzerowany");
}
prefs.wipe();
resetOpenState(false);
rtcTimeValid = true;

head("Ile spac, gdy wieczko jest otwarte");
resetOpenState(true);
CHECK(openWarnSecondsLeft() == OPEN_WARN_FIRST_S,
      "swiezo otwarte -> %u s", openWarnSecondsLeft());
FAKE_NOW = local(2026,8,1,20,0);
trackBoxOpen();
FAKE_NOW += 600;
CHECK(openWarnSecondsLeft() == (uint32_t)(OPEN_WARN_FIRST_S - 600),
      "po 10 min zostalo %u s", openWarnSecondsLeft());
FAKE_NOW += OPEN_WARN_FIRST_S;                     // termin dawno minal
CHECK(openWarnSecondsLeft() == 5u,
      "spozniony sygnal -> krotka drzemka %u s zamiast liczby ujemnej",
      openWarnSecondsLeft());
CHECK(OPEN_WARN_REPEAT_S <= HOUSEKEEP_MAX_S,
      "przerwa miedzy sygnalami nie moze byc dluzsza niz maksymalny sen");
CHECK(OPEN_WARN_MS < 2000, "sygnal ma byc krotki, nie syrena (%d ms)", OPEN_WARN_MS);

/* ================= 14. CO TRAFIA DO HISTORII ================= */
head("Historia pudelka zapisuje tylko rzeczy istotne");
/* Historia ma 32 pozycje. Gdyby wpadaly do niej rutynowe meldunki, jedna
   noc bez sieci wypchnelaby z niej wszystko, co bylo wczesniej - czyli
   dokladnie to, po co ta historia istnieje.                            */
CHECK(!wartoZapisac("-"),                 "puste wybudzenie pomijane");
CHECK(!wartoZapisac("meldunek OK"),       "rutynowy meldunek pomijany");
CHECK(!wartoZapisac("wieczko zamkniete"), "zamkniecie wieczka pomijane");
CHECK(!wartoZapisac("drgniecie styku"),   "odfiltrowane drganie pomijane");

CHECK(wartoZapisac("wyslane"),            "zapisana dawka ZOSTAJE");
CHECK(wartoZapisac("juz dzis brane"),     "ostrzezenie o drugiej dawce ZOSTAJE");
CHECK(wartoZapisac("brak wifi"),          "brak sieci ZOSTAJE");
CHECK(wartoZapisac("baza nie odpowiada"), "awaria bazy ZOSTAJE");
CHECK(wartoZapisac("meldunek bez sieci"), "nieudany meldunek ZOSTAJE");
CHECK(wartoZapisac("dawka pominieta"),    "pominieta dawka ZOSTAJE");
CHECK(wartoZapisac("bateria odcieta"),    "odciecie baterii ZOSTAJE");
CHECK(wartoZapisac("zostawione otwarte"), "zostawione wieczko ZOSTAJE");
CHECK(wartoZapisac("portal WiFi"),        "wejscie w konfiguracje ZOSTAJE");
CHECK(wartoZapisac("uruchomienie"),       "restart ZOSTAJE (moze byc nieoczekiwany)");

CHECK(REPORT_BOOT_EVENT == 0,
      "restart nie trafia do listy zdarzen - ta sluzy do odtwarzania DAWEK");

  /* ═══════════ KOREKTA ZEGARA ═══════════
     NTP potrafi przesunac zegar o godziny. Kolejka byla poprawiana od
     dawna - ale znaczniki w pamieci RTC nie, a to na nich opiera sie
     termin sygnalu o otwartym wieczku.                                */
  head("Korekta zegara obejmuje wszystkie znaczniki");
  {
    rtcLastOpenTs  = 1000000;
    rtcOpenSinceTs = 2000000;
    rtcNextWarnTs  = 2000900;
    rtcLastPushTs  = 0;                    // 0 = nieustawione, ma tak zostac
    rtcTokenExp    = 2003600;

    przesunZnaczniki(7200);
    CHECK(rtcLastOpenTs  == 1007200, "znacznik drgania przesuniety (%lu)", (unsigned long)rtcLastOpenTs);
    CHECK(rtcOpenSinceTs == 2007200, "moment otwarcia przesuniety");
    CHECK(rtcNextWarnTs  == 2008100, "termin sygnalu przesuniety - inaczej pikaloby bez powodu");
    CHECK(rtcTokenExp    == 2010800, "waznosc tokenu przesunieta");
    CHECK(rtcLastPushTs  == 0, "nieustawiony znacznik zostaje nieustawiony");

    /* Odstep miedzy otwarciem a sygnalem musi byc NIENARUSZONY - to on
       decyduje, kiedy uslyszysz przypomnienie o wieczku.              */
    CHECK(rtcNextWarnTs - rtcOpenSinceTs == 900,
          "odstep do sygnalu zachowany (%lu)", (unsigned long)(rtcNextWarnTs - rtcOpenSinceTs));

    przesunZnaczniki(-7200);
    CHECK(rtcNextWarnTs == 2000900, "cofniecie zegara tez dziala");

    /* Absurdalne cofniecie nie moze dac ujemnego znacznika. */
    przesunZnaczniki(-2000000000);
    CHECK(rtcNextWarnTs == 0 && rtcOpenSinceTs == 0,
          "znaczniki nie schodza ponizej zera");
  }

  /* ═══════════ WYKRYWANIE LADOWARKI ═══════════
     Aplikacja ma pokazywac rosnacy procent na zywo, wiec potrzebny jest
     STAN trwajacy miedzy wybudzeniami, a nie pojedynczy skok napiecia. */
  head("Ladowarka: wejscie, plateau, odlaczenie, bezpiecznik");
  {
    rtcCharging = false; rtcVoltMax = 0.0f; rtcChargeIdle = 0;

    CHECK(!trackCharging(3.80f, 3.81f), "samo rozladowywanie to nie ladowanie");
    CHECK(!trackCharging(3.80f, 0.0f),  "pierwszy pomiar bez historii nie zaklada ladowania");

    /* Podlaczenie kabla - napiecie rosnie. */
    CHECK(trackCharging(3.90f, 3.80f), "wzrost napiecia = ladowarka podlaczona");
    CHECK(trackCharging(4.00f, 3.90f), "stan trwa przy dalszym wzroscie");
    CHECK(trackCharging(4.10f, 4.00f), "i dalej");

    /* Plateau przy pelnym ogniwie - kabel wciaz tkwi. */
    for (int i = 0; i < CHARGE_IDLE_MAX + 20; i++) {
      trackCharging(4.19f, 4.19f);
    }
    CHECK(rtcCharging, "pelne ogniwo na kablu nadal liczy sie jako ladowanie");

    /* Wyciagniecie kabla - ogniwo pod obciazeniem od razu siada. */
    CHECK(!trackCharging(4.19f - CHARGE_DROP_V - 0.01f, 4.19f),
          "spadek wzgledem szczytu = kabel wyciagniety");
    CHECK(!rtcCharging, "i stan zostaje wygaszony");
  }

  head("Ladowarka podpieta, zanim pudelko wstalo");
  {
    /* Najczestszy przypadek z zycia: wgrywasz firmware przy podpietym
       kablu. Pamiec RTC sie kasuje, wiec pierwszy pomiar to juz napiecie
       ladowarki - wzrostu nie bedzie NIGDY.                            */
    rtcCharging = false; rtcVoltMax = 0.0f; rtcChargeIdle = 0;
    rtcWysokieZRzedu = 0; rtcBlokWysokie = false;

    CHECK(!trackCharging(4.19f, 0.0f), "pierwszy wysoki odczyt to jeszcze nie dowod");
    CHECK(trackCharging(4.19f, 4.19f), "drugi wysoki z rzedu = kabel podpiety");

    /* Odpiecie: napiecie spada wzgledem szczytu. */
    CHECK(!trackCharging(4.19f - CHARGE_DROP_V - 0.01f, 4.19f), "odpiecie wykryte");
    /* I nie wolno wskoczyc z powrotem tylko dlatego, ze napiecie
       wciaz jest wysokie - inaczej tryb migalby w kolko.            */
    bool wrocilo = false;
    for (int i = 0; i < 5; i++) if (trackCharging(4.16f, 4.16f)) wrocilo = true;
    CHECK(!wrocilo, "po odpieciu nie wracamy do ladowania na samym napieciu");
    /* Blokada puszcza dopiero, gdy napiecie realnie opadnie. */
    trackCharging(4.00f, 4.05f);
    CHECK(!trackCharging(4.19f, 4.00f) || rtcCharging,
          "po opadnieciu napiecia wykrywanie znow dziala");
  }

  head("Ladowarka: bezpiecznik przed zaklinowaniem");
  {
    rtcCharging = false; rtcVoltMax = 0.0f; rtcChargeIdle = 0;
    CHECK(trackCharging(3.90f, 3.80f), "wchodzimy w tryb ladowania");
    /* Napiecie stoi w miejscu i jest DALEKO od pelnego - to nie plateau,
       tylko zaklinowane wykrycie. Po CHARGE_IDLE_MAX pomiarach konczymy,
       zeby nie budzic sie co minute az do rozladowania ogniwa.        */
    bool wyszlo = false;
    for (int i = 0; i < CHARGE_IDLE_MAX + 5; i++) {
      if (!trackCharging(3.90f, 3.90f)) { wyszlo = true; break; }
    }
    CHECK(wyszlo, "stojace napiecie ponizej pelnego konczy tryb ladowania");
    CHECK(!rtcCharging, "bezpiecznik zadzialal");
  }

  head("Ladowarka: brak ogniwa");
  {
    rtcCharging = true; rtcVoltMax = 4.2f;
    CHECK(!trackCharging(0.0f, 4.20f), "pomiar bez sensu nie udaje ladowania");
    CHECK(rtcVoltMax == 0.0f, "i kasuje zapamietany szczyt");
  }

/* ========== 16b. DOMYKANIE ZALEGLYCH DOB PO DLUZSZEJ PRZERWIE =======
   Wczesniej powstawal DOKLADNIE JEDEN wpis "missed", ze znacznikiem
   wczoraj 23:59, a rtcRolloverDay skakal od razu na dzis. Po kilku dniach
   bez zasilania dni posrednie nie dostawaly nic - w kalendarzu wygladaly
   jak "brak danych" zamiast "nie wziete", czyli odwrotnie niz naprawde. */
head("Zalegle doby domykane po dluzszej przerwie");
{
  prefs.wipe();
  rtcTimeValid = true; rtcTzOffsetMin = 120;
  parseSchedule("20:00");
  batteryPercentage = 80; realBatteryVoltage = 4.0f;

  /* rtcRolloverDay = 1 sierpnia znaczy "wszystko PRZED 1 sierpnia jest
     rozliczone, a 1 sierpnia wlasnie trwa". Budzimy sie 6 sierpnia rano,
     wiec skonczyly sie doby 1, 2, 3, 4 i 5 sierpnia - piec sztuk.
     Dawek nie bylo zadnych, wiec kazda z nich to pominieta dawka.   */
  rtcRolloverDay = 20260801; rtcTakenDay = 0;
  FAKE_NOW = local(2026, 8, 6, 3, 1);
  checkDayRollover();

  std::set<unsigned long> doby;
  String rec;
  int guard = 0;
  while (queuePeek(rec) && guard++ < 40) {
    unsigned long ts = (unsigned long)rec.substring(0, rec.indexOf(';')).toInt();
    doby.insert((unsigned long)localDayNumber((time_t)ts));
    queuePop();
  }
  CHECK(doby.size() == 5, "piec zaleglych dob domknietych, nie jedna (%zu)", doby.size());
  CHECK(doby.count(20260801) && doby.count(20260802) && doby.count(20260803) &&
        doby.count(20260804) && doby.count(20260805),
        "i sa to dokladnie doby 1-5 sierpnia");
  CHECK(!doby.count(20260731), "doba sprzed znacznika nie wraca");
  CHECK(!doby.count(20260806), "biezaca doba jeszcze sie nie skonczyla");
  CHECK(rtcRolloverDay == 20260806u, "znacznik przestawiony na dzis (%lu)",
        (unsigned long)rtcRolloverDay);

  /* Doba, w ktorej dawka JEDNAK byla, nie moze trafic do pominietych. */
  prefs.wipe();
  rtcRolloverDay = 20260801; rtcTakenDay = 20260803;
  FAKE_NOW = local(2026, 8, 5, 3, 1);
  checkDayRollover();
  doby.clear(); guard = 0;
  while (queuePeek(rec) && guard++ < 40) {
    unsigned long ts = (unsigned long)rec.substring(0, rec.indexOf(';')).toInt();
    doby.insert((unsigned long)localDayNumber((time_t)ts));
    queuePop();
  }
  CHECK(!doby.count(20260803), "doba z wzieta dawka NIE jest oznaczana jako pominieta");
  CHECK(doby.count(20260802) && doby.count(20260804),
        "ale sasiednie puste doby juz tak");

  /* Bardzo dluga przerwa nie moze wypchnac z kolejki prawdziwych dawek. */
  prefs.wipe();
  rtcRolloverDay = 20250101; rtcTakenDay = 0;
  FAKE_NOW = local(2026, 8, 6, 3, 1);
  checkDayRollover();
  CHECK(queueCount() <= MAX_ROLLOVER_DNI,
        "miesiace przerwy daja najwyzej %d wpisow, a nie setki (%u)",
        MAX_ROLLOVER_DNI, queueCount());

  /* Zwykly przypadek: jedna doba, jeden wpis - bez regresji. */
  prefs.wipe();
  rtcRolloverDay = 20260805; rtcTakenDay = 0;
  FAKE_NOW = local(2026, 8, 6, 3, 1);
  checkDayRollover();
  CHECK(queueCount() == 1, "pojedyncza zalegla doba to dalej jeden wpis (%u)", queueCount());
  queuePeek(rec);
  CHECK(localDayNumber((time_t)(unsigned long)rec.substring(0, rec.indexOf(';')).toInt())
        == 20260805u, "i dotyczy wlasciwej doby");
  prefs.wipe();
}

head("Dziennik wieczka jest escapowany tak samo jak czarna skrzynka");
{
  prefs.wipe();
  /* Wpis z cudzyslowem nie moze rozwalic JSON-a. Dzis tresc lepi
     snprintf, wiec cudzyslow sie tam nie znajdzie - ale blizniaczy
     logbookJson() jest zabezpieczony i te dwa maja byc symetryczne. */
  prefs.putString("ll0", "1750000000;1;\"reed\"");
  prefs.putUShort("llCnt", 1);
  String j = lidLogJson();
  CHECK(j.indexOf("\\\"reed\\\"") >= 0,
        "cudzyslow w tresci jest escapowany: %s", j.c_str());
  int cudz = 0;
  for (int i = 0; i < j.length(); i++)
    if (j.charAt(i) == '"' && (i == 0 || j.charAt(i-1) != '\\')) cudz++;
  CHECK(cudz % 2 == 0, "liczba niezaescapowanych cudzyslowow jest parzysta (%d)", cudz);
  prefs.wipe();
}

/* ====== 16b2. ALARM NIE MOZE DZWONIC W KOLKO PRZEZ TRZY GODZINY ======
   Zglosil Kuba z wyjazdu: "pika juz 3 raz od 18:30, o 19:30 tez".

   Po wyczerpaniu prob alarm zapisywal "missed" i czyscil rtcPendingSlot,
   ale nie zostawial sladu, ze dla tej doby juz zadzwonil. matchSlot()
   dopasowuje pore leku w oknie +/-MATCH_WINDOW_MIN, czyli dla 20:00
   zwraca slot 0 przez CALE trzy godziny (18:30-21:30). Kazde wybudzenie
   w tym oknie zaczynalo alarm od nowa - a bez sieci pudelko budzi sie
   tam wielokrotnie, bo ponawia wysylke kolejki po 15, 30, 60 minutach. */
head("Odzwoniony alarm nie wraca w tym samym oknie dopasowania");
{
  prefs.wipe();
  parseSchedule("20:00");
  rtcTimeValid = true; rtcTzOffsetMin = 120;
  rtcTakenDay = 0; rtcTakenTs = 0;
  rtcAlarmDoneDay = 0; rtcAlarmDoneTs = 0; rtcAlarmDoneMask = 0;

  /* 20:00 - pora leku. Alarm dzwoni i po trzech probach sie poddaje. */
  FAKE_NOW = local(2026, 8, 8, 20, 0);
  CHECK(matchSlot(FAKE_NOW) == 0, "o porze leku alarm ma prawo zadzwonic");
  CHECK(!alarmJuzObsluzony(0), "na starcie nic nie jest jeszcze odzwonione");
  oznaczAlarmObsluzony(0);                   // to robi galaz "pominieta dawka"

  /* Kolejne wybudzenia w oknie +/-90 min - dokladnie te, ktore Kuba slyszal. */
  for (const auto& p : { std::pair<int,int>{20,15}, {21,0}, {21,29} }) {
    FAKE_NOW = local(2026, 8, 8, p.first, p.second);
    CHECK(matchSlot(FAKE_NOW) == 0,
          "%02d:%02d wciaz miesci sie w oknie dopasowania", p.first, p.second);
    CHECK(alarmJuzObsluzony(0),
          "...ale o %02d:%02d alarm juz NIE dzwoni drugi raz", p.first, p.second);
  }

  /* Nowa doba - alarm musi wrocic, inaczej pudelko zamilkloby na zawsze. */
  FAKE_NOW = local(2026, 8, 9, 20, 0);
  CHECK(!alarmJuzObsluzony(0), "nazajutrz alarm znowu dziala");

  /* To samo bez zegara: pudelko offline po resecie. */
  prefs.wipe();
  rtcTimeValid = false; rtcAlarmDoneDay = 0; rtcAlarmDoneTs = 0; rtcAlarmDoneMask = 0;
  FAKE_NOW = 5000;
  CHECK(!alarmJuzObsluzony(0), "bez zegara tez zaczynamy od ciszy");
  oznaczAlarmObsluzony(0);
  FAKE_NOW = 5000 + 3600;
  CHECK(alarmJuzObsluzony(0), "godzine pozniej alarm nie wraca");
  FAKE_NOW = 5000 + ONE_DOSE_WINDOW_S + 60;
  CHECK(!alarmJuzObsluzony(0), "po uplywie okna wraca - to juz nowa doba");
}

/* ====== 16b3. DRUGIE PRZYPOMNIENIE MUSI ZADZWONIC (D56) ==============
   Naprawa D23 zalatala dzwonienie w kolko, ale znacznik postawila na CALA
   DOBE. Skutek: pierwsze przypomnienie, poddajac sie, wyciszalo wszystkie
   nastepne - czyli druga pozycja w harmonogramie ("przypomnij jeszcze raz
   o 23:00") milczala dokladnie wtedy, kiedy jest potrzebna.            */
head("Pierwsze przypomnienie nie wycisza drugiego");
{
  prefs.wipe();
  parseSchedule("20:00|23:00");
  rtcTimeValid = true; rtcTzOffsetMin = 120;
  rtcTakenDay = 0; rtcTakenTs = 0;
  rtcAlarmDoneDay = 0; rtcAlarmDoneTs = 0; rtcAlarmDoneMask = 0;
  CHECK(slotCount == 2, "dwa przypomnienia w harmonogramie (%d)", slotCount);

  /* 20:00 - dzwoni, brak odzewu, wyczerpane proby. */
  FAKE_NOW = local(2026, 8, 8, 20, 0);
  CHECK(matchSlot(FAKE_NOW) == 0, "o 20:00 pasuje slot 0");
  CHECK(!alarmJuzObsluzony(0), "slot 0 jeszcze nie dzwonil");
  oznaczAlarmObsluzony(0);
  CHECK(alarmJuzObsluzony(0), "slot 0 odzwoniony");

  /* Wybudzenia w oknie slotu 0 - cisza, dokladnie jak po D23. */
  FAKE_NOW = local(2026, 8, 8, 21, 0);
  CHECK(matchSlot(FAKE_NOW) == 0 && alarmJuzObsluzony(0),
        "o 21:00 slot 0 nadal milczy (D23 nietkniete)");

  /* 23:00 - TO jest ta zmiana. Drugie przypomnienie MUSI zadzwonic. */
  FAKE_NOW = local(2026, 8, 8, 23, 0);
  CHECK(matchSlot(FAKE_NOW) == 1, "o 23:00 pasuje slot 1 (%d)", matchSlot(FAKE_NOW));
  CHECK(!alarmJuzObsluzony(1), "drugie przypomnienie NIE jest wyciszone przez pierwsze");

  /* Dopiero gdy i ono sie podda - cisza do konca doby. */
  oznaczAlarmObsluzony(1);
  CHECK(alarmJuzObsluzony(1), "po odzwonieniu slot 1 tez milczy");
  CHECK(alarmJuzObsluzony(0), "a slot 0 nadal milczy - maska nie kasuje sasiada");

  /* Nowa doba kasuje CALA maske, nie tylko biezacy bit. */
  FAKE_NOW = local(2026, 8, 9, 20, 0);
  CHECK(!alarmJuzObsluzony(0) && !alarmJuzObsluzony(1),
        "nazajutrz oba przypomnienia wracaja");

  /* Wzieta tabletka wycisza WSZYSTKIE przypomnienia - to osobna ochrona
     (juzDzisBrane), niezalezna od maski, i ma dzialac dalej.          */
  zapiszDawke();
  CHECK(juzDzisBrane(), "dawka zapisana");
  FAKE_NOW = local(2026, 8, 9, 23, 0);
  CHECK(juzDzisBrane(), "o 23:00 pudelko wie, ze dzis juz bylo - drugie tez milczy");
}

head("Ktore przypomnienie jest ostatnia szansa w dobie");
{
  parseSchedule("20:00|23:00");
  CHECK(ostatniSlotDoby() == 1, "z 20:00 i 23:00 ostatnie jest 23:00 (%d)", ostatniSlotDoby());
  parseSchedule("08:00");
  CHECK(ostatniSlotDoby() == 0, "przy jednym przypomnieniu ono samo");
  /* Kolejnosc liczona w ramie doby lekowej: 07:30 nalezy juz do NASTEPNEJ
     doby wzgledem 23:00, wiec ostatnia szansa dzis to 23:00.          */
  parseSchedule("23:00|07:30");
  CHECK(ostatniSlotDoby() == 0,
        "07:30 to juz kolejna doba - ostatnia szansa dzis to 23:00 (%d)", ostatniSlotDoby());
  parseSchedule("07:30|23:00");
  CHECK(ostatniSlotDoby() == 1, "ta sama para w innej kolejnosci daje ten sam wynik");
  slotCount = 0;
  CHECK(ostatniSlotDoby() == -1, "bez harmonogramu nie ma ostatniej szansy");
}

head("Wziecie tabletki tez konczy alarm na te dobe");
{
  prefs.wipe();
  rtcTimeValid = true; rtcTakenDay = 0; rtcTakenTs = 0;
  rtcAlarmDoneDay = 0; rtcAlarmDoneTs = 0; rtcAlarmDoneMask = 0;
  FAKE_NOW = local(2026, 8, 8, 19, 0);
  zapiszDawke();
  oznaczAlarmObsluzony(0);                   // tak robi galaz "otwarto w trakcie"
  FAKE_NOW = local(2026, 8, 8, 20, 0);
  CHECK(juzDzisBrane(), "dawka zapisana");
  CHECK(alarmJuzObsluzony(0), "i alarm o 20:00 juz nie zadzwoni");
}

/* ====== 16c. OCHRONA PRZED DRUGA DAWKA **BEZ INTERNETU** ============
   Zglosil to Kuba z wyjazdu: pudelko bez sieci, wzial tabletke, zamknal
   wieczko, otworzyl ponownie - i CISZA. Zadnego "juz dzis brales".

   Przyczyna: cala ochrona byla bramkowana rtcTimeValid, a to ustawia
   WYLACZNIE syncTimeNTP(). Po resecie (wgranie programu przed wyjazdem,
   zanik zasilania) i bez internetu flaga nigdy nie stawala sie prawda,
   wiec pudelko ani nie ZAPISYWALO wzietej dawki, ani nie mialo na czym
   oprzec ostrzezenia. Ochrona przed podwojna dawka nie istniala.     */
head("Ostrzezenie o drugiej dawce dziala bez internetu");
{
  prefs.wipe();
  parseSchedule("20:00");

  /* Pudelko po wgraniu programu, bez sieci: zegar liczy od zera. */
  rtcTimeValid = false;
  rtcTakenDay = 0; rtcRolloverDay = 0; rtcTakenTs = 0;
  FAKE_NOW = 3600;                            // godzina od startu plytki

  CHECK(!juzDzisBrane(), "przed pierwsza dawka pudelko milczy");

  zapiszDawke();                              // wziete o "3600"
  CHECK(rtcTakenTs != 0, "dawka odnotowana MIMO braku zegara (%lu)",
        (unsigned long)rtcTakenTs);

  FAKE_NOW = 3600 + 30;                       // zamykasz i otwierasz po 30 s
  CHECK(juzDzisBrane(), "TO JEST TEN OBJAW: po ponownym otwarciu pudelko ostrzega");

  FAKE_NOW = 3600 + 4 * 3600;                 // cztery godziny pozniej
  CHECK(juzDzisBrane(), "po czterech godzinach nadal ostrzega");

  FAKE_NOW = 3600 + ONE_DOSE_WINDOW_S - 60;
  CHECK(juzDzisBrane(), "tuz przed koncem okna jeszcze ostrzega");

  FAKE_NOW = 3600 + ONE_DOSE_WINDOW_S + 60;
  CHECK(!juzDzisBrane(),
        "po uplywie okna milknie - to juz nowa doba, dawke trzeba wziac");
}

head("Powrot internetu przypisuje dawke do wlasciwej doby");
{
  prefs.wipe();
  rtcTimeValid = false; rtcTakenDay = 0; rtcTakenTs = 0;
  FAKE_NOW = 7200;
  zapiszDawke();                              // dawka wzieta "na slepo"
  CHECK(rtcTakenDay == 0, "bez zegara numer doby jeszcze nieznany");

  /* Dwie godziny pozniej wraca siec i NTP ustawia prawdziwy czas. */
  const uint32_t temu = 2 * 3600;
  FAKE_NOW = 7200 + temu;                     // stara, nieprawdziwa skala
  time_t prawdziwyTeraz = local(2026, 8, 1, 22, 0);
  /* To samo, co robi syncTimeNTP() przy przejsciu nieznany -> znany. */
  {
    uint32_t roznica = (uint32_t)(FAKE_NOW - (time_t)rtcTakenTs);
    FAKE_NOW = prawdziwyTeraz;
    rtcTimeValid = true;
    time_t kiedy = FAKE_NOW - (time_t)roznica;
    rtcTakenTs = (uint32_t)kiedy;
    setTakenDay(localDayNumber(kiedy));
  }
  CHECK(rtcTakenDay == localDayNumber(local(2026, 8, 1, 20, 0)),
        "dawka sprzed 2 h przypisana do doby, w ktorej naprawde byla (%lu)",
        (unsigned long)rtcTakenDay);
  CHECK(juzDzisBrane(), "i pudelko dalej wie, ze dawka byla");
}

head("Z wiarygodnym zegarem nic sie nie zmienilo");
{
  prefs.wipe();
  rtcTimeValid = true; rtcTakenDay = 0; rtcTakenTs = 0;
  FAKE_NOW = local(2026, 8, 1, 20, 0);
  CHECK(!juzDzisBrane(), "przed dawka milczy");
  zapiszDawke();
  CHECK(rtcTakenDay == 20260801u, "znacznik doby ustawiony (%lu)", (unsigned long)rtcTakenDay);
  FAKE_NOW = local(2026, 8, 1, 23, 30);
  CHECK(juzDzisBrane(), "tego samego wieczora ostrzega");
  FAKE_NOW = local(2026, 8, 2, 2, 0);
  CHECK(juzDzisBrane(), "o 2 w nocy TEZ - to wciaz ta sama doba lekowa");
  FAKE_NOW = local(2026, 8, 2, 4, 0);
  CHECK(!juzDzisBrane(), "po granicy doby (3:00) milknie - czas na nowa dawke");

  /* Zegar wiarygodny, ale znacznika brak - nie wolno ostrzegac na zapas. */
  rtcTakenDay = 0;
  CHECK(!juzDzisBrane(), "brak znacznika doby to nie jest 'juz brane'");
}

/* ================= 17a. POTWIERDZENIE DAWKI (B1) ====================
   Najwrazliwsze miejsce w calym projekcie i do tej pory jedyne bez
   ANI JEDNEGO testu - bo runAlarmWindow() to petla na millis() z
   delay() i odczytem pinu, nie do uruchomienia w tescie. Sama decyzja
   jest teraz osobna funkcja, wiec da sie ja przebadac.

   Testy PRZYPINAJA stan dzisiejszy, razem z bledem B1. Kazdy z trzech
   wariantow naprawy zmieni ktoras z tych linii - i wtedy bedzie widac
   dokladnie, ktore zachowanie sie zmienilo, zamiast wchodzic w alarm
   po omacku.                                                          */
head("Co liczy sie jako potwierdzenie dawki (B1)");
{
  CHECK(alarmPotwierdzony(true,  false),
        "wieczko otwarte w trakcie dzwonienia = dawka wzieta");
  CHECK(!alarmPotwierdzony(false, false),
        "zamkniete przez cale okno = dawka nie potwierdzona");
  CHECK(!alarmPotwierdzony(false, true),
        "zamkniecie wieczka NIE jest dzis potwierdzeniem (to wariant B)");

  /* TO JEST BLAD B1, przypiety swiadomie.
     Pudelko z przesunietym magnesem zglasza "otwarte" bez przerwy.
     Alarm konczy sie potwierdzeniem w pierwszej setnej sekundy, wiec
     w telefonie jest zielone "wziete", a pudelko nawet nie zadzwonilo. */
  CHECK(alarmPotwierdzony(true, true),
        "wieczko otwarte JUZ PRZED alarmem tez potwierdza dawke - "
        "to jest blad B1, nie cecha; zmiana tej linii to wariant A");

  /* Decyzja nie moze zalezec od niczego poza podanym stanem - inaczej
     naprawa B1 zachowywalaby sie roznie przy tych samych wejsciach. */
  FAKE_BOX_OPEN = false;
  CHECK(alarmPotwierdzony(true, false), "wynik nie zalezy od globalnego stanu pinu");
  FAKE_BOX_OPEN = true;
  CHECK(!alarmPotwierdzony(false, false), "ani w druga strone");
  FAKE_BOX_OPEN = false;
  /* Tego, ze petla alarmu naprawde pyta przez te funkcje - a nie omija
     jej bokiem - pilnuje audyt (audit_firmware.py, sekcja B1).        */
}

/* ================= 17b. TOKEN FIREBASE ==============================
   Cale rozumowanie stojace za D13 opiera sie na zalozeniu, ktorego
   nikt nie sprawdzil: "401 to wygasly token, po odswiezeniu przejdzie".
   Jesli token sie NIE odswieza, kazda wysylka wraca 401, flushQueue()
   zwraca false w nieskonczonosc, a zaden licznik sie nie rusza -
   ani dropped, ani nvsFail. Aplikacja nie ma o czym krzyknac.       */
head("Token z pamieci: kiedy wolno mu zaufac");
{
  prefs.wipe();
  idToken = "";
  const String dobryToken = "eyJhbGciOiJSUzI1NiIsImtpZCI6InRoaXNJc0FUb2tlbiJ9";
  prefs.putString("tok", dobryToken);
  FAKE_NOW = 1750000000;

  rtcTimeValid = true; rtcTokenExp = (uint32_t)FAKE_NOW + 3600;
  idToken = "";
  CHECK(tokenZPamieci(), "wazny token z zapasem czasu jest przyjmowany");
  CHECK(idToken.length() > 20, "i naprawde wlazi do zmiennej");

  idToken = "";
  rtcTokenExp = (uint32_t)FAKE_NOW + TOKEN_MARGIN_S - 1;
  CHECK(!tokenZPamieci(),
        "token gasnacy w ciagu marginesu jest odrzucany - inaczej wygasnie "
        "w polowie wysylki i dostaniemy 401");
  CHECK(idToken.length() == 0, "i nie zostawia po sobie smiecia");

  idToken = "";
  rtcTokenExp = (uint32_t)FAKE_NOW - 1;
  CHECK(!tokenZPamieci(), "token juz wygasly jest odrzucany");

  /* Bez wiarygodnego zegara nie da sie ocenic waznosci - tokenZPamieci()
     odmawia. firebaseSignIn() w tej samej sytuacji ufa tokenowi, ktory
     siedzi juz w RAM, wiec martwy token moze przejsc dalej i spalic
     JEDNO zapytanie. Nic wiecej: rtdbSend() na 401/403 wola
     zapomnijToken(), ktory zeruje rtcTokenExp i kasuje kopie z NVS -
     wiec najblizsze firebaseSignIn() loguje sie haslem od nowa.
     Sprawdzamy obie polowki osobno, zeby ta samonaprawa nie zniknela
     niezauwazona.                                                    */
  idToken = "";
  rtcTimeValid = false; rtcTokenExp = (uint32_t)FAKE_NOW + 3600;
  CHECK(!tokenZPamieci(), "bez wiarygodnego zegara token z NVS nie jest uzywany");

  rtcTimeValid = true; rtcTokenExp = 0;
  idToken = "";
  CHECK(!tokenZPamieci(), "brak zapamietanego terminu waznosci = brak zaufania");

  /* Zapomnienie musi czyscic OBIE kopie - RAM i NVS. Zostawiona w NVS
     wrocilaby przy nastepnym wybudzeniu i blad powtorzylby sie w kolko. */
  idToken = dobryToken; rtcTokenExp = 999; rtcTimeValid = true;
  zapomnijToken();
  CHECK(idToken.length() == 0, "zapomnijToken czysci token w pamieci RAM");
  CHECK(rtcTokenExp == 0, "i termin waznosci");
  CHECK(prefs.getString("tok", "") == String(""), "i kopie w NVS");

  /* Korekta zegara po NTP musi ruszyc takze terminem waznosci tokenu -
     inaczej po skoku o godziny token wyglada na wieczny albo na trupa. */
  rtcTokenExp = 1750000000; rtcLastOpenTs = 0; rtcLastPushTs = 0;
  rtcOpenSinceTs = 0; rtcNextWarnTs = 0;
  przesunZnaczniki(3600);
  CHECK(rtcTokenExp == 1750003600u,
        "termin waznosci tokenu przesuwa sie razem z zegarem (%lu)",
        (unsigned long)rtcTokenExp);
}

head("Kolejka pod nieustajacym 401");
{
  /* 401 NIE jest na liscie trwale odrzuconych (D13) - i sluszne, bo po
     odswiezeniu tokenu wpis przejdzie. Tu sprawdzamy druga polowe tej
     umowy: dopoki token nie wroci, NIC nie ma prawa zginac.         */
  prefs.wipe();
  rtcTimeValid = true;
  rtcQueueDropped = 0; rtcNvsFail = 0;
  FAKE_MILLIS = 0; awakeDeadlineMs = AWAKE_LIMIT_MS;
  batteryPercentage = 80; realBatteryVoltage = 4.0f;
  for (int i = 0; i < 5; i++) queuePush(makeRecordAt("open", 0, 1750000000u + i));

  FAKE_HTTP = 401;
  FAKE_SENT.clear();
  CHECK(!flushQueue(), "401 konczy wysylke niepowodzeniem");
  CHECK(queueCount() == 5, "wszystkie dawki zostaja w kolejce (%u)", queueCount());
  CHECK(rtcQueueDropped == 0, "nic nie zostalo skasowane - 401 to nie 400");
  CHECK(FAKE_SENT.size() == 1, "po pierwszym 401 nie probujemy reszty na sile");

  /* Piec wybudzen z rzedu i dalej to samo: zero sladu w licznikach. */
  for (int i = 0; i < 5; i++) flushQueue();
  CHECK(queueCount() == 5, "po pieciu wybudzeniach nadal 5 dawek (%u)", queueCount());
  CHECK(rtcQueueDropped == 0 && rtcNvsFail == 0,
        "i nadal nic nie jest liczone jako strata - bo nic nie stracono");

  /* A po odswiezeniu tokenu wszystko ma przejsc bez straty - to jest
     wlasnie zalozenie, na ktorym stoi decyzja D13.                   */
  FAKE_HTTP = 200;
  CHECK(flushQueue(), "po odzyskaniu tokenu wysylka przechodzi");
  CHECK(queueCount() == 0, "kolejka pusta (%u)", queueCount());
  CHECK(rtcQueueDropped == 0, "i ani jedna dawka nie zostala skasowana po drodze");

  FAKE_HTTP = 200;
  prefs.wipe();
}

/* ================= 18. ILE SPAC (planNextSleep) =====================
   Jedyna funkcja decydujaca o tym, czy pudelko obudzi sie na 20:00.
   Szesc warunkow konkuruje tu ze soba i kazdy potrafi sen skrocic.
   Do tej pory pilnowal jej wylacznie audyt tekstowy - sprawdzal, ze
   pewne napisy sa w zrodle, nie ze wynik jest poprawny.            */
{
  /* Stan wyjsciowy: harmonogram 20:00, poludnie, nic sie nie pali. */
  auto spokoj = [&](int godz, int minuta){
    prefs.wipe();
    parseSchedule("20:00");
    rtcTimeValid = true; rtcTzOffsetMin = 120;
    rtcPendingSlot = -1; rtcAlarmRetries = 0; rtcRetryCount = 0;
    rtcCharging = false; rtcStatusDirty = false; rtcOpenClearPend = false;
    resetOpenState(false);
    rtcOpenReported = false;                 // zgodnie z FAKE_BOX_OPEN
    FAKE_NOW = local(2026,8,1,godz,minuta);
  };

  head("Ile spac: zwykly dzien");
  spokoj(12, 0);
  CHECK(planNextSleep() == 8*3600u - 30u,
        "poludnie -> do dawki minus 30 s zapasu (%lu)", (unsigned long)planNextSleep());
  spokoj(19, 55);
  CHECK(planNextSleep() == 300u - 30u,
        "piec minut przed dawka -> %lu s", (unsigned long)planNextSleep());

  head("Ile spac: drzemka alarmu ma pierwszenstwo");
  spokoj(20, 0);
  rtcPendingSlot = 0; rtcAlarmRetries = 0;
  CHECK(planNextSleep() == (uint32_t)SNOOZE_S, "alarm czeka -> drzemka");
  /* Drzemka bije WSZYSTKO. Gdyby ladowarka albo kolejka potrafily ja
     skrocic, pudelko wrociloby do dzwonienia szybciej niz co 5 min i
     zjadloby okno alarmu na darmo.                                   */
  rtcCharging = true; rtcStatusDirty = true;
  for (int i = 0; i < 3; i++) queuePush(makeRecordAt("open", 0, 1750000000u + i));
  CHECK(planNextSleep() == (uint32_t)SNOOZE_S,
        "ladowarka ani kolejka nie skracaja drzemki (%lu)", (unsigned long)planNextSleep());

  spokoj(20, 0);
  rtcPendingSlot = 0; rtcAlarmRetries = MAX_ALARM_RETRIES;
  CHECK(planNextSleep() != (uint32_t)SNOOZE_S,
        "po wyczerpaniu prob drzemka sie konczy");

  head("Ile spac: backoff przy zaleglosciach w kolejce");
  /* 15 min, 30, 60, 120, 240 - potem juz nie rzadziej. Za krotko zjada
     bateria przy zerwanej sieci, za dlugo opoznia dawke w kalendarzu. */
  {
    const uint32_t oczek[] = { 900u, 1800u, 3600u, 7200u, 14400u, 14400u, 14400u };
    for (int i = 0; i < 7; i++) {
      spokoj(12, 0);
      queuePush(makeRecordAt("open", 0, 1750000000u));
      rtcRetryCount = (uint8_t)i;
      CHECK(planNextSleep() == oczek[i],
            "proba %d -> %lu s (oczekiwano %lu)", i,
            (unsigned long)planNextSleep(), (unsigned long)oczek[i]);
    }
    CHECK(oczek[4] == (uint32_t)RETRY_MAX_S, "sufit backoffu to RETRY_MAX_S");
  }
  /* Backoff wolno sen SKRACAC, nigdy wydluzac. Inaczej zalegly wpis
     w kolejce przesunalby pobudke na dawke o cztery godziny.        */
  spokoj(19, 50);
  queuePush(makeRecordAt("open", 0, 1750000000u));
  rtcRetryCount = 4;                          // backoff = 4 h
  CHECK(planNextSleep() == 600u - 30u,
        "backoff nie wydluza snu ponad termin dawki (%lu)", (unsigned long)planNextSleep());

  head("Ile spac: aplikacja bez aktualnego stanu");
  /* Bez tego punktu nieudany status czekal do najblizszej dawki albo
     12 h - a telefon przez ten czas pokazywalby nieprawde.          */
  spokoj(12, 0);
  rtcStatusDirty = true;
  CHECK(planNextSleep() == 900u, "nieudany status -> ponowna proba za 15 min");
  spokoj(12, 0);
  rtcOpenClearPend = true;
  CHECK(planNextSleep() == 900u, "odwolanie alarmu tez wymusza ponowienie");
  spokoj(12, 0);
  rtcOpenReported = true;                     // baza mysli ze otwarte, a jest zamkniete
  CHECK(planNextSleep() == 900u, "rozjazd stanu wieczka tez wymusza ponowienie");
  spokoj(12, 0);
  rtcStatusDirty = true; rtcRetryCount = 3;
  CHECK(planNextSleep() == 7200u, "ponowienia statusu ida tym samym backoffem");

  /* ODSTEPY MUSZA NAPRAWDE ROSNAC PRZEZ KOLEJNE NIEUDANE WYBUDZENIA.

     Powyzsze kontrole sprawdzaly MATEMATYKE backoffu - przy podstawionym
     rtcRetryCount wychodzily wlasciwe liczby. Nikt nie sprawdzil, czy ten
     licznik ma jak urosnac, a nie mial: podnosil sie wylacznie w
     reportEvent(), czyli na sciezce otwarcia wieczka.

     Skutek zobaczyl Kuba na historii pudelka: "meldunek bez sieci" co 17
     minut, od 9:33 do 17:46, bateria z 73% na 50% w osiem godzin. Testy
     przez caly ten czas swiecily na zielono.

     Ten test idzie wiec przez KOLEJNE wybudzenia, tak jak robi to
     goToSleep(), i sprawdza ciag odstepow - a nie pojedynczy przypadek. */
  {
    const uint32_t oczekiwane[5] = { 900u, 1800u, 3600u, 7200u, 14400u };
    uint8_t przesun = 0;
    for (int i = 0; i < 5; i++) {
      spokoj(12, 0);
      rtcStatusDirty = true;
      rtcRetryCount  = przesun;
      CHECK(planNextSleep() == oczekiwane[i],
            "kolejne nieudane wybudzenie rozrzedza probe");
      przesun = kolejnePrzesuniecie(przesun, true);   // to samo robi goToSleep()
    }
    /* Sufit: dalej juz nie rzadziej niz co 4 h. */
    for (int i = 0; i < 10; i++) przesun = kolejnePrzesuniecie(przesun, true);
    spokoj(12, 0);
    rtcStatusDirty = true; rtcRetryCount = przesun;
    CHECK(planNextSleep() == 14400u, "...ale nie rzadziej niz co 4 h");
  }

  /* Dowiezione wybudzenie kasuje przesuniecie OD RAZU - inaczej pudelko
     po odzyskaniu sieci nadal milczaloby przez cztery godziny.        */
  CHECK(kolejnePrzesuniecie(4, false) == 0,
        "udane wybudzenie zeruje backoff, nie zmniejsza go o krok");
  CHECK(kolejnePrzesuniecie(0, true) == 1, "pierwsza porazka podnosi przesuniecie");
  CHECK(kolejnePrzesuniecie(200, true) == 200, "licznik nie przekreca sie w kolko");

  head("Ile spac: ladowarka i granica doby");
  spokoj(12, 0);
  rtcCharging = true;
  CHECK(planNextSleep() == (uint32_t)CHARGE_POLL_S,
        "na kablu meldunek co minute (%lu)", (unsigned long)planNextSleep());
#if MIDNIGHT_CHECK
  /* 02:30 lokalnego: do granicy doby 30 min + minuta, do dawki 17,5 h. */
  spokoj(2, 30);
  CHECK(planNextSleep() == 30u*60u + 60u,
        "granica doby blizej niz dawka -> %lu s", (unsigned long)planNextSleep());
#endif

  head("Ile spac: otwarte wieczko przycina sen");
  spokoj(12, 0);
  resetOpenState(true);
  rtcOpenReported = true;                     // zeby nie mieszal rozjazd stanu
  FAKE_NOW = local(2026,8,1,12,0);
  trackBoxOpen();                             // ustawia rtcNextWarnTs
  CHECK(planNextSleep() == (uint32_t)OPEN_WARN_FIRST_S,
        "budzimy sie dokladnie na sygnal (%lu)", (unsigned long)planNextSleep());

  head("Ile spac: sufit i wlasnosci ogolne");
  /* Bez zegara i bez harmonogramu wszystko zwraca HOUSEKEEP_MAX_S -
     sen musi zostac przyciety do 12 h, bo to jedyna okazja na sync. */
  spokoj(12, 0);
  rtcTimeValid = false;
  CHECK(planNextSleep() <= (uint32_t)HOUSEKEEP_MAX_S,
        "bez zegara nie spimy dluzej niz 12 h (%lu)", (unsigned long)planNextSleep());

  /* Wlasnosc zbiorcza: przez wszystkie kombinacje flag sen NIGDY nie
     moze wyjsc zerowy (natychmiastowe wybudzenie = petla i pusta
     bateria) ani przekroczyc czasu do najblizszej dawki.            */
  {
    int zero = 0, zaDlugo = 0, ponadSufit = 0, prob = 0;
    for (int godz = 0; godz < 24; godz++)
    for (int mask = 0; mask < 32; mask++)
    for (int rc = 0; rc <= 5; rc++) {
      spokoj(godz, 17);
      rtcRetryCount   = (uint8_t)rc;
      rtcCharging     = mask & 1;
      rtcStatusDirty  = mask & 2;
      rtcOpenClearPend= mask & 4;
      if (mask & 8)  { resetOpenState(true); rtcOpenReported = true;
                       FAKE_NOW = local(2026,8,1,godz,17); trackBoxOpen(); }
      if (mask & 16) queuePush(makeRecordAt("open", 0, 1750000000u));
      uint32_t s = planNextSleep();
      uint32_t doDawki = secondsToNextSlot(FAKE_NOW);
      prob++;
      if (s == 0) zero++;
      if (s > doDawki) zaDlugo++;
      if (s > (uint32_t)HOUSEKEEP_MAX_S) ponadSufit++;
    }
    CHECK(zero == 0, "zaden z %d wariantow nie zasypia na 0 s (%d takich)", prob, zero);
    CHECK(zaDlugo == 0, "zaden nie przesypia dawki (%d takich)", zaDlugo);
    CHECK(ponadSufit == 0, "zaden nie przekracza sufitu 12 h (%d takich)", ponadSufit);
  }

  /* sprzatanie po sekcji */
  spokoj(12, 0);
  prefs.wipe();
}

/* ================= 20. HASLO URZADZENIA I AKTUALIZACJA (D59) =========
   Stawka jest tu wieksza niz wygoda. Binarka budowana przez automat NIE
   ZNA hasla do bazy - pudelko loguje sie tym z wlasnej pamieci trwalej.
   Gdyby ktos kiedys odwrocil te kolejnosc albo przepuscil aktualizacje
   przy pustej pamieci, pudelko po restarcie nie odezwaloby sie juz do
   aplikacji i nie byloby zdalnej drogi, zeby to naprawic.             */
{
  prefs.wipe();

  head("Haslo urzadzenia: pamiec trwala przed config.h");

  CHECK(!hasloJestPrawdziwe(String("")), "puste haslo nie jest haslem");
  CHECK(!hasloJestPrawdziwe(String(PASSWORD_PLACEHOLDER)),
        "placeholder z repo nie jest haslem");
  CHECK(hasloJestPrawdziwe(String("prawdziwe123")), "zwykly tekst jest haslem");

  CHECK(!hasloWPamieci(), "swieza pamiec nie ma hasla");
  CHECK(hasloDoLogowania() == String(DEVICE_PASSWORD),
        "bez pamieci logujemy sie tym z config.h");

  CHECK(hasloUtrwal(String("tajne-haslo")), "zapis hasla sie udaje");
  CHECK(hasloWPamieci(), "i pamiec od razu o nim wie");
  CHECK(hasloDoLogowania() == String("tajne-haslo"),
        "pamiec trwala ma pierwszenstwo przed config.h");

  /* Placeholder nie moze trafic do pamieci jako prawdziwe haslo -
     inaczej binarka z automatu "utrwalilaby" wlasna pustke i pudelko
     uznaloby sie za samodzielne, nie bedac nim.                      */
  prefs.wipe();
  CHECK(!hasloUtrwal(String(PASSWORD_PLACEHOLDER)),
        "placeholder NIE zapisuje sie jako haslo");
  CHECK(!hasloWPamieci(), "i pamiec nadal jest pusta");

  /* Nieudany zapis do NVS nie moze udawac sukcesu - od tej wartosci
     zalezy zgoda na aktualizacje (D46, D47).                        */
  prefs.wipe();
  prefs.failKeys.insert("fbpass");
  CHECK(!hasloUtrwal(String("tajne-haslo")),
        "nieudany zapis NVS zwraca falsz, a nie ciche 'ok'");
  CHECK(!hasloWPamieci(), "i pudelko NIE uwaza sie za samodzielne");
  prefs.wipe();

  /* A TERAZ AWARIA, PRZED KTORA TEN ODCZYT NAPRAWDE BRONI: zapis melduje
     sukces i nic nie zostaje. Do 1.45.0 atrapa tego nie umiala, wiec
     usuniecie odczytu kontrolnego z hasloUtrwal() nie psulo ani jednej
     kontroli - a to od tej wartosci zalezy zgoda na aktualizacje. */
  prefs.cichoGubKlucze.insert("fbpass");
  CHECK(!hasloUtrwal(String("tajne-haslo")),
        "zapis hasla, ktory MELDUJE sukces i nic nie zapisal, jest wykryty odczytem");
  CHECK(!hasloWPamieci(), "i pudelko nadal NIE uwaza sie za samodzielne");
  prefs.wipe();

  head("Decyzja o aktualizacji: czego pilnuje");

  const String DOBRA  = "0123456789abcdef0123456789abcdef";
  const String INNA   = "ffffffffffffffffffffffffffffffff";
  const String PUSTA  = "";
  const uint32_t ROZM = 1200000;
  /* Zlecenie starsze niz ostatnia proba - czyli NIE swieze. Dzieki temu
     domyslny wzorzec sprawdza zwykla sciezke, a nie wyjatek dla
     swiadomego zadania (ten ma wlasne testy nizej).                   */
  const uint32_t TS_SWIEZE = 0;

  /* Wzorzec: wszystko w porzadku. Kazdy kolejny test psuje DOKLADNIE
     jedna rzecz, zeby bylo widac, ktora z nich odpowiada za odmowe.  */
  CHECK(otaDecyzja(true, 0, 90, false, 0, 1000000, 0, ROZM,
                   TS_SWIEZE, DOBRA, INNA, PUSTA) == OTA_ROB,
        "komplet warunkow -> pobieramy");

  CHECK(otaDecyzja(true, 0, 90, false, 0, 1000000, 0, ROZM,
                   TS_SWIEZE, DOBRA, DOBRA, PUSTA) == OTA_NIC_NOWEGO,
        "ta sama suma co wgrana -> NIE pobieramy drugi raz");

  /* To jest odpowiedz na "zeby nie pobierala sie ta sama wersja":
     rozstrzyga SUMA, nie numer. Numer moze byc nawet ten sam.       */
  CHECK(otaDecyzja(true, 0, 90, false, 0, 1000000, 0, ROZM,
                   TS_SWIEZE, INNA, DOBRA, PUSTA) == OTA_ROB,
        "inna suma -> pobieramy, choc numer wersji moze byc ten sam");

  CHECK(otaDecyzja(false, 0, 90, false, 0, 1000000, 0, ROZM,
                   TS_SWIEZE, DOBRA, INNA, PUSTA) == OTA_BEZ_HASLA,
        "bez hasla w pamieci NIE aktualizujemy (odcielibysmy pudelko od bazy)");

  CHECK(otaDecyzja(true, 3, 90, false, 0, 1000000, 0, ROZM,
                   TS_SWIEZE, DOBRA, INNA, PUSTA) == OTA_KOLEJKA,
        "niewyslane dawki maja pierwszenstwo przed aktualizacja");

  CHECK(otaDecyzja(true, 0, OTA_MIN_BATT_PCT - 1, false, 0, 1000000, 0, ROZM,
                   TS_SWIEZE, DOBRA, INNA, PUSTA) == OTA_BATERIA,
        "za malo baterii poza ladowarka -> czekamy");
  CHECK(otaDecyzja(true, 0, OTA_MIN_BATT_PCT - 1, true, 0, 1000000, 0, ROZM,
                   TS_SWIEZE, DOBRA, INNA, PUSTA) == OTA_ROB,
        "...ale na ladowarce prog baterii nie obowiazuje");

  CHECK(otaDecyzja(true, 0, 90, false, OTA_MAX_FAILS, 1000000, 0, ROZM,
                   TS_SWIEZE, DOBRA, INNA, PUSTA) == OTA_PODDANO,
        "po trzech nieudanych probach przestajemy probowac");


  CHECK(otaDecyzja(true, 0, 90, false, 0, 1000000, 0, ROZM,
                   TS_SWIEZE, DOBRA, INNA, DOBRA) == OTA_ZEPSUTA,
        "wersja z czarnej listy nie jest pobierana drugi raz");

  head("Decyzja o aktualizacji: brak dobowego odstepu");

  /* Dobowego odstepu nie ma (zniesiony w 1.41.0). Zadne urzadzenie nie kaze
     czlowiekowi "wrocic jutro", a petla ponowien nie moze powstac: OTA rusza
     wylacznie na jawne zlecenie, ktore da sie odwolac przyciskiem. Przed
     probowaniem w kolko chroni licznik niepowodzen, nie zegar.          */
  CHECK(otaDecyzja(true, 0, 90, false, 0, 1000000, 999999, ROZM, 0,
                   DOBRA, INNA, PUSTA) == OTA_ROB,
        "proba sprzed sekundy NIE blokuje kolejnej");
  CHECK(otaDecyzja(true, 0, 90, false, OTA_MAX_FAILS, 1000000, 999999, ROZM, 0,
                   DOBRA, INNA, PUSTA) == OTA_PODDANO,
        "...a przed petla chroni licznik niepowodzen");

  /* ...ale licznik chroni przed pudelkiem probujacym W KOLKO SAMO z siebie.
     Gdy czlowiek prosi PONOWNIE - juz po tych nieudanych probach - to nie
     jest petla, tylko swiadoma decyzja. Aplikacja pisze wprost "Nacisnij
     przycisk jeszcze raz, zeby zlecic od nowa", wiec nacisniecie MUSI cos
     zmieniac; wczesniej nie zmienialo nic (zgloszenie Kuby).          */
  CHECK(otaDecyzja(true, 0, 90, false, OTA_MAX_FAILS, 1000000, 999999, ROZM, 999999 + 1,
                   DOBRA, INNA, PUSTA) == OTA_ROB,
        "nowe zlecenie PO serii niepowodzen znosi 'poddalem sie'");
  CHECK(otaDecyzja(true, 0, 90, false, OTA_MAX_FAILS, 1000000, 999999, ROZM, 999000,
                   DOBRA, INNA, PUSTA) == OTA_PODDANO,
        "...ale stare zlecenie nadal nie wznawia niczego");

  head("Decyzja o aktualizacji: opis ze smieci");

  CHECK(otaDecyzja(true, 0, 90, false, 0, 1000000, 0, ROZM,
                   TS_SWIEZE, String("krotka"), INNA, PUSTA) == OTA_ZLY_OPIS,
        "suma inna niz 32 znaki -> to nie jest opis wersji");
  CHECK(otaDecyzja(true, 0, 90, false, 0, 1000000, 0, OTA_MIN_BIN_SIZE - 1,
                   TS_SWIEZE, DOBRA, INNA, PUSTA) == OTA_ZLY_OPIS,
        "plik za maly na firmware (np. strona bledu 404)");
  CHECK(otaDecyzja(true, 0, 90, false, 0, 1000000, 0, OTA_MAX_BIN_SIZE + 1,
                   TS_SWIEZE, DOBRA, INNA, PUSTA) == OTA_ZLY_OPIS,
        "plik wiekszy niz partycja - wiemy to PRZED pobraniem");

  /* Kazdy powod musi miec swoj tekst. Bez tego aplikacja pokazywalaby
     "nieznany stan" akurat wtedy, gdy uzytkownik pyta "dlaczego nie". */
  const OtaDecyzja WSZYSTKIE[] = {
    OTA_ROB, OTA_NIC_NOWEGO, OTA_BEZ_HASLA, OTA_KOLEJKA, OTA_BATERIA,
    OTA_PODDANO, OTA_ZEPSUTA, OTA_ZLY_OPIS
  };
  int bezOpisu = 0;
  for (OtaDecyzja d : WSZYSTKIE)
    if (String(otaOpisDecyzji(d)) == String("nieznany stan")) bezOpisu++;
  CHECK(bezOpisu == 0, "kazdy powod odmowy ma swoj opis dla aplikacji (%d bez)", bezOpisu);

  prefs.wipe();
}

/* ═══════════════ BOT TELEGRAM  (D67) ═══════════════
   Powiadomienie o pominietej dawce Warfinu na telefon. Wysylka to czyste
   we/wy (HTTPS), wiec testujemy dwie rzeczy, ktore testowac sie DAJA:
   pamiec trwala na token i decyzje, czy w ogole pisac.                */
{
  head("Bot Telegram: token w pamieci trwalej");

  const String TOKEN = "1234567890:AAHdqTcvCH1vGWJxfSeofSAs0K5PALDsaw";
  const String CZAT  = "123456789";

  prefs.wipe();
  CHECK(!tgSkonfigurowany(), "swieze pudelko nie ma bota");
  CHECK(tgTokenZPamieci().length() == 0, "i nie ma tokenu");

  CHECK(tgUtrwal(TOKEN, CZAT), "zapis tokenu i czatu sie udaje");
  CHECK(tgSkonfigurowany(), "po zapisie bot jest podlaczony");
  CHECK(tgTokenZPamieci() == TOKEN, "token wraca z pamieci bez zmian");
  CHECK(tgChatZPamieci() == CZAT, "czat wraca z pamieci bez zmian");

  /* Odczyt kontrolny - to od niego zalezy, czy skasujemy token z bazy
     (zasada 9). Zapis "na wiare" dalby stan, w ktorym aplikacja pokazuje
     "bot podlaczony", pudelko go nie ma, a token zniknal z bazy.      */
  prefs.wipe();
  prefs.failKeys.insert("tgTok");
  CHECK(!tgUtrwal(TOKEN, CZAT), "nieudany zapis tokenu zwraca falsz, nie ciche 'ok'");
  CHECK(!tgSkonfigurowany(), "i bot NIE jest uznany za podlaczony");
  prefs.failKeys.clear();

  /* Polowiczna konfiguracja to najgorszy z mozliwych stanow: wyglada jak
     dzialajaca. Sam czat bez tokenu nie ma czym napisac.              */
  prefs.wipe();
  prefs.failKeys.insert("tgChat");
  CHECK(!tgUtrwal(TOKEN, CZAT), "nieudany zapis CZATU tez zwraca falsz");
  CHECK(!tgSkonfigurowany(), "polowiczna konfiguracja nie liczy sie jako bot");
  prefs.failKeys.clear();

  /* CICHA STRATA - jedyna awaria, przed ktora broni odczyt kontrolny.
     NVS melduje sukces, a wartosci nie ma. Bez tego przypadku usuniecie
     odczytu z tgUtrwal() przechodzilo przez caly zestaw (D46, D47).  */
  prefs.wipe();
  prefs.cichoGubKlucze.insert("tgTok");
  CHECK(!tgUtrwal(TOKEN, CZAT),
        "zapis, ktory MELDUJE sukces i nic nie zapisal, jest wykryty odczytem");
  CHECK(!tgSkonfigurowany(), "i bot NIE jest uznany za podlaczony");
  prefs.wipe();
  prefs.cichoGubKlucze.insert("tgChat");
  CHECK(!tgUtrwal(TOKEN, CZAT), "to samo dla cicho zgubionego czatu");
  prefs.wipe();

  prefs.wipe();
  CHECK(!tgUtrwal(String(""), CZAT), "pusty token odrzucamy przed zapisem");
  CHECK(!tgUtrwal(TOKEN, String("")), "pusty czat odrzucamy przed zapisem");
  {
    String zaDlugi;
    for (int i = 0; i < TG_TOKEN_MAX + 5; i++) zaDlugi += "x";
    CHECK(!tgUtrwal(zaDlugi, CZAT), "token dluzszy niz limit odrzucamy przed zapisem");
    String zaDlugiCzat;
    for (int i = 0; i < TG_CHAT_MAX + 5; i++) zaDlugiCzat += "9";
    CHECK(!tgUtrwal(TOKEN, zaDlugiCzat), "czat dluzszy niz limit tez odrzucamy");
  }
  CHECK(!tgSkonfigurowany(), "zaden z odrzuconych zapisow nie podlaczyl bota");

  CHECK(tgUtrwal(TOKEN, CZAT) && tgSkonfigurowany(), "ponowne podlaczenie dziala");
  tgZapomnij();
  CHECK(!tgSkonfigurowany(), "odlaczenie bota kasuje go z pamieci");
  CHECK(tgTokenZPamieci().length() == 0, "i token naprawde znika, nie tylko flaga");

  head("Bot Telegram: kiedy pisac, a kiedy nie");

  const uint32_t POWSTALO = 1750000000;
  /* Wzorzec: jest o czym pisac, jest komu, powstalo przed chwila. */
  CHECK(tgDecyzja(true, true, POWSTALO, POWSTALO + 60) == TG_WYSLIJ,
        "komplet warunkow -> piszemy");

  /* Pusta skrzynka WYCHODZI PIERWSZA - to jest warunek, dzieki ktoremu
     zwykle wybudzenie nie placi za te funkcje ani miliampera. Sprawdzamy
     go takze bez bota, zeby kolejnosc pytan byla wymuszona testem,
     a nie tylko opisana w komentarzu.                                */
  CHECK(tgDecyzja(true,  false, POWSTALO, POWSTALO + 60) == TG_NIC,
        "nic nie czeka -> nie ruszamy radia");
  CHECK(tgDecyzja(false, false, POWSTALO, POWSTALO + 60) == TG_NIC,
        "pusta skrzynka wygrywa nawet nad brakiem bota (kolejnosc pytan)");

  CHECK(tgDecyzja(false, true, POWSTALO, POWSTALO + 60) == TG_BRAK_BOTA,
        "bez bota nie mamy komu pisac - i mowimy to wprost");

  /* Przeterminowanie. "Nie wziales tabletki o 20:00" dostarczone nazajutrz
     w poludnie nie jest przypomnieniem, tylko dezinformacja - dawka dawno
     zostala wzieta albo dzien dawno zamkniety.                        */
  CHECK(tgDecyzja(true, true, POWSTALO, POWSTALO + TG_MAX_WIEK_S + 1) == TG_ZA_STARE,
        "powiadomienie starsze niz limit -> kasujemy, nie wysylamy");
  CHECK(tgDecyzja(true, true, POWSTALO, POWSTALO + TG_MAX_WIEK_S) == TG_WYSLIJ,
        "dokladnie na granicy wieku jeszcze wysylamy");
  CHECK(tgDecyzja(true, true, POWSTALO, POWSTALO + TG_MAX_WIEK_S - 1) == TG_WYSLIJ,
        "chwile przed granica oczywiscie tez");

  /* Nieznany czas nie jest powodem do milczenia - ta sama zasada co przy
     dniach bez leku: milkniemy WYLACZNIE wtedy, gdy wiemy na pewno.  */
  CHECK(tgDecyzja(true, true, 0, POWSTALO + 999999) == TG_WYSLIJ,
        "bez znanego czasu powstania wysylamy (nie umiemy zmierzyc wieku)");
  CHECK(tgDecyzja(true, true, POWSTALO, 0) == TG_WYSLIJ,
        "bez wiarygodnego zegara wysylamy, zamiast milczec");

  /* Zegar cofniety przez NTP po dlugim offline. Roznica wychodzi ujemna,
     a na liczbach bez znaku to jest wielka liczba dodatnia - dokladnie
     ten rodzaj wpadki, ktory kasowalby swieze powiadomienia jako stare. */
  CHECK(tgDecyzja(true, true, POWSTALO, POWSTALO - 5000) == TG_WYSLIJ,
        "zegar cofniety w tyl nie robi ze swiezej wiadomosci starej");

  prefs.wipe();
}

printf("\n──────────────────────────────────────\n");
printf("  ZALICZONE: %d    BLEDY: %d\n", PASS, FAIL);
printf("──────────────────────────────────────\n");
return FAIL ? 1 : 0;
}
