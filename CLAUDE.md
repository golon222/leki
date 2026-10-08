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
**642 + 52 + 133 firmware, 1507 (×6 pór doby) + 92 + 52 aplikacja, 48 zgodności,
156 reguł bazy, 375 kontroli audytu, 50 kontroli statycznych — 0 błędów.**

**Nazwy leku w pudełku tygodniowym NIE MA** (D135, cofnięte z D126 na
wyraźną prośbę Kuby — nie przywracaj). Nazwa jest potrzebna tam, gdzie się
jej **używa**: w raporcie dla lekarza. W pudełku dziennym domyślną „Warfin"
zostawiamy, bo ma za sobą kilkaset wpisów z tą nazwą (D134) — każde inne
pudełko zaczyna bez nazwy i pokazuje „Lek". Podpis z lekiem powstaje
w **jednym** miejscu (`opisLeku()`); stał w dwóch, słowo w słowo.

**Przy profilu, który widzi mniej, domyślną odpowiedzią jest SCHOWAĆ**, a nie
zostawić „na wszelki wypadek". Kuba zgłosił to trzy razy z rzędu (skan sieci,
nazwa leku, „INR a regularność") i za każdym razem miał rację.

**To samo dotyczy DŹWIĘKU i wszystkiego innego, co pudełko komunikuje**
(D140, D147): informacja, której odbiorca nie może użyć do żadnej decyzji,
**nie jest neutralna — zajmuje miejsce tej, która może**. Numer komory
wypikiwany przy każdym otwarciu (dwie sekundy piknięć) zagłuszał jedyną
różnicę, która coś znaczy: czy dawka została zapisana, czy ta komora była
już dziś otwierana. Zostały dwa dźwięki: w górę = zapisane, w dół = już było.

**Sprawdzianem jest użyteczność, nie ilość** — i to działa w obie strony.
Ta sama liczba (numer komory) w trzech zastosowaniach: „środa" **odpadła**
(może być nieprawdą — zależy od kolejności rezystorów), siedem piknięć
**odpadło** (nie wiadomo, co z tym zrobić), a **„komora 3 jest otwarta"
ZOSTAJE** (D148), bo mówi, której klapki szukać. Zanim coś dodasz albo
usuniesz, zapytaj: **czy odbiorca może na tej podstawie coś zrobić.**

**Pudełko tygodniowe widzi mniej z DWÓCH powodów i to są różne powody**
(D126 i D133): czego nie potrzebuje (INR, zapas, raport) i czego nie umie
(sieć WiFi, autotest w bazie, historia wybudzeń). Ekran, za którym nic nie
stoi, wygląda jak zepsuty, nie jak nieistniejący. Ukrycie przycisku nie
wystarcza — `showTab()` odsyła z `inr`, `sinr`, `wifi` i `hist`.

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
funkcje z `PillBox.ino`, **`tests/extract_tydzien.py` z `PillBoxWeek.ino`**,
`tests/build_app_module.mjs` buduje moduł z `index.html`.
Jeśli zmieniasz nazwę wyciąganej funkcji, popraw też właściwy `extract*.py`.

**Pudełko tygodniowe do `0.15.0` nie miało ANI JEDNEGO testu jednostkowego** —
stała za nim wyłącznie kontrola statyczna, a ta **czyta kod, nie uruchamia go**.
Kosztowało to B31 i D145: trzy błędy w logice, którą test na biurku łapie
w milisekundy. Dokładasz coś do tego szkicu — dołóż też test (krok `1c/10`).

**Atrapa Firebase (`tests/firebase_stub.mjs`) sprawdza każdy zapis
prawdziwym `database.rules.json`** i umie zawieść na żądanie:

```js
__db.tryb = "blad";     // baza nieosiagalna (siec, timeout)
__db.tryb = "odmowa";   // baza osiagalna i odmawia (PERMISSION_DENIED)
__db.tryb = "wisi";     // Firebase offline - obietnica nigdy się nie kończy (D1)
__db.sprawdzajReguly = false;  // tylko dla testów piszących celowo śmieci
```

**`database.rules.json` to TYLKO PLIK — Firebase go nie czyta.** Reguły,
które naprawdę działają, to te wklejone w konsoli. Nic w repozytorium ich tam
nie wysyła. Po każdej zmianie tego pliku **trzeba go opublikować ręcznie**
(Realtime Database → Rules → wklej całość → Publish) i trzeba o tym Kubie
powiedzieć w tej samej wiadomości, w której zmieniasz reguły.

