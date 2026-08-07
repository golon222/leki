# Dziennik decyzji

Kod mówi *co* robi. Ten plik mówi **dlaczego akurat tak** — i czego nie cofać.

Szczegóły starszych błędów: `PROJEKT-PillBox-kontekst.md` sekcja 3.
Sprawy świadomie odłożone: tamże sekcja 8.

**Dopisując:** nowy wiersz na górę tabeli, krótko. Cofniętej decyzji **nie kasuj**
— przenieś do sekcji 4. Ona niesie tyle samo wiedzy co obowiązująca: mówi,
czego nie próbować drugi raz.

---

## 1. Tymczasowe — do usunięcia

> Ta sekcja ma być pusta. Jeśli coś stoi tu długo, stało się stałe przez
> zaniedbanie, a nie przez decyzję.

| Co | Po co powstało | Kiedy usunąć |
|---|---|---|
| **Dziennik wieczka**<br>`lidLog*`, `LIDLOG_SLOTS`, ekran „Dziennik wieczka", gałąź `lidlog`, `users/<uid>/lidMarks` | Zmierzyć, czy pudełko melduje otwarcia, których nikt nie zrobił — np. gdy magnes przesunie się w plecaku. Bez tego **B1** to zgadywanie. | Gdy wróci tygodniowy pomiar i zapadnie decyzja w sprawie **B1**. Wtedy leci wszystko: firmware, ekran w aplikacji, dane w bazie. |

---

## 2. Bugi — do poprawy lub zastanowienia

| # | Bug | Objaw | Czemu jeszcze stoi |
|---|---|---|---|
| **B1** | Otwarte wieczko w porze dawki zapisuje „wzięte" bez udziału człowieka | `runAlarmWindow()` pyta „czy jest otwarte", nie „czy ktoś je otworzył". Przy przesuniętym magnesie pudełko **codziennie** zapisuje dawkę, której nie było, i **nie dzwoni**. W telefonie zielone „wzięte". | Trzy warianty naprawy, każdy z realnym kosztem (niżej). Czekamy na pomiar z sekcji 1. Alarm to najwrażliwsze miejsce w projekcie.<br>**Od D15 decyzja jest osobną funkcją `alarmPotwierdzony()` z testami** — każdy wariant to zmiana jednej linii, nie wejście w alarm po omacku. |
| **B8** | Wygasły token zatyka kolejkę **bez śladu w licznikach** | `firebaseSignIn()` przy `!rtcTimeValid` zwraca `true` na tokenie z RAM, nie sprawdzając ważności. Martwy token → każdy push wraca **401** → `flushQueue()` zwraca `false` przy każdym wybudzeniu. Kolejka stoi, `dropped` się nie rusza, `nvsFail` się nie rusza. Aplikacja **nie ma o czym krzyknąć**. | Trzeci sposób na zatkanie kolejki, obok B3 i tego z D14 — i jedyny, który nie zostawia śladu. Naprawa to prawdopodobnie licznik „N nieudanych wysyłek z rzędu" w statusie. Zachowanie **przypięte testem**, żeby zmiana była świadoma. |
| **B7** | Próg „mało tabletek" znaczy co innego w pudełku i w aplikacji | Pudełko piszczy przy `pillsLeft < 7` — czyli **7 tabletek**. Aplikacja pisze „czas po receptę" przy `floor(left/perDay) < 7` — czyli **7 dni zapasu**. Przy jednej dawce dziennie to ta sama liczba (i tak działa pudełko Kuby). Przy dwóch dawkach aplikacja ostrzega przy 13 tabletkach, a pudełko milczy aż do szóstej. | Kuba bierze jedną dawkę dziennie, więc dziś to nie boli. Rozjazd **przypięty testem zgodności**, żeby zmiana progu po którejkolwiek stronie była widoczna. |
| **B6** | Trwale odrzucony zapis blokuje kolejkę w telefonie | `oczekWyslij()` przerywa na pierwszym niepowodzeniu. Wpis, którego reguły nie przyjmą **nigdy** (nieznane pole), stoi na czele i blokuje wszystko za sobą — dokładnie jak **B3** po stronie pudełka, tylko że tu nie ma odpowiednika `queueDrop()`. | Ryzyko jest dziś małe, bo test reguł (D15) pilnuje, żeby aplikacja nie wysyłała nieznanych pól. Zachowanie **przypięte testem**. |
| **B2** | Autotest (3 kliknięcia) nie dochodzi do końca | Kuba: *„czekało na synchronizację, a jak już była, to nic się nie pojawiało"*. Nie wiadomo, czy winne zasypianie, czy co innego. | Niezbadane. |
| **B5** | ~~Wynik zapisu do pamięci trwałej (NVS) nigdy nie sprawdzany~~ **naprawione — D14** | `prefs.putString()` zwraca liczbę zapisanych bajtów — `0` gdy się nie udało. W siedmiu miejscach wyrzucaliśmy tę wartość do kosza, więc nieudany zapis wyglądał dokładnie jak udany. W kolejce oznaczało to **cichą utratę dawki**. | — |
| **B4** | ~~Dzień dosłany wstecz nie odejmował tabletki~~ **naprawione — D12** | Licznik liczył różnicowo i pomijał dni starsze od znacznika `pillsCountedUntil`. Pudełko offline od wtorku, w czwartek dopisujesz ręcznie środę → znacznik skacze na środę → wtorek dosłany w piątek nie odejmie się **nigdy**. Licznik rósł ponad stan faktyczny. | — |
| **B3** | ~~Odrzucony wpis zatyka kolejkę dawek na zawsze~~ **naprawione — D13** | `flushQueue()` przy niepowodzeniu wychodził **nie zdejmując wpisu z kolejki**. Wpis odrzucony przez reguły (HTTP 400) nigdy nie zostanie przyjęty, więc blokował wszystko za sobą — dawki piętrzyły się, aż pierścień 120 wpisów zaczynał nadpisywać najstarsze. | — |

