# Dziennik decyzji

**Po co ten plik.** Kod mówi *co* robi. Nie mówi, *dlaczego akurat tak* — a to
jest właśnie ta wiedza, która ginie pierwsza. Bez niej kolejna sesja „upraszcza"
rozwiązanie, które wyglądało na dziwaczne, bo nie wiedziała, jaki problem ono
rozwiązywało. W tym projekcie już się to zdarzyło.

**Zasada: nie cofaj niczego z tej listy, nie przeczytawszy powodu.**

Trzy inne miejsca, żeby nie szukać po omacku:
- `PROJEKT-PillBox-kontekst.md` sekcja 3 — dziewięć błędów, które kosztowały
  najwięcej, wraz z prawdziwymi przyczynami. Tam są rzeczy sprzed tego dziennika.
- `PROJEKT-PillBox-kontekst.md` sekcja 8 — sprawy otwarte, świadomie odłożone.
- `CLAUDE.md` — twarde ograniczenia w formie skróconej.

---

## Jak dopisywać

Nowe wpisy na górę odpowiedniej sekcji. Każdy ma odpowiadać na cztery pytania:

1. **Co** zostało zrobione
2. **Jaki problem** to rozwiązywało — konkretnie, z objawem
3. **Status** — obowiązuje / tymczasowe / cofnięte
4. **Kiedy to przestanie być potrzebne** — dla rzeczy tymczasowych obowiązkowo

Jeśli decyzja zostaje cofnięta, **nie kasuj wpisu** — dopisz przy nim, że jest
cofnięty i dlaczego. Cofnięta decyzja niesie tyle samo wiedzy co obowiązująca:
mówi, czego nie próbować drugi raz.

---

# 1. Rzeczy TYMCZASOWE — do usunięcia

Ta sekcja ma być pusta. Jeśli coś w niej stoi za długo, to znaczy, że stało się
stałe przez zaniedbanie, a nie przez decyzję.

### T1 · Dziennik wieczka (`lidLogAdd`/`Count`/`Json`/`Clear`, `LIDLOG_SLOTS`)
*Dodane 2026-08-05, firmware 1.22.0 · aplikacja: ekran „Dziennik wieczka"*

**Po co.** Nie wiemy, czy pudełko melduje otwarcia, których nikt nie zrobił —
np. gdy magnes przesunie się w plecaku. To nie jest ciekawostka: jeśli
kontaktron pokazuje „otwarte" w porze dawki, alarm uznaje dawkę za wziętą
i **zapisuje ją bez udziału człowieka** (patrz D0 poniżej). Zanim to naprawimy,
musimy wiedzieć, czy problem w ogóle istnieje i jak często.

**Kiedy usunąć.** Gdy Kuba wróci z tygodniowym pomiarem i odpowiemy na pytanie:
czy zdarzają się otwarcia bez jego znacznika „to ja". Po podjęciu decyzji
o punkcie D0 — cały dziennik, ekran w aplikacji i gałąź `lidlog` w bazie idą do
kosza. Znaczniki z `users/<uid>/lidMarks` też.

**Czego NIE ruszać, dopóki tu stoi.** Osobnego bufora — patrz D4. Kasowania
dopiero po potwierdzeniu 2xx — patrz D3.

---

# 2. Sprawy otwarte, które czekają na pomiar

### D0 · Otwarte wieczko w porze dawki zapisuje „wzięte" bez udziału człowieka
*Znalezione 2026-08-05, NIENAPRAWIONE — świadomie*

**Objaw.** `runAlarmWindow()` (`PillBox.ino`) sprawdza tylko `boxIsOpen()`, czyli
czy wieczko jest otwarte **teraz** — nie czy ktoś je właśnie otworzył. Jeśli
w chwili alarmu wieczko już stoi otwarte, funkcja wraca `true` po pierwszym
piknięciu i dawka leci do bazy jako wzięta. Alarm nie dzwoni.

**Dlaczego to groźne.** Przy przesuniętym magnesie pudełko codziennie o porze
dawki zapisuje ją jako wziętą, a w telefonie widać zielone „wzięte" za dzień,
w którym nic nie zostało wzięte. Przy leku przeciwzakrzepowym to najgorszy
kierunek błędu: system **potwierdza** dawkę, której nie było.

