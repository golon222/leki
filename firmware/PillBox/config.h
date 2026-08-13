/* ############################################################
   #  UWAGA: TO JEST WERSJA Z REPOZYTORIUM - BEZ HASLA.       #
   #  Przed wgraniem na plytke wpisz haslo w DEVICE_PASSWORD. #
   #  NIGDY nie wysylaj tu z powrotem wersji z haslem.        #
   ############################################################ */
/* =====================================================================
 *  config.h  -  Inteligentne pudelko na leki (XIAO ESP32-C3)
 *
 *  TO JEST PLIK USTAWIEN. Drugi plik to PillBox.ino - kod programu.
 *  Jesli widzisz tu "#include <WiFi.h>" albo "void setup()", to znaczy
 *  ze pliki sie pomieszaly - pobierz oba na nowo.
 * ===================================================================== */
#pragma once

/*  Znacznik dla PillBox.ino - pozwala wykryc, ze config.h ma wlasciwa
 *  tresc. Nie usuwaj tej linii.                                       */
#define PILLBOX_CONFIG_VERSION 2

/* ---------------------------------------------------------------------
 * 1. IDENTYFIKATOR URZADZENIA
 * ------------------------------------------------------------------ */
#define DEVICE_ID           "pillbox01"     // klucz w /devices/<DEVICE_ID>
#define FW_VERSION          "1.39.3"   // widoczna w aplikacji - po wgraniu sprawdz, czy sie zmienila

/* ---------------------------------------------------------------------
 * 2. FIREBASE  (Realtime Database + Auth email/haslo)
 *    - RTDB_HOST : bez https:// i bez ukosnika na koncu
 *    - WEB_API_KEY: Firebase Console -> Ustawienia projektu -> Web API Key
 *    - DEVICE_EMAIL/PASS: konto utworzone w Authentication tylko dla plytki
 * ------------------------------------------------------------------ */
#define RTDB_HOST           "pudelko-na-leki-default-rtdb.europe-west1.firebasedatabase.app"
#define WEB_API_KEY         "AIzaSyD7YwKvgn8PmqNKcxUEPdc8i6oJShgOkKg"
#define DEVICE_EMAIL        "pillbox01@device.local"

/*  Haslo konta pillbox01@device.local z Firebase Authentication.
 *
 *  UWAGA: ten plik JEST w repo, ale wylacznie z placeholderem powyzej.
 *  Wersji z wpisanym haslem NIGDY nie wrzucaj z powrotem na GitHuba.
 *
 *  OD 1.38.0 TO JEST TYLKO ZIARNO, NIE JEDYNE ZRODLO (D59).
 *  Przy pierwszym udanym logowaniu haslo przepisuje sie do pamieci
 *  trwalej (NVS, klucz "fbpass") i od tej pory pudelko czyta je STAMTAD.
 *  Dzieki temu binarke aktualizacji moze zbudowac automat z tego repo -
 *  z placeholderem, bez znajomosci hasla - a pudelko i tak sie zaloguje.
 *  Bez tego kazda aktualizacja przez WiFi odcinalaby je od bazy.
 *
 *  Wgrywasz kablem dokladnie tak jak dotad: wpisujesz tutaj prawdziwe
 *  haslo. Roznica jest tylko taka, ze robisz to OSTATNI raz.            */
#define DEVICE_PASSWORD     "TUTAJ_WPISZ_HASLO"
/*  ^ TO JEST JEDYNE MIEJSCE, W KTORYM WPISUJESZ HASLO.                 */

