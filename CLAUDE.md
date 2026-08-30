# PillBox — instrukcje dla Claude Code

Inteligentne pudełko na leki (Seeed XIAO ESP32-C3) + PWA na iPhone.
Użytkownik: **Kuba**, Warfin 5 mg raz dziennie o 20:00. Lek przeciwzakrzepowy —
pominięta albo podwójna dawka to nie jest drobiazg.

**Rozmawiaj po polsku.**

---

## Jak czytać ten projekt, nie przepalając kontekstu

Trzy pliki są większe niż jakiekolwiek zadanie, które ich dotyczy:
`index.html` (~130 tys. tokenów), `PillBox.ino` (~89 tys.), `tests/test_app.mjs`
(~74 tys.). **Nie wczytuj żadnego z nich w całości.**

| chcesz | czytaj |
|---|---|
| znaleźć miejsce w kodzie | **`MAPA.md`** — spis treści z numerami linii, potem `sed -n 'od,dop' plik` |
| dowiedzieć się, dlaczego coś jest tak zrobione | **`DECYZJE.md`** — indeks jednolinijkowy, potem **jeden** plik z `decyzje/` |
| poznać historię błędu, który wrócił | `decyzje/bugi.md` |
| sprawdzić, czego nie próbować drugi raz | `decyzje/cofniete.md` |
| pełne tło projektu (raz na sesję, gdy naprawdę trzeba) | `PROJEKT-PillBox-kontekst.md` |

`MAPA.md` jest **generowana** przy każdym przebiegu testów (`tests/mapa.py`) —
nie poprawiaj jej ręcznie i nie zakładaj, że kłamie.

**Każdą własną decyzję dopisz od razu**: pełny wpis na górę właściwego pliku
w `decyzje/`, jedna linijka na górę indeksu w `DECYZJE.md`. Kontrola statyczna
sprawdza, że jedno zgadza się z drugim.

---

## Zanim cokolwiek zmienisz

```bash
bash tests/run_all.sh
```

Musi przejść przed zmianą i po zmianie. Stan wyjściowy:
**638 + 52 firmware, 1268 (×6 pór doby) + 92 + 52 aplikacja, 48 zgodności,
136 reguł bazy, 354 kontrole audytu, 30 kontroli statycznych — 0 błędów.**

**Runner jest cichy przy sukcesie i głośny przy błędzie** (D66). Udany przebieg
to 12 linii — **i te 12 linii TO JEST potwierdzenie, nie jego skrót.** Nie
odpalaj zestawu drugi raz z `SZCZEGOLY=1` „żeby sprawdzić dokładniej" i nie
przepuszczaj go przez `grep`: liczby podaje każdy krok sam o sobie, a `✔` na
końcu pojawia się wyłącznie wtedy, gdy **wszystkie** kroki wyszły.
(`SZCZEGOLY=1 bash tests/run_all.sh` daje pełny wypis, ~2300 linii — służy do
grzebania w konkretnym teście, nie do upewniania się, że zielone jest zielone.)

Krok, który zawiedzie, pokazuje **wszystkie** swoje linie błędu i zostawia
pełny log na dysku — `cat` na tej ścieżce jest tańszy niż powtórny przebieg
całości. Nie skracaj tej części: oszczędzamy wyłącznie na informacji
„nic się nie stało".

Testy pracują na **prawdziwym kodzie**, nie na kopii: `tests/extract.py` wycina
funkcje z `PillBox.ino`, `tests/build_app_module.mjs` buduje moduł z `index.html`.
Jeśli zmieniasz nazwę wyciąganej funkcji, popraw też `extract.py`.

**Atrapa Firebase (`tests/firebase_stub.mjs`) sprawdza każdy zapis
prawdziwym `database.rules.json`** i umie zawieść na żądanie:

```js
__db.tryb = "blad";     // baza nieosiagalna (siec, timeout)
__db.tryb = "odmowa";   // baza osiagalna i odmawia (PERMISSION_DENIED)
__db.tryb = "wisi";     // Firebase offline - obietnica nigdy się nie kończy (D1)
__db.sprawdzajReguly = false;  // tylko dla testów piszących celowo śmieci
```

