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
| **B1** | Otwarte wieczko w porze dawki zapisuje „wzięte" bez udziału człowieka | `runAlarmWindow()` pyta „czy jest otwarte", nie „czy ktoś je otworzył". Przy przesuniętym magnesie pudełko **codziennie** zapisuje dawkę, której nie było, i **nie dzwoni**. W telefonie zielone „wzięte". | Trzy warianty naprawy, każdy z realnym kosztem (niżej). Czekamy na pomiar z sekcji 1. Alarm to najwrażliwsze miejsce w projekcie. |
| **B2** | Autotest (3 kliknięcia) nie dochodzi do końca | Kuba: *„czekało na synchronizację, a jak już była, to nic się nie pojawiało"*. Nie wiadomo, czy winne zasypianie, czy co innego. | Niezbadane. |
| **B5** | Wynik zapisu do pamięci trwałej (NVS) nigdy nie sprawdzany | `prefs.putString()` zwraca liczbę zapisanych bajtów — `0` gdy się nie udało (pamięć pełna, uszkodzony wpis). W siedmiu miejscach firmware wyrzucamy tę wartość do kosza: harmonogram, kolejka zdarzeń, token, dziennik wieczka. Nieudany zapis wygląda dokładnie jak udany, a przy kolejce oznacza **cichą utratę dawki** — rodzina błędu 3.5. | Nie widziano tego objawu ani razu; NVS na ESP32 zawodzi rzadko. Ale to jedyne miejsce, gdzie utrata danych jest z założenia niewykrywalna. Naprawa: sprawdzać wynik i przy `0` zapalić flagę widoczną w `status`, żeby aplikacja mogła krzyknąć. |
| **B4** | ~~Dzień dosłany wstecz nie odejmował tabletki~~ **naprawione — D12** | Licznik liczył różnicowo i pomijał dni starsze od znacznika `pillsCountedUntil`. Pudełko offline od wtorku, w czwartek dopisujesz ręcznie środę → znacznik skacze na środę → wtorek dosłany w piątek nie odejmie się **nigdy**. Licznik rósł ponad stan faktyczny. | — |
| **B3** | Odrzucony wpis zatyka kolejkę dawek na zawsze | `flushQueue()` przy niepowodzeniu wychodzi **nie zdejmując wpisu z kolejki**. Wpis odrzucony przez reguły (HTTP 400) nigdy nie zostanie przyjęty, więc blokuje wszystko za sobą — dawki piętrzą się, aż pierścień 120 wpisów zacznie nadpisywać najstarsze. | Dziś nie boli: firmware wysyła dokładnie te 6 pól, które reguły znają. Mina uzbraja się przy pierwszym dodanym polu. Naprawa: odróżnić 400 (nigdy nie przejdzie → odłóż na bok) od 401/403 (token → ponawiaj). |

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
