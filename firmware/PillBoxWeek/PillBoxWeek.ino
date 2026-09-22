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
 *    komora    PON  WT   SR   CZW  PT   SOB  ND     zamkniete
 *    napiecie  105  170  300  388  557  633  761    1125 mV
 *
 *  Wybudzanie z glebokiego snu przez najwyzsza galaz (761 mV) - dziala,
 *  sprawdzone. Margines do progu zera (825 mV) wynosi 64 mV, wiec
 *  ZADEN rezystor galezi nie moze byc wiekszy niz 3 kOhm: niedziela
 *  przestalaby budzic pudelko.
 * ===================================================================== */

#include "config.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <time.h>
#include "esp_sleep.h"
#include "driver/gpio.h"

#if !defined(PILLBOX_WEEK_CONFIG_VERSION)
#error "config.h nie pasuje do tego szkicu - pobierz oba pliki na nowo."
#endif

#define LOG(...)  Serial.printf(__VA_ARGS__)

static const char* NAZWY_DNI[7] = { "PON","WT","SR","CZW","PT","SOB","ND" };
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

/* =====================================================================
 *  STAN BIEZACEGO WYBUDZENIA
 * ===================================================================== */
Preferences nvs;
String  idToken;
int     battProcent   = -1;
float   battVolt      = 0;
int     komoraPoStarcie = -3;      // -1 zamkniete, -2 kilka naraz, 0..6 komora
bool    czasZsync     = false;

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
   siega kilku miliwoltow - przy odstepie 65 mV miedzy poniedzialkiem
   a wtorkiem to jeszcze nic, ale zgadywanie zamiast pomiaru bylo
   dokladnie tym, czego caly ten projekt unika.                        */
int ktoraKomora(uint16_t mV) {
  if (mV > PROGI[7]) return -1;
  if (mV < PROGI[0]) return -2;
  for (int i = 6; i >= 0; i--) if (mV >= PROGI[i]) return i;
  return -2;
}

const char* opisKomory(int k) {
  if (k == -1) return "zamkniete";
  if (k == -2) return "kilka naraz";
  return (k >= 0 && k < 7) ? NAZWY_DNI[k] : "?";
}

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

/* Tyle piknięc, ktory to dzien: PON=1 ... ND=7. Kuba prosil wprost,
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
void czytajBaterie() {
  uint32_t suma = 0;
  for (int i = 0; i < 32; i++) suma += analogReadMilliVolts(PIN_BATERIA);
  battVolt = (suma / 32.0f) * 2.0f / 1000.0f;

  /* Krzywa uproszczona: 4,20 V = 100%, 3,30 V = 0%. Nie jest liniowa
     naprawde, ale do ostrzezenia "laduj" wystarcza, a udawanie precyzji
     bez pomiaru rozladowania byloby zgadywaniem.                      */
  float p = (battVolt - 3.30f) / (4.20f - 3.30f) * 100.0f;
  battProcent = (int)(p < 0 ? 0 : (p > 100 ? 100 : p));
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

void hasloUtrwal(const String& h) {
  if (h == "TUTAJ_WPISZ_HASLO" || !h.length()) return;
  if (nvs.getString("haslo", "") == h) return;
  nvs.putString("haslo", h);
  /* Potwierdzenie ODCZYTEM ZWROTNYM. Zapis do NVS potrafi sie nie udac
     po cichu, a haslo, ktorego pudelko nie ma, odcina je od bazy - czyli
     od jedynej drogi naprawy bez kabla.                               */
  if (nvs.getString("haslo", "") == h) LOG("[FB ] haslo zapisane w pamieci\n");
  else                                 LOG("[FB ] UWAGA: haslo NIE zapisalo sie\n");
}

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
  doc["battery"] = rec.substring(p2 + 1, p3).toInt();
  doc["volt"]    = rec.substring(p3 + 1, p4).toFloat();
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

void wyslijStatus() {
  JsonDocument doc;
  doc["battery"] = battProcent;
  doc["volt"]    = battVolt;
  doc["fw"]      = FW_VERSION;
  doc["rssi"]    = WiFi.RSSI();
  doc["queue"]   = kolejkaIle();
  doc["wakes"]   = (uint32_t)rtcWybudzen;
  doc["ts"]      = (uint32_t)time(nullptr);
  String body; serializeJson(doc, body);
  rtdbWyslij("PATCH", "/devices/" DEVICE_ID "/status.json", body);
}

/* Pobiera harmonogram przypomnien z bazy i zapisuje w NVS, zeby pudelko
   znalo go takze bez sieci.                                           */