/* ---------------------------------------------------------------------
 * 3. PINY  (nazwy D0..D3 sa zdefiniowane przez plytke "XIAO_ESP32C3")
 *    D0 = GPIO2 (ADC1_CH2)  - pomiar baterii przez dzielnik 100k/100k
 *    D1 = GPIO3             - kontaktron (wake z deep sleep)
 *    D2 = GPIO4             - pasywny buzzer piezo (LEDC PWM)
 *    D3 = GPIO5             - przycisk (portal WiFi: trzymaj przy RESECIE)
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
 *    Ze snu budzi WYLACZNIE kontaktron. Maska wybudzania ma jeden wspolny
 *    poziom wyzwalania, a te dwa piny potrzebuja przeciwnych (kontaktron
 *    stanu WYSOKIEGO, przycisk NISKIEGO), wiec obu naraz uzbroic sie nie da.
 *    Portal WiFi ma zamiast tego wlasny gest: przytrzymaj przycisk i nacisnij
 *    RESET - dziala bez deep sleep, wiec nie potrzebuje wybudzania.
 * ------------------------------------------------------------------ */
#define REED_OPEN_LEVEL     HIGH            // stan pinu gdy pudelko OTWARTE
#define BUTTON_PRESS_LEVEL  LOW             // stan pinu gdy przycisk WCISNIETY

/*    USE_INTERNAL_PULLS:
 *      1 = wewnetrzne pull-up (~45 kOhm). Dziala od reki, ale przy
 *          ZAMKNIETYM pudelku kontaktron zwiera je do masy -> ~73 uA non stop.
 *      0 = zewnetrzny rezystor 1 MOhm z D1 do 3V3 (i opcjonalnie z D3 do 3V3).
 *          Pobor spada do ~3 uA. Jeden rezystor = ok. 2x dluzsza praca.
 * ------------------------------------------------------------------ */
/*    UWAGA do wybudzania ze snu: rdzen ESP32-C3 tuz przed zasnieciem sam
 *    przestawia podciagniecia na pinie wybudzania - dla pinu czekajacego na
 *    stan WYSOKI wlacza rezystor ~10 kOhm DO MASY. To zabijalo kontaktron:
 *    wewnetrzne 10 kOhm wygrywalo z zewnetrznymi 100 kOhm i pin zostawal na
 *    ~0,4 V zamiast 3,3 V, wiec pudelko nigdy sie nie budzilo.
 *    PillBox.ino obchodzi to przez gpio_hold_en() tuz przed snem - patrz
 *    komentarz w goToSleep() i zgloszenie espressif/esp-idf#12183.
 *    Dzieki temu ponizsze ustawienie dziala tak, jak sie tego spodziewasz. */
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

/* Ochrona przed druga dawka BEZ WIARYGODNEGO ZEGARA.
   Pudelko po resecie i bez internetu nie wie, ktory jest dzien - ale
   nadal potrafi zmierzyc, ILE CZASU minelo od poprzedniej dawki, bo
   zegar RTC tyka takze w deep sleepie. Jesli minelo mniej niz tyle,
   traktujemy otwarcie jako podejrzenie powtorki i ostrzegamy.
   20 h, a nie 24: przy braniu o roznych porach doba lekowa i tak sie
   przesuwa, a falszywe ostrzezenie kosztuje tu jedno pikniecie.     */
#define ONE_DOSE_WINDOW_S   72000
#define LOW_STOCK_WARN      7               // ostrzegaj, gdy zostalo < X tabletek

/* Ile zaleglych dob pudelko domyka po dluzszej przerwie (rozladowane
   ogniwo, wyjazd). Wiecej wypchneloby z 120-elementowej kolejki prawdziwe
   dawki, a przerwa dluzsza niz to i tak znaczy "urzadzenie nie dzialalo",
   a nie "ktos nie wzial leku".                                        */
#define MAX_ROLLOVER_DNI    7