**Dlaczego nienaprawione.** Trzy warianty naprawy, każdy z realnym kosztem:
- **A — liczy tylko przejście zamknięte→otwarte.** Domyka dziurę całkowicie,
  ale gdy Kuba naprawdę ma otwarte pudełko o porze dawki i bierze lek, dostanie
  „pominięte". Błąd odwrotny, łagodniejszy, ale realny.
- **B — jeśli było otwarte, potwierdzeniem jest ZAMKNIĘCIE.** Łapie przypadek
  „stoję nad otwartym pudełkiem", ale Kuba bywa tym, który zostawia wieczko
  otwarte — wtedy znowu fałszywe „pominięte".
- **C — nie ufaj kontaktronowi, gdy jest otwarty od dawna.** Dane są za darmo
  (`rtcOpenSinceTs`), zachowuje obecne zachowanie w normalnych sytuacjach,
  ale pokrycie jest częściowe i wymaga progu wziętego z sufitu.

**Czego brakuje do decyzji.** Pomiaru z T1. Jeśli okaże się, że fałszywych
otwarć nie ma, nie ruszamy alarmu wcale — to najwrażliwsze miejsce w projekcie.

---

# 3. Decyzje obowiązujące

### D7 · `config.h` wraca do repo — COFNIĘCIE D5
*2026-08-05*

**Co.** `firmware/PillBox/config.h` jest śledzony przez gita, z placeholderem
`TUTAJ_WPISZ_HASLO`. `config.example.h` usunięty.

**Dlaczego.** D5 (poniżej) było zgodne z zapisaną zasadą i **złamało sposób,
w jaki Kuba faktycznie pracuje**. On nie klonuje repo — pobiera folder
`firmware/` i otwiera go wprost w Arduino IDE. Bez `config.h` w komplecie szkic
się nie otwiera, bo Arduino IDE pokazuje pliki `.h` jako zakładki tylko wtedy,
gdy leżą obok `.ino` o nazwie zgodnej z folderem. Efekt: przed każdym wgraniem
ręczna zmiana nazwy pliku, na każdym komputerze osobno. Potknął się o to na
MacBooku w przeddzień wyjazdu.

Zabezpieczenie chroniło przed ryzykiem, które **nigdy się nie zmaterializowało**
— w całej historii repo `DEVICE_PASSWORD` było placeholderem. Koszt był realny
i powtarzał się przy każdym pobraniu.

**Morał szerszy niż ten plik.** Zasada zapisana w dokumencie nie bije sposobu
pracy człowieka, który tego używa. Jeśli poprawka „zgodna z zasadą" dokłada mu
kroku przy każdym użyciu — to zasada jest do przedyskutowania, nie on.

**Jedyne, co zostaje do pilnowania:** nie wrzucać `config.h` z wpisanym hasłem.
`.gitignore` przed tym nie obroni przy wrzucaniu przez stronę GitHuba.

---

### D6 · Znaczniki „to ja" leżą w `users/<uid>/lidMarks`, nie przy zdarzeniu
*2026-08-05*

**Dlaczego nie przy zdarzeniu.** `database.rules.json` ma na `events/$eventId`
regułę `"$other": { ".validate": false }` — czyli baza **odrzuca cały wpis**,
jeśli trafi w nim choć jedno nieznane pole. Dopisanie tam własnego znacznika
skończyłoby się odrzuceniem zdarzenia z pudełka.

To nie jest teoria: dokładnie ten mechanizm ugryzł już ten projekt przy polu
`openTs` i dlatego w `index.html` istnieje zapasowa ścieżka ponawiająca zapis
bez pól dodanych później.

**Skutek uboczny, korzystny:** gałąź `users/<uid>` nie ma `$other: false`, więc
panel działa **bez publikowania nowych reguł Firebase** — a Kuba ich nie
opublikował.

---

### D5 · `config.h` wyjęty z repo na rzecz `config.example.h` — **COFNIĘTE przez D7**
*2026-08-05, cofnięte tego samego dnia*

Zostawione w dzienniku celowo. **Nie próbować tego drugi raz** bez przeczytania
D7 — powód cofnięcia nie jest widoczny w kodzie ani w historii gita.

---