Jeśli dokładasz pole do zapisu — **dopisz je też do `database.rules.json`**.
Gałąź `events` i pojedyncza dawka mają `$other: false`, więc nieznane pole
odrzuca **cały** wpis kodem 400, a `trwaleOdrzucony(400)` go wtedy **kasuje**
(D6, D13, D15).

---

## Twarde ograniczenia — NIE ŁAMAĆ

1. **Firmware to dokładnie dwa pliki**: `PillBox.ino` + `config.h`. Scalanie odrzucone.
2. **Żadnych zmian sprzętowych.** Płytka jest zlutowana i docelowo zaklejona.
3. **`config.h` JEST w repo — celowo, i tak ma zostać.**
   Trzyma wyłącznie placeholder `TUTAJ_WPISZ_HASLO`, nigdy prawdziwego hasła.

   Placeholder nie jest kompromisem dla wygody — **jest warunkiem, na którym
   stoi aktualizacja przez WiFi**: binarkę buduje automat z **tego** repo,
   publicznie, więc wkompilowane hasło byłoby jego wyciekiem. Działa to dlatego,
   że hasło żyje w pamięci trwałej pudełka (NVS), a `config.h` jest już tylko
   **ziarnem** przy pierwszym wgraniu kablem. Patrz ograniczenie 10 — to jedna
   decyzja z dwóch stron. Drugi powód: Kuba pobiera folder `firmware/` z GitHuba
   i otwiera go wprost w Arduino IDE; bez `config.h` szkic się nie otwiera.
   Próba zastąpienia go `config.example.h` **została cofnięta na jego wyraźną
   prośbę** — nie przywracaj jej (D5 w `decyzje/cofniete.md`, D7).

   `WEB_API_KEY` zostaje świadomie: ten sam klucz jest publiczny w `index.html`
   na GitHub Pages, a barierą jest `database.rules.json`, nie jego tajność.
4. **`DAY_START_HOUR = 3`** identycznie w firmware i aplikacji.
4b. **`cfg.schedule` to godziny PRZYPOMNIEŃ, nie pory brania leku.**
   Kuba bierze tabletkę kiedy chce — o 10, o 14, o 21, czasem o 2 w nocy.
   Pudełko ma tylko przypomnieć, jeśli do danej godziny jeszcze jej nie wziął.
   Dawka jest **jedna dziennie** (`ONE_DOSE_PER_DAY`) i zawsze siedzi
   w slocie **0**. Druga pozycja w harmonogramie znaczy „przypomnij jeszcze
   raz o 23:00", a nie „weź drugą tabletkę". Nie licz dawek przez
   `schedule.length` — od tego był błąd B9.
4c. **Dawek dziennie jest jedna, ale TABLETEK w niej zmienna liczba** (D36).
   `dawkaNaDzien(key)`: wyjątek na datę → rozpisanie tygodniowe → `defaultDose`.
   Indeks w `doseWeek` to `getDay()`/`tm_wday`, czyli **0 = niedziela** —
   tak samo w firmware. Schemat niekompletny odrzucamy w całości i wracamy
   do `defaultDose`: brakujące pole odczytane jako zero to cichy dzień bez
   leku przeciwzakrzepowego. Dzień z zerem ma status `off` i **nie wchodzi
   ani do licznika, ani do mianownika** skuteczności — w czterech miejscach,
   każde z własną pętlą.
   Pudełko zna to samo rozpisanie (`rtcDoseWeek`, `rtcDoseExDay`) i w dniu
   rozpisanym na zero **nie dzwoni i nie zgłasza „missed"**. Wycisza się
   **wyłącznie przy pewnym zerze**: bez zegara albo bez rozpisania dzwoni.
4d. **Dzień lekowy rozstrzyga się z KOŃCEM doby, nie z ostatnim
   przypomnieniem** (D64). Wzięta dawka rozstrzyga dzień od razu; niewzięta
   dopiero po `dzienZamkniety()`, czyli po granicy `DAY_START_HOUR`. Obowiązuje
   w **trzech** miejscach naraz: kalendarz, pierścień skuteczności i raport dla
   lekarza. Wpis **ręczny** wygrywa także dziś. Bez tego kalendarz malował
   dzisiejszy dzień na czerwono o 20:06, a skuteczność sama się cofała.
5. **Każdy zapis użytkownika w aplikacji idzie przez `zapiszPewnie()`.**
   Nigdy gołe `set()`. Firebase offline nie odrzuca obietnicy, tylko wisi —
   ekran pokazuje sukces, dane nie docierają. Test tego pilnuje.
