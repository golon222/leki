# PillBox — instrukcje dla Claude Code

Inteligentne pudełko na leki (Seeed XIAO ESP32-C3) + PWA na iPhone.
Użytkownik: **Kuba**, Warfin 5 mg raz dziennie o 20:00. Lek przeciwzakrzepowy —
pominięta albo podwójna dawka to nie jest drobiazg.

**Rozmawiaj po polsku.**

> **Przeczytaj `PROJEKT-PillBox-kontekst.md`** przed pierwszą zmianą.
> Są tam przyczyny dziewięciu błędów, które kosztowały godziny szukania.
> Nie cofaj poprawki, nie znając powodu jej powstania.
>
> **Przeczytaj `DECYZJE.md`** — dziennik decyzji: co jest tymczasowe i kiedy
> to usunąć, co zostało zrobione dlaczego, co już raz **cofnięto** i czego
> nie próbować drugi raz. Każdą własną decyzję dopisz tam od razu, nie potem.

---

## Zanim cokolwiek zmienisz

```bash
bash tests/run_all.sh
```

Musi przejść przed zmianą i po zmianie. Stan wyjściowy:
**484 + 51 firmware, 888 (×6 pór doby) + 92 + 49 aplikacja, 48 zgodności,
93 reguł bazy, 276 kontroli audytu — 0 błędów.**

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
odrzuca **cały** wpis kodem 400, a `trwaleOdrzucony(400)` go wtedy **kasuje**.
Test to złapie, ale komunikat zrozumiesz szybciej, znając powód (D6, D13, D15).

---

## Twarde ograniczenia — NIE ŁAMAĆ

1. **Firmware to dokładnie dwa pliki**: `PillBox.ino` + `config.h`. Scalanie odrzucone.
2. **Żadnych zmian sprzętowych.** Płytka jest zlutowana i docelowo zaklejona.
3. **`config.h` JEST w repo — celowo, i tak ma zostać.**
   Trzyma wyłącznie placeholder `TUTAJ_WPISZ_HASLO`, nigdy prawdziwego hasła.
   Powód: Kuba pracuje tak, że pobiera folder `firmware/` z GitHuba i otwiera
   go wprost w Arduino IDE. Bez `config.h` w komplecie szkic się nie otwiera,
   a on musi zmieniać nazwy plików na telefonie albo MacBooku przed wyjazdem.
   Krótka próba trzymania tu tylko `config.example.h` **została cofnięta na
   jego wyraźną prośbę** — nie przywracaj jej.
   Prawdziwe hasło żyje wyłącznie na jego dysku i **nigdy nie wraca do repo**.
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
5. **Każdy zapis użytkownika w aplikacji idzie przez `zapiszPewnie()`.**
   Nigdy gołe `set()`. Firebase offline nie odrzuca obietnicy, tylko wisi —
   ekran pokazuje sukces, dane nie docierają. Test tego pilnuje.
6. **Nic nie kasujemy z pamięci pudełka przed potwierdzonym 2xx.**
   Kolejka, flagi statusu, dziennik wieczka. Rodzina błędu 3.5.
   **Jeden wyjątek, świadomy:** `queueDrop()` zdejmuje wpis, którego baza
   nie przyjmie **nigdy** (HTTP 400/413 albo rekord uszkodzony) — zostawiony
   blokował wszystkie dawki za sobą. Strata idzie na licznik `dropped`
   w statusie, więc aplikacja o niej krzyczy. Powód w `DECYZJE.md` D13.
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
   `hasloDoLogowania()` daje pierwszeństwo pamięci trwałej; `config.h` jest
   już tylko **ziarnem** przy pierwszym wgraniu kablem. Powód jest twardy:
   binarkę aktualizacji buduje automat z tego repo, a w repo stoi placeholder.
   Wgranie jej pudełku, które hasła nie ma w NVS, odcięłoby je od bazy —
   czyli od jedynej drogi naprawy bez kabla. Dlatego `otaDecyzja()` odmawia
   aktualizacji bez hasła w pamięci, a `hasloUtrwal()` potwierdza zapis
   **odczytem zwrotnym**. Nie upraszczaj żadnego z tych trzech kroków.

11. **Aktualizacja rusza wyłącznie z `goToSleep()`.** To jedyne miejsce, przez
   które przechodzi każda ścieżka wybudzenia, i jedyne, w którym dawka jest
   już zapisana i potwierdzona. Wywołanie z `fetchConfig()` albo z obsługi
   kontaktronu wcisnęłoby minutę radia między otwarcie wieczka a zapis dawki
   Warfinu. Audyt to sprawdza i złapie taką zmianę.

Blok pomiaru napięcia **wolno** zmieniać (zakaz zniesiony). Audyt nie blokuje —
zgłasza tylko uwagę, żeby zmiana przypadkowa nie wyglądała jak świadoma.

---

## Struktura

