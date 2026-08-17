# PillBox — pełny kontekst projektu

**Dokument przekazania.** Wgraj go do wiedzy projektu w Claude razem z plikami
z katalogu `firmware/` i z korzenia repo. Wtedy Claude na dowolnym urządzeniu wie to,
co wiedział Claude prowadzący ten projekt — łącznie z przyczynami decyzji,
które kosztowały godziny szukania.

Stan na: **17 sierpnia 2026** · firmware **1.44.0** · aplikacja **2026-08-12.43**

---

## 1. Co to jest

Inteligentne pudełko na leki dla Kuby. Warfin 5 mg, **raz dziennie**,
przypomnienie domyślnie o 20:00. Aplikacja śledzi też wyniki INR — to lek
przeciwzakrzepowy, więc pominięta albo podwójna dawka to nie jest drobiazg.

**Sprzęt** (zlutowany, docelowo zaklejony — nie do zmiany):

| element | pin | uwagi |
|---|---|---|
| Seeed XIAO ESP32-C3 | — | ogniwo LiPo 470 mAh |
| pomiar baterii | D0 / GPIO2 | dzielnik 100k/100k, ADC1_CH2 |
| kontaktron | D1 / GPIO3 | do GND, podciągnięcie do 3V3; **otwarte = HIGH** |
| buzzer piezo | D2 / GPIO4 | pasywny, LEDC PWM, rezonans ~2700 Hz |
| ukryty przycisk | D3 / GPIO5 | do GND; **wciśnięty = LOW** |

**Aplikacja**: PWA na iPhone, GitHub Pages + Firebase Realtime Database.
Bez konta Apple Developer, wszystko darmowe.

**Układ repo.** Katalog `app/` **już nie istnieje** — pliki strony leżą
w korzeniu repozytorium i stamtąd serwuje je GitHub Pages:

```
firmware/PillBox/PillBox.ino        główny kod (~2600 linii)
firmware/PillBox/config.h           ustawienia (w repo, bez hasła)
firmware/PillBoxTest/               osobny szkic diagnostyczny
index.html                          cała PWA w jednym pliku
sw.js                               service worker
manifest.json, icon-192.png, icon-512.png   manifest i ikony PWA
tabletka.gif                        tabletka na ekranie głównym
tests/                              testy + audyt
database.rules.json                 reguły Firebase
.github/workflows/firmware.yml      automat budujący binarkę do OTA
WGRYWANIE.md                        instrukcja wgrywania kablem dla Kuby
CLAUDE.md                           instrukcje dla Claude Code
DECYZJE.md                          dziennik decyzji (D1–D65)
PROJEKT-PillBox-kontekst.md         ten plik
```

**`DECYZJE.md` jest ważniejszy, niż wygląda.** Ten dokument opisuje stan;
tamten opisuje **dlaczego** — co jest tymczasowe i kiedy to usunąć, co już raz
**cofnięto** i czego nie próbować drugi raz. Przy sprzeczności między nimi
rozstrzyga `DECYZJE.md`, bo jest prowadzony na bieżąco, wpis po wpisie.

---

## 2. Twarde ograniczenia — NIE ŁAMAĆ

1. **Firmware to dokładnie dwa pliki**: `PillBox.ino` + `config.h`.
   Scalanie zostało wprost odrzucone: *„nie nie zostanimy przy dwoch plikach"*.
2. **Żadnych zmian sprzętowych**: *„ja nie będę zmieniał ani dodawał rezystorów"*.
   Każde rozwiązanie musi działać na powyższym schemacie.
3. **`config.h` JEST w repo — celowo.** Trzyma wyłącznie placeholder
   `TUTAJ_WPISZ_HASLO`; prawdziwe hasło nigdy tu nie wraca.

   Była próba wyjęcia go z repo na rzecz `config.example.h`. **Cofnięta na
   wyraźną prośbę Kuby** — i to jest dobra lekcja o tym, czyj komfort się liczy.
   On nie klonuje repo: pobiera folder `firmware/` i otwiera go wprost
   w Arduino IDE. Bez `config.h` w komplecie szkic się nie otwiera, więc przed
   każdym wgraniem musiałby zmieniać nazwy plików — raz na komputerze, raz na
   MacBooku, raz w pośpiechu przed wyjazdem. Zabezpieczenie chroniło przed
   ryzykiem, które **nigdy się nie zmaterializowało** (w historii repo hasło
   zawsze było placeholderem), a kosztowało przy każdym pobraniu.

   `WEB_API_KEY` zostaje świadomie: ten sam klucz jest publiczny w `index.html`
   na GitHub Pages, a barierą jest `database.rules.json`, nie jego tajność.

   Jedyna realna zasada do pilnowania: **nie wrzucać `config.h` z wpisanym
   hasłem**. `.gitignore` przed tym nie obroni przy wrzucaniu przez stronę
   GitHuba, więc to kwestia uwagi, nie mechanizmu.
4. **`DAY_START_HOUR = 3`** musi być identyczne w firmware i w aplikacji.
   Osobny test przepuszcza ~177 000 znaczników czasu przez kod obu stron.
5. **Każdy zapis użytkownika w aplikacji idzie przez `zapiszPewnie()`** —
   nigdy gołe `set()`. Firebase offline nie odrzuca obietnicy, tylko wisi
   w nieskończoność: ekran pokazuje sukces, a dane nie docierają.
