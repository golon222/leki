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
#define FW_VERSION          "0.5.0"         // widoczna w aplikacji

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

/*  Ponizej tego napiecia plytka po prostu nie chodzi - wiec taki odczyt
 *  znaczy "dzielnik nie ma kontaktu", a nie "bateria pusta". Zglaszamy
 *  wtedy brak danych zamiast zerowego procentu (B30).                  */
/*  Hamulec na petle wybudzen. Pudelko, ktore budzi sie z pinu raz za razem
 *  i za kazdym razem nie ma czego zglosic, jest zepsute - wszystko jedno
 *  z jakiego powodu. Po tylu pustych wybudzeniach z rzedu rozbrajamy pin
 *  na chwile, zeby petla nie zjadla ogniwa (B30).                       */
#define PUSTE_WYBUDZENIA_MAX 3
#define PO_PUSTYCH_SEN_S     300

#define BATT_MIN_SENS_V     3.00f

#define BATT_WARN_PCT       15     // ponizej tego ostrzezenie dzwiekowe
#define KOLEJKA_MAX         40     // ile zdarzen czeka na siec

/*  Ile sekund pudelko probuje zlapac siec. Dluzej nie ma sensu:
 *  zdarzenie i tak trafi do kolejki i pojdzie przy nastepnym wybudzeniu. */
#define WIFI_TIMEOUT_S      20

/* ---------------------------------------------------------------------
 * 7. PORTAL KONFIGURACJI WiFi
 *
 *  Pudelko tworzy wlasna siec, telefon laczy sie z nia i otwiera strone
 *  z lista sieci. Jedyna droga do zmiany WiFi, ktora nie potrzebuje ani
 *  sieci, ani bazy, ani kabla - a pudelko stoi u kogos innego.
 *
 *  AP_PASS musi miec co najmniej 8 znakow (wymog WPA2). Jest jawne
 *  w repozytorium i tak ma zostac: zabezpiecza siec, ktora zyje kilka
 *  minut, stoi na wyciagniecie reki i niczego nie udostepnia. Prawdziwa
 *  bariera to reguly bazy i haslo urzadzenia, nie to.
 * ------------------------------------------------------------------ */
#define PORTAL_ENABLED      1
#define AP_SSID             "Pudelko-na-leki"
#define AP_PASS             "pudelko123"
#define PORTAL_TIMEOUT_S    300    // po tylu sekundach bez nikogo zamykamy AP

/*  Ile trzymac przycisk przy zimnym starcie, zeby wejsc w portal zamiast
 *  w autotest. Krotkie nacisniecie zostaje autotestem - do sprawdzenia
 *  pudelka na sluch, juz zamknietego w obudowie.                      */
#define PORTAL_HOLD_MS      3000

/*  Przycisk zwarty na stale budzilby uklad w kolko. Po tylu wybudzeniach
 *  z rzedu, przy ktorych styk nadal jest zwarty, przestajemy go uzbrajac
 *  - dokladnie tak samo jak pusta klapka (B30).                        */
#define PRZYCISK_ZWARTY_MAX 3

/* ---------------------------------------------------------------------
 * 8. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)
 *
 *  Binarke buduje automat na GitHubie i kladzie ja obok aplikacji na
 *  GitHub Pages. Pudelko pobiera najpierw MALY plik z opisem, a caly
 *  program dopiero wtedy, gdy suma kontrolna rozni sie od tej, ktora juz
 *  ma. Rozstrzyga SUMA, nie numer wersji: numer pisze czlowiek i da sie
 *  go zapomniec podbic, suma liczy sie z pliku i sklamac nie umie.
 *
 *  NAZWY PLIKOW SA INNE NIZ W PUDELKU DZIENNYM i to nie jest kosmetyka:
 *  wgranie sobie nawzajem programow konczy sie dwoma cegłami. Pudelko
 *  dzienne pobiera PillBox.bin, tygodniowe PillBoxWeek.bin.
 * ------------------------------------------------------------------ */
#define OTA_ENABLED         1
#define OTA_BASE_URL        "https://golon222.github.io/leki/firmware/"
#define OTA_JSON_FILE       "PillBoxWeek.json"  // opis wersji: kilkaset bajtow
#define OTA_BIN_FILE        "PillBoxWeek.bin"   // sam program: ~1,1 MB

/*     Ponizej tego progu tylko na ladowarce - a poniewaz to pudelko nie
 *     wie, czy na niej stoi, prog obowiazuje zawsze. Sama aktualizacja to
 *     okolo 1-2 mAh, ale na wyczerpanym ogniwie kazdy grosz jest pozyczka. */
#define OTA_MIN_BATT_PCT    25

/*     Po tylu nieudanych probach z rzedu pudelko przestaje samo probowac.
 *     Zdejmuje to dopiero SWIEZE zlecenie z aplikacji - czyli swiadoma
 *     prosba czlowieka, a nie petla urzadzenia.                        */
#define OTA_MAX_FAILS       3

/*     Zdrowy rozsadek co do rozmiaru. Plik mniejszy to prawie na pewno
 *     nie program (np. strona bledu 404 zapisana jako plik), a wiekszy
 *     nie zmiesci sie w partycji.                                      */
#define OTA_MIN_BIN_SIZE    300000
#define OTA_MAX_BIN_SIZE    1900000

/*     Limit na POJEDYNCZY odczyt ze strumienia. Musi miescic sie
 *     w uint16_t - `HTTPClient::setTimeout()` bierze wlasnie tyle
 *     i wieksza wartosc obcina sie po cichu.                           */
#define OTA_HTTP_READ_MS    30000

/*     Ile razy nowy program moze wystartowac i NIE dojsc do zasniecia,
 *     zanim uznamy go za zepsuty i wrocimy na poprzednia partycje.     */
#define OTA_BOOT_TRIES      3
