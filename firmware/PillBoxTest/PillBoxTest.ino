/* =====================================================================
 *  PillBoxTest.ino  -  program DIAGNOSTYCZNY inteligentnego pudelka
 *
 *  NIE ZASTEPUJE PillBox.ino. To osobny program, ktory wgrywasz tylko na
 *  czas sprawdzenia sprzetu - np. po upadku pudelka. Nic nie kasuje i nie
 *  zmienia ustawien; jedyny slad, jaki zostawia, to jeden wpis testowy
 *  w Firebase (devices/<ID>/status/selftest).
 *
 *  ---------------------------------------------------------------------
 *  JAK UZYC
 *  1. Utworz folder o nazwie  PillBoxTest
 *  2. Wrzuc do niego ten plik ORAZ kopie swojego  config.h
 *     (bez config.h program tez zadziala, ale pominie testy WiFi
 *      i Firebase - sprawdzi sam sprzet)
 *  3. Ustawienia plytki takie same jak zwykle:
 *        Board            : XIAO_ESP32C3
 *        USB CDC On Boot  : Enabled
 *        Partition Scheme : Huge APP (3MB No OTA/1MB SPIFFS)
 *  4. Wgraj, otworz Monitor portu szeregowego (115200) i wykonuj polecenia
 *     ktore sie pojawia. Test trwa okolo 2 minut.
 *  5. Po tescie wgraj z powrotem PillBox.ino.
 *
 *  Na koncu dostajesz podsumowanie: co dziala, co nie dziala i na co
 *  konkretnie patrzec.
 * ===================================================================== */

#if __has_include("config.h")
  #include "config.h"
  #define MAM_CONFIG 1
#else
  #define MAM_CONFIG 0
  /* Awaryjne wartosci, gdyby ktos wgral sam ten plik bez config.h.
     Zgodne ze zlutowanym schematem.                                    */
  #define PIN_BATTERY        D0
  #define PIN_REED           D1
  #define PIN_BUZZER         D2
  #define PIN_BUTTON         D3
  #define GPIO_REED          3
  #define GPIO_BUTTON        5
  #define REED_OPEN_LEVEL    HIGH
  #define BUTTON_PRESS_LEVEL LOW
  #define REED_MODE          INPUT_PULLUP
  #define BUTTON_MODE        INPUT_PULLUP
  #define BUZZER_FREQ_HZ     2700
  #define NVS_NAMESPACE      "pillbox"
  #define DEVICE_ID          "pillbox01"
#endif

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include <esp_sleep.h>
#include <soc/soc_caps.h>
#include <driver/gpio.h>
#include <time.h>
#include <stdarg.h>

/* Pamiec RTC - przezywa deep sleep, dzieki czemu po ostatnim tescie
   program wie, ze ma dokonczyc i wypisac wynik.                       */
RTC_DATA_ATTR int etapSnu     = 0;      // 0 = start, 1 = wracamy ze snu
RTC_DATA_ATTR int iluZdanych  = 0;
RTC_DATA_ATTR int iluOblanych = 0;

Preferences prefs;

/* ---------------------------------------------------------------------
 *  Drobny szkielet raportu
 * ------------------------------------------------------------------ */
int zdane = 0, oblane = 0, uwagi = 0;

void naglowek(const char* t) {
  Serial.println();
  Serial.print("=== "); Serial.print(t); Serial.println(" ===");
}
void wynik(bool ok, const char* opis) {
  Serial.print(ok ? "  [OK]   " : "  [BLAD] ");
  Serial.println(opis);
  ok ? zdane++ : oblane++;
}
void wynikF(bool ok, const char* fmt, ...) {
  char buf[220];
  va_list ap; va_start(ap, fmt); vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap);
  wynik(ok, buf);
}
void uwagaF(const char* fmt, ...) {
  char buf[220];
  va_list ap; va_start(ap, fmt); vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap);
  Serial.print("  [UWAGA] "); Serial.println(buf);
  uwagi++;
}
void info(const char* s) { Serial.print("         "); Serial.println(s); }

/* ---------------------------------------------------------------------
 *  1. ZASILANIE I POMIAR BATERII
 *     Po upadku najczesciej odchodzi wlasnie tu: przewod ogniwa albo
 *     lutowanie dzielnika napiecia.
 * ------------------------------------------------------------------ */
