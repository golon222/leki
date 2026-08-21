/* =====================================================================
 *  Przepuszcza znaczniki czasu przez PRAWDZIWE funkcje wyciete
 *  z PillBox.ino, zeby dalo sie porownac ich wynik z ta sama decyzja
 *  podjeta po stronie aplikacji.
 *
 *  Rozjazd miedzy pudelkiem a telefonem jest najgrozniejszym bledem w tym
 *  projekcie: obie strony dzialaja "poprawnie", tylko przypisuja te sama
 *  tabletke do dwoch roznych dni albo do dwoch roznych dawek - i nikt tego
 *  nie widzi, dopoki nie zrobi sie dziura w kalendarzu.
 *
 *  Uzycie:
 *    echo "dzien <offset_min> <ts>..."               | ./crosscheck
 *    echo "slot  <offset_min> <hh:mm,hh:mm> <ts>..." | ./crosscheck
 * ===================================================================== */
#include "arduino_shim.h"
#include "../firmware/PillBox/config.h"
#include <cstring>
#include <cstdio>

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
/* Znaczniki powiadomien (D83) - zgodnosc dni ich nie dotyczy, ale
   logic.inc wciaga funkcje, ktore je czytaja.                        */
int16_t rtcPillsLeft = -1;
bool    rtcTgStockCzeka = false;
bool    rtcTgStockZgloszony = false;
char    rtcInrDue[11] = "";
char    rtcTgInrZgloszony[11] = "";
bool    rtcTgInrCzeka = false;
int16_t rtcTzOffsetMin = 120;
uint32_t rtcTakenDay = 0;
uint32_t rtcRolloverDay = 0;
uint32_t awakeDeadlineMs = AWAKE_LIMIT_MS;
uint8_t rtcBattPct = 255;
uint8_t rtcBattUp  = 0;
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

/* Rozpisanie dawek - tu zawsze "nic nie wiem", bo ten program sprawdza
   zgodnosc DNI LEKOWYCH miedzy pudelkiem a aplikacja i dni bez leku nie
   moga mu w tym przeszkadzac.                                          */
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

int main() {
  char tryb[16];
  long off;
  if (scanf("%15s", tryb) != 1) return 1;
  if (scanf("%ld", &off) != 1) return 1;
  rtcTzOffsetMin = (int16_t)off;

  long long ts;

  if (strcmp(tryb, "slot") == 0) {
    /* Harmonogram wczytujemy TA SAMA funkcja, ktorej uzywa pudelko -
       wlasny parser w tescie badalby juz co innego niz produkcja.    */
    char plan[128];
    if (scanf("%127s", plan) != 1) return 1;
    parseSchedule(String(plan));
    while (scanf("%lld", &ts) == 1)
      printf("%d\n", matchSlot((time_t)ts));
    return 0;
  }

  while (scanf("%lld", &ts) == 1)
    printf("%lu\n", (unsigned long)localDayNumber((time_t)ts));
  return 0;
}
