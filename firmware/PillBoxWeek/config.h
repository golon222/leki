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
#define FW_VERSION          "0.13.0"         // widoczna w aplikacji

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

/*  Czujnik naladowania MAX17048 (Adafruit, STEMMA QT) na I2C.
 *  D4 i D5 to domyslna magistrala XIAO ESP32-C3 i obie byly wolne.     */
#define PIN_SDA             6      // D4
#define PIN_SCL             7      // D5

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

/* ---------------------------------------------------------------------
 * 6a. CZUJNIK NALADOWANIA  (MAX17048)
 *
 *  Uklad siedzi na ogniwie i liczy stan naladowania wlasnym modelem
 *  LiPo - znacznie uczciwiej, niz da sie to wyliczyc z jednego napiecia.
 *  Podaje gotowy procent i napiecie przez I2C.
 *
 *  JEST OPCJONALNY i to jest wazne: przy braku czujnika na magistrali
 *  pudelko wraca do dzielnika, a przy zepsutym dzielniku - do uczciwego
 *  "nie wiem". Ten sam program chodzi na plytce z czujnikiem i bez (D139).
 * ------------------------------------------------------------------ */
#define GAUGE_ENABLED       1
#define GAUGE_ADDR          0x36   // staly adres MAX17048/MAX17043

/*  Granice zdrowego rozsadku dla odczytu z czujnika. Poza nimi uznajemy,
 *  ze czujnik nie jest jeszcze gotowy (po wlaczeniu potrzebuje chwili)
 *  albo odpowiada smieciami - i schodzimy na dzielnik.                 */
#define GAUGE_MIN_V         2.0f
#define GAUGE_MAX_V         5.0f

/*  LADOWANIE POZNAJEMY PO TEMPIE, NIE PO NAPIECIU  (D143)
 *
 *  MAX17048 ma rejestr CRATE (0x16): tempo zmiany naladowania w %/h,
 *  ze znakiem. Ladowanie ogniwa 470 mAh pradem z USB to kilkadziesiat
 *  %/h, a pudelko spiace rozladowuje sie w tempie ulamka %/h - wiec
 *  prog 2 %/h jest daleko od obu i nie da sie go przypadkiem przekroczyc.
 *
 *  Pudelko dzienne zgaduje ladowanie ze WZROSTU napiecia miedzy
 *  wybudzeniami, bo nie ma czym zmierzyc tempa. Tutaj nie zgadujemy.
 *
 *  Gorna granica rozsadku: 17043 (ten sam adres, inny uklad) nie ma
 *  tego rejestru i odda smieci. Powyzej tej wartosci uznajemy, ze
 *  tempa nie znamy, zamiast meldowac ladowanie, ktorego nie ma.      */
#define GAUGE_CRATE_PROG    2.0f    // %/h - powyzej tego mowimy "laduje sie"
#define GAUGE_CRATE_MAX     300.0f  // %/h - powyzej tego to nie jest pomiar

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
 * 8. POWIADOMIENIA NA TELEFON  (bot Telegram)
 *
 *  Dzwonek slychac w pokoju, wiadomosc dociera wszedzie. Wysyla je
 *  PUDELKO, nie aplikacja: telefon spi razem z wlascicielem, a iOS nie
 *  budzi stron dodanych do ekranu glownego.
 *
 *  TOKENU BOTA TU NIE MA I BYC NIE MOZE - z tego samego powodu co hasla
 *  do bazy (ograniczenie 3 i 10). Przychodzi z aplikacji przez baze
 *  i mieszka w pamieci trwalej pudelka.
 * ------------------------------------------------------------------ */
#define TG_ENABLED          1
#define TG_HOST             "api.telegram.org"

/*     Po tylu sekundach powiadomienie przestaje miec sens i KASUJEMY je
 *     zamiast wysylac. To jedyny wyjatek od zasady 6 w tym obszarze -
 *     i nie dotyczy zadnych danych o leku, tylko przypomnienia, ktore
 *     przyszloby trzy godziny za pozno.                               */
#define TG_MAX_WIEK_S       10800           // 3 h

/*     Powyzej tego poziomu wolno znow ostrzec o baterii. Bez tego progu
 *     druga wiadomosc nie przyszlaby nigdy, a ogniwo rozladuje sie
 *     jeszcze wiele razy.                                             */
#define TG_BATT_RESET_PCT   50

#define TG_TOKEN_MAX        64              // tyle znakow ma token z BotFathera
#define TG_CHAT_MAX         24              // id czatu to liczba, czasem ujemna

/* ---------------------------------------------------------------------
 * 9. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)
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

/*     PROG BATERII ZNIESIONY - i to jest decyzja, nie przeoczenie (D138).
 *
 *     W pudelku dziennym stoi tu 25: ponizej tego poziomu aktualizacja
 *     rusza tylko na ladowarce, bo 1-2 mAh na wyczerpanym ogniwie to
 *     pozyczka. Tam ma to sens, bo tam pomiar baterii DZIALA.
 *
 *     Tutaj nie dziala i nie wiemy dlaczego (D136): plytka melduje 2,32 V,
 *     czyli wartosc, przy ktorej by nie chodzila. Prog oparty na liczbie,
 *     ktorej nie rozumiemy, nie jest zabezpieczeniem - jest LOTERIA:
 *     przy odczycie niemozliwym (-1) nie zadziala wcale, a gdyby odczyt
 *     wyladowal kiedys tuz nad progiem czulosci, zablokowalby aktualizacje
 *     bez powodu.
 *
 *     A stawka jest asymetryczna. Zle zablokowana aktualizacja odcina
 *     JEDYNA zdalna droge naprawy tego pudelka - zostaje kabel i moj
 *     komputer, a pudelko stoi u kogos innego. Zle przepuszczona kosztuje
 *     1-2 mAh.
 *
 *     PRZYWROC 25, gdy pomiar baterii zostanie wyjasniony - i dopiero
 *     wtedy. Kontrola statyczna pilnuje, ze te dwie rzeczy chodza razem. */
#define OTA_MIN_BATT_PCT    0

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