int adcMediana() {
  int v[9];
  for (int i = 0; i < 9; i++) { v[i] = analogRead(PIN_BATTERY); delay(3); }
  for (int i = 0; i < 8; i++)
    for (int j = i + 1; j < 9; j++)
      if (v[j] < v[i]) { int t = v[i]; v[i] = v[j]; v[j] = t; }
  return v[4];
}

float napiecieOgniwa(int* rawOut = nullptr) {
  pinMode(PIN_BATTERY, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_BATTERY, ADC_11db);
  delay(5);
  (void)analogRead(PIN_BATTERY);
  int raw = adcMediana();
  if (rawOut) *rawOut = raw;
  return ((raw / 4095.0f) * 3.3f) * 2.0f * 0.921f;
}

void testBaterii() {
  naglowek("1. Zasilanie i pomiar baterii");

  int raw = 0;
  float u = napiecieOgniwa(&raw);
  Serial.printf("         surowy odczyt ADC: %d\n", raw);
  Serial.printf("         napiecie ogniwa  : %.3f V\n", u);

  wynikF(raw > 0, "przetwornik ADC cokolwiek zwraca (%d)", raw);

  if (raw < 30) {
    wynik(false, "odczyt prawie zerowy - urwany dzielnik lub odlaczone ogniwo");
    info("Sprawdz lutowanie dwoch rezystorow 100k oraz przewod +BAT.");
  } else if (u < 2.0f) {
    uwagaF("napiecie %.2f V - to wyglada na prace z samego USB, bez ogniwa", u);
    info("Jesli bateria jest podlaczona, to znaczy ze jej nie widzi.");
  } else if (u < 3.0f) {
    wynikF(false, "napiecie %.2f V - ogniwo gleboko rozladowane albo uszkodzone", u);
  } else if (u > 4.35f) {
    wynikF(false, "napiecie %.2f V - za wysokie, podejrzany dzielnik", u);
  } else {
    wynikF(true, "napiecie w normie: %.2f V", u);
  }

  /* Stabilnosc: dziesiec pomiarow pod rzad. Duzy rozrzut to zwykle
     nadlamane lutowanie, ktore raz styka, a raz nie.                  */
  int mn = 4095, mx = 0;
  for (int i = 0; i < 10; i++) {
    int r = adcMediana();
    if (r < mn) mn = r;
    if (r > mx) mx = r;
    delay(20);
  }
  int rozrzut = mx - mn;
  Serial.printf("         rozrzut 10 pomiarow: %d (min %d, max %d)\n", rozrzut, mn, mx);
  if (rozrzut > 250) {
    wynikF(false, "odczyt skacze o %d - podejrzenie nadlamanego polaczenia", rozrzut);
    info("Porusz delikatnie przewodem baterii i powtorz test.");
  } else {
    wynikF(true, "odczyt stabilny (rozrzut %d)", rozrzut);
  }
}

/* ---------------------------------------------------------------------
 *  2. BUZZER
 * ------------------------------------------------------------------ */
void ton(uint32_t hz, uint32_t ms) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWriteTone(PIN_BUZZER, hz);
#else
  ledcWriteTone(0, hz);
#endif
  delay(ms);
}
void ciszaBuzzera() {
  ton(0, 0);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(PIN_BUZZER, 0);
#else
  ledcWrite(0, 0);
#endif
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
}

void testBuzzera() {
  naglowek("2. Buzzer");
  info("Zaraz uslyszysz trzy tony: niski, sredni, wysoki.");
  info("SLUCHAJ. Jesli cisza - buzzer albo jego lutowanie poszlo.");
  delay(1200);

#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(PIN_BUZZER, BUZZER_FREQ_HZ, 10);
#else
  ledcSetup(0, BUZZER_FREQ_HZ, 10);
  ledcAttachPin(PIN_BUZZER, 0);
#endif

  Serial.println("         ton 1/3 (1000 Hz)"); ton(1000, 400); ton(0, 150);
  Serial.println("         ton 2/3 (2000 Hz)"); ton(2000, 400); ton(0, 150);
  Serial.println("         ton 3/3 (3000 Hz)"); ton(3000, 400);
  ciszaBuzzera();

  wynik(true, "sygnal wyslany na buzzer (ocen sluchem)");
  info("Nie slyszales nic? To jedyny test, ktorego program nie zweryfikuje sam.");
}