/* ---------------------------------------------------------------------
 * 6c. GRANICA DOBY LEKOWEJ
 *     O ktorej godzinie zaczyna sie "nowy dzien" z punktu widzenia leku.
 *     3 oznacza: otwarcie o 01:30 czy 02:00 liczy sie jeszcze do dnia
 *     POPRZEDNIEGO. Dzieki temu tabletka wzieta po polnocy nie wyglada
 *     jak podwojna dawka nastepnego dnia, a wczorajszy dzien nie zostaje
 *     oznaczony jako pominiety, dopoki naprawde nie minie okno.
 *
 *     UWAGA: ta sama liczba musi byc ustawiona w aplikacji
 *     (index.html w korzeniu repo, stala DAY_START_HOUR). Inaczej pudelko
 *     liczylyby doby inaczej i kalendarz by sie rozjechal.
 *
 *     Dopuszczalny zakres 0-6. Wartosc 0 = klasyczna polnoc.
 * ------------------------------------------------------------------ */
#define DAY_START_HOUR      3

/* ---------------------------------------------------------------------
 * 6d. PUDELKO ZOSTAWIONE OTWARTE
 *     Po OPEN_WARN_FIRST_S od otwarcia pudelko daje jeden dluzszy sygnal,
 *     a potem powtarza go co OPEN_WARN_REPEAT_S, dopoki wieczko nie
 *     zostanie zamkniete. Ton jest rowny i pojedynczy, wiec nie da sie go
 *     pomylic z potwierdzeniem dawki ani z ostrzezeniem "juz brales".
 *
 *     OPEN_WARN_MAX chroni przed sytuacja, w ktorej magnes sie przesunal
 *     albo kontaktron sie zaklinowal: po tylu sygnalach pudelko milknie,
 *     ale dalej sprawdza stan i informuje aplikacje. Domyslnie 24 razy
 *     po 30 minut, czyli okolo 12 godzin dzwonienia.
 * ------------------------------------------------------------------ */
#define OPEN_WARN_FIRST_S   900             // pierwszy sygnal po 15 min
#define OPEN_WARN_REPEAT_S  1800            // potem co 30 min
#define OPEN_WARN_MAX       24              // po tylu sygnalach juz tylko cisza
#define OPEN_WARN_TONE_HZ   2000            // rowny ton, inny niz pozostale
#define OPEN_WARN_MS        500             // dlugosc sygnalu
#define OPEN_REPORT_APP     1               // pokazywac otwarcie w aplikacji

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

/* ---------------------------------------------------------------------
 * 7a2. LISTA ZNANYCH SIECI
 *     Pudelko pamieta kilka sieci, nie jedna. Po co: siec da sie wtedy
 *     dodac Z APLIKACJI, a nie tylko portalem przy otwartym pudelku.
 *
 *     Paradoks jest pozorny. Zeby pudelko odebralo nowa siec, musi miec
 *     lacznosc - wiec na liscie siedzi na stale HOTSPOT Z TELEFONU. Na
 *     wyjezdzie wystarczy go wlaczyc: pudelko sie z nim laczy, odbiera
 *     siec hotelowa i od tej pory dziala juz przez nia.
 *
 *     Kolejnosc prob zaczyna sie od tej, ktora zadzialala ostatnio, wiec
 *     w domu to nadal JEDNA proba - reszta listy nic nie kosztuje.
 * ------------------------------------------------------------------ */
#define WIFI_SIECI_MAX      4               // ile sieci pamietamy
#define WIFI_ALT_TIMEOUT_MS 8000            // krocej na kazda KOLEJNA probe

/*     Wyniki usuwania sieci. Osobne wartosci, a nie samo true/false, bo
 *     "nie ma takiej sieci" i "nie usune, bo to jedyna droga do mnie" to
 *     dwie rozne odpowiedzi i uzytkownik musi je odroznic.              */