To jedyne miejsce w projekcie, gdzie **zielony zestaw testów niczego nie
gwarantuje**: atrapa bazy sprawdza każdy zapis plikiem z repozytorium, więc
testy przechodzą niezależnie od tego, co stoi w bazie. Kosztowało to już
jedno zgłoszenie — konto dziewczyny dostawało odmowę mimo poprawnego wpisu
w `owners`, bo baza wciąż miała reguły sprzed dołożenia `owners`.

Jeśli dokładasz pole do zapisu — **dopisz je też do `database.rules.json`**.
Gałąź `events` i pojedyncza dawka mają `$other: false`, więc nieznane pole
odrzuca **cały** wpis kodem 400, a `trwaleOdrzucony(400)` go wtedy **kasuje**
(D6, D13, D15).

---

## Twarde ograniczenia — NIE ŁAMAĆ

1. **Firmware pudełka dziennego to dokładnie dwa pliki**: `PillBox.ino` + `config.h`.
   Scalanie odrzucone. Pudełko **tygodniowe** ma własną parę plików w
   `firmware/PillBoxWeek/` — i to też jest decyzja, nie przypadek (D121):
   wspólny szkic z rozgałęzieniem znaczyłby, że każda zmiana w pudełku
   dziewczyny dotyka kodu pilnującego Warfinu.
2. **Żadnych zmian sprzętowych** — z jednym świadomym wyjątkiem, który
   zniósł sam Kuba: **czujnik baterii MAX17048 w pudełku tygodniowym**
   (D139, kupił go i podłączył). Płytka pudełka dziennego jest zlutowana
   i docelowo zaklejona; tam zakaz obowiązuje bez zmian.
   Czujnik jest **opcjonalny**: ma pierwszeństwo przed dzielnikiem, ale
   gdy nie odpowiada, pudełko schodzi na dzielnik, a potem na uczciwe
   „nie wiem". **Ten sam program chodzi z czujnikiem i bez** — inaczej
   jedno odejście przewodu zamieniłoby działające pudełko w martwe.
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
   Kolejka, flagi statusu, historia nieudanych zapisów. Rodzina błędu 3.5.
   **Jeden wyjątek, świadomy:** `queueDrop()` zdejmuje wpis, którego baza
   nie przyjmie **nigdy** (HTTP 400/413 albo rekord uszkodzony) — zostawiony
   blokował wszystkie dawki za sobą. Strata idzie na licznik `dropped`
   w statusie, więc aplikacja o niej krzyczy (D13).
7. **Do gałęzi `events` nie dokładamy pól.** Reguła `$other: false` odrzuca
   **cały** wpis, gdy trafi w nim nieznane pole — czyli otwarcie pudełka
   przepada w całości. Nowe dane idą do nowej gałęzi.