/* ---------------------------------------------------------------------
 *  3. KONTAKTRON  (test z Twoim udzialem)
 * ------------------------------------------------------------------ */
bool pudelkoOtwarte()     { return digitalRead(PIN_REED)   == REED_OPEN_LEVEL; }
bool przyciskWcisniety()  { return digitalRead(PIN_BUTTON) == BUTTON_PRESS_LEVEL; }

bool testKontaktronu() {
  naglowek("3. Kontaktron (magnes)");
  info("PRZYSUWAJ I ODSUWAJ magnes od kontaktronu przez 20 sekund.");
  info("Kazda zmiana pojawi sie ponizej. Potrzebne sa co najmniej 2.");
  delay(1500);

  bool stan = pudelkoOtwarte();
  Serial.printf("         stan poczatkowy: pin=%d (%s)\n",
                digitalRead(PIN_REED), stan ? "OTWARTE" : "zamkniete");

  int zmian = 0;
  bool widzianoOtwarte = stan, widzianoZamkniete = !stan;
  uint32_t start = millis();
  while (millis() - start < 20000) {
    bool teraz = pudelkoOtwarte();
    if (teraz != stan) {
      stan = teraz;
      zmian++;
      if (teraz) widzianoOtwarte = true; else widzianoZamkniete = true;
      Serial.printf("         zmiana %d -> %s (pin=%d)\n",
                    zmian, teraz ? "OTWARTE" : "ZAMKNIETE", digitalRead(PIN_REED));
    }
    delay(15);
  }

  wynikF(zmian >= 2, "wykryto %d zmian stanu (potrzeba min. 2)", zmian);
  bool obaStany = widzianoOtwarte && widzianoZamkniete;
  wynik(obaStany, "widziano oba stany: otwarte i zamkniete");
  if (!obaStany) {
    if (!widzianoOtwarte)
      info("Pin nigdy nie poszedl w gore - sprawdz rezystor podciagajacy do 3V3.");
    if (!widzianoZamkniete)
      info("Pin nigdy nie poszedl w dol - sprawdz polaczenie kontaktronu z GND.");
  }
  return zmian >= 2 && obaStany;
}

/* ---------------------------------------------------------------------
 *  4. UKRYTY PRZYCISK
 * ------------------------------------------------------------------ */
void testPrzycisku() {
  naglowek("4. Ukryty przycisk");
  info("NACISNIJ przycisk kilka razy przez 12 sekund.");
  delay(1200);

  bool stan = przyciskWcisniety();
  int wcisniec = 0;
  uint32_t start = millis();
  while (millis() - start < 12000) {
    bool teraz = przyciskWcisniety();
    if (teraz != stan) {
      stan = teraz;
      if (teraz) { wcisniec++; Serial.printf("         wcisniecie %d\n", wcisniec); }
    }
    delay(15);
  }
  wynikF(wcisniec >= 1, "wykryto %d wcisniec", wcisniec);
  if (!wcisniec)
    info("Zero reakcji - styk przycisku albo przewod do GND. "
         "Bez niego dziala wszystko oprocz konfiguracji WiFi.");
}

/* ---------------------------------------------------------------------
 *  5. PAMIEC NIEULOTNA  (tu mieszka licznik dawki i kolejka offline)
 * ------------------------------------------------------------------ */
void testPamieci() {
  naglowek("5. Pamiec nieulotna (NVS)");

  const uint32_t wzor = 0xC0FFEE01;
  bool otwarta = prefs.begin("pbtest", false);
  wynik(otwarta, "otwarcie obszaru pamieci");
  if (!otwarta) { info("Pamiec flash moze byc uszkodzona."); return; }

  prefs.putULong("probka", wzor);
  uint32_t czyt = prefs.getULong("probka", 0);
  wynikF(czyt == wzor, "zapis i odczyt zgodne (0x%08lX)", (unsigned long)czyt);
  prefs.remove("probka");
  prefs.end();

  /* Podglad prawdziwych danych pudelka - nie ruszamy ich, tylko czytamy. */
  if (prefs.begin(NVS_NAMESPACE, true)) {
    String harm = prefs.getString("sched", "(brak)");
    uint32_t dzien = prefs.getULong("takenDay", 0);
    Serial.printf("         zapisany harmonogram   : %s\n", harm.c_str());
    Serial.printf("         ostatnia zapisana dawka: %lu\n", (unsigned long)dzien);
    prefs.end();
    wynik(true, "dane pudelka czytelne");
  } else {
    uwagaF("brak obszaru '%s' - pudelko jeszcze nic nie zapisalo", NVS_NAMESPACE);
  }
}