#define WIFI_USUN_OK        0
#define WIFI_USUN_BRAK      1               // nie ma jej na liscie
#define WIFI_USUN_OSTATNIA  2               // jedyna i wlasnie przez nia gadamy
#define WIFI_USUN_BLAD      3               // zapis do pamieci sie nie udal

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
 * 7b2. DNI BEZ LEKU
 *     Warfaryne lekarz rozpisuje nierowno, a przed zabiegiem odstawia ja
 *     calkiem. W dniu rozpisanym na ZERO pudelko nie ma o czym przypominac
 *     - dzwonek zachecalby wtedy do wziecia tabletki, ktorej brac nie wolno.
 *
 *     ZASADA BEZPIECZENSTWA: wyciszamy sie WYLACZNIE wtedy, gdy wiemy na
 *     pewno, ze dzis przypada zero. Brak rozpisania, nieznany zegar, dane
 *     nie do odczytania - dzwonimy. W razie watpliwosci lepiej przypomniec
 *     o dawce niepotrzebnie niz przemilczec potrzebna.
 *
 *     DOSE_NIEZNANA to wartosc "nic o tym dniu nie wiem". Dawki trzymamy
 *     w dziesiatych czesciach tabletki (10 = jedna tabletka), zeby zmiescic
 *     polowki w jednym bajcie.
 * ------------------------------------------------------------------ */
#define DOSE_EX_MAX         8               // ile wyjatkow na daty pamietamy
#define DOSE_NIEZNANA       255             // "brak rozpisania na ten dzien"

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
 *
 *     UWAGA: te progi znacza cos zupelnie innego niz przed wprowadzeniem
 *     krzywej LiPo. Przy dawnym liniowym przeliczaniu 10% wypadalo przy
 *     3,39 V - czyli tuz nad napieciem odciecia, gdy do konca zostawal
 *     mniej wiecej dzien. Teraz 10% to ok. 3,69 V i realnie okolo tygodnia
 *     zapasu, a 25% to ok. 3,75 V, czyli jakies trzy tygodnie.
 * ------------------------------------------------------------------ */
#define BATT_WARN_PCT       25              // ok. 3,75 V - "naladuj w tych dniach"
#define BATT_CRIT_PCT       10              // ok. 3,69 V - "naladuj teraz"

/* Maksymalna zmiana wskazania miedzy dwoma wybudzeniami. Realne zuzycie
   to okolo 1 punkt na dobe, wiec 5 w dol zostawia spory zapas, a odcina
   skoki z pojedynczego zaklocenia. Ruch w gore o 1 punkt pozwala takiemu
   zakloceniu samemu sie zagoic, zamiast zostac w pamieci na stale.     */
#define BATT_STEP_DOWN      5
#define BATT_STEP_UP        1

/* Martwa strefa: roznica mniejsza niz tyle punktow to szum, a nie zmiana.
   Bez niej wskazanie drgalo o jeden punkt przy KAZDYM wybudzeniu, bo szum
   odczytu raz podbijal, raz obnizal wynik wzgledem tego, co pokazywalismy.
   BATT_RISE_STREAK mowi, ile razy Z RZEDU odczyt musi byc wyraznie wyzszy,
   zanim uznamy wzrost za prawdziwy - jeden wysoki pomiar to przypadek,
   trzy pod rzad to juz ladowanie albo odpoczynek ogniwa.               */
#define BATT_DEADBAND       3
#define BATT_RISE_STREAK    3

/* ---------------------------------------------------------------------
 * 7f. OKNO TESTOWE PO RESECIE
 *     Deep sleep odlacza port USB, wiec wybudzenia kontaktronem nie widac
 *     w monitorze portu szeregowego. Zaraz po resecie pudelko czuwa przez
 *     BOOT_HOLD_MS i wypisuje kazda zmiane stanu stykow - wtedy mozna
 *     sprawdzic okablowanie na oczy. Kazda zmiana przedluza okno, ale nie
 *     dluzej niz BOOT_TEST_MAX_MS.
 * ------------------------------------------------------------------ */
#define BOOT_TEST_MAX_MS    60000           // sufit okna testowego (1 min)