void pobierzUstawienia() {
  String odp;
  int code = rtdbWyslij("GET", "/devices/" DEVICE_ID "/config.json", "", &odp);
  if (code != 200) { LOG("[FB ] config HTTP %d\n", code); return; }

  JsonDocument doc;
  if (deserializeJson(doc, odp)) return;

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
 *  11.  SEN
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

/* Czy ktorakolwiek klapka jest teraz otwarta. */
bool klapkiOtwarte() { return ktoraKomora(czytajKlapki(16)) != -1; }

void idzSpac() {
  wifiWylacz();

  /*  KLAPKA ZOSTAWIONA OTWARTA JEST PULAPKA, i to nie teoretyczna.
      Wybudzanie reaguje na POZIOM niski, nie na zbocze: przy otwartej
      klapce pin jest nisko caly czas, wiec pudelko obudziloby sie
      natychmiast po zasnieciu - i tak w kolko, az do rozladowania
      ogniwa, zasmiecajac po drodze kolejke powtorzonymi otwarciami.

      Czekamy wiec chwile na zamkniecie. Gdy nie nastepuje (ktos wlasnie
      napelnia pudelko), zasypiamy na SAM ZEGAR i wracamy za chwile.   */
  uint32_t start = millis();
  while (klapkiOtwarte() && millis() - start < CZEKAJ_ZAMKNIECIE_S * 1000UL) delay(200);
  const bool otwarte = klapkiOtwarte();
  if (otwarte) LOG("[SEN] klapka nadal otwarta - usypiam na sam zegar\n");

  uint64_t sek = otwarte ? (uint64_t)SEN_PRZY_OTWARTEJ_S : sekundDoNastepnego();
  LOG("[SEN] spie na %llu min (kolejka: %d)\n", sek / 60, kolejkaIle());
  Serial.flush();
  nvs.end();

  /* Wewnetrzne podciagniecia WYLACZAMY. Rownolegle do naszego 10 kOhm
     podnosilyby wszystkie napiecia drabinki - niedziela (761 mV)
     przekroczylaby prog zera i przestalaby budzic pudelko.            */
  gpio_pullup_dis((gpio_num_t)PIN_KLAPKI);
  gpio_pulldown_dis((gpio_num_t)PIN_KLAPKI);

  if (!otwarte) esp_deep_sleep_enable_gpio_wakeup(BIT(PIN_KLAPKI), ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_sleep_enable_timer_wakeup(sek * 1000000ULL);
  esp_deep_sleep_start();
}

/* =====================================================================
 *  12.  SETUP  -  cala logika. loop() nigdy nie jest osiagany.
 * ===================================================================== */
void setup() {
  /* ---- ODCZYT KLAPKI JEST PIERWSZY I TO NIE JEST KOSMETYKA ----
     Klapka potrafi wrocic w sekunde, a Serial.begin z czekaniem na monitor
     to juz za pozno. W tescie na plytce pudelko obudzilo sie poprawnie,
     ale zobaczylo komore JUZ ZAMKNIETA i nie wiedzialo, ktora to byla. */
  esp_sleep_wakeup_cause_t powod = esp_sleep_get_wakeup_cause();
  analogSetAttenuation(ADC_2_5db);
  analogReadResolution(12);
  if (powod == ESP_SLEEP_WAKEUP_GPIO) komoraPoStarcie = ktoraKomora(czytajKlapki());

  Serial.begin(115200);
  delay(300);
  rtcWybudzen++;

  nvs.begin("pbweek", false);
  wczytajHarmonogram();
  pinMode(PIN_PRZYCISK, INPUT_PULLUP);
  czytajBaterie();

  LOG("\n===== PillBoxWeek %s  (wybudzenie %lu) =====\n",
      FW_VERSION, (unsigned long)rtcWybudzen);
  LOG("[BAT] %d%%  %.2f V\n", battProcent, battVolt);

  /*  Zegar ESP32 chodzi przez caly deep sleep, wiec doby domykamy JESZCZE
      PRZED radiem i przed zapisem biezacego zdarzenia. Kolejnosc w kolejce
      wychodzi wtedy chronologiczna sama z siebie: wczorajsze "missed",
      potem dzisiejsze otwarcie.                                        */
  domknijDoby();

  /* ---------- A. OBUDZILA NAS KLAPKA ---------- */
  if (powod == ESP_SLEEP_WAKEUP_GPIO) {
    int k = komoraPoStarcie;
    LOG("[EV ] klapka: %s\n", opisKomory(k));

    if (k >= 0) {
      pikniecia(k + 1);                   // potwierdzenie na sluch
      zapiszOtwarcie(k);
      beepAck();
    } else if (k == -2) {
      /* Kilka klapek naraz to NAPELNIANIE, nie dawka. Zapisanie tego jako
         wziecia zmyliloby kalendarz na caly tydzien do przodu.         */
      LOG("[EV ] kilka klapek naraz - napelnianie, nie zapisuje dawki\n");
      beepKilkaNaraz();
    } else {
      LOG("[EV ] klapka zdazyla sie zamknac\n");
      beepBlad();
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

  zglos("boot", 0);
  /*  Po zimnym starcie pamiec RTC jest pusta. domknijDoby() zapisze
      biezaca dobe jako punkt wyjscia i NIE zglosi niczego wstecz -
      pudelko nie wie, co dzialo sie, gdy bylo bez zasilania.         */
  domknijDoby();
  idzSpac();
}

void loop() { }