/* ---------------------------------------------------------------------
 *  6. WiFi
 * ------------------------------------------------------------------ */
bool testWifi() {
  naglowek("6. WiFi");

  WiFi.mode(WIFI_STA);
  delay(100);
  int n = WiFi.scanNetworks();
  wynikF(n > 0, "skanowanie znalazlo %d sieci (antena dziala)", n);

  /* Wyniki skanowania zostaja w pamieci, a polaczenie szyfrowane z Firebase
     potrzebuje jej kilkadziesiat kilobajtow naraz. Bez tego zwolnienia
     uscisk TLS potrafi sie nie udac - i wygladalo to jak zle haslo. */
  WiFi.scanDelete();

  if (n <= 0) {
    info("Zero sieci to zwykle uszkodzony uklad radiowy albo antena.");
    return false;
  }

  Serial.println("         lacze sie zapamietanymi danymi...");
  WiFi.begin();                       // uzywa danych zapisanych przez PillBox.ino
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) delay(200);

  if (WiFi.status() != WL_CONNECTED) {
    uwagaF("nie polaczono w 15 s - moglo sie zmienic haslo do sieci");
    info("To nie musi byc usterka sprzetu. Radio dziala, skoro widzi sieci.");
    return false;
  }

  wynikF(true, "polaczono z '%s', IP %s", WiFi.SSID().c_str(),
         WiFi.localIP().toString().c_str());
  int rssi = WiFi.RSSI();
  Serial.printf("         moc sygnalu: %d dBm\n", rssi);
  if (rssi < -85) uwagaF("sygnal slaby (%d dBm) - pudelko lezy daleko od routera", rssi);
  else            wynikF(true, "moc sygnalu w normie (%d dBm)", rssi);

  /* Napiecie pod obciazeniem radia - slabe ogniwo widac wlasnie tutaj. */
  float uPod = napiecieOgniwa();
  Serial.printf("         napiecie przy wlaczonym radiu: %.3f V\n", uPod);
  if (uPod > 2.0f && uPod < 3.2f)
    wynikF(false, "napiecie zapada do %.2f V przy radiu - ogniwo do wymiany", uPod);
  else
    wynik(true, "napiecie trzyma sie przy wlaczonym radiu");
  return true;
}

/* ---------------------------------------------------------------------
 *  7. ZEGAR (NTP)
 * ------------------------------------------------------------------ */
bool testCzasu() {
  naglowek("7. Synchronizacja zegara");

  /* Te same trzy serwery co w PillBox.ino i dwa podejscia zamiast jednego.
     Pierwsze zapytanie po polaczeniu bywa gubione (DNS + UDP potrafia sie
     ociagac), a to jest stan sieci, nie usterka plytki - dlatego nieudana
     synchronizacja to UWAGA, nie blad sprzetu.                          */
  time_t t = 0;
  bool ok = false;
  for (int proba = 1; proba <= 2 && !ok; proba++) {
    Serial.printf("         proba %d z 2...\n", proba);
    configTime(0, 0, "pool.ntp.org", "time.google.com", "time.cloudflare.com");
    uint32_t start = millis();
    while (millis() - start < 15000) {
      t = time(nullptr);
      if (t > 1700000000) { ok = true; break; }
      delay(250);
    }
  }

  if (ok) {
    wynikF(true, "zegar ustawiony (epoch=%lu)", (unsigned long)t);
  } else {
    uwagaF("serwer czasu nie odpowiedzial (epoch=%lu)", (unsigned long)t);
    info("To zwykle chwilowy stan sieci, a nie usterka pudelka.");
    info("Router moze blokowac ruch NTP albo po prostu byl zajety.");
    info("Jesli w normalnej pracy widzisz w logu [NTP] OK - wszystko gra.");
  }

  if (ok) {
    struct tm tmv; time_t lokalny = t + 7200; gmtime_r(&lokalny, &tmv);
    Serial.printf("         czas lokalny (UTC+2): %04d-%02d-%02d %02d:%02d:%02d\n",
                  tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
                  tmv.tm_hour, tmv.tm_min, tmv.tm_sec);
  }
  return ok;
}