/* ---------------------------------------------------------------------
 * 7g. CO TRAFIA DO HISTORII I DO BAZY
 *
 *     Pudelko budzi sie kilka razy dziennie i wiekszosc tych wybudzen jest
 *     zupelnie nieciekawa: obudzilo sie, zameldowalo, poszlo spac. Zapisy-
 *     wanie ich wszystkich zasmieca historie tak, ze nie widac w niej tego,
 *     co naprawde ma znaczenie - czyli dawek i awarii.
 *
 *     LOGBOOK_VERBOSE = 0 -> historia zawiera tylko rzeczy istotne:
 *        dawki, ostrzezenia, brak sieci, odciecie baterii, portal WiFi,
 *        zostawione otwarte wieczko, nieoczekiwane restarty.
 *     LOGBOOK_VERBOSE = 1 -> wszystko, lacznie z rutynowymi meldunkami.
 *        Przydatne tylko przy szukaniu bledu.
 *
 *     REPORT_BOOT_EVENT decyduje, czy uruchomienie plytki ma trafiac do
 *     listy zdarzen w bazie. Ta lista sluzy do odtwarzania kalendarza
 *     DAWEK, a restart nie jest dawka - przy kazdym wgrywaniu programu
 *     robil sie tam smietnik. Restarty i tak widac w historii pudelka.
 * ------------------------------------------------------------------ */
/* ---------------------------------------------------------------------
 * 7h. GESTY SERWISOWE  (dostepne bez odkrecania pudelka)
 *
 *     Przycisk NIE MOZE budzic ukladu z glebokiego snu razem z kontaktronem
 *     - maska wybudzania ma jeden wspolny poziom, a te dwa piny potrzebuja
 *     przeciwnych. Ale to nie problem: kontaktron budzi pudelko przy KAZDYM
 *     otwarciu wieczka, a potem plytka i tak czuwa, czekajac az je zamkniesz.
 *     W tym oknie przycisk dziala normalnie.
 *
 *     Stad gesty zaczynaja sie od otwarcia wieczka:
 *       otworz wieczko + 3 x nacisnij przycisk   -> autotest
 *       otworz wieczko + przytrzymaj przycisk    -> portal WiFi
 *
 *     Nic to nie kosztuje: pudelko i tak czekalo w tym czasie na zamkniecie.
 * ------------------------------------------------------------------ */
/* Jak dlugo pudelko CZUWA przy otwartym wieczku, zamiast isc spac.
   Dzieki temu w chwili zamkniecia od razu wysyla wszystko i aplikacja
   pokazuje prawde bez opoznienia. Rownolegle liczy sie czas do pierwszego
   ostrzezenia o zostawionym wieczku - wiec te dwie rzeczy sa celowo tej
   samej dlugosci: jesli w 15 minut nie zamkniesz, przechodzimy w tryb
   przypominania i dalej juz spimy miedzy sygnalami.                    */
/*     RADIO_OTWARTE_S - jak dlugo przy otwartym wieczku radio zostaje
 *     wlaczone. Przez ten czas polaczenie jest gotowe, wiec po zamknieciu
 *     stan leci do aplikacji od razu, bez czekania na logowanie do sieci.
 *     Potem radio gasnie, bo to ono zjada tu wiekszosc pradu - a jesli
 *     pudelko stoi otwarte kwadrans, to i tak nikt nie patrzy w telefon.
 *     Po zamknieciu radio wraca samo.                                    */
#define RADIO_OTWARTE_S     600             // 10 minut

/* ---------------------------------------------------------------------
 * 6f. PODGLAD LADOWANIA NA ZYWO
 *     Na ladowarce prad jest za darmo, wiec pudelko moze meldowac sie
 *     czesto - dzieki temu w aplikacji widac rosnacy procent bez zadnego
 *     klikania. Poza ladowarka nic sie nie zmienia.
 *
 *     CHARGE_POLL_S  - co ile pudelko wysyla stan podczas ladowania
 *     CHARGE_DROP_V  - spadek wzgledem szczytu = ladowarka odlaczona
 *     CHARGE_FULL_V  - powyzej tego napiecia brak wzrostu znaczy
 *                      "naladowane i nadal na kablu", a nie "odlaczone"
 *     CHARGE_IDLE_MAX- bezpiecznik: tyle pomiarow bez wzrostu i ponizej
 *                      CHARGE_FULL_V konczy tryb, zeby zaklinowane
 *                      wykrycie nie mielilo baterii budzeniem co minute
 * ------------------------------------------------------------------ */