6. **Nic nie kasujemy z pamięci pudełka przed potwierdzonym 2xx.**
   Kolejka, flagi statusu, dziennik wieczka — rodzina błędu 3.5.
7. **Do gałęzi `events` nie dokładamy pól.** Reguła `$other: false` odrzuca
   **cały** wpis, gdy trafi w nim nieznane pole, więc otwarcie pudełka
   przepada w całości. Nowe dane idą do nowej gałęzi.
8. **Narzędzie diagnostyczne nie może uszkodzić danych o leku.**
9. **Dawka jest jedna dziennie, ale TABLETEK w niej zmienna liczba** (D36).
   `dawkaNaDzien()`: wyjątek na datę → rozpisanie tygodniowe → `defaultDose`.
   Indeks w `doseWeek` to `getDay()`/`tm_wday`, czyli **0 = niedziela** — tak
   samo po obu stronach. Schemat niekompletny odrzucamy w całości i wracamy do
   `defaultDose`: brakujące pole odczytane jako zero to cichy dzień bez leku
   przeciwzakrzepowego. Dzień z zerem ma status `off` i **nie wchodzi ani do
   licznika, ani do mianownika** skuteczności. Pudełko zna to samo rozpisanie
   i w dniu rozpisanym na zero **nie dzwoni i nie zgłasza „missed"** — ale
   wycisza się **wyłącznie przy pewnym zerze**: bez zegara albo bez rozpisania
   dzwoni.
10. **Hasło do WiFi kasujemy z bazy dopiero po potwierdzonym zapisie w NVS**
    (D38). `wifiSiecDodaj()` zwraca wynik i ten wynik trzeba sprawdzić.
    Odwrotna kolejność traci sieć, której nikt już nie zna — a z nią jedyną
    drogę do pudełka poza portalem fizycznym.
11. **Hasło do Firebase czytamy z NVS, nigdy wprost z `config.h`** (D59).
    `config.h` jest już tylko **ziarnem** przy pierwszym wgraniu kablem. Powód
    jest twardy: binarkę aktualizacji buduje automat z tego repo, a w repo stoi
    placeholder. Dlatego `otaDecyzja()` odmawia aktualizacji bez hasła w pamięci,
    a `hasloUtrwal()` potwierdza zapis **odczytem zwrotnym**. Nie upraszczaj
    żadnego z tych trzech kroków.
12. **Aktualizacja i skan sieci ruszają wyłącznie z `goToSleep()`.** To jedyne
    miejsce, przez które przechodzi każda ścieżka wybudzenia, i jedyne, w którym
    dawka jest już zapisana i potwierdzona. Wywołanie z `fetchConfig()` albo
    z obsługi kontaktronu wcisnęłoby minutę radia między otwarcie wieczka
    a zapis dawki Warfinu. Audyt to sprawdza.
13. **`queueDrop()` to jedyny świadomy wyjątek od zasady 6.** Zdejmuje wpis,
    którego baza nie przyjmie **nigdy** (HTTP 400/413 albo rekord uszkodzony) —
    zostawiony blokował wszystkie dawki za sobą. Strata idzie na licznik
    `dropped` w statusie, więc aplikacja o niej krzyczy (D13).

**Zniesione ograniczenie:** blok pomiaru napięcia (`CALIBRATION_FACTOR = 0.921`
i przeliczenia) **wolno** zmieniać — Kuba zniósł tę zasadę 2026-08-05. Audyt
nie blokuje: zgłasza uwagę i przechodzi dalej. Te liczby były kalibrowane na
sprzęcie, a w diffie zmiana przypadkowa wygląda identycznie jak świadoma.

---

## 3. Błędy, które kosztowały najwięcej — i ich przyczyny

To jest najcenniejsza część dokumentu. Każdy z tych błędów wyglądał z zewnątrz
zupełnie inaczej, niż wyglądał w kodzie.

