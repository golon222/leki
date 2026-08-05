#pragma once

/* ---------------------------------------------------------------------
 * 1. IDENTYFIKATOR URZADZENIA
 * ------------------------------------------------------------------ */
#define DEVICE_ID           "pillbox01"     // klucz w /devices/<DEVICE_ID>
#define FW_VERSION          "1.0.0"

/* ---------------------------------------------------------------------
 * 2. FIREBASE  (Realtime Database + Auth email/haslo)
 *    - RTDB_HOST : bez https:// i bez ukosnika na koncu
 *    - WEB_API_KEY: Firebase Console -> Ustawienia projektu -> Web API Key
 *    - DEVICE_EMAIL/PASS: konto utworzone w Authentication tylko dla plytki
 * ------------------------------------------------------------------ */
#define RTDB_HOST           "pudelko-na-leki-default-rtdb.europe-west1.firebasedatabase.app"
#define WEB_API_KEY         "AIzaSyD7YwKvgn8PmqNKcxUEPdc8i6oJShgOkKg"
#define DEVICE_EMAIL        "pillbox01@device.local"

/*  <<<<<<<<<<  DO UZUPELNIENIA  >>>>>>>>>>
 *  Wpisz haslo [C] - to, ktore wymysliles zakladajac konto pillbox01@device.local
 *  w Firebase Authentication. Bez niego plytka sie nie zaloguje (HTTP 400).      */
#define DEVICE_PASSWORD     "TUTAJ_WPISZ_HASLO_C"

/* ---------------------------------------------------------------------
 * 3. PINY  (nazwy D0..D3 sa zdefiniowane przez plytke "XIAO_ESP32C3")
 *    D0 = GPIO2 (ADC1_CH2)  - pomiar baterii przez dzielnik 100k/100k
 *    D1 = GPIO3             - kontaktron (wake z deep sleep)
 *    D2 = GPIO4             - pasywny buzzer piezo (LEDC PWM)
 *    D3 = GPIO5             - przycisk (wake z deep sleep -> portal WiFi)
 *    UWAGA: na ESP32-C3 z deep sleep moga budzic TYLKO GPIO0..GPIO5.
 * ------------------------------------------------------------------ */
#define PIN_BATTERY         D0
#define PIN_REED            D1
#define PIN_BUZZER          D2
#define PIN_BUTTON          D3

#define GPIO_REED           3               // numer GPIO dla maski wybudzania
#define GPIO_BUTTON         5

/* ---------------------------------------------------------------------
 * 4. POLARYZACJA WEJSC  -  zgodna z Twoim zlutowanym schematem
 *
 *      REED   : D1 --[kontaktron]-- GND, podciagniecie do 3V3
 *               magnes przy zakretce (pudelko ZAMKNIETE) -> styk zwarty -> LOW
 *               pudelko OTWARTE -> styk rozwarty -> HIGH  => wybudzenie
 *      BUTTON : D3 --[przycisk]-- GND, podciagniecie do 3V3
 *               zwolniony -> HIGH,  wcisniety -> LOW      => wybudzenie
 *
 *    ESP32-C3 pozwala ustawic poziom wybudzania OSOBNO dla kazdego pinu
 *    (dwa wywolania esp_deep_sleep_enable_gpio_wakeup) - dlatego rozne
 *    poziomy nie sa problemem i nic nie trzeba przelutowywac.
 * ------------------------------------------------------------------ */
#define REED_OPEN_LEVEL     HIGH            // stan pinu gdy pudelko OTWARTE
#define BUTTON_PRESS_LEVEL  LOW             // stan pinu gdy przycisk WCISNIETY

/*    USE_INTERNAL_PULLS:
 *      1 = wewnetrzne pull-up (~45 kOhm). Dziala od reki, ale przy
 *          ZAMKNIETYM pudelku kontaktron zwiera je do masy -> ~73 uA non stop.
 *      0 = zewnetrzny rezystor 1 MOhm z D1 do 3V3 (i opcjonalnie z D3 do 3V3).
 *          Pobor spada do ~3 uA. Jeden rezystor = ok. 2x dluzsza praca.
 * ------------------------------------------------------------------ */
#define USE_INTERNAL_PULLS  1
#if USE_INTERNAL_PULLS
  #define REED_MODE         INPUT_PULLUP
  #define BUTTON_MODE       INPUT_PULLUP
#else
  #define REED_MODE         INPUT           // rezystory 1M zewnetrzne
  #define BUTTON_MODE       INPUT_PULLUP    // przycisk moze zostac wewnetrzny:
#endif                                      // rozwarty w spoczynku = 0 uA

/* ---------------------------------------------------------------------
 * 5. ALARM
 * ------------------------------------------------------------------ */
#define BUZZER_FREQ_HZ      2700            // rezonans typowego piezo 23mm
#define BEEP_MS             180
#define BEEPS_PER_BURST     3
#define BURST_GAP_MS        4000            // przerwa miedzy seriami piknięć
#define ALARM_WINDOW_S      120             // ile sekund dzwoni jedna proba
#define SNOOZE_S            300             // przerwa miedzy probami (5 min)
#define MAX_ALARM_RETRIES   3               // po tylu probach -> "missed"

/* ---------------------------------------------------------------------
 * 6. HARMONOGRAM DOMYSLNY (uzywany zanim aplikacja przysle wlasny)
 *    Format "HH:MM", oddzielone '|'.  Ty bierzesz raz dziennie.
 * ------------------------------------------------------------------ */
