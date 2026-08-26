# Dziennik decyzji — indeks

Kod mówi *co* robi. Ten plik mówi **dlaczego akurat tak** — i czego nie cofać.

**Pełne wpisy leżą w `decyzje/`.** Tu jest jedna linijka na decyzję: numer, o co
chodziło, w którym pliku stoi rozwinięcie. Wchodząc w zadanie przeczytaj **ten
indeks w całości** i **tylko ten plik tematyczny**, którego zadanie dotyka —
zamiast 60 tys. tokenów wchodzi 2,5 tys. plus jeden obszar.

| plik | o czym | ile |
|---|---|---|
| `decyzje/pudelko.md` | Pudełko — sen, alarm, NVS, WiFi | 21 |
| `decyzje/ota.md` | Aktualizacja przez WiFi i skan sieci | 8 |
| `decyzje/telegram.md` | Powiadomienia Telegram | 5 |
| `decyzje/aplikacja.md` | Aplikacja — ekrany i wygląd | 34 |
| `decyzje/dane.md` | Dane, kolejka, dawkowanie | 17 |
| `decyzje/testy.md` | Testy, audyt, kompilacja | 12 |
| `decyzje/bugi.md` | Błędy **zamknięte** — historia objawów | 25 |
| `decyzje/cofniete.md` | Cofnięte — **nie próbować drugi raz** | 5 |

**Dopisując decyzję:** pełny wpis na górę właściwego pliku w `decyzje/`, jedna
linijka na górę indeksu poniżej. Cofniętej **nie kasuj** — przenieś do
`decyzje/cofniete.md`. Kontrola statyczna pilnuje, że indeks i pliki się zgadzają.

Szczegóły starszych błędów: `PROJEKT-PillBox-kontekst.md` sekcja 3.
Sprawy świadomie odłożone: tamże sekcja 8.

---

## 1. Tymczasowe — do usunięcia

> Ta sekcja ma być pusta. Jeśli coś stoi tu długo, stało się stałe przez
> zaniedbanie, a nie przez decyzję.

| Co | Po co powstało | Kiedy usunąć |
|---|---|---|
| **Dziennik wieczka**<br>`lidLog*`, `LIDLOG_SLOTS`, ekran „Dziennik wieczka", gałąź `lidlog`, `users/<uid>/lidMarks` | Zmierzyć, czy pudełko melduje otwarcia, których nikt nie zrobił — np. gdy magnes przesunie się w plecaku. Bez tego **B1** to zgadywanie. | Gdy wróci tygodniowy pomiar i zapadnie decyzja w sprawie **B1**. Wtedy leci wszystko: firmware, ekran w aplikacji, dane w bazie. |

---

## 2. Błędy otwarte

Zamknięte (z objawami i przyczynami) — `decyzje/bugi.md`.