/* ---------------------------------------------------------------------
 *  8. FIREBASE  (tylko gdy jest config.h z Twoimi danymi)
 * ------------------------------------------------------------------ */
#if MAM_CONFIG
String tokenId;

/* Wyciaga wartosc pola tekstowego z odpowiedzi JSON.

   Musi byc odporne na spacje i znaki nowej linii, bo Google zwraca
   odpowiedz sformatowana do czytania: "idToken": "eyJ..." - ze spacja po
   dwukropku. Wyszukiwanie wprost ciagu "idToken":" tego nie znajduje
   i wygladalo to jak nieudane logowanie, mimo ze serwer odpowiadal
   HTTP 200 z poprawnym tokenem. Glowny firmware tego bledu nie ma, bo
   uzywa prawdziwego parsera JSON.                                      */
String wytnijPole(const String& src, const char* nazwa) {
  String klucz = String("\"") + nazwa + "\"";
  int i = src.indexOf(klucz);
  if (i < 0) return "";
  i += klucz.length();

  auto pomijBiale = [&]() {
    while (i < (int)src.length() &&
           (src[i] == ' ' || src[i] == '\t' || src[i] == '\n' || src[i] == '\r')) i++;
  };

  pomijBiale();
  if (i >= (int)src.length() || src[i] != ':') return "";
  i++;
  pomijBiale();
  if (i >= (int)src.length() || src[i] != '"') return "";
  i++;
  int j = src.indexOf('"', i);
  return (j < 0) ? "" : src.substring(i, j);
}

bool firebaseLogowanie() {
  Serial.printf("         wolna pamiec przed polaczeniem: %u B\n",
                (unsigned)ESP.getFreeHeap());

  WiFiClientSecure c; c.setInsecure();
  HTTPClient h;
  String url = String("https://identitytoolkit.googleapis.com/v1/accounts:"
                      "signInWithPassword?key=") + WEB_API_KEY;

  if (!h.begin(c, url)) {
    info("Nie udalo sie nawet otworzyc polaczenia szyfrowanego.");
    info("Najczestszy powod to brak pamieci, a nie zle haslo.");
    return false;
  }
  h.addHeader("Content-Type", "application/json");
  String body = String("{\"email\":\"") + DEVICE_EMAIL +
                "\",\"password\":\"" + DEVICE_PASSWORD +
                "\",\"returnSecureToken\":true}";
  int code = h.POST(body);
  String odp = (code > 0) ? h.getString() : String();
  h.end();

  Serial.printf("         odpowiedz serwera: HTTP %d, %d znakow\n", code, odp.length());

  if (code <= 0) {
    info("Polaczenie zerwane w trakcie - to strona sieci, nie haslo.");
    return false;
  }
  if (code != 200) {
    Serial.printf("         tresc: %s\n", odp.substring(0, 200).c_str());
    if (odp.indexOf("INVALID_PASSWORD") >= 0 || odp.indexOf("INVALID_LOGIN") >= 0)
      info("Serwer mowi wprost: zle haslo w config.h (DEVICE_PASSWORD).");
    else if (odp.indexOf("EMAIL_NOT_FOUND") >= 0)
      info("Nie ma takiego konta - sprawdz DEVICE_EMAIL w Firebase Authentication.");
    else if (odp.indexOf("API key not valid") >= 0)
      info("Zly WEB_API_KEY w config.h.");
    else if (odp.indexOf("TOO_MANY_ATTEMPTS") >= 0)
      info("Za duzo prob pod rzad - odczekaj kilka minut i powtorz.");
    return false;
  }

  tokenId = wytnijPole(odp, "idToken");
  if (tokenId.length() == 0) {
    info("Serwer odpowiedzial poprawnie, ale nie znalazlem tokenu.");
    Serial.printf("         tresc: %s\n", odp.substring(0, 200).c_str());
    return false;
  }
  String konto = wytnijPole(odp, "localId");
  Serial.printf("         token: %d znakow, konto: %s\n",
                tokenId.length(), konto.c_str());
  return tokenId.length() > 100;
}

int rtdb(const char* metoda, const String& sciezka, const String& body, String* odp) {
  WiFiClientSecure c; c.setInsecure();
  HTTPClient h;
  String url = String("https://") + RTDB_HOST + sciezka + "?auth=" + tokenId;
  if (!h.begin(c, url)) return -1;
  h.addHeader("Content-Type", "application/json");
  int code;
  if (String(metoda) == "GET") code = h.GET();
  else code = h.sendRequest(metoda, (uint8_t*)body.c_str(), body.length());
  if (odp && code > 0) *odp = h.getString();
  h.end();
  return code;
}

