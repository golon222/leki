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