**Warianty naprawy B1** — każdy zamienia jeden błąd na inny:

| | Co robi | Koszt |
|---|---|---|
| **A** | Liczy tylko przejście zamknięte→otwarte | Domyka dziurę całkowicie. Ale gdy naprawdę masz otwarte pudełko o porze dawki i bierzesz lek — dostaniesz „pominięte" |
| **B** | Gdy było otwarte, potwierdzeniem jest **zamknięcie** | Łapie „stoję nad otwartym pudełkiem". Ale ty bywasz tym, który zostawia wieczko otwarte |
| **C** | Nie ufaj kontaktronowi otwartemu od dawna (`rtcOpenSinceTs` już to wie) | Zachowuje obecne zachowanie w normalnych sytuacjach. Pokrycie częściowe, próg wzięty z sufitu |

---

## 3. Decyzje — dlaczego tak

| # | Decyzja | Co by było bez tego |
|---|---|---|
| **D15** | Testy **wykonują** kod tam, gdzie dotąd czytały jego treść | Cztery obszary miały pokrycie tylko z nazwy:<br>**1. Reguły bazy** — nic nie sprawdzało, czy zapis przejdzie przez `database.rules.json`. Jedyną obroną był skan, czy pięć nazw pól występuje gdziekolwiek w pliku — przechodził nawet wtedy, gdy pole siedziało pod złym węzłem. Teraz `tests/rules_engine.mjs` czyta prawdziwy plik reguł i jest wpięty w atrapę Firebase, więc **wszystkie testy aplikacji sprawdzają przy okazji zgodność z regułami**. Osobny `test_rules.mjs` pilnuje payloadów firmware — lista pól jest wyciągana ze źródła, więc dołożenie pola do `pushEventRecord()` wywala test. Bez tego powtórka **D6** kończy się kodem 400, a po **B3/D13** dawka jest dodatkowo **kasowana** z kolejki.<br>**2. Kolejka zapisów w telefonie** — 13 wyrażeń regularnych po treści `index.html`, pierwsze sprawdzało dosłownie tyle, że `zapiszPewnie()` istnieje. Żadne jej nie uruchomiło, bo atrapa bazy **nie umiała zawieść**. Teraz umie (`__db.tryb = "blad" \| "wisi"`), więc da się odtworzyć Firebase offline z **D1**: obietnica, która nie jest odrzucana, tylko wisi.<br>**3. `planNextSleep()`** — jedyna funkcja decydująca, czy pudełko obudzi się na 20:00, miała wyłącznie audyt tekstowy. Doszła własność zbiorcza po 3456 kombinacjach flag: sen nigdy nie wychodzi zerowy ani dłuższy niż czas do najbliższej dawki.<br>**4. Zgodność pudełko–aplikacja** poza numerem doby: dopasowanie otwarcia do dawki (`matchSlot()` kontra skan w `doReconcile()`) i kontrakt licznika tabletek.<br>Każdy obszar sprawdzony **mutacjami źródła** — test, który niczego nie wykrywa, jest gorszy niż jego brak, bo daje fałszywe poczucie pokrycia. |
| **D15b** | Decyzja o potwierdzeniu dawki wydzielona z `runAlarmWindow()` (`alarmPotwierdzony()`) | **Zachowanie niezmienione** — funkcja zwraca dziś dokładnie to samo co wcześniej, razem z błędem **B1**. Chodzi wyłącznie o możliwość testowania: pętla alarmu to `millis()`, `delay()` i odczyt pinu, więc nie da się jej uruchomić w teście; decyzja da się. Bez tego którykolwiek z trzech wariantów naprawy **B1** wszedłby do najwrażliwszego miejsca w projekcie **bez ani jednego testu**. Teraz każdy wariant to zmiana jednej linii z gotowym zestawem testów obok, a audyt pilnuje, żeby pętla nie omijała funkcji skrótem do pinu — inaczej naprawa weszłaby w jedno miejsce, a działałoby drugie. |
| **D14** | Nieudany zapis do NVS jest **liczony i widoczny** (naprawa **B5**) | `nvsPutStr()` / `nvsPutU16()` sprawdzają wynik zapisu i podbijają `rtcNvsFail`. Licznik jedzie w statusie jako `nvsFail`, aplikacja krzyczy przy wartości > 0.<br>Osobno: `queuePush()` podnosi licznik kolejki **tylko wtedy, gdy treść naprawdę weszła**. Wcześniej nieudany zapis zostawiał wpis-widmo — `queuePeek()` nie miał czego odczytać, wysyłka stawała w miejscu, a licznik nigdy nie schodził do zera. To był drugi sposób na zatkanie kolejki, obok B3.<br>Liczniki żyją w pamięci RTC, więc zerują się po odłączeniu zasilania — status idzie w każdym wybudzeniu, więc okno na zauważenie jest wystarczające, a NVS to akurat ta pamięć, której w tym scenariuszu nie ufamy. |
| **D13** | `queueDrop()` — jeden świadomy wyjątek od „nic przed 2xx" (naprawa **B3**) | Zasada chroni przed utratą danych przy **chwilowej** awarii. Wpis odrzucony regułami (HTTP 400) albo z uszkodzoną treścią nie zostanie przyjęty ani teraz, ani za tydzień — a zostawiony na czele blokuje **wszystkie dawki za sobą**. Tracimy jeden wpis zamiast całej kolejki.<br>Które kody są „na stałe": **400** i **413**. Świadomie **nie ma** tam 401/403 (wygasły token — po odświeżeniu przejdzie) ani **404** (zły adres bazy skasowałby całą kolejkę). Test pilnuje każdego z tych przypadków osobno.<br>Strata idzie na licznik `dropped` w statusie — kasujemy głośno, nie po cichu. |
| **D12** | Licznik tabletek liczy **od bazy**, nie różnicowo (naprawa **B4**) | Nowe pola w `config`: `pillsBase` (stan na początek dnia `pillsBaseFrom`) i `pillsBaseFrom`. `pillsLeft` zostaje, ale jest już **wynikiem**, nie licznikiem: baza minus wszystko wzięte od tej daty, przeliczane od zera przy każdej zmianie. Dzięki temu kolejność przestaje mieć znaczenie — dzień dosłany wstecz też wchodzi do sumy.<br>`pillsCountedUntil` **zostaje w bazie i dalej idzie do przodu**, choć nowy kod go nie używa: telefon z zakeszowaną starą wersją liczy po staremu i bez tego odjąłby wszystko drugi raz.<br>Ręczny wpis („ile tabletek jest w opakowaniu") ustawia bazę **na dziś** — to policzone tabletki leżące teraz, więc dni zamknięte wcześniej są już w tej liczbie i pudełko dosyłając je później nie może odjąć drugi raz.<br>Migracja jednorazowa i automatyczna: `pillsBase = pillsLeft`, `pillsBaseFrom` = dzień po znaczniku. Stan licznika się nie zmienia. Reguły: `config` ma `$other: true`, więc **nie trzeba publikować nowych** (walidacje dopisane na przyszłość). Firmware bez zmian — czyta z `config` tylko `pillsLeft`, żeby piknąć „mało tabletek". |
| **D11** | Diagnostyka na osobnym ekranie, ale **ostrzeżenia zostają w Ustawieniach** | Cztery karty diagnostyczne otwiera się, gdy coś nie gra — nie po to, żeby zmienić godzinę leku; trzymane w Ustawieniach zmuszały do przewijania czarnej skrzynki przy każdej zmianie dawki. Ale dwa komunikaty z nich (kolejka zapisów, przestarzałe reguły) oznaczają **utratę danych** — schowane głęboko mogłyby wisieć miesiącami niezauważone, więc zostają na wierzchu i pokazują się tylko wtedy, gdy jest o czym mówić.<br>Nazwa „Diagnostyka”, nie „Deweloperskie”: to stan urządzenia, nie kod do debugowania. |
| **D10** | Odstęp między pomiarami INR podaje **użytkownik**, nie aplikacja | Aplikacja liczy dni i zaznacza termin w kalendarzu — nie sugeruje, jak często mierzyć. To ustala lekarz. `inrEveryDays` leży w `devices/<id>/config`, gdzie reguły mają `$other: true`, więc **nie wymaga publikowania nowych reguł**. `0` = przypominanie wyłączone, brak wartości = 21 dni. Termin liczy się od **ostatniego** pomiaru, więc nowy wynik przesuwa go sam. |
| **D9** | Aplikacja czyta **oba** formaty dziennika wieczka | Zmienilem sciezke `lidlog` (wspolny wezel -> osobne paczki) i **nie podbilem `FW_VERSION`** — obie wersje noszą 1.22.0. Z numeru nie da sie poznac, co siedzi w pudelku. Aplikacja przyjmuje jedno i drugie, wiec nikt nie musi wgrywac firmware pod presja.<br>**Morał: zmiana zachowania firmware bez podbicia wersji zabiera jedyny sposob na sprawdzenie, co jest wgrane.** |
| **D8** | `CHARGE_AWAKE_MAX_S` = 4 h | Pudełko na kablu czuwa, żeby dało się w tym czasie coś dograć. 4 h to bezpiecznik na wypadek, gdyby nie zauważyło odłączenia — z włączonym radiem zjadłoby ogniwo |
| **D7** | `config.h` **jest** w repo, z placeholderem hasła | Kuba pobiera folder i otwiera wprost w Arduino IDE. Bez `config.h` szkic się nie otwiera — ręczna zmiana nazwy przed każdym wgraniem, na każdym komputerze osobno |
| **D6** | Znaczniki „to ja" w `users/<uid>/lidMarks`, nie przy zdarzeniu | `events` ma w regułach `$other: false` — nieznane pole = odrzucenie **całego** zdarzenia. Ugryzło już raz, przy `openTs` |
| **D4** | Dziennik wieczka ma **własny** bufor, nie `queuePush()` | Kolejka przy zapełnieniu nadpisuje najstarszy wpis. Wieczko trzęsące się w plecaku wyrzuciłoby z niej **zapisy dawek** |
| **D3** | Dziennik wysyłany pod osobnymi kluczami paczek | Pudełko kasuje po wysłaniu, więc wspólny węzeł = druga synchronizacja nadpisuje pierwszą. Po kilku dniach wyjazdu zostałby ostatni dzień |
| **D2** | Kasujemy z pamięci pudełka dopiero po potwierdzonym 2xx | Jedna nieudana wysyłka niszczy **jedyną** kopię danych. To rodzina błędu 3.5 |
| **D1** | Zapis „to ja" idzie przez `zapiszPewnie()` | Firebase offline nie odrzuca obietnicy, tylko wisi w nieskończoność. Ekran pokazuje ptaszka, dane nie docierają |
| **N2** | `rtcStuckOpen` usunięty | Licznik zwiększany w dwóch miejscach i **nigdy przez nic nieczytany**. Skan potwierdził, że był jedyny taki w firmware |
| **N1** | `cstdint` w shimie + ścieżki po usunięciu `app/` | Cały zestaw testów nie startował. Szczegóły: kontekst 3.10 |

---

## 4. Cofnięte — nie próbować drugi raz

| Co | Dlaczego cofnięte |
|---|---|
| **D5** — wyjęcie `config.h` z repo na rzecz `config.example.h` | Zgodne z zapisaną zasadą, ale złamało sposób pracy Kuby (patrz **D7**). Chroniło przed ryzykiem, które **nigdy się nie zmaterializowało** — hasło w historii repo zawsze było placeholderem.<br>**Morał: zasada z dokumentu nie bije sposobu pracy człowieka, który z tego korzysta.** |
| **Zakaz ruszania bloku pomiaru napięcia** | Zniesiony przez Kubę 2026-08-05. Blok wolno modyfikować. Audyt **nie blokuje** — zgłasza uwagę i przechodzi dalej (`0 bledow, 1 uwag`). Te liczby były kalibrowane na sprzęcie, a w diffie zmiana przypadkowa wygląda identycznie jak świadoma; uwaga kosztuje sekundę, a odróżnia jedno od drugiego. |