6. **Nic nie kasujemy z pamięci pudełka przed potwierdzonym 2xx.**
   Kolejka, flagi statusu, dziennik wieczka. Rodzina błędu 3.5.
   **Jeden wyjątek, świadomy:** `queueDrop()` zdejmuje wpis, którego baza
   nie przyjmie **nigdy** (HTTP 400/413 albo rekord uszkodzony) — zostawiony
   blokował wszystkie dawki za sobą. Strata idzie na licznik `dropped`
   w statusie, więc aplikacja o niej krzyczy (D13).
7. **Do gałęzi `events` nie dokładamy pól.** Reguła `$other: false` odrzuca
   **cały** wpis, gdy trafi w nim nieznane pole — czyli otwarcie pudełka
   przepada w całości. Nowe dane idą do nowej gałęzi.
8. **Narzędzie diagnostyczne nie może uszkodzić danych o leku.**
   Stąd dziennik wieczka ma własny bufor zamiast kolejki dawek.
9. **Hasło do WiFi kasujemy z bazy dopiero po potwierdzonym zapisie w NVS**
   (D38). `wifiSiecDodaj()` zwraca wynik i ten wynik trzeba sprawdzić.
   Odwrotna kolejność traci sieć, której nikt już nie zna — a z nią jedyną
   drogę do pudełka poza portalem. Portal fizyczny zostaje na zawsze.
10. **Hasło do Firebase czytamy z NVS, nigdy wprost z `config.h`** (D59).
   To ta sama decyzja co ograniczenie 3, widziana od strony kodu.
   `hasloDoLogowania()` daje pierwszeństwo pamięci trwałej. Wgranie binarki
   z placeholderem pudełku, które hasła nie ma w NVS, odcięłoby je od bazy —
   czyli od jedynej drogi naprawy bez kabla. Dlatego `otaDecyzja()` odmawia
   aktualizacji bez hasła w pamięci, a `hasloUtrwal()` potwierdza zapis
   **odczytem zwrotnym**. Nie upraszczaj żadnego z tych trzech kroków.
11. **Aktualizacja i skan sieci ruszają wyłącznie z `goToSleep()`.** To jedyne
   miejsce, przez które przechodzi każda ścieżka wybudzenia, i jedyne, w którym
   dawka jest już zapisana i potwierdzona. Wywołanie z `fetchConfig()` albo
   z obsługi kontaktronu wcisnęłoby minutę radia między otwarcie wieczka a zapis
   dawki Warfinu. Audyt to sprawdza.
   `otaSprobuj()` **sam dopytuje bazę** o zlecenie (D61) — nie polegaj na tym,
   co `fetchConfig()` widziało na początku wybudzenia (D62).
   `skanujSieci()` publikuje listę sieci z siłą sygnału do **własnej gałęzi
   `scan`** (D65) i kasuje zlecenie `wifiScan` dopiero po potwierdzonym zapisie.
12. **Token bota Telegram idzie tą samą drogą co hasło WiFi i podlega tej
   samej zasadzie 9** (D67). Aplikacja → baza → zapis w NVS → **odczyt
   kontrolny** → dopiero potem kasowanie z bazy. W `config.h` stać nie może
   z tego samego powodu co hasło do Firebase (ograniczenie 10).
   Wysyłka rusza **wyłącznie z `goToSleep()`** i jako **pierwsza** z trzech
   rzeczy przed snem — skan potrafi zerwać łącze, a udana aktualizacja
   kończy się restartem. Przy pustej skrzynce `tgWyslijZalegle()` wychodzi
   **przed** włączeniem radia. Powiadomienie starsze niż `TG_MAX_WIEK_S`
   **kasujemy zamiast wysyłać** — jedyny wyjątek od zasady 6 w tym obszarze,
   i nie dotyczy żadnych danych o leku. Token nie trafia **ani do logu, ani do
   statusu**. Audyt pilnuje każdego z tych punktów.
13. **Osłona rysowania (`rysuj()`) obejmuje WYŁĄCZNIE rysowanie** (D71).
   Wyjątek połknięty w renderze ratuje ekran; połknięty w zapisie gubi dawkę
   po cichu. `doReconcile()`, `settlePills()`, `zapiszPewnie()` i `zapiszCfg()`
   nigdy nie idą przez osłonę — zapis, który się nie udał, ma krzyknąć.
   Kontrola statyczna to sprawdza i była sprawdzona mutacją.