| # | Bug | Objaw | Czemu jeszcze stoi |
|---|---|---|---|
| **N3** | Test ikonki terminu INR renderował ją we „własnym" miesiącu, nie w tym, gdzie wypada „dziś" | Znaleziony przypadkiem: losowa strefa GMT-12 w teście 2b/10 przesunęła „dziś" testu na 11 sierpnia, a termin (dziś + 21 dni) wypadł we wrześniu. Kalendarz domyślnie renderuje miesiąc „teraz", więc dnia terminu nie było w ogóle w sprawdzanej siatce — test padał, choć kod był poprawny.<br>**To nie był traf losowej strefy — to była kwestia czasu.** Sierpień ma 31 dni, `31 − 21 = 10`: dla „dziś" ≥ 11 sierpnia test padłby w **dowolnej** strefie czasowej, bez żadnego udziwnienia. Złapane 10 sierpnia, dzień przed tym, jak zaczęłoby się psuć samo.<br>Naprawa: `__setView(rok, miesiac)` — nowy test-owy setter `viewYear`/`viewMonth` — i test renderuje teraz dwie osobne kratki, każdą we WŁAŚCIWYM dla niej miesiącu, zamiast zakładać, że „dziś" i termin zawsze mieszczą się w jednym. |
| **B20** | **Dziesięć zdarzeń nie zeszło z pudełka mimo internetu w domu** | Zgłoszone przez Kubę po powrocie z wyjazdu, 2026-08-10. Pudełko (firmware **1.22.0**) złapało WiFi, zameldowało się o 03:01 — i jedyne, co się zmieniło, to napis „10 zdarzeń w kolejce" oraz otwarcia z 7.08 i 9.08 w dzienniku wieczka. Dawki z 6–9.08 nie dojechały; kalendarz pokazuje cztery czerwone dni, choć tabletka była brana codziennie.<br>**Że pudełko miało łączność, wiadomo na pewno:** status idzie **po** próbie opróżnienia kolejki (`flushQueue(); pushStatus();`), więc świeży `lastSeen` z niezerowym `queued` to zapis z chwili, w której sieć, logowanie i reguły działały. To nie jest brak zasięgu. | **Przyczyna: 1.22.0 nie ma ani jednej z dwóch napraw, które dokładnie to opisują.** `flushQueue()` w tej wersji wychodzi na pierwszym wpisie bez HTTP 200 i **nigdy go nie zdejmuje** (**B3**), a `queuePush()` podnosi licznik nawet wtedy, gdy treść do NVS nie weszła — zostaje **wpis‑widmo**: `queuePeek()` nie ma czego odczytać, wysyłka staje w miejscu, licznik nigdy nie schodzi do zera (**B5/D14**). Obie dają dokładnie ten obraz: stała liczba w kolejce, zero błędów, zero pikania.<br>**Czego nie wiem:** która z dwóch. 1.22.0 nie miała czym tego zgłosić — `dropped`, `nvsFail` i log odpowiedzi bazy powstały później. Rozstrzygnie pierwszy log z kabla albo — po wgraniu **1.28.0** — same liczniki w aplikacji.<br>**To samo tłumaczy drugą połowę zgłoszenia:** dziennik wieczka ma tylko 3 otwarcia z pięciu dni, bo `lidLogAdd()` zapisuje przez ten sam nieopakowany `putString()`. |
| **B1** | Otwarte wieczko w porze dawki zapisuje „wzięte" bez udziału człowieka | `runAlarmWindow()` pyta „czy jest otwarte", nie „czy ktoś je otworzył". Przy przesuniętym magnesie pudełko **codziennie** zapisuje dawkę, której nie było, i **nie dzwoni**. W telefonie zielone „wzięte". | Trzy warianty naprawy, każdy z realnym kosztem (niżej). Alarm to najwrażliwsze miejsce w projekcie.<br>**Od D15 decyzja jest osobną funkcją `alarmPotwierdzony()` z testami** — każdy wariant to zmiana jednej linii, nie wejście w alarm po omacku.<br>**2026-08-11, po zdjęciu mechanizmu:** Kuba obejrzał fizycznie zatrzask magnetyczny i ocenił, że łapie zdecydowanie pewnie — mała szansa na przesunięcie. To **obserwacja wizualna, nie pomiar** z dziennika wieczka; nadal nie wiemy, co pokaże tydzień realnego noszenia. Decyzja o wariancie (A/B/C) czeka na te dane, nie na wygląd mechanizmu.<br>**2026-08-12 — Kuba zamyka temat jako nie-problem:** *„z zatrzaskiem ani razu nie było problemu, wszystko śmiga dobrze"*. To decyzja właściciela urządzenia i ryzyka, i tak ją traktujemy: **nie wdrażamy żadnego z wariantów A/B/C**. Nie ruszaj tego z własnej inicjatywy.<br>**Czego to zgłoszenie z natury nie może pokryć — do zapamiętania, nie do podważania:** B1 jest niewidoczny z zewnątrz. Fałszywy zapis daje zielony dzień i **ciszę** zamiast dzwonka, czyli wygląda dokładnie jak dzień, w którym wszystko poszło dobrze. „Nic nie zauważyłem" jest więc słabym dowodem **z konstrukcji błędu**, a nie dlatego, że Kuba obserwuje nieuważnie — jedynym świadkiem byłby dziennik wieczka. Gdyby kiedyś pojawił się zielony dzień, którego Kuba nie pamięta, to jest ten trop i wtedy wraca wariant **C** (próg z danych, nie z sufitu).<br>Rekomendacja techniczna pozostaje bez zmian i leży niżej — gdyby temat wrócił, nie trzeba jej odtwarzać od zera. |
| **B18** | Podejrzenie dryfu zegara — **WYCOFANE, brak dowodu** | Policzyłem „3,2 % = 46 min/dobę" z jednego faktu: alarm zadzwonił o 18:30, a przypomnienie jest o 20:00. Kuba to zakwestionował — słusznie.<br>`MATCH_WINDOW_MIN` = 90 min, więc dla przypomnienia o 20:00 pudełko uznaje „teraz jest pora leku" **od 18:30 do 21:30**. Alarm o 18:30 wypada **dokładnie na krawędzi tego okna** i jest w pełni możliwy przy zegarze co do sekundy poprawnym. Powtórki o 19:30 tłumaczy **B17**, a nie rozjazd. | Zostaje jako podejrzenie **bez potwierdzenia**. Rozstrzygnie je pierwsza synchronizacja po długim offline: `syncTimeNTP()` liczy wtedy `delta = czas_po − czas_przed` i loguje ją przez `przesunZnaczniki()`. **To jest prawdziwy pomiar rozjazdu** — jedna liczba w logu, zamiast wnioskowania z godziny pikania. |