### D4 · Dziennik wieczka ma OSOBNY bufor, nie korzysta z `queuePush()`
*2026-08-05*

**Dlaczego.** `queuePush()` przy zapełnieniu **nadpisuje najstarszy wpis**
(gałąź „pelno -> nadpisz najstarszy"). Wieczko trzęsące się w plecaku
wygenerowałoby setki zdarzeń i po cichu wyrzuciło z kolejki **zapisy dawek**.

Narzędzie diagnostyczne nie może niszczyć danych o leku. Stąd własne miejsce
i własny limit. Osobny test tego pilnuje — sprawdza, że 40 ruchów wieczkiem nie
zmienia `queueCount()`.

**Dodatkowo:** po zapełnieniu dziennik **nie nadpisuje**, tylko liczy zgubione.
Do diagnozy ważniejszy jest początek zjawiska („od której godziny zaczęło
świrować") niż koniec, a licznik strat mówi o skali: „64 zapisane + 900
zgubionych" to zupełnie inna historia niż „6 zapisanych".

---

### D3 · Dziennik wieczka wysyłany pod OSOBNYMI kluczami paczek
*2026-08-05 — poprawka błędu popełnionego tego samego dnia*

**Błąd.** Pierwsza wersja robiła `PUT` na wspólny węzeł `/lidlog`. Pudełko
kasuje dziennik po wysłaniu, więc **druga synchronizacja nadpisałaby pierwszą**
— po kilku dniach wyjazdu zostałby tylko ostatni dzień.

**Naprawa.** Każda paczka dostaje własny klucz (znacznik czasu albo `b<numer
bootu>`, gdy zegar nieznany). Paczki się sumują.

**Reguła ogólna, która się z tego bierze:** jeśli nadawca kasuje u siebie po
wysłaniu, odbiorca **musi** dostawać dopisanie, nie podmianę.

---

### D2 · Kasowanie dziennika DOPIERO po potwierdzonym 2xx
*2026-08-05*

Ta sama zasada, co w `pushStatus()` — i z tego samego powodu, dla którego
powstała tam (sekcja 3.5 dokumentu kontekstu). Wyczyszczenie na wiarę oznacza,
że jedna nieudana wysyłka po cichu niszczy **jedyną kopię** pomiaru.

---

### D1 · Zapis znacznika „to ja" idzie przez `zapiszPewnie()`, nie przez `set()`
*2026-08-05*

**Kuszące było pójść na skróty** — to przecież jedno pole diagnostyczne. Test
`test_app.mjs` to wychwycił i słusznie.

**Dlaczego to nie jest formalność.** Firebase offline **nie odrzuca obietnicy**,
tylko trzyma ją w nieskończoność. Zaznaczenie „to ja" na plaży bez zasięgu
pokazałoby ptaszka na ekranie i nigdy nie doszło — a wtedy cały tydzień pomiaru
jest do wyrzucenia, bo nie wiadomo, które otwarcia były Kuby.

Reguła obowiązuje **każdy** zapis użytkownika w aplikacji, bez wyjątków. Pilnuje
jej test „żaden zapis użytkownika nie omija kolejki".

---

# 4. Decyzje narzędziowe (testy, repo)

### N2 · `rtcStuckOpen` usunięty
*2026-08-05*

Licznik inkrementowany w dwóch miejscach i **nigdy przez nic nieczytany**.
Zero wpływu na działanie. Skan całego firmware potwierdził, że był jedynym
takim przypadkiem — pięć innych podejrzanych okazało się fałszywymi alarmami.

Gdyby wrócił jako potrzebny (jest gotowym sygnałem dla wariantu C w D0), to
świadomie i z użyciem, nie „na wszelki wypadek".

---

### N1 · `#include <cstdint>` w `tests/arduino_shim.h` + ścieżki po usunięciu `app/`
*2026-08-05*

Cały zestaw testów nie startował. Szczegóły w `PROJEKT-PillBox-kontekst.md`
sekcja 3.10, razem z pułapką, która przy tym wyszła: sprawdzanie poprawki na
kopii repo z podstawionym katalogiem `app/` zamaskowało piątą zepsutą ścieżkę.

**Morał:** poprawki ścieżek sprawdzaj na `git checkout-index`, nie na
spreparowanym katalogu.
