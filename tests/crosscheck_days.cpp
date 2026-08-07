/* =====================================================================
 *  Wypisuje numer DOBY LEKOWEJ dla znacznikow czasu podanych na wejsciu,
 *  uzywajac PRAWDZIWEJ funkcji localDayNumber() wycietej z PillBox.ino.
 *
 *  Sluzy do porownania z ta sama decyzja podjeta po stronie aplikacji.
 *  Rozjazd miedzy pudelkiem a telefonem jest najgrozniejszym bledem w tym
 *  projekcie: obie strony dzialaja "poprawnie", tylko przypisuja te sama
 *  tabletke do dwoch roznych dni - i nikt tego nie widzi, dopoki nie
 *  zrobi sie dziura w kalendarzu.
 *
 *  Uzycie:  echo "<offset_min> <ts> <ts> ..." | ./crosscheck
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
int     slotCount = 0;
int     batteryPercentage = 0;
int     batteryRawPercentage = 0;
float   realBatteryVoltage = 0.0f;
bool    rtcTimeValid = true;
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
uint16_t rtcQueueDropped = 0;
/* flushQueue() jest teraz wyciagane do testow, wiec potrzebuje atrapy
   wysylki. Tu nikt jej nie wola - wystarczy, ze istnieje.            */
int pushEventRecord(const String&) { return 200; }

#include "logic.inc"

int main() {
  long off;
  if (scanf("%ld", &off) != 1) return 1;
  rtcTzOffsetMin = (int16_t)off;

  long long ts;
  while (scanf("%lld", &ts) == 1)
    printf("%lu\n", (unsigned long)localDayNumber((time_t)ts));
  return 0;
}