14. **System wizualny ma reguły — nie zmieniaj ich „na oko"** (D74).
   Kolor niesie znaczenie: zielony/żółty/czerwony należą do stanu dawki
   i nigdzie indziej. Odstępy idą po skali `--s1..--s7` (4 px), promienie
   po `--r*`, krawędzie to półprzezroczysta biel (`--line`), nie pełny
   kolor. Domyślny przycisk ma 44 px wysokości. Kontrola statyczna pilnuje
   każdego z tych punktów.
   **Pasek nawigacji wolno zmieniać** — zakaz zdjęty na prośbę Kuby (D78).
   Zostają dwa niezmienniki: rezerwa `env(safe-area-inset-bottom)` i to, że
   półprzezroczyste tło idzie zawsze razem z rozmyciem (z prefiksem
   `-webkit-`). Wiedza z D48–D52 zostaje jako ostrzeżenie, nie zakaz: gdyby
   objaw „pasek ucieka przy przewijaniu" wrócił, **najpierw zmierz**, czym
   różni się klatka, w której ucieka — pięć podejść po omacku nic nie dało.
   Wygląd sprawdzaj **na renderze**, nie w wyobraźni.
15. **Wyjaśnienia mieszkają w Instrukcji, nie na ekranach** (D75).
   Na ekranie zostaje tylko to, czego brak prowadzi do **złej decyzji
   o leku**; wszystko, co tłumaczy „jak to działa", idzie do ekranu
   `tab-help`. Kontrola statyczna pilnuje progu 200 znaków na akapit
   poza Instrukcją. Dwa świadome wyjątki: kroki parowania bota i przebieg
   autotestu.
   Ostrzeżenia dzielimy po **skutku**: dotyka dawek → na wierzchu
   w Ustawieniach (D11); nie dotyka → cicho w Diagnostyce (D74a).

Blok pomiaru napięcia **wolno** zmieniać (zakaz zniesiony). Audyt nie blokuje —
zgłasza tylko uwagę, żeby zmiana przypadkowa nie wyglądała jak świadoma.

---

## Struktura

```
firmware/PillBox/PillBox.ino     główny kod (~6050 linii)
firmware/PillBox/config.h        ustawienia (w repo, bez hasła)
firmware/PillBoxTest/            osobny szkic diagnostyczny
index.html                       cała PWA w jednym pliku
sw.js, tabletka.webp             service worker + tabletka na ekranie głównym
tabletka.gif                     zapas dla przeglądarki bez WEBP (D72)
tests/                           testy + audyt
tests/statyczna.py               kontrola statyczna (krok 4/10)
tests/mapa.py                    generator MAPA.md
database.rules.json              reguły Firebase
MAPA.md                          spis treści dużych plików (generowany)
DECYZJE.md                       indeks decyzji; pełne wpisy w decyzje/
decyzje/                         dziennik decyzji po obszarach
PROJEKT-PillBox-kontekst.md      pełny kontekst projektu
WGRYWANIE.md                     instrukcja wgrywania kablem dla Kuby
.github/workflows/firmware.yml   automat budujący binarkę do OTA
```

Po zmianie w plikach aplikacji **podbij `APP_VERSION` i `CACHE` w `sw.js`** — inaczej
telefon zostanie na starej wersji. Po zmianie firmware podbij `FW_VERSION`.

---

## Kompilacja firmware

```bash
bash tests/kompiluj_firmware.sh
```

Buduje **oba** szkice prawdziwym toolchainem Arduino, **dwa razy: jako `.cpp`
i przez prawdziwą ścieżkę `.ino` z wygenerowanymi prototypami** (B21/D26 —
przez miesiąc sprawdzaliśmy tylko `.cpp` i firmware nie dawał się wgrać),
na `esp32:esp32@3.3.11` i z **ustawieniami płytki z nagłówka `PillBox.ino`**.
Nie jest częścią `run_all.sh`: wymaga sieci i ~500 MB toolchainu.
**Uruchom to po każdej zmianie w firmware.**