#define DEFAULT_SCHEDULE    "20:00"
#define DEFAULT_TZ_OFFSET   120             // minuty od UTC (PL lato = 120)

/* ---------------------------------------------------------------------
 * 6b. TRYB "RAZ DZIENNIE"
 *     ONE_DOSE_PER_DAY = 1 -> liczy sie TYLKO pierwsze otwarcie w danym dniu.
 *     Kazde kolejne otwarcie tego samego dnia = ostrzegawcze pikniecie
 *     ("juz brales, nie bierz drugi raz") i ZERO zapisu do Firebase.
 * ------------------------------------------------------------------ */
#define ONE_DOSE_PER_DAY    1
#define LOW_STOCK_WARN      7               // ostrzegaj, gdy zostalo < X tabletek

/* ---------------------------------------------------------------------
 * 7. OKNA CZASOWE
 * ------------------------------------------------------------------ */
#define MATCH_WINDOW_MIN    90              // +/- minut: otwarcie liczy sie do dawki
#define WIFI_TIMEOUT_MS     15000           // dluzej nie czekamy - idziemy spac
/* ---------------------------------------------------------------------
 * 7a. PORTAL KONFIGURACJI WiFi
 *     Po wcisnieciu ukrytego przycisku pudelko tworzy wlasna siec WiFi.
 *     Laczysz sie z nia telefonem, otwiera sie strona z lista sieci.
 *     Haslo musi miec MINIMUM 8 znakow - wymog WPA2.
 * ------------------------------------------------------------------ */
#define AP_SSID             "PillBox-setup"
#define AP_PASS             "pillbox123"
#define PORTAL_TIMEOUT_S    300             // 5 minut na wpisanie hasla

#define HOUSEKEEP_MAX_S     43200           // max 12h snu (sync czasu i baterii)
                                            // mniej = dokladniejszy zegar, wiecej pradu

/* Twardy limit czuwania. Gdyby cokolwiek sie zablokowalo, petle oczekiwania
   przerywaja sie i uklad idzie spac zamiast zjadac bateria.               */
#define AWAKE_LIMIT_MS      150000          // 2,5 minuty

/* ---------------------------------------------------------------------
 * 7b. PONAWIANIE WYSYLKI PO AWARII WiFi
 *     Gdy w kolejce sa zaleglosci, pudelko budzi sie wczesniej niz
 *     wynikaloby z harmonogramu: 15 min, 30, 60, 120, 240 min (max).
 * ------------------------------------------------------------------ */
#define RETRY_BASE_S        900             // pierwsza ponowna proba po 15 min
#define RETRY_MAX_S         14400           // nie rzadziej niz co 4 h

/* ---------------------------------------------------------------------
 * 7c. ROLOWANIE DOBY
 *     Wybudzenie minute po polnocy: jesli wczorajsza dawka nie zostala
 *     wzieta, do kolejki trafia zdarzenie "missed". BEZ wlaczania WiFi -
 *     poleci przy najblizszym polaczeniu. Koszt: ~0,006 mAh na dobe.
 * ------------------------------------------------------------------ */
#define MIDNIGHT_CHECK      1

/* ---------------------------------------------------------------------
 * 7d. OCHRONA OGNIWA LiPo
 *     Ponizej BATT_SAFE_V pudelko przestaje wlaczac WiFi (loguje offline).
 *     Ponizej BATT_CUTOFF_V zasypia na dobre - budzi tylko przycisk.
 *     Odczyty < 2,0 V traktowane sa jako "brak baterii / zasilanie z USB".
 * ------------------------------------------------------------------ */
#define BATT_SAFE_V         3.35f
#define BATT_CUTOFF_V       3.20f
#define BATT_RECOVER_V      3.50f           // powyzej tego wracamy do normalnej pracy
#define CUTOFF_RECHECK_S    900             // co 15 min sprawdzamy, czy podlaczono ladowarke
#define CHARGE_RISE_V       0.08f           // wzrost napiecia = wykryte ladowanie

/* ---------------------------------------------------------------------
 * 7e. DZWIEKOWE OSTRZEZENIE O BATERII (bez zagladania do aplikacji)
 *     Po kazdym potwierdzeniu dawki, jesli stan spadl ponizej progu,
 *     pudelko dodaje charakterystyczny sygnal. Ponizej progu krytycznego
 *     sygnal jest dluzszy i powtarzany takze przy alarmie.
 * ------------------------------------------------------------------ */
#define BATT_WARN_PCT       25              // "naladuj mnie w tym tygodniu"
#define BATT_CRIT_PCT       10              // "naladuj mnie dzis"

/* ---------------------------------------------------------------------
 * 8. KOLEJKA OFFLINE
 *     120 wpisow = ok. 2 miesiace przy jednej dawce dziennie.
 * ------------------------------------------------------------------ */
#define QUEUE_CAPACITY      120             // ile zdarzen miesci sie w NVS
#define NVS_NAMESPACE       "pillbox"

/* ---------------------------------------------------------------------
 * 9. DIAGNOSTYKA
 *    Ustaw 0 przed finalnym montazem - Serial przy USB kosztuje prad.
 * ------------------------------------------------------------------ */
#define DEBUG_SERIAL        1

#if DEBUG_SERIAL
  #define LOG(...)   Serial.printf(__VA_ARGS__)
  #define LOGLN(x)   Serial.println(x)
#else
  #define LOG(...)
  #define LOGLN(x)
#endif

