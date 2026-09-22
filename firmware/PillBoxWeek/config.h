/* ############################################################
   #  UWAGA: TO JEST WERSJA Z REPOZYTORIUM - BEZ HASEL.       #
   #  Przed wgraniem na plytke wpisz DEVICE_PASSWORD          #
   #  oraz siec WiFi. NIGDY nie wysylaj tu wersji z haslem.   #
   ############################################################ */
/* =====================================================================
 *  config.h  -  Pudelko TYGODNIOWE na leki (XIAO ESP32-C3)
 *
 *  Siedem klapek, jedna zasada: OTWARTE = WZIETE.
 *  Drugi plik to PillBoxWeek.ino - kod programu.
 * ===================================================================== */
#pragma once

#define PILLBOX_WEEK_CONFIG_VERSION 1

/* ---------------------------------------------------------------------
 * 1. IDENTYFIKATOR URZADZENIA
 *    MUSI byc inny niz pudelka dziennego, inaczej oba pisalyby w to samo
 *    miejsce w bazie i kalendarze wymieszalyby sie po cichu.
 * ------------------------------------------------------------------ */
#define DEVICE_ID           "pillbox02"     // klucz w /devices/<DEVICE_ID>
#define FW_VERSION          "0.1.0"         // widoczna w aplikacji

/* ---------------------------------------------------------------------
 * 2. FIREBASE
 *    Konto zakladasz w Authentication OSOBNO dla tego pudelka.
 *    WEB_API_KEY jest ten sam co w aplikacji - jest publiczny, a bariera
 *    to reguly bazy, nie jego tajnosc.
 * ------------------------------------------------------------------ */
#define RTDB_HOST           "pudelko-na-leki-default-rtdb.europe-west1.firebasedatabase.app"
#define WEB_API_KEY         "AIzaSyD7YwKvgn8PmqNKcxUEPdc8i6oJShgOkKg"
#define DEVICE_EMAIL        "pillbox02@device.local"

/*  Haslo konta pillbox02@device.local.
 *
 *  W REPOZYTORIUM STOI TU PLACEHOLDER I TAK MA ZOSTAC. Wpisujesz haslo
 *  TYLKO na swoim komputerze, przed wgraniem kablem. Po pierwszym udanym
 *  logowaniu pudelko przepisuje je do wlasnej pamieci nieulotnej (NVS)
 *  i od tej pory bierze je stamtad - dokladnie jak pudelko dzienne.    */
#define DEVICE_PASSWORD     "TUTAJ_WPISZ_HASLO"

/* ---------------------------------------------------------------------
 * 3. SIEC WiFi
 *    Na tym etapie jedna siec wpisana na sztywno. Portal konfiguracyjny
 *    i lista sieci dojda pozniej, tak jak w pudelku dziennym.
 * ------------------------------------------------------------------ */
#define WIFI_SSID           "TUTAJ_WPISZ_SIEC"
#define WIFI_PASS           "TUTAJ_WPISZ_HASLO_WIFI"

/* ---------------------------------------------------------------------
 * 4. PINY   (zmierzone i potwierdzone na plytce)
 * ------------------------------------------------------------------ */
#define PIN_KLAPKI          3      // D1 - drabinka rezystorowa, ADC + wybudzanie
#define PIN_BUZZER          4      // D2 - piezo pasywny
#define PIN_PRZYCISK        5      // D3 - przycisk do masy
#define PIN_BATERIA         2      // D0 - dzielnik 100k/100k z BAT+

/* ---------------------------------------------------------------------
 * 5. PROGI DRABINKI  -  Z POMIARU, NIE Z OBLICZEN
 *
 *  Zmierzone na plytce 2026-09-22. Kalibracje powtarzasz szkicem
 *  TestPudelka (komenda 'k'), gdy przelutujesz ktorykolwiek rezystor.
 *
 *  Kolejnosc: ponizej PROGI[0] = kilka klapek naraz (napelnianie),
 *  powyzej PROGI[7] = wszystko zamkniete.
 * ------------------------------------------------------------------ */
#define PROGI_KLAPEK        { 72, 137, 235, 344, 472, 595, 697, 943 }

/*  Rezonans piezo. Sprawdz skanem czestotliwosci (TestPudelka, 'f') -
 *  rozni sie miedzy egzemplarzami nawet o kilkaset Hz, a przy
 *  przypomnieniu o leku glosnosc jest cala funkcja.                   */
#define BUZZER_HZ           2700

/* ---------------------------------------------------------------------
 * 6. ZACHOWANIE
 * ------------------------------------------------------------------ */
#define DAY_START_HOUR      3      // granica doby lekowej - TA SAMA co w aplikacji
#define ALARM_POWTORZEN     6      // ile razy piknac w jednym przypomnieniu
#define ALARM_PRZERWA_MS    2500   // odstep miedzy piknieciami
#define ALARM_PONOWIEN      3      // ile razy wrocic, jesli nikt nie otworzyl
#define ALARM_PONOW_MIN     10     // co ile minut wracac

/*  Klapka zostawiona otwarta trzyma pin nisko, a wybudzanie reaguje na
 *  POZIOM. Bez czekania na zamkniecie pudelko budziloby sie w kolko. */
#define CZEKAJ_ZAMKNIECIE_S 60     // ile czekac na zamkniecie klapki
#define SEN_PRZY_OTWARTEJ_S 120    // sen na sam zegar, gdy nadal otwarta

#define BATT_WARN_PCT       15     // ponizej tego ostrzezenie dzwiekowe
#define KOLEJKA_MAX         40     // ile zdarzen czeka na siec

/*  Ile sekund pudelko probuje zlapac siec. Dluzej nie ma sensu:
 *  zdarzenie i tak trafi do kolejki i pojdzie przy nastepnym wybudzeniu. */
#define WIFI_TIMEOUT_S      20