void testFirebase() {
  naglowek("8. Firebase");

  Serial.printf("         konto: %s\n", DEVICE_EMAIL);
  Serial.printf("         baza : %s\n", RTDB_HOST);

  if (String(DEVICE_PASSWORD) == "TUTAJ_WPISZ_HASLO_C") {
    uwagaF("DEVICE_PASSWORD w config.h nie jest uzupelnione - pomijam");
    return;
  }

  bool zalog = firebaseLogowanie();
  if (zalog) wynikF(true,  "zalogowano (token %d znakow)", tokenId.length());
  else       wynik(false, "logowanie NIE POWIODLO SIE");
  if (!zalog) {
    info("Sprawdz haslo [C] w config.h i konto w Firebase Authentication.");
    return;
  }

  String odp;
  int code = rtdb("GET", "/devices/" DEVICE_ID "/config.json", "", &odp);
  wynikF(code == 200, "odczyt ustawien z bazy (HTTP %d)", code);
  if (code == 200) Serial.printf("         %s\n", odp.substring(0, 160).c_str());

  String body = String("{\"kiedy\":") + (uint32_t)time(nullptr) + ",\"wynik\":\"test\"}";
  code = rtdb("PUT", "/devices/" DEVICE_ID "/status/selftest.json", body, nullptr);
  wynikF(code == 200, "zapis do bazy (HTTP %d)", code);
  if (code != 200)
    info("Zapis odrzucony - najczesciej przestarzale reguly bezpieczenstwa.");
}
#endif

/* ---------------------------------------------------------------------
 *  9. WYBUDZANIE ZE SNU KONTAKTRONEM
 *     Najwazniejszy test calego pudelka - i jedyny, ktory wymaga
 *     faktycznego zasniecia. Dlatego jest ostatni.
 * ------------------------------------------------------------------ */
void podsumowanie();

void testSnuStart() {
  naglowek("9. Wybudzanie ze snu kontaktronem");
  info("Teraz pudelko zasnie na 25 sekund.");
  info("RUSZ MAGNESEM, gdy tylko zgasnie port szeregowy.");
  info("Monitor portu na chwile zamilknie - to normalne.");
  Serial.flush();
  delay(2500);

  /* Zapamietujemy dotychczasowy wynik, bo deep sleep czysci zwykle zmienne. */
  iluZdanych = zdane; iluOblanych = oblane;
  etapSnu = 1;

  pinMode(PIN_REED, REED_MODE);
  gpio_pullup_en((gpio_num_t)GPIO_REED);
  gpio_pulldown_dis((gpio_num_t)GPIO_REED);

  /* Jesli pudelko jest juz otwarte, czekamy na zamkniecie - inaczej
     uklad obudzilby sie natychmiast, w petli.                          */
  bool otwarte = pudelkoOtwarte();
  esp_deepsleep_gpio_wake_up_mode_t tryb =
      otwarte ? ((REED_OPEN_LEVEL == HIGH) ? ESP_GPIO_WAKEUP_GPIO_LOW  : ESP_GPIO_WAKEUP_GPIO_HIGH)
              : ((REED_OPEN_LEVEL == HIGH) ? ESP_GPIO_WAKEUP_GPIO_HIGH : ESP_GPIO_WAKEUP_GPIO_LOW);
  Serial.printf("         czekam na: %s\n", otwarte ? "ZAMKNIECIE wieczka" : "OTWARCIE wieczka");

  esp_deep_sleep_enable_gpio_wakeup(1ULL << GPIO_REED, tryb);
  esp_sleep_enable_timer_wakeup(25ULL * 1000000ULL);

  /* Te dwie linie sa niezbedne - bez nich rdzen wlacza wewnetrzny
     rezystor do masy i kontaktron nie ma jak obudzic ukladu.
     Patrz espressif/esp-idf#12183.                                     */
  gpio_hold_en((gpio_num_t)GPIO_REED);
  gpio_deep_sleep_hold_en();

  Serial.flush();
  esp_deep_sleep_start();
}