### 3.1 Kontaktron nie budził pudełka
Pierwsza teoria (nadpisywanie maski wybudzania) była **błędna**.
Prawdziwa przyczyna: [espressif/esp-idf#12183](https://github.com/espressif/esp-idf/issues/12183) —
`esp_deep_sleep_start()` sam włącza wewnętrzne podciągnięcie ~10 kΩ **do masy**
na pinie czekającym na stan wysoki. Wygrywa z zewnętrznymi 100 kΩ, pin siedzi
na ~0,4 V i wybudzenie nigdy nie następuje.

Obejście: `gpio_hold_en(pin)` + `gpio_deep_sleep_hold_en()` **przed** snem,
zwolnienie przez `gpio_deep_sleep_hold_dis()` + `gpio_hold_dis(pin)` **po**
wybudzeniu. Potwierdzone działanie na sprzęcie.

### 3.2 „Synchronizacja się psuje" — ścieżka bez radia
Ścieżka *„już dziś brałeś"* (powtórne otwarcie w ciągu doby) **w ogóle nie
włączała WiFi**: pikała i szła czekać na zamknięcie. Pełny status leciał dopiero
po powrocie z czekania, czyli po zamknięciu wieczka. Użytkownik otwierał pudełko,
patrzył w telefon dwie minuty i nie widział nic — bo pudełko było **ciche**, nie wolne.

Naprawa: `pushLidState()` wywoływane na **wejściu** do `czekajNaZamkniecieIGest()`,
z każdej ścieżki, warunkowane `!rtcOpenReported`.

### 3.3 Każde zapytanie płaciło własne uzgodnienie TLS
`rtdbSend()` tworzył nowego `WiFiClientSecure` przy każdym wywołaniu. Uzgodnienie
TLS na ESP32-C3 to ~1–2 s, a jedno otwarcie pudełka to sześć zapytań.
Naprawa: jeden globalny `rtdbClient` na całe wybudzenie + `http.setReuse(true)`.

### 3.4 Token Firebase kasowany przez deep sleep
`idToken` był zwykłą zmienną. Przy kilku wybudzeniach dziennie bez znaczenia —
ale podgląd ładowania budzi co minutę, co dałoby **1440 logowań hasłem na dobę**.
Google potrafi za to czasowo zablokować konto. Token siedzi teraz w NVS razem
z czasem ważności (`rtcTokenExp`), odświeżany z zapasem `TOKEN_MARGIN_S`,
kasowany przy HTTP 401/403.

### 3.5 `pushStatus()` kłamał sam sobie
`rtcOpenReported` ustawiane **przed** wysyłką i bez patrzenia na wynik.
Nieudany pakiet oznaczał „aplikacja już wie" — i ponowienia nie było **nigdy**.
Teraz sukces = potwierdzony kod 2xx, porażka zostawia `rtcStatusDirty`,
a `planNextSleep()` planuje wcześniejsze wybudzenie.

### 3.6 Korekta zegara obejmowała tylko kolejkę
NTP potrafi przesunąć zegar o godziny. `queueShiftTimestamps()` poprawiało
zdarzenia — ale nie pięciu znaczników w RTC. Najgroźniejszy `rtcNextWarnTs`:
skok w przód wyrzucał termin sygnału w przeszłość i pudełko piszczałoby bez
powodu. Naprawa: `przesunZnaczniki()` wołane razem z korektą kolejki.

### 3.7 Ładowanie wykrywane tylko po wzroście napięcia
Zakładało pomiar przed podłączeniem kabla i po nim. Przy wgrywaniu firmware
z podpiętym kablem pamięć RTC jest kasowana, pierwszy pomiar to już 4,2 V
i wzrostu **nie będzie nigdy**. Dodany drugi dowód: dwa wysokie odczyty z rzędu
(≥ `CHARGE_FULL_V`), z blokadą `rtcBlokWysokie` przeciw migotaniu po odpięciu.

### 3.8 Procentu naładowania na kablu NIE DA SIĘ zmierzyć
Układ ładujący trzyma na ogniwie swoje napięcie niezależnie od stanu
naładowania. Każdy odczyt wychodzi 100%. Rozwiązanie: podczas ładowania
aplikacja pokazuje **wyszarzony odczyt sprzed ładowania** plus szacowany czas
do pełna liczony z `chargeSince` i `chargeFromPct`. Prawdziwy pomiar następuje
`CHARGE_SETTLE_S` = 20 s po odłączeniu kabla.

### 3.9 Audyt sprawdzał tekst, którego w kodzie nie ma — DWA RAZY
- Wzorzec na literały znakowe `'(\\.|[^'\\])*'` połykał wszystko od
  `autocapitalize='off'` w HTML portalu. Audyt cicho przepuszczał błędy.
  Naprawa: literał znakowy to **dokładnie jeden** znak.
- Wycinanie ciała funkcji trafiało w **deklarację zapowiadającą**
  (`bool syncTimeNTP();`) zamiast w definicję. Naprawa: funkcja `cialo()`
  szukająca nagłówka zakończonego `{`.

Morał: narzędziu weryfikującemu też trzeba nie ufać.

### 3.10 Cały zestaw testów nie startował — z dwóch powodów naraz
Objaw: `bash tests/run_all.sh` kończyło się lawiną **189 błędów kompilacji**
w kroku 1/7, wyglądającą na rozsypane firmware. Logika była nietknięta.

- **`tests/arduino_shim.h` nie włączał `<cstdint>`.** `uint8_t` i `uint32_t`
  przychodziły tam tranzytywnie przez `<cstdio>`; od g++ 13 już nie przychodzą.
  Jedna brakująca linia dawała 150 błędów „does not name a type".
- **Katalog `app/` został usunięty**, a `index.html`, `sw.js` i `manifest.json`
  przeniesione do korzenia (commity *„Delete app directory"* + *„Add files via
  upload"*). Testy nadal szukały ich w `app/` — w pięciu plikach.

Pułapka przy naprawie: sprawdzanie poprawki na **kopii repo z podstawionym
katalogiem `app/`** pokazało zielono, bo podstawienie zamaskowało piątą, ostatnią
zepsutą ścieżkę — tę w `audit_firmware.py`. Ujawniła się dopiero przy teście na
prawdziwym `git checkout-index`.

Morał: poprawkę ścieżek sprawdzaj na tym, co faktycznie jest w repo, a nie na
spreparowanym katalogu. Środowisko testowe potrafi zataić dokładnie ten błąd,
którego szukasz.

---

### 3.11 „Kliknij i pudełko przyjmie" nie działało przez tydzień — cztery różne przyczyny pod jednym objawem

To najdroższa historia w całym projekcie i jedyna, w której **objaw ani razu
się nie zmienił**, a przyczyna cztery razy. Kuba naciskał przycisk, pudełko nie
brało aktualizacji, aplikacja pisała „nie zrobiło". Za każdym razem coś innego.

**(a) Pobranie odrzucało się, zanim ruszyło** (naprawione w 1.42.1).
`otaWgraj()` miało `if (len <= 0 || (uint32_t)len != rozmiar) return false;`,
gdzie `len` to `http.getSize()`. Przy odpowiedzi w kodowaniu porcjowym
(`Transfer-Encoding: chunked`) serwer nie podaje `Content-Length`, a `getSize()`
zwraca **−1** — więc warunek zawsze był prawdziwy. Kod czytał to jako „plik ma
zły rozmiar": objaw wskazywał na serwer, winna była własna asercja.
Naprawa jest ta sama, którą stosuje oficjalna biblioteka `HTTPUpdate` z rdzenia
Arduino-ESP32, razem z komentarzem tłumaczącym dlaczego:
`// use HTTP/1.0 for update since the update handler not support any transfer Encoding`
→ `http.useHTTP10(true)`.

**(b) Limit czasu obcinał się po cichu** (1.42.3). `HTTPClient::setTimeout()`
przyjmuje **`uint16_t`**, a podawaliśmy tam 90000. Kompilator nie mówi nic —
liczba zawija się do **24464 ms**. Źródło zapowiadało limit, którego program nie
miał. Ta sama rodzina co B21: kompilacja gładka, zachowanie inne niż zapis.

**(c) Zlecenie ginęło w wyścigu** (1.43.0). `rtcOtaProsba` ustawiał się wyłącznie
w `fetchConfig()`, na **początku** wybudzenia; aktualizacja rusza na **końcu**,
z `goToSleep()`. Zlecenie złożone pomiędzy tymi chwilami było już w bazie,
pudełko meldowało świeży `lastSeen` — i o zleceniu nie wiedziało.

**(d) WŁAŚCIWA przyczyna: po wzięciu tabletki otwarcie wieczka nie czytało
ustawień** (1.43.1). `fetchConfig()` wisiało wyłącznie na `reportEvent()`, czyli
na ścieżce zapisu dawki. Drugie otwarcie w ciągu doby (`juz dzis brane`) wychodzi
z obsługi kontaktronu **wcześniej**, przez `break` — `reportEvent()` się tam nie
wykonuje. Końcowy meldunek i tak włączał radio i słał status, więc `lastSeen` się
odświeżał, a ustawień pudełko nie czytało nigdy. W praktyce: **przez większość
doby otwarcie wieczka nie mogło uruchomić aktualizacji** — a to jedyny gest,
którym człowiek naturalnie próbuje ją wywołać.

**Co to rozstrzygnęło.** Nie rozumowanie, tylko jedna linijka z „Historii
pudełka": trzy wybudzenia, każde z `zamkn->wyslane`, i **ani jednego
`ota:pobieram`**. Czyli pudełko łączyło się i **nie próbowało** — co jest czymś
zupełnie innym niż „próbowało i nie dało rady", w co celowały trzy poprzednie
naprawy.

**Morały, wszystkie kosztowne:**

- **Zapytaj najpierw „czy on w ogóle startuje", a dopiero potem „czemu nie
  dobiega do mety".** Trzy rundy naprawiałem to, co umiałem zmierzyć (nagłówki
  HTTP, limity czasu, wyścigi), zamiast sprawdzić rzecz najprostszą.
- **Komunikat na ekranie jest częścią mechanizmu, nie opisem obok niego.**
  Aplikacja pisała „naciśnij przycisk jeszcze raz", a przy zleceniu w toku
  żadnego takiego przycisku na ekranie nie było — i nawet gdyby był, nowe
  zlecenie nie ruszało licznika. Rada dwa razy nieprawdziwa, wyglądająca jak pomoc.
- **„Nie wiem" to osobny stan, nie łagodniejsza odmiana „nie".** Ekran
  oskarżał pudełko o awarię, do którego wiadomość po prostu nie doszła. Ta sama
  lekcja co przy WiFi (D25, D32) i przy kalendarzu (D64) — w tym projekcie
  wróciła cztery razy.
- **Zanim dołożysz mechanizm, sprawdź, czy nie stoi już w kodzie.** Na pytanie
  o nocne wybudzenie odpowiedziałem, że kosztowałoby prąd co dobę — a odbywało
  się od dawna (`MIDNIGHT_CHECK`).

---

## 4. Zachowanie pudełka — stan obecny

- **Otwarcie** → natychmiastowy sygnał *„już dziś brałeś"* (jeśli dawka wzięta),
  wysyłany **przed** startem portu szeregowego i pomiarem baterii.
  `loadDayMarkers()` musi być wywołane wcześniej — po resecie RTC jest puste.
- **Przy otwartym wieczku pudełko czuwa**, nie śpi. Radio zostaje włączone przez
  `RADIO_OTWARTE_S` = 600 s, potem `wifiUspij()` (**nie** `wifiOff()` — `esp_wifi_stop()`
  ubija sterownik i powrót bywa zawodny). Łącze podtrzymywane co 5 s.
- **Zamknięcie** → natychmiastowy `pushLidState()` (PATCH, trzy pola),
  potem pełny `pushStatus()`. Czas mierzony i zapisywany jako `zamkn->wyslane N ms`.
- **Po 15 minutach** otwarcia → pierwszy sygnał, potem co 30 min (`OPEN_WARN_*`).
- **Na ładowarce** pudełko czuwa bez usypiania do 4 h (`CHARGE_AWAKE_MAX_S`),
  co 30 s mierzy i **pobiera ustawienia** (zmiana godziny wchodzi w ~30 s).
  Otwarcie wieczka albo pora dawki → `goToSleep(1)`, czyli oddanie sterowania
  normalnej ścieżce (kontaktron stoi w stanie wybudzenia → natychmiastowy WAKE_REED).
- **Autotest**: trzy kliknięcia przyciskiem. 1 piknięcie (= dowód, że buzzer działa)
  → ruch wieczkiem, 20 s → 2 piknięcia → reszta po cichu (bateria, pamięć, WiFi,
  zegar, baza). Przycisku **nie testujemy** — uruchomienie testu jest jego dowodem.
  Jedyny sygnał błędu to łagodne pulsowanie 5 s = brak sieci.
- **Portal WiFi**: przytrzymanie przycisku. Kończy się **zamknięciem wieczka**.
- **O 3:00 (granica doby lekowej, `MIDNIGHT_CHECK`) pudełko budzi się samo.**
  Domyka poprzednią dobę (`checkDayRollover()` wystawia „missed" za każdy
  niezamknięty dzień), synchronizuje zegar przez NTP, czyta ustawienia,
  opróżnia kolejkę i wysyła status. Ten sam moment **dowozi aktualizację** —
  potwierdzone na płytce. To wybudzenie istniało od dawna; przez chwilę
  proponowałem dołożenie go „na nowo", nie zauważywszy, że już jest.
- **Aktualizacja przez WiFi rusza wyłącznie z `goToSleep()`** — po zapisie
  dawki, wysłaniu statusu i opróżnieniu kolejki. Dotyczy to **każdej** ścieżki
  wybudzenia: otwarcia wieczka, ładowarki i wybudzenia o 3:00.
  `otaSprobuj()` **sam dopytuje bazę** o zlecenie, zamiast ufać temu, co
  `fetchConfig()` widziało na początku wybudzenia (3.11c).
- **Skan sieci WiFi na żądanie** (`config/wifiScan`): pudełko rozgląda się
  dookoła i publikuje listę z siłą sygnału do własnej gałęzi `scan`. Chodzi
  w `goToSleep()` z tego samego powodu co OTA — zabiera radio na kilka sekund
  i potrafi zerwać połączenie. Sieci ukryte i nazwy dłuższe niż 32 znaki
  odsiewa u siebie: jeden element niezgodny z regułami odrzuciłby **cały** wpis.

---

## 5. Aplikacja — stan obecny

- **Kolejka zapisów** w `localStorage`: każdy zapis użytkownika trafia najpierw
  do kolejki, znika po potwierdzeniu. Firebase offline **nie odrzuca** obietnicy,
  tylko trzyma ją w nieskończoność — stąd wyścig z 8-sekundowym limitem.
  Ustawienia zapisywane **polami**, żeby ponowienie nie skasowało cudzej zmiany.
- **Siatka bezpieczeństwa**: raz na minutę `brakujePokrycia()` sprawdza lokalnie,
  czy któreś otwarcie z pudełka nie ma wpisu w kalendarzu. Nie trzeba wiedzieć,
  co zawiodło — skutek naprawia się sam w ciągu minuty.
- **Ręczne wpisy wygrywają zawsze** (`cur.source === "manual"` → `continue`).
- **Otwarcia biją pominięcia**, a wśród otwarć wygrywa najwcześniejsze.
- Dni przed `trackingSince` to **brak danych**, nie „nie wzięte".
- Tabletka na ekranie głównym to `tabletka.gif` (58 px), wyszarzona do czasu dawki.
- **Dzień lekowy rozstrzyga się z końcem doby, nie z ostatnim przypomnieniem**
  (D64). Pudełko wysyła „missed" po ostatnim przypomnieniu i jako **zdarzenie**
  to jest prawda — ale przypomnienie nie jest porą brania leku (ograniczenie 4b),
  a Kuba bierze tabletkę także o 22:00. Wzięta dawka rozstrzyga dzień od razu;
  niewzięta dopiero po zamknięciu doby (`dzienZamkniety()`, granica 3:00).
  Obowiązuje w trzech miejscach naraz: kalendarz, pierścień skuteczności, raport
  dla lekarza. Wpis **ręczny** wygrywa także dziś — skoro sam zaznaczyłeś, wiesz
  lepiej niż reguła.
- **Ekran aktualizacji** rozróżnia cztery stany, bo każdy znaczy co innego dla
  tego, co masz zrobić: „zlecone, jeszcze się nie łączyło", „właśnie to robi",
  „łączyło się i nie zrobiło — oto powód" oraz „poddało się po trzech próbach"
  (wtedy jest przycisk, który naprawdę wznawia). Trzeci stan nie może pojawiać
  się w trakcie udanej aktualizacji — przez minutę pobierania `lastSeen` jest
  już świeższy niż zlecenie, więc rozstrzyga `otaProsba` z pudełka, nie zegar.
- **Lista sieci widzianych przez pudełko** — przycisk nad polem nazwy. Ręczne
  wpisywanie zostaje nietknięte i **musi zostać**: sieci ukrytych żaden skan nie
  pokaże. Aplikacja nie skanuje sama, bo Safari nie udostępnia takiego API —
  ale lista od pudełka jest i tak lepsza, bo to ono ma się połączyć i stoi
  w innym miejscu niż telefon.

---

## 6. Testy — jak są zbudowane

**Zasada: testy pracują na PRAWDZIWYM kodzie, nie na kopii.**

- `tests/extract.py` wycina wskazane funkcje z `PillBox.ino` do `logic.inc`
  i kompiluje je g++ na zaślepkach Arduino (`arduino_shim.h`).
- `tests/build_app_module.mjs` buduje moduł z `index.html` i uruchamia go
  na zaślepkach Firebase/DOM.
- `tests/firebase_stub.mjs` sprawdza **każdy zapis prawdziwym
  `database.rules.json`** i umie zawieść na żądanie (`blad`, `odmowa`, `wisi`).
- `tests/rules_engine.mjs` + `test_rules.mjs` — reguły bazy jako osobny
  zestaw: czy prawdziwy kształt wysyłany przez pudełko przejdzie i czy śmieć
  nie przejdzie. Powstał dlatego, że **zapis odrzucony przez reguły wygląda
  dokładnie tak samo jak „nic się nie stało"** — a szukanie tego objawu
  kosztowało w tym projekcie kilka dni.
- `tests/audit_firmware.py` — kontrole strukturalne: kolejność definicji,
  parowanie `prefs.begin/end`, pętle bez ogranicznika, kolejność operacji
  w `setup()`, zasady z sekcji 2 zapisane jako testy.
- `tests/crosscheck_days.cpp` + `test_crosscheck.mjs` — zgodność liczenia dób
  między firmware a aplikacją.
- `tests/statyczna.py` — kontrola statyczna repo (brakujące elementy HTML,
  niesparowane znaczniki, handlery bez definicji, nawiasy w firmware). Do
  2026-08-17 była wklejona w runnera jako heredoc, więc nie dało się jej ani
  uruchomić osobno, ani sprawdzić mutacją bez odpalania całego zestawu.
- `bash tests/run_all.sh` uruchamia całość. **Cichy przy sukcesie, głośny przy
  błędzie**: udany przebieg to 12 linii zamiast 900, ale krok, który zawiedzie,
  pokazuje wszystkie swoje linie błędu i zostawia pełny log na dysku.
  `SZCZEGOLY=1` przywraca pełny wypis.
- `bash tests/kompiluj_firmware.sh` — **prawdziwy toolchain Arduino**
  (`esp32:esp32@3.3.11`), oba szkice, dwa razy: jako `.cpp` i przez prawdziwą
  ścieżkę `.ino` z generowanymi prototypami (B21/D26 — przez miesiąc
  sprawdzaliśmy tylko `.cpp` i firmware nie dawał się wgrać). Nie jest częścią
  `run_all.sh`: wymaga sieci i ~500 MB toolchainu.

**Mutacja jest tu obowiązkiem, nie ozdobą.** Każda nowa kontrola musi zostać
sprawdzona przez celowe zepsucie kodu — inaczej łatwo napisać test, który
przechodzi zawsze. Zdarzyło się to naprawdę: kontrola obecności `fetchConfig()`
w bloku przechodziła **nawet po usunięciu tej linijki**, bo nazwa funkcji padała
kilka razy w komentarzu obok. Zaliczała się na opisie naprawy zamiast na
naprawie. Stąd `_bez_komentarzy()` i `cialo_surowe()` — a także zasada, że
`strip()` zjada literały, więc treść napisów sprawdza się na surowym źródle.

**Stan: 484 + 51 firmware, 888 (×6 pór doby) + 92 + 49 aplikacja, 48 zgodności,
105 reguł bazy, 276 kontroli audytu — 0 błędów.**

---

## 7. Co zweryfikowano, a czego nie

Uczciwie, bo to ma znaczenie przy ocenie ryzyka.

**Sprawdzone na płytce:**

- **Firmware kompiluje się prawdziwym toolchainem** i **działa na urządzeniu** —
  Kuba wgrywa je kablem od dawna. `PillBox.ino` zajmuje **63% flasha**
  (1 241 697 B z 1 966 080), zapas ~724 kB.
- **Aktualizacja przez WiFi z przycisku w aplikacji, 2026-08-16.** Kuba wgrał
  kablem 1.43.1, nacisnął przycisk, otworzył wieczko — 1.43.2 weszła sama.
  Dwa piknięcia („zaczynam"), jedno („zapisane, restartuję"), fanfara po
  starcie, nowy numer wersji na ekranie. **To był pierwszy raz.**
- **Aktualizacja przy nocnym wybudzeniu o 3:00, 2026-08-17.** Zlecenie złożone
  wieczorem, rano nowa wersja — bez otwierania wieczka i bez ładowarki.
- **Hasło z NVS.** Binarka budowana przez automat ma w `config.h` placeholder
  i mimo to loguje się do bazy. Cała konstrukcja z D59 stoi.

**Nadal niesprawdzone:**

- **Rollback po nieudanym starcie.** Żadna wersja się jeszcze nie wysypała, więc
  własny licznik startów nigdy nie zadziałał naprawdę. To samo dotyczy **czarnej
  listy zepsutych sum** i **zachowania przy przerwanym pobieraniu**.
- **`setCACert()` — niespłacony dług i jedyna prawdziwa obrona.** Pudełko łączy
  się bez weryfikacji certyfikatu **ze wszystkim**: z Firebase i z GitHub Pages.
  W 1.40.0 **usunięto** porównywanie sumy z bazy z sumą z pliku, bo przy dwóch
  nieweryfikowanych kanałach nie dawało nic poza kosztem (D59). Przed uszkodzonym
  pobraniem chroni `Update.setMD5()`; przed **podmianą** nie chroni dziś nic.
- **Jakim kodem baza odrzuca wpis łamiący reguły.** Decyzja D13 zakłada 400 — bo
  tylko wtedy `trwaleOdrzucony()` zdejmie wpis z kolejki. Nikt tego nie zmierzył;
  `pushEventRecord()` loguje teraz odpowiedź bazy, więc pierwszy log rozstrzygnie.
- **Prąd ładowania 350 mA** to wartość katalogowa XIAO, nie pomiar. Szacunek
  czasu ładowania opiera się na niej i na założeniu ~250 mA netto.
- **Model wykrywania ładowania** testowany na przebiegach wymyślonych z tych
  samych założeń, z których napisano kod. To realna cykliczność.

---

## 8. Otwarte sprawy

0. ~~**Wiele sieci WiFi z hierarchią, zarządzane z aplikacji**~~ — **ZROBIONE.**
   Pudełko pamięta listę sieci w NVS, aplikacja pokazuje je z zaznaczeniem tej,
   przez którą jest właśnie połączone, i pozwala **przełączyć** albo **usunąć**
   (`config/wifiCmd`, akcje `priorytet` / `usun`). Jedynej sieci nie da się
   skasować — pudełko straciłoby drogę do świata. Dodawanie idzie przez
   `config/wifiNowa`, a hasło kasujemy z bazy **dopiero po potwierdzonym zapisie
   w NVS** (D38, ograniczenie 9).

   Od 1.44.0 doszła **lista sieci widzianych przez pudełko** (D65): aplikacja
   prosi (`config/wifiScan`), pudełko skanuje przed snem i publikuje nazwy
   z siłą sygnału do gałęzi `scan`. Powód, dla którego skanuje pudełko, a nie
   telefon, jest ważniejszy niż ograniczenie Safari: **liczy się to, co widzi
   pudełko** — sieć świetnie widoczna z kanapy potrafi nie docierać za ścianę.

   **Obie uwagi z pierwotnego zapisu nadal obowiązują** i warto je pamiętać:
   paradoks kury i jajka (żeby zmienić listę z telefonu, pudełko musi **już**
   mieć internet — dla zupełnie nowej sieci zostaje portal fizyczny) oraz to,
   że hasła WiFi przechodzą przez bazę. Reguły ograniczają dostęp do właściciela
   i pudełka, ale to była decyzja świadoma, nie przypadek.

1. **Powiadomienia push na telefon** — odłożone do hasła **„dawaj kod"**.
   Rekomendacja: bot Telegram (jedno zapytanie HTTP z pudełka, darmowe,
   natywne powiadomienia na iPhonie).
2. **Tanie wybudzenie kontrolne co 30 min** bez włączania radia, żeby
   wykrywać podłączenie ładowarki szybciej. Zaproponowane, **czeka na decyzję** —
   dokłada warunek obok ścieżki alarmu o dawce, czyli w najbardziej wrażliwym
   miejscu w całym kodzie.
3. **Gesty przyciskiem nie działają** podczas ładowania i podczas portalu.
   Świadomy kompromis.
4. ~~**Reguły bazy czekają na opublikowanie**~~ — **opublikowane** (2026-08-17,
   razem z gałęzią `scan` i polem `wifiScan`). Zasada zostaje: **każde nowe pole
   dopisujemy do `database.rules.json` i publikujemy w konsoli**, bo zapis
   odrzucony przez reguły wygląda dokładnie tak samo jak „nic się nie stało".
   Od tej pory pilnuje tego osobny zestaw testów (`test_rules.mjs`, 105 kontroli).
5. **Log z autotestu** — użytkownik zgłaszał przerywanie testu; w 1.21.1 dodano
   `etapTestu()` logujące każdy etap z czasem. Czeka na log z monitora portu.

6. ~~**Aktualizacja firmware przez WiFi (OTA)**~~ — **DZIAŁA, potwierdzone na
   płytce dwiema drogami** (D59–D63). Historia czterech nieudanych tygodni jest
   w sekcji 3.11 i warto ją przeczytać przed dotykaniem tego kodu.

   **Jak to działa, w jednym akapicie.** Automat na GitHubie buduje binarkę przy
   każdej zmianie w `firmware/**` i kładzie ją obok aplikacji na GitHub Pages
   razem z opisem (`PillBox.json`: wersja, suma MD5, rozmiar, commit). Przycisk
   w aplikacji **kolejkuje** aktualizację (`config/otaCmd`). Pudełko wykonuje ją
   przed zaśnięciem — przy otwarciu wieczka, na ładowarce albo o 3:00.

   ```
   huge_app  (do 1.37.0):  app0 = 3 MB               ← OTA niemożliwe
   min_spiffs (od 1.38.0):  app0 = 1,875 MB
                            app1 = 1,875 MB          ← OTA możliwe
   ```

   Firmware zajmuje **63%** (1 241 697 B), zapas ~724 kB.

   **Czego pilnuje kod, i dlaczego akurat tego:**

   - **Dawka przed aktualizacją.** OTA rusza wyłącznie z `goToSleep()`, czyli po
     zapisie dawki, wysłaniu statusu i opróżnieniu kolejki. Przy niepustej
     kolejce `otaDecyzja()` odmawia. Audyt sprawdza, że wywołanie jest jedno
     i że nie wsunęło się do `fetchConfig()` ani w obsługę kontaktronu.
   - **Suma, nie numer wersji.** Numer pisze człowiek w `config.h` i da się go
     zapomnieć podbić. Pudełko pobiera najpierw opis (kilkaset bajtów), a 1,24 MB
     dopiero gdy suma się różni od zapisanej. Suma jest ważna **tylko razem
     z `otaFw`** — po wgraniu kablem dotyczyłaby już czegoś innego.
   - ~~**Dwa kanały**~~ — **cofnięte w 1.40.0.** Porównywanie sumy z bazy z sumą
     z pliku miało chronić przed podmianą, ale pudełko łączy się przez
     `setInsecure()` z **jednym i drugim**: dwa kanały, ta sama dziura. Zysk
     zerowy, koszt codzienny (każda publikacja unieważniała świeże zlecenie).
     **`setCACert()` pozostaje niespłaconym długiem i jedyną prawdziwą obroną.**
   - **Hasło w NVS.** Binarka z automatu ma placeholder, więc pudełko loguje się
     hasłem z własnej pamięci trwałej; bez niego OTA jest zablokowane. Droga
     powrotna bez kabla: pole hasła w portalu WiFi, widoczne tylko gdy NVS go
     nie ma. **Sprawdzone — to działa.**
   - **Własny rollback.** Arduino buduje ESP32 bez bootloaderowego, więc liczymy
     starty sami: wersja, która nie dochodzi do `goToSleep()`, po trzech próbach
     trafia na czarną listę i wracamy na starą partycję. **Nigdy nie uruchomiony
     naprawdę** — patrz sekcja 7.
   - ~~**Doba przerwy między próbami**~~ — **zniesiona w 1.41.0** na żądanie
     Kuby: *„znieś to, że trzeba dobę czekać, czy widziałeś gdzieś, żeby jakieś
     urządzenie tak miało"*. Miał rację: żaden sprzęt nie mówi „wróć jutro",
     a przed pętlą chroni licznik `OTA_MAX_FAILS`, bo liczy **niepowodzenia**,
     a nie czas. Świeże zlecenie znosi też „poddałem się" po trzech próbach —
     licznik chroni przed pudełkiem próbującym w kółko samo z siebie, nie przed
     człowiekiem stojącym nad pudełkiem.
   - **Każda nieudana próba zostawia pomiar**, nie tylko powód: co serwer
     powiedział o długości pliku i ile zostało RAM-u. Bez tych dwóch liczb każde
     dochodzenie zaczyna się od zgadywania.

   **Jednorazowy koszt przy przejściu:** wgranie kablem z podziałem `min_spiffs`
   i prawdziwym hasłem w `config.h`. NVS leży pod tym samym adresem w obu
   podziałach, więc kolejka, sieci i znaczniki doby to przeżyją — **o ile nie
   zaznaczyć „Erase All Flash Before Sketch Upload"**. Instrukcja dla Kuby:
   `WGRYWANIE.md`.

   **Tańszy środek, gdyby OTA kiedyś zawiodło:** ESP Web Tools — strona wgrywająca
   `.bin` przez USB z przeglądarki. Chrome na MacBooku tak, **iPhone nie**
   (Safari nie ma WebSerial).

7. **Celowe sprawdzenie rollbacku.** Jedyny sposób, żeby przestać zgadywać, to
   wypuścić wersję, która **umyślnie się wysypuje przy starcie**, i zobaczyć, czy
   pudełko wróci do poprzedniej. Ryzykowne przy leku przeciwzakrzepowym —
   zaproponowane Kubie, **czeka na jego decyzję**.

---

## 9. Jak rozmawiać z Kubą o tym projekcie

- Pisze po polsku, bezpośrednio, często skrótowo. Oczekuje konkretów, nie asekuracji.
- **Jego opisy objawów są cenniejsze niż moje hipotezy.** Kilka razy w tej sesji
  naprawiałem rzeczy, które nie były przyczyną; przełomem zawsze był jego
  konkretny opis z liczbami („2,5 minuty", „40 sekund").
- **Gdy mówi, że coś nie działa, to nie działa — nawet jeśli kod mówi inaczej.**
  Przy aktualizacji trzy razy z rzędu ogłaszałem częściowy sukces na podstawie
  logów, a on odpowiadał: *„coś jest nie tak z całym procesem, on niby jest ale
  go nie ma"*. Miał rację za każdym razem. Jedyny wynik, który się liczy, to
  ten, który on widzi na ekranie — nie ten, który da się wyczytać z kodu.
- **Zrzut ekranu od niego bije każdą hipotezę.** Tydzień szukania przyczyny
  zamknęła jedna linijka z „Historii pudełka": brak wpisu `ota:pobieram`.
  Gdy utykasz, poproś o **jedną konkretną rzecz** do sprawdzenia, a nie o „opisz
  co się dzieje".
- **Nie ogłaszaj sukcesu, którego on nie widzi.** „Technicznie zadziałało"
  przy niezmienionym numerze wersji na ekranie brzmi jak wykręt, i słusznie.
- Gdy coś jest niepewne — powiedzieć wprost, że to hipoteza, **i dołożyć pomiar**
  zamiast zgadywać kolejny raz. Każdy pomiar, który przetrwał (sumy na ekranie,
  wolny RAM w historii, nagłówek serwera), rozstrzygał spór w kilka sekund.
- Nie zgadywać po raz kolejny, gdy poprzednie zgadywanie zawiodło. Lepiej
  poprosić o log albo zbudować mechanizm, który naprawia się sam.
- **Jego pomysły bywają lepsze niż moje odruchy.** Wybudzenie o 3:00 i lista
  sieci widzianych przez pudełko to jego propozycje; przy pierwszej odpowiedziałem
  odruchowo „to kosztowałoby prąd", zanim sprawdziłem, że dzieje się od dawna.