**Warianty naprawy B1** — każdy zamienia jeden błąd na inny:

| | Co robi | Koszt |
|---|---|---|
| **A** | Liczy tylko przejście zamknięte→otwarte | Domyka dziurę całkowicie. Ale gdy naprawdę masz otwarte pudełko o porze dawki i bierzesz lek — dostaniesz „pominięte" |
| **B** | Gdy było otwarte, potwierdzeniem jest **zamknięcie** | Łapie „stoję nad otwartym pudełkiem". Ale ty bywasz tym, który zostawia wieczko otwarte |
| **C** | Nie ufaj kontaktronowi otwartemu od dawna (`rtcOpenSinceTs` już to wie) | Zachowuje obecne zachowanie w normalnych sytuacjach. Pokrycie częściowe, próg wzięty z sufitu |

---

## 3. Indeks decyzji

Od najnowszej. Kolumna „gdzie" mówi, który plik w `decyzje/` trzymać otwarty.

| # | O co chodziło | gdzie |
|---|---|---|
| **D94** | Żadna akcja użytkownika nie może wywalić się bez słowa — przycisk „Drukuj” przy pustej liście zakresu nie robił nic (aplikacja `2026-08-22.68`) | `aplikacja` |
| **D93** | Wpisany wynik INR przesuwa termin przez lokalną kopię, nie przez wyścig z nasłuchem — i dopiero teraz ma to test po stronie aplikacji (aplikacja `2026-08-22.67`) | `aplikacja` |
| **D92** | Powrót do aplikacji był traktowany jak ręczne odświeżenie — handler dostawał obiekt zdarzenia w miejsce flagi `recznie` (aplikacja `2026-08-22.66`) | `aplikacja` |
| **D91** | Godzina przypomnienia musi być godziną — „25:00” robiło z pół doby „porę leku”; przy okazji silnik reguł w testach nauczył się `matches()` (firmware `1.47.2`) | `pudelko` |
| **D90** | Podpowiedź przy każdej kropce wykresu pór mówiła „undefined” — `analyze()` nie zwracało pola, które wykres rysował (aplikacja `2026-08-22.65`) | `aplikacja` |
| **D89** | Kopia zapasowa wymieniała pole, którego nie ma — `inrInterval` zamiast `inrEveryDays`, czyli odstęp pomiarów INR nie był kopiowany (aplikacja `2026-08-22.64`) | `aplikacja` |
| **D88** | Domykanie doby liczone od północy DOBY LEKOWEJ, nie zegara; znaczniki doby w NVS zapisywane sprawdzanym `nvsPutU32()` (firmware `1.47.1`) | `dane` |
| **D87** | Dawka zapisana bez zegara odzyskuje datę, gdy zegar wróci — znacznik względny zamiast twardego zera (firmware `1.47.0`) | `dane` |
| **D86** | Dziennik rozbity na indeks + obszary, duże pliki dostają generowaną `MAPA.md` — wejście w zadanie 5,7 tys. zamiast 60 tys. tokenów | `testy` |
| **D85** | Cicha kopia na Telegram: `disable_notification` PLUS własny czat, który wolno wyciszyć (aplikacja… | `telegram` |
| **D84** | Przypomnienia o INR: cztery razy, i cisza od chwili wpisania wyniku (firmware `1.46.1`, aplikacja… | `telegram` |
| **D83** | Pudełko pisze o kończącym się opakowaniu i o terminie INR (firmware `1.46.0`, aplikacja `2026-08-21.58`) | `telegram` |
| **D82** | Okoliczności przy INR: zamknięta lista tagów na dniu, zestawiana z wynikiem (aplikacja `2026-08-21.57`) | `aplikacja` |
| **D81** | Kopia zapasowa trzema drogami: plik, baza, Telegram — i odtwarzanie, które dokłada zamiast zastępować… | `aplikacja` |
| **D80** | Trzy poprawki znalezione przy przeglądzie: goły `remove()` w usuwaniu INR, brak poprawiania pomiaru,… | `aplikacja` |
| **D79** | Historia rozpisania dawki — zmiana planu obowiązuje OD DNIA zmiany, nie wstecz (aplikacja… | `dane` |
| **D78** | Zakaz dotykania paska nawigacji ZDJĘTY; pasek dostaje rozmyte tło, pigułkę pod aktywną zakładką i… | `aplikacja` |
| **D77** | Analiza przestaje oceniać PORĘ brania — zostaje obraz, znika miara odchylenia (aplikacja… | `aplikacja` |
| **D76** | Wykresy w Analizie: siatka rytmu, pora brania w czasie, emfaza zamiast siedmiu kolorów (aplikacja… | `aplikacja` |
| **D75** | Wyjaśnienia przenoszą się do osobnego ekranu „Instrukcja"; na ekranach zostaje tylko to, czego brak… | `aplikacja` |
| **D74a** | Straty, które nie dotyczą leku, schodzą z Ustawień do Diagnostyki (aplikacja `2026-08-21.50`) | `aplikacja` |
| **D74** | Przebudowa wyglądu aplikacji: system wizualny zamiast zbioru poprawek (aplikacja `2026-08-20.49`) | `aplikacja` |
| **D73** | Przycisk „Wziąłem teraz" na ekranie głównym — jedno tapnięcie zamiast czterech kroków (aplikacja… | `aplikacja` |
| **D72** | Tabletka na ekranie głównym w WEBP — 135 kB zamiast 407 kB, GIF zostaje jako zapas (aplikacja… | `aplikacja` |
| **D71** | Osłona rysowania: jeden wysypany ekran nie zabiera ze sobą reszty ani zapisu (aplikacja `2026-08-20.48`) | `aplikacja` |
| **D70** | Backoff nie działał w życiu, choć miał test — i zjadał baterię: „meldunek bez sieci" co 17 minut przez… | `pudelko` |
| **D69** | „Aktualizacji nie zrobiło" wyskakiwało po UDANEJ aktualizacji — bo wiszące zlecenie stało wyżej niż… | `ota` |
| **D68** | Przytrzymanie przycisku gubiło się w czasie, gdy pudełko gadało z siecią — portal WiFi wstawał „po… | `pudelko` |
| **D67** | Powiadomienia na telefon — bot Telegram wysyłany PRZEZ PUDEŁKO (1.45.0). Hasło padło: *„Dobra czas… | `telegram` |
| **D67c** | Kod parowania bota — bo „Znajdź mnie" brało czat OBCEJ osoby (1.45.1) | `telegram` |
| **D67a** | `strip()` w audycie psuł się na `//` wewnątrz literału — i przez to audyt sprawdzał tekst, którego w… | `testy` |
| **D67b** | Atrapa NVS nie umiała zgubić danych po cichu — czyli nie umiała odtworzyć jedynej awarii, przed którą… | `testy` |
| **D66** | Zestaw testów cichy przy sukcesie, głośny przy błędzie — 9700 → 185 tokenów na przebieg, bez utraty… | `testy` |
| **D65** | Lista sieci WiFi widzianych PRZEZ PUDEŁKO, na żądanie z aplikacji — razem z siłą sygnału (1.44.0) | `ota` |
| **D64** | Dzień lekowy rozstrzyga się z końcem doby, a nie z ostatnim przypomnieniem — „missed" z pudełka nie… | `dane` |
| **D63** | 2026-08-16: aktualizacja z przycisku w aplikacji przeszła całą drogę — pierwszy raz (1.43.1 → 1.43.2).… | `ota` |
| **D62** | WŁAŚCIWA przyczyna: po wzięciu tabletki otwarcie wieczka NIE czytało ustawień, więc pudełko nie miało… | `ota` |
| **D61** | Zlecenie aktualizacji czytamy TUŻ PRZED próbą, a nie na początku wybudzenia (1.43.0) — to była właściwa… | `ota` |
| **D60** | Dlaczego aktualizacja przez WiFi nie przechodziła — i trzy naprawy, z których jedna jest przyczyną, a… | `ota` |
| **D59** | Aktualizacja firmware przez WiFi (OTA), zlecana przyciskiem w aplikacji — zdejmuje D18 (prośba Kuby:… | `ota` |
| **D58** | `kompiluj_firmware.sh` podstawia atrapę `ctags`, gdy nie ma go w systemie | `testy` |
| **D57** | Aplikacja liczy odstęp między dawkami i pokazuje żywy licznik od poprzedniej tabletki, tykający co… | `aplikacja` |
| **D56** | Znacznik „alarm już odzwoniony" jest maską slotów, nie flagą na całą dobę; „missed" leci dopiero po… | `pudelko` |
| **D55** | Analiza w ogóle nie wie o przypomnieniach — żadnej godziny z harmonogramu, ani w liczbach, ani w… | `aplikacja` |
| **D54** | Raport dla lekarza też przestaje mierzyć zgodność z godziną przypomnienia — zwartość liczy się względem… | `aplikacja` |
| **D53** | Kafelek analizy pokazuje średnią godzinę wzięcia leku, a nie odchylenie od 20:00 — i cała statystyka… | `aplikacja` |
| **D36** | Dawka przestaje być jedną liczbą — jest rozpisaniem tygodniowym plus wyjątkami na konkretne dni (prośba… | `dane` |
| **D49** | Dolny pasek nawigacji stracił `backdrop-filter` (rozmycie tła) w całości, tło zrobione nieprzezroczyste… | `aplikacja` |
| **D48** | Dolny pasek nawigacji dostał `backface-visibility:hidden` obok `translateZ(0)` — hipoteza, nie… | `aplikacja` |
| **D47** | Historia nieudanych zapisów NVS trafia teraz do bazy, nie tylko do RAM (prośba Kuby: *„może w bazie… | `pudelko` |
| **D46** | „Pudełko zgłasza utratę danych" po każdym incydencie brzmiało tak samo groźnie — teraz mówi, który… | `pudelko` |
| **D45** | Test na wstrzyknięcie HTML sprawdzał dwa duchy (odpowiedź na pytanie Kuby „czy mamy jakieś niepotrzebne… | `testy` |
| **D44** | Nad `<header>` nie wolno postawić niczego (naprawa dwóch objawów z D43) | `aplikacja` |
| **D43** | Ustawienia to spis treści, a nie ściana formularzy; listy dostają własne ekrany (prośba Kuby) | `aplikacja` |
| **D42** | Sieci da się usunąć i przełączyć z aplikacji (prośba Kuby) | `pudelko` |
| **D41** | Lista sieci nie odcina poświadczeń sterownika; stan „otwarte" mówi, jak jest stary (naprawa regresji z… | `pudelko` |
| **D40** | Ustawienia jako sekcje zwijane, ustawienia INR przeniesione na ekran INR (prośba Kuby) | `aplikacja` |
| **D39** | `nvsPutStr("")` to sukces, nie awaria (naprawa błędu z D36/D38, zgłoszonego przez Kubę) | `pudelko` |
| **D38** | Pudełko pamięta listę sieci, a nową dostaje z aplikacji (prośba Kuby) | `pudelko` |
| **D37** | Pudełko też zna rozpisanie i milczy w dniu bez leku (dokończenie D36) | `pudelko` |
| **D35** | Decyzja „nadpisać czy nie" przeniesiona z lokalnej kopii `doses` do transakcji na żywym stanie serwera… | `dane` |
| **D34** | `boot()` wysyła zaległą kolejkę na starcie, nie tylko przy powrocie łączności (naprawa B28) | `dane` |
| **D33** | Zdarzenie „pominięte" wolno tylko założyć brakujący wpis, nigdy zmienić istniejący (naprawa B27) | `dane` |
| **D32** | `zapiszPewnie()` odróżnia odmowę bazy od braku sieci (przy okazji B27) | `dane` |
| **D31** | Diagnostyka pokazuje to, co się nie zgadza, a nie wszystko po kolei (prośba Kuby) | `aplikacja` |
| **D30** | Atrapa Firebase odwzorowuje przerywanie `forEach`, a kontrola źródła zakazuje formy bez klamer (naprawa… | `testy` |
| **D29** | Nieudany zapis dawki ma własny sygnał dźwiękowy (naprawa B24) | `pudelko` |
| **D28** | Uzupełnianie kalendarza idzie przed rysowaniem, a siatka bezpieczeństwa liczy slot tak samo jak zapis… | `dane` |
| **D27** | Atrapa `Preferences` w testach zachowuje się jak prawdziwe NVS, a audyt zna pojęcie „funkcja… | `testy` |
| **D26** | Kompilacja sprawdza obie ścieżki: `.cpp` i tę, którą naprawdę wgrywa Arduino IDE (naprawa B21) | `testy` |
| **D25** | Aplikacja odróżnia „nie ma zasięgu" od „kolejka stoi mimo połączenia", a pudełko melduje, ile zostało… | `dane` |
| **D24** | Aplikacja mówi wprost, gdy pudełko przestało się odzywać | `aplikacja` |
| **D23** | Odzwoniony alarm zostawia znacznik „ta doba załatwiona" (naprawa B17) | `pudelko` |
| **D22** | Ochrona przed drugą dawką działa bez zegara — na czasie od poprzedniej dawki (naprawa B16) | `pudelko` |
| **D21** | Zakres INR i odstęp pomiarów wybiera się z list, nie wpisuje (prośba Kuby) | `aplikacja` |
| **D20** | Zakres terapeutyczny INR ograniczony do 1–5, z krokiem 0,1 (naprawa B15) | `aplikacja` |
| **D19** | Przegląd na żądanie: dwanaście błędów, w tym jeden groźny (naprawy B9–B14) | `aplikacja` |
| **D18** | OTA odłożone do czasu, aż B1 i B2 będą zrobione zdjęte — D59 (2026-08-12), oba warunki spełnione | `ota` |
| **D17** | Firmware jest teraz kompilowane prawdziwym toolchainem (`tests/kompiluj_firmware.sh`) | `testy` |
| **D16** | Odmowa bazy przesuwa nas do następnego wpisu, zamiast przerywać kolejkę (naprawa B6) | `dane` |
| **D15** | Testy wykonują kod tam, gdzie dotąd czytały jego treść | `testy` |
| **D15b** | Decyzja o potwierdzeniu dawki wydzielona z `runAlarmWindow()` (`alarmPotwierdzony()`) | `pudelko` |
| **D14** | Nieudany zapis do NVS jest liczony i widoczny (naprawa B5) | `pudelko` |
| **D13** | `queueDrop()` — jeden świadomy wyjątek od „nic przed 2xx" (naprawa B3) | `dane` |
| **D12** | Licznik tabletek liczy od bazy, nie różnicowo (naprawa B4) | `dane` |
| **D11** | Diagnostyka na osobnym ekranie, ale ostrzeżenia zostają w Ustawieniach | `aplikacja` |
| **D10** | Odstęp między pomiarami INR podaje użytkownik, nie aplikacja | `aplikacja` |
| **D9** | Aplikacja czyta oba formaty dziennika wieczka | `aplikacja` |
| **D8** | `CHARGE_AWAKE_MAX_S` = 4 h | `pudelko` |
| **D7** | `config.h` jest w repo, z placeholderem hasła | `pudelko` |
| **D6** | Znaczniki „to ja" w `users/<uid>/lidMarks`, nie przy zdarzeniu | `dane` |
| **D4** | Dziennik wieczka ma własny bufor, nie `queuePush()` | `pudelko` |
| **D3** | Dziennik wysyłany pod osobnymi kluczami paczek | `pudelko` |
| **D2** | Kasujemy z pamięci pudełka dopiero po potwierdzonym 2xx | `dane` |
| **D1** | Zapis „to ja" idzie przez `zapiszPewnie()` | `dane` |
| **N2** | `rtcStuckOpen` usunięty | `pudelko` |
| **N1** | `cstdint` w shimie + ścieżki po usunięciu `app/` | `testy` |