```
firmware/PillBox/PillBox.ino     główny kod (~2600 linii)
firmware/PillBox/config.h        ustawienia (w repo, bez hasła)
firmware/PillBoxTest/            osobny szkic diagnostyczny
index.html                   cała PWA w jednym pliku
sw.js, tabletka.gif      service worker + tabletka na ekranie głównym
tests/                           testy + audyt
database.rules.json              reguły Firebase
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
na `esp32:esp32@3.3.11`
i z **ustawieniami płytki z nagłówka `PillBox.ino`** — czyli tak, jak Kuba
naprawdę wgrywa. Nie jest częścią `run_all.sh`: wymaga sieci i ~500 MB
toolchainu (pierwsze uruchomienie kilka minut, kolejne szybkie).
**Uruchom to po każdej zmianie w firmware.**

Stan: `PillBox.ino` **63% flasha** (1,24 MB z 1,875 MB), `PillBoxTest.ino` 20%.
Zapas ~727 kB.

**Podział pamięci musi być `Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)`**,
bo tak jest w nagłówku szkicu. Zmieniony w 1.38.0 z `Huge APP` (D59):
aktualizacja przez WiFi zapisuje program do **drugiej** partycji, a `Huge APP`
ma tylko jedną. Program ma teraz 1,875 MB zamiast 3 MB — nadal dużo, ale to
już nie jest nieskończoność. Skrypt sam sprawdza, czy nagłówek nadal zapowiada
ten podział, i przerywa, gdy się rozjadą. Uwaga na mylące nazwy opcji:
`CDCOnBoot=default` znaczy **włączone**, `CDCOnBoot=cdc` wyłączone.

**Binarkę do aktualizacji buduje automat** (`.github/workflows/firmware.yml`)
przy każdej zmianie w `firmware/**` i kładzie ją jako `firmware/PillBox.bin`
+ `PillBox.json` na GitHub Pages. Nigdy nie buduj jej ręcznie do repo —
`OTA_OUT=<katalog> bash tests/kompiluj_firmware.sh` służy do sprawdzenia,
nie do publikacji.

Szczegóły obejść (ctags, `.cpp` zamiast `.ino`, atrapa `dfu-util`) — D17.

## Czego nie zweryfikowano

- Firmware **się kompiluje**, ale **nigdy nie było uruchomione z tego repo** na
  płytce. Kompilacja niczego nie wgrywa. Nie twierdź, że „działa".
- Prąd ładowania 350 mA to wartość katalogowa, nie pomiar.
- **Jakim kodem baza odrzuca wpis łamiący reguły.** Cała decyzja D13 zakłada
  400 — bo tylko wtedy `trwaleOdrzucony()` zdejmie wpis z kolejki. Nikt tego
  nie zmierzył. `pushEventRecord()` loguje teraz odpowiedź bazy przy każdym
  niepowodzeniu, więc pierwszy log z pudełka to rozstrzygnie.
- **`setCACert()` — niespłacony dług, i to jedyna prawdziwa obrona.**
  Pudełko łączy się bez weryfikacji certyfikatu **ze wszystkim**: z Firebase
  (`rtdbClient.setInsecure()`) i z GitHub Pages przy pobieraniu programu.
  W 1.40.0 **usunięto** porównywanie sumy z bazy z sumą z pliku, bo przy dwóch
  nieweryfikowanych kanałach nie dawało ono nic poza kosztem (D59). Przed
  uszkodzonym pobraniem chroni `Update.setMD5()`; przed **podmianą** nie chroni
  dziś nic. Nie udawaj, że jest inaczej.
- **Aktualizacja przez WiFi z przycisku w aplikacji — DZIAŁA, potwierdzone
  na płytce 2026-08-16** (D59–D62). Kuba wgrał kablem 1.43.1, nacisnął
  przycisk, otworzył wieczko i **1.43.2 weszła sama**: dwa piknięcia
  („zaczynam"), jedno („zapisane, restartuję"), fanfara po starcie i nowy
  numer wersji w aplikacji. Cała droga — zlecenie z telefonu, pobranie
  1,24 MB, zapis do drugiej partycji, restart, potwierdzenie — przeszła
  od początku do końca. **To był pierwszy raz.**
  Wcześniejsze „udane" OTA nie liczą się jako dowód tej drogi: 1.39.0
  (2026-08-12) pobrało się samo, ale nikt tego nie zauważył, a wersji
  zamówionej z przycisku pudełko nie przyjęło ani razu aż do 1.43.2.
  **Wybudzenie o 3:00 też dowozi aktualizację — potwierdzone 2026-08-17.**
  Kuba złożył zlecenie wieczorem i poszedł spać; rano pudełko miało nową
  wersję, bez otwierania wieczka i bez ładowarki. Granica doby lekowej
  (`MIDNIGHT_CHECK`) daje więc pełny meldunek: sync zegara, odczyt
  ustawień, opróżnienie kolejki i szansa dla aktualizacji.
  **Nadal niesprawdzone:** rollback po nieudanym starcie (żadna wersja się
  jeszcze nie wysypała), czarna lista zepsutych sum i zachowanie przy
  przerwanym pobieraniu.

---

## Jak pracować z Kubą

- Konkretnie, bez asekuracji. Po polsku.
- **Jego opisy objawów są cenniejsze niż twoje hipotezy.** W poprzedniej sesji
  kilka razy naprawiano rzeczy, które nie były przyczyną; przełomem zawsze był
  jego konkretny opis z liczbami.
- Gdy coś jest niepewne — powiedz wprost, że to hipoteza, i **dołóż pomiar**
  zamiast zgadywać kolejny raz.
- Powiadomienia push: czeka na hasło **„dawaj kod"**. Rekomendacja: bot Telegram.