8. **Narzędzie diagnostyczne nie może uszkodzić danych o leku** — ani
   spowolnić drogi, którą one jadą. Dziennik wieczka miał z tego powodu
   własny bufor zamiast kolejki dawek, a i tak został usunięty (D109):
   dwa zapisy do flasha przy każdym ruchu wieczka stały **przed** startem
   radia, na ścieżce, która ma być najszybsza w urządzeniu. Test
   „40 ruchów wieczka nie tyka kolejki dawek" został i pilnuje obu połów.
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
   samej zasadzie 9** (D67, w pudełku tygodniowym D132). Aplikacja → baza → zapis w NVS → **odczyt
   kontrolny** → dopiero potem kasowanie z bazy. W `config.h` stać nie może
   z tego samego powodu co hasło do Firebase (ograniczenie 10).
   Wysyłka rusza **wyłącznie z `goToSleep()`** i jako **pierwsza** z trzech
   rzeczy przed snem — skan potrafi zerwać łącze, a udana aktualizacja
   kończy się restartem. Przy pustej skrzynce `tgWyslijZalegle()` wychodzi
   **przed** włączeniem radia. Powiadomienie starsze niż `TG_MAX_WIEK_S`
   **kasujemy zamiast wysyłać** — jedyny wyjątek od zasady 6 w tym obszarze,
   i nie dotyczy żadnych danych o leku. Token nie trafia **ani do logu, ani do
   statusu**. Audyt pilnuje każdego z tych punktów w pudełku dziennym,
   a kontrola statyczna — tych samych sześciu w tygodniowym (audyt czyta
   wyłącznie `PillBox.ino`).
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
   **Motyw pudełka tygodniowego (D128) MOŻE przygasić także kolory
   znaczeń** — na wyraźną prośbę Kuby („odejdźmy od neonowych, dawaj
   pudrowe"). Zasada broni **przypisania i rozpoznawalności**, nie
   konkretnych wartości: zielony ma dalej znaczyć „wzięte". Kontrola
   statyczna to **mierzy**, a nie zakazuje: sprawdza rodzinę odcieni
   każdego koloru i odległość każdej pary w przestrzeni Lab (próg 25).
   Motyw, który przygasza kolor, **musi wzmocnić jego tło** — przygaszony
   kolor niesie mniej sygnału, a kratka kalendarza to mała plama.
   **Od D129 motyw tygodniowy jest JASNY**: tło pudrowe, karty białe,
   tekst ciemny, akcent śliwkowy. Dlatego **kolor chromu nigdy nie stoi
   wprost w regule** — idzie przez `--aura`, `--hdr-rgb`, `--nav-bg`,
   `--nav-pier`, `--toast-bg`, `--zaslona`, `--blask-rgb` oraz
   `--ok-txt`/`--warn-txt`/`--bad-txt`. Cztery takie wartości wpisane
   wprost dotrwały do D129 i **żadnej nie zgłosił żaden test**: nagłówek
   malował czarny tytuł na czarnym, a poświata `#16203a` brudziła całą
   górę ekranu na szaro. Teraz mierzy to kontrola statyczna — jasność
   każdej powierzchni chromu (ciemna w palecie, jasna w motywie)
   i kontrast WCAG liczby dnia na kratce kalendarza (próg 4,5), a także
   tekstu drugorzędnego (`--dim`, `--dim2`) na karcie — próg 4,0, bo tyle
   ma paleta podstawowa. Kuba zgłosił po pierwszej wersji: „nie widać dni
   tygodnia w ogóle"; `--dim2` przeniesione z ciemnego motywu dawało na
   białej karcie 3,1, a `PN`/`WT`/`ŚR` to 10 px wersalikami.
   Wygląd sprawdzaj **na renderze**, nie w wyobraźni — i jest czym (D127):
   `node tests/podglad.mjs` buduje `tests/podglad.html`, stronę działającą
   bez Firebase i bez logowania, z podstawionym stanem. Otwierasz ją albo
   robisz zrzut Chromium jednym poleceniem:
   `chrome --headless=new --screenshot=x.png --window-size=560,1250
   "file://.../tests/podglad.html?profil=tydzien"`.
   Pierwszy taki zrzut od razu pokazał cztery rzeczy, których nie widział
   żaden test.
15. **Wyjaśnienia mieszkają w Instrukcji, nie na ekranach** (D75).
   Na ekranie zostaje tylko to, czego brak prowadzi do **złej decyzji
   o leku**; wszystko, co tłumaczy „jak to działa", idzie do ekranu
   `tab-help`. Kontrola statyczna pilnuje progu 200 znaków na akapit
   poza Instrukcją. Dwa świadome wyjątki: kroki parowania bota i przebieg
   autotestu.
   Ostrzeżenia dzielimy po **skutku**: dotyka dawek → na wierzchu
   w Ustawieniach (D11); nie dotyka → cicho w Diagnostyce (D74a).

16. **Do sieci prowadzi JEDNA droga: `wifiConnect()`** (D111). Nie pisz
   drugiego mechanizmu łączenia „bo tu nie można blokować". Taki mechanizm
   już był — powstał w D98, żeby pudełko przy otwartym wieczku nie było
   ślepe na jego zamknięcie — i przez **sześć wersji nie zameldował
   otwarcia ani razu**, podczas gdy blokujące `wifiConnect()` obok, na tej
   samej płytce, ściągało 1,2 MB aktualizacji.
   Ślepotę, dla której powstał, zdejmuje **dozorca**: `wifiDozorca`
   wołany z wnętrza czekania na `WL_CONNECTED` czyta kontaktron i przycisk
   co ~20 ms i przerywa **całe** łączenie w chwili zamknięcia wieczka.
   Obserwacja ma **jedną kopię** (`dozorKrok()`) — używa jej i pętla
   czekania, i łączenie. Dwie kopie rozjadą się przy pierwszej poprawce.
   **Meldunek o otwarciu jest ponawiany, nie jednorazowy**: za zgłoszone
   uznajemy dopiero potwierdzony zapis. Dziewięć kontroli audytu, każda
   sprawdzona mutacją.

17. **Dane człowieka idą przez `korzenDanych()`, nigdy wprost przez
   `users/${uid}`** (D124). W ścieżce dawek nie było numeru pudełka, więc
   jedno przełączenie na pudełko tygodniowe wpisywało jego otwarcia jako
   **dawki Warfinu**. Pudełko dzienne zostaje pod `users/<uid>` bez
   końcówki — nie z wygody, tylko żeby nie przenosić kilkuset wpisów
   o leku przeciwzakrzepowym; każde inne ma `users/<uid>/pud/<id>`.
   Kontrola statyczna pilnuje, że nikt tego nie obszedł.
   **Dostęp do pudełka daje `owner` ALBO `owners/<uid>`** — dziewczyna ma
   własne konto i własne hasło, a konto urządzenia
   (`pillbox02@device.local`) to osobny rodzaj konta i nie wolno go mieszać
   z kontem człowieka: jego hasło siedzi w zaklejonym pudełku.
   Aplikacja sprawdza dostęp **przed** nasłuchami; pudełko odcina
   **wyłącznie jawna odmowa reguł**, nigdy błąd sieci.
   **DZIAŁA — potwierdzone przez Kubę 2026-09-27: *„widzi i zapisuje"*.**
   Czyli reguły z `owners` i z gałęzią `users/<uid>/pud/<id>` są
   opublikowane w konsoli, a nie tylko w pliku. Oba warunki zamyka to
   jedno zdanie: „widzi" dowodzi `owners`, „zapisuje" dowodzi `pud` —
   samo „widzi" wychodziłoby tak samo przy starych regułach.

Blok pomiaru napięcia **wolno** zmieniać (zakaz zniesiony). Audyt nie blokuje —
zgłasza tylko uwagę, żeby zmiana przypadkowa nie wyglądała jak świadoma.

---

## Struktura

```
firmware/PillBox/PillBox.ino     główny kod pudełka DZIENNEGO (~6050 linii)
firmware/PillBox/config.h        ustawienia (w repo, bez hasła)
firmware/PillBoxTest/            osobny szkic diagnostyczny
firmware/PillBoxWeek/            pudełko TYGODNIOWE - siedem klapek (D121)
index.html                       cała PWA w jednym pliku
sw.js, tabletka.webp             service worker + tabletka na ekranie głównym
tabletka.gif                     zapas dla przeglądarki bez WEBP (D72)
tests/                           testy + audyt
tests/extract_tydzien.py         wyciecie kodu PUDELKA TYGODNIOWEGO do testow
tests/test_tydzien.cpp           testy logiki tygodniowego (krok 1c/10, D145)
tests/statyczna.py               kontrola statyczna (krok 4/10)
tests/mapa.py                    generator MAPA.md
tests/podglad.mjs                podglad wygladu w przegladarce (D127)
database.rules.json              reguły Firebase
MAPA.md                          spis treści dużych plików (generowany)
DECYZJE.md                       indeks decyzji; pełne wpisy w decyzje/
decyzje/                         dziennik decyzji po obszarach
PROJEKT-PillBox-kontekst.md      pełny kontekst projektu
WGRYWANIE.md                     instrukcja wgrywania kablem dla Kuby
WGRYWANIE-TYDZIEN.md             uruchomienie pudelka tygodniowego (konta, baza)
.github/workflows/firmware.yml   automat budujący binarkę do OTA
```

**Instrukcje dla Kuby nie mogą zawierać kroku, w którym jedno nieuważne
kliknięcie kasuje dane o leku** (D125). W konsoli Firebase wszystko dodajemy
**polami przez `+`**, nigdy `Import JSON` — import zastępuje zawartość węzła
i raz już skasował całą gałąź `devices`. Ostrzeżenie obok złego kroku nie
jest zabezpieczeniem, tylko przypisem do niego.

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

Stan: `PillBox.ino` **64% flasha** (1 265 417 B z 1,875 MB), `PillBoxWeek.ino` **62%** (1 235 191 B),
`PillBoxTest.ino` 20% bez `config.h` i **57%** z nim. Szkic diagnostyczny budujemy w OBU
konfiguracjach — bez tego drugiego przebiegu 722 kB jego kodu (logowanie do
bazy, zapis wyniku) nie było kompilowane ani razu (D103).
Zapas ~700 kB.

**Podział pamięci musi być `Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)`**,
bo tak jest w nagłówku szkicu i bo OTA zapisuje program do **drugiej** partycji
(D59). Skrypt sam sprawdza, czy nagłówek nadal zapowiada ten podział. Uwaga na
mylące nazwy opcji: `CDCOnBoot=default` znaczy **włączone**, `CDCOnBoot=cdc`
wyłączone.

**Binarki do aktualizacji buduje automat** (`.github/workflows/firmware.yml`)
przy każdej zmianie w `firmware/**` i kładzie je na GitHub Pages:
`firmware/PillBox.bin` + `PillBox.json` (dzienne) oraz `PillBoxWeek.bin`
+ `PillBoxWeek.json` (tygodniowe). **Każde pudełko ma własny odcisk źródła**
(`tests/zrodlo_firmware.sh dzienne|tygodniowe`) i publikuje się niezależnie —
wspólny kazałby pudełku dziewczyny ściągać megabajt z baterii przy każdej
poprawce Warfinu (D130). Nigdy nie buduj jej ręcznie do repo —
`OTA_OUT=<katalog> bash tests/kompiluj_firmware.sh` służy do sprawdzenia,
nie do publikacji. Szczegóły obejść — D17.

---

## Czego nie zweryfikowano

- Firmware **się kompiluje**, ale **nigdy nie było uruchomione z tego repo** na
  płytce. Kompilacja niczego nie wgrywa. Nie twierdź, że „działa".
- **Pudełko tygodniowe DZIAŁA — potwierdzone na płytce 2026-09-27, `0.3.0`.**
  Zmierzony przebieg Kuby: zimny start → `wybudzenie 1`, a **następne
  wybudzenie to prawdziwe otwarcie klapki**: `klapka: PON (105 mV, progi
  72..943)` — od `0.11.0` ten sam log mówi `komora 1`, bo komora przestała
  znaczyć dzień tygodnia (D140). 105 mV to **co do miliwolta** wartość z kalibracji na stole,
  więc drabinka rozpoznaje komorę po wybudzeniu z głębokiego snu tak samo
  jak na biurku. Pętli wybudzeń nie ma. Działa też logowanie do bazy, zapis
  hasła w NVS, pobranie harmonogramu i wysyłka zdarzeń.
  **Droga do tego zajęła trzy wersje i dwie moje błędne hipotezy** (B30):
  `0.1.0` budziło się w kółko i piszczało bez przerwy, bo `analogRead()`
  zostawia pin w trybie analogowym, a to wyłącza bufor wejścia cyfrowego,
  którym komparator wybudzania czyta stan pinu. Najpierw zwaliłem to na
  przypomnienie o 20:00 (Kuba: *„cały czas nie ma 20, jest 13:15"*), potem
  szukałem błędu w kodzie, którego na płytce nie było, bo numer wersji
  mieszkał w `config.h` i kłamał.
  **Aktualizacja przez WiFi — dodana w `0.4.0` (D130), NIESPRAWDZONA na
  płytce.** Kod jest kopią tej z pudełka dziennego (tam potwierdzona
  2026-08-16), `otaDecyzja()` jest znak w znak ta sama i kontrola
  statyczna tego pilnuje — ale żadna jej część nie została uruchomiona
  na tym urządzeniu. Warunkiem jest hasło w NVS; bez niego `otaDecyzja()`
  odmawia i to jest celowe.
  **Portal WiFi z przycisku — dodany w `0.5.0` (D131), NIESPRAWDZONY na
  płytce, i jest tu jedna rzecz warta uwagi przed wgraniem:** przycisk
  jest teraz **źródłem wybudzenia z głębokiego snu**, razem z klapkami,
  jedną maską na wspólnym poziomie niskim. W pudełku dziennym się tego
  nie dało (przeciwne poziomy), więc ta ścieżka nie ma za sobą żadnego
  przebiegu na sprzęcie. To ta sama rodzina co B30 — jeśli pudełko po
  wgraniu zacznie budzić się w kółko, **najpierw zobacz w logu, czy
  meldunek mówi `[BTN]`**, i napisz. Zabezpieczenia są dwa: przycisku
  wciśniętego przy zasypianiu nie uzbrajamy, a po trzech wybudzeniach
  z rzędu odpoczywa jeden sen.
  **Powiadomienia Telegram — dodane w `0.6.0` (D132), NIESPRAWDZONE na
  płytce.** Pisze o dwóch rzeczach (nieodebrane przypomnienie z **nazwą
  klapki**, słaba bateria), nie o czterech — bez zapasu tabletek i INR.
  `tgDecyzja()` jest znak w znak ta sama co w pudełku dziennym i kontrola
  statyczna to porównuje. Wymaga podłączenia bota w aplikacji.
  **Meldunek o otwartej klapce — dodany w `0.8.0` (D137), NAPRAWIONY
  PONOWNIE w `0.14.0` (B31).** Kuba, już z wgranym `0.11.0`: *„nie
  pokazuje się w aplikacji, jak otwieram cokolwiek"*. Status z `boxOpen`
  wychodził **wyłącznie** ze ścieżki zapisu dawki, a ta ma trzy wyjścia,
  które ją omijają: ta sama komora już dziś zgłoszona, kilka klapek
  naraz, komora nierozpoznana. **Odrzucenie powtórki dotyczy DANYCH
  o leku, `boxOpen` dotyczy STANU urządzenia — jedno nie ma prawa
  blokować drugiego.** Moja pierwsza hipoteza (stary program na płytce)
  była błędna i sprostował ją jednym zdaniem. Kuba: *„w
  aplikacji nie pokazuje się, że jest otwarte, jak jest otwarte"*.
  Przyczyny były dwie i obie po stronie pudełka: `boxOpen` nie było
  wysyłane w ogóle, a chwilę „ostatnio widziane" pudełko słało jako `ts`
  — pole, którego aplikacja **nie czyta nigdzie** (czyta `lastSeen`,
  w kilkunastu miejscach). **Jeśli dokładasz pole do statusu, sprawdź
  nazwę w aplikacji** — kontrola statyczna porównuje je teraz po nazwach.
  **Pudełko tygodniowe ma teraz wszystko, co dzienne** poza skanem sieci
  (niepotrzebnym, bo ma portal) i dziennikiem wieczka.
- **Pomiar baterii pudełka tygodniowego — od `0.10.0` jest CZUJNIK**
  (D139, rozszerzony w D141): Kuba kupił MAX17048; **lutowania jeszcze nie
  ma**. Czujnik ma pierwszeństwo, dzielnik został zapasem.
  **Niesprawdzone na płytce** — stan czujnika rozróżnia **trzy** rzeczy,
  nie dwie, i każdą naprawia się inaczej: pomiar z czujnika (gotowe),
  „czujnik jeszcze liczy" (przewody dobre, odczekaj minutę) oraz
  „czujnik się nie odzywa" (dopiero to jest lutownica).
  **Widać to w Ustawieniach → Urządzenie → Pomiar baterii, nie tylko
  w logu** (D142) — Kuba: *„nie będę patrzył na monitor, zobaczę
  w aplikacji"*. Diagnostyka, której warunkiem jest kabel i komputer,
  w praktyce nie istnieje: pudełko stoi u kogoś innego. Pola `gauge`
  i `voltDz` w statusie; `status` ma w regułach `$other: true`, więc
  **nic nie trzeba publikować w konsoli**.
  **Ładowanie poznaje po TEMPIE z czujnika** (D143, rejestr CRATE, próg
  2 %/h) — nie po wzroście napięcia, jak zgaduje pudełko dzienne. Pole
  `charging` wychodzi **wyłącznie** wtedy, gdy czujnik podał tempo, i to
  pilnuje kontrola statyczna. Dwa skutki: procent przy kablu jest
  prawdziwy, więc aplikacja go **nie wyszarza** (w pudełku dziennym
  wyszarza, bo dzielnik mierzy wtedy ładowarkę), a pudełko na ładowarce
  **śpi minutę zamiast godzin** — inaczej „pokaże się, że się ładuje"
  znaczyłoby „za cztery godziny".
  **Odczyt z czujnika ponawiamy trzy razy** (D141): pierwsze włączenie po
  przylutowaniu jest jedyną chwilą, w której ta ścieżka coś rozstrzyga,
  i najmniej pewną — MAX17048 potrzebuje chwili po podaniu zasilania.
  Dopóki czujnika nikt nie widział na płytce, poniższe zostaje w mocy,
  bo dzielnik nadal jest tym, co odpowiada przy milczącym czujniku.
- **Sam dzielnik — NIEWYJAŚNIONY, ale od `0.7.0`
  MIERZALNY Z ZEWNĄTRZ** (D136): napięcie idzie w statusie **zawsze**, także
  przy niemożliwym odczycie. Do `0.6.0` szło razem z procentem, więc przy
  zepsutym pomiarze nie szło nic. Pytanie do rozstrzygnięcia jest jedno:
  **czy ta liczba rusza się przy ładowaniu.** Jeśli tak — to dzielnik
  i da się to skalibrować; jeśli stoi — to zły pin albo brak kontaktu.
  **Od `0.11.0` odpowie na to sam czujnik** (D141): gdy odpowiada, pudełko
  mierzy w tej samej chwili także dzielnik. Dwa pomiary tej samej baterii
  w jednej sekundzie to pierwszy punkt odniesienia, jaki ta płytka miała.
  Jedzie to do aplikacji jako wiersz **„Dzielnik obok czujnika"**
  w Urządzeniu (D142) i do logu z ilorazem — **ten jeden wiersz
  rozstrzyga sprawę.**
  Procentu przy tym nie zgadujemy: `0%` znaczy „naładuj natychmiast".
  **Próg baterii dla aktualizacji jest w tym pudełku ZNIESIONY** (D138,
  `OTA_MIN_BATT_PCT = 0`): próg oparty na liczbie, której nie rozumiemy,
  nie jest zabezpieczeniem, tylko loterią — a źle zablokowana aktualizacja
  odcina jedyną zdalną drogę naprawy. **Wraca razem z naprawionym
  pomiarem**, nie wcześniej, i kontrola statyczna pilnuje, że te dwie
  rzeczy chodzą razem.
  Melduje `2,32 V`,
  czyli napięcie, przy którym płytka by nie chodziła. Program zgłasza to
  uczciwie jako brak danych (`-1%`), ale **przyczyny nie znamy**. Najpierw
  zwaliłem to na urwany pad BAT+ — Kuba sprostował, że na **tej** płytce pad
  jest cały, więc to było przeniesienie uszkodzenia ze starej. Do zmierzenia:
  co naprawdę stoi na dzielniku i czy trafia do właściwego pinu.
- **Powiadomienia Telegram — DZIAŁAJĄ, potwierdzone przez Kubę 2026-09-01:**
  *„Telegram działa jak coś, przypomnienia wysyłają się, kopie też się
  wysyłają"*. Wysyła je **pudełko**, z `goToSleep()`. To był najdłużej
  wiszący dług „kod się kompiluje, ale nic nie wyszło z płytki" — spłacony.
- **Meldunek o wieczku — DZIAŁA, potwierdzone przez Kubę 2026-08-31**, po
  `1.53.0`, na trzech otwarciach: *„po 3 sek pokazało się, że otwarte,
  zamknąłem i od razu pokazało — o to mi chodziło"*. Obie połowy, o które
  prosił od początku: „od razu, że otwarte" i „od razu, że zamknięte".
  Pomiar z tego samego zrzutu: `radio 3,3 s · baza 3,7 s` przy -56 dBm.
  Droga: 82,4 s (1.49.0) → 35,2 s (1.51.0) → **3,3 s** (1.53.0).
  To była **najważniejsza funkcja urządzenia** i naprawiało ją siedem
  wersji — nie ruszaj tej ścieżki bez potrzeby, a jeśli musisz, przeczytaj
  najpierw D111, D112 i D113 razem.
- **Dlaczego łączenie w tle nie działało — JUŻ WIADOMO** (D112,
  potwierdzone tym samym zrzutem). `netSkad` powiedział: **„pamięć
  sterownika — wpis na liście sieci nie zadziałał"**. Mechanizm w tle
  próbował wyłącznie podpowiedzi i listy, a u Kuby łączą tylko
  poświadczenia sterownika. Sześć wersji ciszy z jednego pominiętego
  kandydata.
- **Czy lista sieci w ogóle ma wpis — NIEZMIERZONE, i to był mój błąd
  w diagnostyce.** `netSkad = 254` znaczy tylko „połączyła pamięć
  sterownika" i wychodzi tak samo, gdy lista jest **pusta** (nic się nie
  zepsuło), jak i gdy ma **zły wpis**. Aplikacja pisała zawsze to drugie,
  czyli straszyła awarią, której może nie być — poprawione w
  `2026-09-01.78`, rozróżnia po polu `nets`. Rozstrzygnie kafelek WiFi
  w Ustawieniach: „zna N sieci". Dopóki pamięć sterownika działa, nic nie
  boli — ale ginie ona przy pełnym kasowaniu układu, więc warto to
  domknąć.
- **Czy wieczko domyka się mechanicznie — RACZEJ TAK, bez kompletu danych.**
  Po `1.53.0`: 60 drgnięć styku i tylko **3** odczyty bez ustalenia na 10
  wybudzeń. Styk drga (normalne przy zakręcaniu), ale niemal zawsze się
  uspokaja — to wskazuje na odbicia, nie na magnes, który nie dosięga.
  Kuba zgłaszał siłowanie przy zakręcaniu, więc nie zamykaj tematu;
  obserwuj `reedNiepewne` (próg 20).
- Prąd ładowania 350 mA to wartość katalogowa, nie pomiar.
- **Jakim kodem baza odrzuca wpis łamiący reguły.** Cała decyzja D13 zakłada
  400 — bo tylko wtedy `trwaleOdrzucony()` zdejmie wpis z kolejki. Nikt tego
  nie zmierzył. `pushEventRecord()` loguje odpowiedź bazy przy każdym
  niepowodzeniu, więc pierwszy log z pudełka to rozstrzygnie.
- **`setCACert()` — ŚWIADOMIE ODRZUCONE przez Kubę (D115), nie dług.**
  *„Jebać te certyfikaty, jak wygasają, to pasujemy z tego pomysłu"* —
  powiedziane po tym, jak dostał pełny opis skutków. Pudełko łączy się bez
  weryfikacji certyfikatu **ze wszystkim**: z Firebase
  (`rtdbClient.setInsecure()`) i z GitHub Pages przy pobieraniu programu.
  Przed uszkodzonym pobraniem chroni `Update.setMD5()`; przed **podmianą**
  nie chroni nic, i tak zostaje. Powód jest mocny: certyfikaty wygasają,
  a pudełko ma stać zaklejone latami — zabezpieczenie z terminem ważności
  unieruchomiłoby je w losowym dniu. **Nie wracaj z tym pomysłem** bez
  nowej okoliczności (np. praca na stałe w cudzej sieci).
- **Aktualizacja przez WiFi z przycisku — DZIAŁA, potwierdzone na płytce
  2026-08-16** (1.43.1 → 1.43.2), a wybudzenie o 3:00 też ją dowozi
  (`MIDNIGHT_CHECK`, potwierdzone 2026-08-17). Cała droga przeszła od początku do końca —
  przebieg i wcześniejsze fałszywe „sukcesy" opisuje D63 w `decyzje/ota.md`.
  **Przerwane pobieranie — przeczytane i bezpieczne, choć niewywołane
  celowo:** `Update.writeStream()` oddaje mniej bajtów, wchodzi
  `Update.abort()`, nowa partycja nigdy nie zostaje oznaczona jako
  rozruchowa i pudełko dalej chodzi na starej wersji, meldując „pobrano
  X kB z Y kB". Plik kompletny, ale z niezgodnym MD5, odrzuca
  `Update.end(true)` — tak samo bez szkody.
  **Nadal niesprawdzone na płytce:** rollback po nieudanym starcie i czarna
  lista zepsutych sum. Logika przeczytana i spójna (`otaSprawdzPoStarcie()`
  liczy starty, `otaPotwierdzDzialanie()` zeruje licznik przy pierwszym
  zaśnięciu, więc działająca wersja nie ma jak zostać cofnięta), ale ma
  jedną dziurę **nie do zaklejenia bez rollbacku bootloadera**: program,
  który wysypie się ZANIM dojdzie do `otaSprawdzPoStarcie()`, nigdy nie
  podniesie licznika i pętli startów nikt nie przerwie. Wtedy zostaje
  kabel. Nie udawaj, że jest inaczej.

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