void testSnuKoniec() {
  gpio_deep_sleep_hold_dis();
  gpio_hold_dis((gpio_num_t)GPIO_REED);
  pinMode(PIN_REED, REED_MODE);

  zdane = iluZdanych; oblane = iluOblanych;
  etapSnu = 0;

  naglowek("9. Wybudzanie ze snu - wynik");
  esp_sleep_wakeup_cause_t p = esp_sleep_get_wakeup_cause();

  uint64_t maska = 0;
#if defined(SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP) && SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
  maska = esp_sleep_get_gpio_wakeup_status();
#endif
  Serial.printf("         powod wybudzenia: %d, maska GPIO: 0x%08lX\n",
                (int)p, (unsigned long)maska);

  if (p == ESP_SLEEP_WAKEUP_GPIO) {
    bool toKontaktron = (maska & (1ULL << GPIO_REED)) || maska == 0;
    wynik(toKontaktron, "kontaktron obudzil uklad ze snu");
    if (!toKontaktron)
      info("Obudzil inny pin - sprawdz, czy nic nie zwiera przycisku.");
  } else if (p == ESP_SLEEP_WAKEUP_TIMER) {
    wynik(false, "uklad obudzil sie z timera, nie z kontaktronu");
    info("Albo nie ruszyles magnesem w ciagu 25 s (wtedy powtorz test),");
    info("albo kontaktron nie wystawia stanu wybudzajacego.");
  } else {
    uwagaF("nieoczekiwany powod wybudzenia (%d) - powtorz test", (int)p);
  }
}

/* ---------------------------------------------------------------------
 *  PODSUMOWANIE
 * ------------------------------------------------------------------ */
void podsumowanie() {
  Serial.println();
  Serial.println("========================================");
  Serial.printf ("  ZDANE: %d    BLEDY: %d    UWAGI: %d\n", zdane, oblane, uwagi);
  Serial.println("========================================");
  if (oblane == 0) {
    Serial.println("  Sprzet w porzadku. Mozesz wgrac z powrotem PillBox.ino.");
  } else {
    Serial.println("  Sa bledy - przewin w gore i poszukaj linii [BLAD].");
    Serial.println("  Przy kazdym jest podpowiedz, gdzie szukac przyczyny.");
  }
  Serial.println();
  Serial.println("  Pamietaj: to program TESTOWY. Zeby pudelko wrocilo do");
  Serial.println("  pracy, wgraj ponownie PillBox.ino.");
}

/* =====================================================================
 *  SETUP
 * ===================================================================== */
void setup() {
  Serial.begin(115200);
  delay(1500);                       // czas na otwarcie monitora portu

  pinMode(PIN_REED, REED_MODE);
  pinMode(PIN_BUTTON, BUTTON_MODE);

  /* Powrot z testu snu - dokanczamy i konczymy. */
  if (etapSnu == 1) {
    testSnuKoniec();
    podsumowanie();
    return;
  }

  Serial.println();
  Serial.println("########################################");
  Serial.println("#   PillBox - test podzespolow         #");
  Serial.println("########################################");
  Serial.printf ("  config.h: %s\n", MAM_CONFIG ? "wczytany" : "BRAK (testy sieci pominiete)");
  Serial.println("  Test trwa okolo 2 minut i wymaga Twojej reakcji.");
  Serial.println("  Miej pod reka magnes.");

  testBaterii();
  testBuzzera();
  bool kontaktronOk = testKontaktronu();
  testPrzycisku();
  testPamieci();

#if MAM_CONFIG
  if (testWifi()) {
    testCzasu();
    /* Firebase testujemy niezaleznie od zegara. Polaczenie idzie z
       wylaczonym sprawdzaniem certyfikatu, wiec zla data niczego nie psuje -
       a szkoda byloby nie sprawdzic logowania i zapisu tylko dlatego, ze
       serwer czasu akurat milczal.                                      */
    testFirebase();
  } else {
    uwagaF("bez sieci pomijam testy zegara i Firebase");
  }
#else
  naglowek("6-8. WiFi, zegar, Firebase");
  info("Pominiete - dorzuc kopie config.h do folderu PillBoxTest.");
#endif

  if (kontaktronOk) {
    testSnuStart();                  // konczy sie deep sleepem
  } else {
    naglowek("9. Wybudzanie ze snu kontaktronem");
    wynik(false, "pomijam - kontaktron nie przeszedl testu podstawowego");
    info("Napraw najpierw sam styk, potem powtorz caly test.");
    podsumowanie();
  }
}

void loop() { delay(1000); }
