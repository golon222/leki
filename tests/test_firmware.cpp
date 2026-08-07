/* =====================================================================
 *  Testy logiki firmware. Kompiluja PRAWDZIWE funkcje wyciete
 *  z PillBox.ino (logic.inc) na atrapach Arduino.
 *      python3 extract.py && g++ -O0 -std=c++17 test_firmware.cpp -o t && ./t
 * ===================================================================== */
#include "arduino_shim.h"
#include <cstring>
#include "../firmware/PillBox/config.h"

#include <vector>

time_t FAKE_NOW = 0;
int    FAKE_ADC = 0;
unsigned long FAKE_MILLIS = 0;
FakeSerial Serial;

/* globalne, ktorych uzywa wycieta logika */
Preferences prefs;
String  slots[12];
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
uint16_t rtcQueueDropped = 0;

/* --- atrapy dla pilnowania otwartego wieczka --- */
uint32_t rtcLastOpenTs    = 0;
uint32_t rtcLastPushTs    = 0;
uint32_t rtcTokenExp      = 0;
uint32_t rtcOpenSinceTs   = 0;
uint32_t rtcNextWarnTs    = 0;
uint16_t rtcOpenWarnCount = 0;
bool     rtcOpenReported  = false;
bool     rtcOpenClearPend = false;
bool     rtcStatusDirty   = false;
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

/* Rekord uszkodzony tez nigdy nie przejdzie - nie moze blokowac. */
CHECK(rekordKompletny(makeRecordAt("open", 0, 1750000000u)), "pelny rekord przechodzi");
CHECK(!rekordKompletny(String("cos;bez;pieciu;pol")), "brak piatego pola wykryty");
CHECK(!rekordKompletny(String("")), "pusty rekord wykryty");

/* ================= 5c. NIEUDANY ZAPIS DO NVS (B5) ================= */
head("Nieudany zapis do pamieci nie udaje udanego");
prefs.wipe(); rtcNvsFail = 0;
prefs.failKeys.insert("q0");
CHECK(!queuePush(makeRecordAt("open", 0, 1750000000u)), "queuePush melduje niepowodzenie");
CHECK(queueCount()==0, "licznik nie podniesiony - brak wpisu-widma (%u)", queueCount());
CHECK(rtcNvsFail==1, "awaria policzona: %u", rtcNvsFail);
prefs.failKeys.clear();
CHECK(queuePush(makeRecordAt("open", 0, 1750000001u)), "po ustaniu awarii zapis wchodzi");
CHECK(queueCount()==1, "i tym razem licznik rosnie (%u)", queueCount());

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

printf("\n──────────────────────────────────────\n");
printf("  ZALICZONE: %d    BLEDY: %d\n", PASS, FAIL);
printf("──────────────────────────────────────\n");
return FAIL ? 1 : 0;
}