Stan: `PillBox.ino` **64% flasha** (1 265 383 B z 1,875 MB), `PillBoxTest.ino` 20%
bez `config.h` i **57%** z nim. Szkic diagnostyczny budujemy w OBU
konfiguracjach — bez tego drugiego przebiegu 722 kB jego kodu (logowanie do
bazy, zapis wyniku) nie było kompilowane ani razu (D103).
Zapas ~718 kB.

**Podział pamięci musi być `Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)`**,
bo tak jest w nagłówku szkicu i bo OTA zapisuje program do **drugiej** partycji
(D59). Skrypt sam sprawdza, czy nagłówek nadal zapowiada ten podział. Uwaga na
mylące nazwy opcji: `CDCOnBoot=default` znaczy **włączone**, `CDCOnBoot=cdc`
wyłączone.

**Binarkę do aktualizacji buduje automat** (`.github/workflows/firmware.yml`)
przy każdej zmianie w `firmware/**` i kładzie ją jako `firmware/PillBox.bin`
+ `PillBox.json` na GitHub Pages. Nigdy nie buduj jej ręcznie do repo —
`OTA_OUT=<katalog> bash tests/kompiluj_firmware.sh` służy do sprawdzenia,
nie do publikacji. Szczegóły obejść — D17.

---

## Czego nie zweryfikowano

- Firmware **się kompiluje**, ale **nigdy nie było uruchomione z tego repo** na
  płytce. Kompilacja niczego nie wgrywa. Nie twierdź, że „działa".
- **Powiadomienia Telegram (1.45.0) — ani jedna wiadomość nie wyszła jeszcze
  z płytki.** Kod się kompiluje i ma 82 kontrole, ale to nie jest dowód.
  Rozstrzygnie przycisk „wyślij wiadomość próbną" — po to powstał.
- Prąd ładowania 350 mA to wartość katalogowa, nie pomiar.
- **Jakim kodem baza odrzuca wpis łamiący reguły.** Cała decyzja D13 zakłada
  400 — bo tylko wtedy `trwaleOdrzucony()` zdejmie wpis z kolejki. Nikt tego
  nie zmierzył. `pushEventRecord()` loguje odpowiedź bazy przy każdym
  niepowodzeniu, więc pierwszy log z pudełka to rozstrzygnie.
- **`setCACert()` — niespłacony dług, i to jedyna prawdziwa obrona.**
  Pudełko łączy się bez weryfikacji certyfikatu **ze wszystkim**: z Firebase
  (`rtdbClient.setInsecure()`) i z GitHub Pages przy pobieraniu programu.
  Przed uszkodzonym pobraniem chroni `Update.setMD5()`; przed **podmianą**
  nie chroni dziś nic (D59). Nie udawaj, że jest inaczej.
- **Aktualizacja przez WiFi z przycisku — DZIAŁA, potwierdzone na płytce
  2026-08-16** (1.43.1 → 1.43.2), a wybudzenie o 3:00 też ją dowozi
  (`MIDNIGHT_CHECK`, potwierdzone 2026-08-17). Cała droga przeszła od początku do końca —
  przebieg i wcześniejsze fałszywe „sukcesy" opisuje D63 w `decyzje/ota.md`.
  **Nadal niesprawdzone:** rollback po nieudanym starcie, czarna lista
  zepsutych sum i zachowanie przy przerwanym pobieraniu.

---

## Jak pracować z Kubą

- Konkretnie, bez asekuracji. Po polsku.
- **Publikuj bez pytania.** Jego słowa: *„od razu merguj wszystko, nie pytaj,
  najwyżej będziemy cofać — i tak muszę zobaczyć, jak to wygląda w aplikacji"*.
  Skończona zmiana z zielonym zestawem idzie na `main` od razu. Cofnięcie jest
  tańsze niż czekanie.
- **Jego opisy objawów są cenniejsze niż twoje hipotezy.** Przełomem zawsze był
  jego konkretny opis z liczbami, nie kolejna hipoteza.
- Gdy coś jest niepewne — powiedz wprost, że to hipoteza, i **dołóż pomiar**
  zamiast zgadywać kolejny raz.
- Powiadomienia na telefon: **zrobione w 1.45.0** (D67), rozszerzone w 1.46.0
  o kończące się opakowanie i termin INR (D83) — bot Telegram, wysyła
  **pudełko**. Nie sprawdzone na płytce; wymaga podłączenia bota w aplikacji.