#define CHARGE_POLL_S       60
#define CHARGE_DROP_V       0.04f
#define CHARGE_FULL_V       4.15f
#define CHARGE_IDLE_MAX     30

/*     CHARGE_PUSH_MAX_S - przy ladowaniu pudelko budzi sie co minute, ale
 *     wysyla stan tylko wtedy, gdy procent sie zmienil - albo gdy minelo
 *     tyle sekund. Budzenie jest tanie, ruch w sieci nie.
 *     TOKEN_MARGIN_S - token Firebase zyje godzine; odswiezamy go z takim
 *     zapasem, zeby nie wygasl w polowie wysylki.                       */
#define CHARGE_PUSH_MAX_S   300
#define TOKEN_MARGIN_S      300

/*     CHARGE_AWAKE_MAX_S - bezpiecznik dla ciaglego czuwania na ladowarce.
 *     Gdyby wykrycie odlaczenia zawiodlo, plytka z wlaczonym radiem zjada
 *     ogniwo w kilka godzin. Po tym czasie wracamy do zwyklego cyklu snu,
 *     ktory i tak wykryje ladowanie ponownie, jesli kabel nadal tkwi.
 *     CHARGE_SAMPLE_S - co ile mierzymy napiecie podczas czuwania.       */
#define CHARGE_AWAKE_MAX_S  14400           // 4 h
#define CHARGE_SAMPLE_S     30

/*     CHARGE_SETTLE_S - po odlaczeniu kabla napiecie ogniwa opada do
 *     swojej prawdziwej wartosci dopiero po kilkunastu sekundach. Tyle
 *     czekamy, zanim zmierzymy stan naladowania i wyslemy go do aplikacji. */
#define CHARGE_SETTLE_S     20

#define CZEKAJ_ZAMKNIECIE_MS (OPEN_WARN_FIRST_S * 1000UL)
#define GEST_KLIKNIEC       3               // tyle nacisniec = autotest
#define GEST_PRZYTRZYM_MS   1800            // tyle trzymania = portal WiFi

#define LOGBOOK_VERBOSE     0
#define REPORT_BOOT_EVENT   0

/* ---------------------------------------------------------------------
 * 7i. DZIENNIK WIECZKA  -  TYMCZASOWE, do testu terenowego
 *
 *     Zapisuje kazda zmiane stanu kontaktronu (otwarcie i zamkniecie),
 *     zeby sprawdzic, czy pudelko melduje otwarcia, ktorych nikt nie
 *     zrobil - np. gdy magnes przesunie sie w plecaku.
 *
 *     Wysylany przy pierwszej synchronizacji i dopiero PO potwierdzeniu
 *     kasowany z pamieci pudelka, zeby nie zapychac NVS.
 *
 *     Po zakonczeniu testu cala ta funkcja moze zniknac.
 *
 *     Firmware ma wlasna wartosc awaryjna (#ifndef), wiec starszy config.h
 *     bez tego wpisu nadal sie skompiluje.
 * ------------------------------------------------------------------ */
#define LIDLOG_SLOTS        64              // ile zmian stanu miesci sie w NVS

/* ---------------------------------------------------------------------
 * 7j. AKTUALIZACJA PRZEZ WIFI  (OTA)  -  od 1.38.0, D59
 *
 *     Binarke buduje automat na GitHubie przy kazdej zmianie w firmware
 *     i kladzie ja obok aplikacji na GitHub Pages. Pudelko pobiera
 *     najpierw MALY plik z opisem (wersja, suma, rozmiar), a dopiero
 *     gdy suma rozni sie od tej, ktora juz ma - caly program.
 *
 *     ADRES JEST ZASZYTY TUTAJ, A NIE BRANY Z BAZY. To jest obrona, nie
 *     niewygoda: pudelko nie sprawdza certyfikatu serwera (setInsecure,
 *     dlug opisany w PROJEKT-PillBox-kontekst.md), wiec im mniej rzeczy
 *     da sie podmienic zdalnie, tym lepiej. Suma kontrolna przychodzi
 *     natomiast Z BAZY - czyli innym kanalem niz sam plik. Zeby wgrac
 *     pudelku obcy program, trzeba by podmienic OBA naraz.
 *
 *     Gdyby adres kiedys sie zmienil, trzeba wgrac firmware kablem. To
 *     swiadomy koszt jednego pola, ktorego nikt zdalnie nie ruszy.
 * ------------------------------------------------------------------ */
#define OTA_ENABLED         1
#define OTA_BASE_URL        "https://golon222.github.io/leki/firmware/"
#define OTA_JSON_FILE       "PillBox.json" // opis wersji: kilkaset bajtow
#define OTA_BIN_FILE        "PillBox.bin"  // sam program: ~1,2 MB

/*     Kiedy WOLNO sie aktualizowac.
 *
 *     OTA_MIN_BATT_PCT - ponizej tego progu tylko na ladowarce. Samo
 *     pobranie to okolo 1-2 mAh, czyli grosze, ale na wyczerpanym ogniwie
 *     kazdy grosz jest juz pozyczka.
 *     OTA_MAX_FAILS - po tylu nieudanych probach z rzedu pudelko
 *     przestaje probowac i mowi o tym w aplikacji. Bez tego licznika
 *     niedostepny plik oznaczalby wlaczone radio przy KAZDYM otwarciu
 *     wieczka, codziennie, w nieskonczonosc.
 *     OTA_RETRY_S - odstep miedzy probami. Doba, bo tyle wynosi naturalny
 *     rytm tego urzadzenia: jedna dawka, jedno otwarcie.                */
#define OTA_MIN_BATT_PCT    25
#define OTA_MAX_FAILS       3
#define OTA_RETRY_S         86400

/*     Zdrowy rozsadek co do rozmiaru. Plik mniejszy niz OTA_MIN_BIN_SIZE
 *     nie jest firmwarem tego urzadzenia (najpewniej strona bledu 404
 *     zapisana jako plik), a wiekszy niz OTA_MAX_BIN_SIZE nie zmiesci sie
 *     w partycji - lepiej wiedziec to PRZED pobraniem 1,9 MB przez radio. */
#define OTA_MIN_BIN_SIZE    300000
#define OTA_MAX_BIN_SIZE    1900000

/*     OTA_HTTP_TIMEOUT_MS - pobranie przez slabe WiFi trwa dluzej niz
 *     zwykly zapis do bazy, wiec ma wlasny, hojniejszy limit.
 *     OTA_BOOT_TRIES - ile razy nowy program moze wystartowac i NIE
 *     dojsc do konca, zanim uznamy go za zepsuty i wrocimy do starego.  */
#define OTA_HTTP_TIMEOUT_MS 90000
#define OTA_BOOT_TRIES      3

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

/* ---------------------------------------------------------------------
 * 9b. OKNO NA WGRYWANIE PROGRAMU
 *     Po RESECIE (przycisk R, podlaczenie zasilania) pudelko zostaje
 *     wybudzone przez tyle milisekund, zanim zasnie. Port COM zdazy sie
 *     pojawic i Arduino IDE go zobaczy.
 *     NIE dotyczy zwyklych wybudzen z deep sleep - te dzialaja normalnie,
 *     wiec bateria na tym nie traci (reset zdarza sie kilka razy w zyciu).
 *     Ustaw 0, zeby wylaczyc.
 * ------------------------------------------------------------------ */
#define BOOT_HOLD_MS        10000

#if DEBUG_SERIAL
  #define LOG(...)   Serial.printf(__VA_ARGS__)
  #define LOGLN(x)   Serial.println(x)
#else
  #define LOG(...)
  #define LOGLN(x)
#endif
