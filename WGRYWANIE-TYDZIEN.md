# Pudełko tygodniowe — od zera do działania

Osobne urządzenie, osobny program, osobne konta w bazie. Pudełka dziennego
to nie dotyka w żadnym miejscu — ani jednego pliku, ani jednego wpisu.

> **Skasowała Ci się gałąź `devices` w bazie?** Idź od razu do sekcji
> **7. RATUNEK** na końcu tego pliku. Krótka wersja: historia leków jest
> cała, pudełko dalej dzwoni, żadna dawka nie przepadła.

---

## ŚCIĄGAWKA — to, czego szukasz najczęściej

| co | wartość |
|---|---|
| **sieć pudełka przy zmianie WiFi** | **`Pudelko-na-leki`** |
| **hasło do niej** | **`pudelko123`** |
| strona portalu, gdyby nie otworzyła się sama | `http://192.168.4.1` |
| jak wejść w portal | **ukryty przycisk** (pudełko śpi) albo trzymaj go **3 s** przy starcie |
| jak wyjść | drugie naciśnięcie przycisku — dwa opadające tony |
| konto pudełka w bazie | `pillbox02@device.local` |
| identyfikator w bazie | `pillbox02` |
| sieć i hasło portalu w kodzie | `firmware/PillBoxWeek/config.h`, `AP_SSID` / `AP_PASS` |

**To samo jest w aplikacji**, na telefonie, zawsze pod ręką:
**Ustawienia → Instrukcja → „Zmiana WiFi w pudełku"** (na koncie
tygodniowym). Kontrola statyczna porównuje oba napisy z `config.h`, więc
instrukcja nie ma jak zacząć kłamać.

Całość to jakieś 25 minut:

0. **opublikuj reguły bazy** — bez tego reszta nie zadziała (2 min)
1. konto **pudełka** i konto **dziewczyny** — to dwie różne rzeczy (10 min)
2. gałąź `pillbox02` w bazie (5 min)
3. `config.h` — trzy linie (2 min)
4. wgranie kablem z Arduino IDE (5 min)
5. sprawdzenie, czy się zameldowało

---

# 0. NAJPIERW: opublikuj reguły bazy

**Bez tego kroku nic dalej nie zadziała i wygląda to na zepsutą aplikację.**

`database.rules.json` z repozytorium to **tylko plik**. Firebase go nie
czyta — reguły, które naprawdę działają, to te wklejone w konsoli. Dopóki
ich nie opublikujesz, baza nie wie, co znaczy `owners`, więc konto
dziewczyny dostanie odmowę mimo poprawnego wpisu.

Kuba trafił dokładnie na to: UID w `owners` był właściwy, a aplikacja
i tak pisała *„to konto nie ma dostępu do żadnego pudełka"*.

**Jak to zrobić:**

1. https://console.firebase.google.com/project/pudelko-na-leki/database/pudelko-na-leki-default-rtdb/rules
2. Otwórz `database.rules.json` z repozytorium, zaznacz **całość** (Ctrl+A)
3. W konsoli zaznacz całość tego, co jest w edytorze, i **wklej na wierzch**
4. **Publish** (niebieski przycisk u góry)

Konsola od razu powie, jeśli plik ma błąd składni — wtedy nie publikuj
i napisz mi.

**Rób to po KAŻDEJ zmianie w `database.rules.json`.** Testy sprawdzają plik
z repozytorium, więc świecą na zielono niezależnie od tego, co stoi w bazie —
to jedyne miejsce w całym projekcie, gdzie zielony zestaw testów niczego nie
gwarantuje.

---

# 0. Dlaczego są DWA konta, a nie jedno

To jest jedyna rzecz w całej instrukcji, którą warto przeczytać uważnie,
bo wszystko dalej z niej wynika.

**Konto pudełka — `pillbox02@device.local`.** Loguje się nim **urządzenie**,
nie człowiek. Nikt nigdy nie wpisuje go w aplikacji. Jego hasło ląduje
w pamięci trwałej pudełka i po to tam jest, żeby reguły bazy mogły
powiedzieć: *„to urządzenie pisze wyłącznie do swojej gałęzi i do niczego
więcej"*. Adres jest zmyślony (`@device.local` nie istnieje w internecie)
i to jest w porządku — Firebase nie wysyła na niego żadnej poczty.

**Konto dziewczyny — jej prawdziwy e-mail i hasło, które sama zna.** Tym
się loguje w aplikacji, na swoim telefonie. Dokładnie tak samo jak Ty
swoim adresem.

**Dlaczego pudełko nie może używać jej konta.** Dwa niezależne powody,
każdy wystarczający:

* Hasło, którym loguje się pudełko, przechodzi przez `config.h` i siedzi
  potem w jego pamięci. Wpisanie tam jej prawdziwego hasła znaczyłoby, że
  hasło do jej konta leży w urządzeniu, które ma być **zaklejone** i stać
  latami — nie do zmiany bez kabla.
* Konto pudełka jest **celowo ograniczone**: może pisać tylko do
  `devices/pillbox02`. Jej konto ma widzieć kalendarz, ustawienia,
  wszystko. To dwa różne poziomy dostępu i mieszanie ich znosi jedyną
  barierę, jaka tu jest.

**To nie jest „jakieś domyślne hasło".** To hasło maszyny — ma być długie
i nieciekawe, i nikt go nie zapamiętuje. Jej hasło jest **jej** i tylko
ona je zna.

---

# 1a. Konto pudełka

**Wejdź:**
https://console.firebase.google.com/project/pudelko-na-leki/authentication/users

Zobaczysz listę, a na niej `pillbox01@device.local` i swój adres.

**Kliknij `Add user`** (prawy górny róg listy).

| pole | co wpisać |
|---|---|
| Email | `pillbox02@device.local` |
| Password | **to samo hasło co przy `pillbox01`** |

To samo hasło co drugie pudełko to nie lenistwo — oba są kontami maszyn
w Twoim prywatnym projekcie, a to hasło masz już zapisane. Jeśli wolisz
nowe: wymyśl i **zapisz sobie teraz**, bo Firebase nie pokaże go drugi raz.

**Skopiuj UID tego konta** — ten długi ciąg w kolumnie `User UID`. Najedź
myszką, pokaże się ikonka kopiowania. Wklej go sobie w notatnik.

# 1b. Konto dziewczyny

Ten sam ekran, znowu **`Add user`**:

| pole | co wpisać |
|---|---|
| Email | jej **prawdziwy** adres |
| Password | hasło, które jej podasz — najlepiej niech zmieni je u siebie |

**Zmiana hasła po jej stronie.** Na ekranie logowania w aplikacji jest
odzyskiwanie hasła — Firebase wyśle jej link na ten adres. Dlatego adres
ma być prawdziwy: nie po to, żeby dostawała powiadomienia, ale żeby mogła
odzyskać dostęp bez Ciebie.

**Skopiuj też jej UID.** Masz teraz w notatniku **trzy** ciągi znaków
i najłatwiej je pomylić:

| UID | czyj | do czego |
|---|---|---|
| Twój | człowiek | `owner` — właściciel pudełka |
| jej | człowiek | `owners` — współwłaściciel |
| `pillbox02@device.local` | maszyna | `deviceUid` — samo pudełko |

Twój UID jest już w bazie, skopiujesz go w kroku 2a — nie szukaj go tutaj.

---

# 2. Gałąź `pillbox02` w bazie

**Wejdź:**
https://console.firebase.google.com/project/pudelko-na-leki/database/pudelko-na-leki-default-rtdb/data

Zobaczysz drzewo. Rozwiń `devices` — jest tam `pillbox01`.

## 2a. Skopiuj SWÓJ UID

Rozwiń `devices` → `pillbox01`. Pierwsze pole to **`owner`** — to jest UID
Twojego konta.

**Skopiuj tę wartość**, nie przepisuj. Jedna pomylona litera i baza odrzuci
wszystko, co pudełko wyśle, a błąd będzie wyglądał jak awaria sieci.

## 2b. Utwórz `pillbox02` — polami, nie importem

> **TU BYŁ MÓJ BŁĄD I ZOSTAŁ USUNIĘTY.** Wcześniej stało tu „użyj
> `Import JSON`". Import **zastępuje zawartość węzła**, a w konsoli łatwo
> trafić o jedno piętro wyżej — i wtedy kasuje całą gałąź `devices`. Kuba
> tak zrobił i skasował oba pudełka. Dlatego teraz wszystko dodajemy
> **polami, przez `+`**: to więcej klikania, ale `+` może tylko **dodać**.
> Niczego nie da się tak skasować.

Najedź na węzeł **`devices`** — po prawej pojawią się ikonki. Kliknij **`+`**.

Pojawi się wiersz na nazwę i wartość. Za każdym razem działa to tak samo:
**wpisz nazwę**, a potem albo **wpisz wartość**, albo **kliknij `+`** obok,
żeby wejść piętro niżej. `Add` zatwierdza całość.

Buduj to drzewo:

```
pillbox02
├── owner       ← Twój UID
├── owners
│   └── <JEJ UID>   ← wartość: true
├── deviceUid   ← UID konta pillbox02@device.local
└── config
    ├── schedule
    │   └── 0   ← wartość: 20:00
    └── profil  ← wartość: tydzien
```

Krok po kroku, bo zagnieżdżanie w tej konsoli nie jest oczywiste:

1. `+` na `devices` → **Name:** `pillbox02`, **Value** zostaw puste
2. `+` obok → **Name:** `owner`, **Value:** wklej swój UID
3. `+` znowu → **Name:** `owners`, **Value** puste → `+` obok →
   **Name:** jej UID, **Value:** `true`
4. `+` → **Name:** `deviceUid`, **Value:** UID konta pudełka
5. `+` → **Name:** `config`, puste → `+` obok →
   **Name:** `schedule`, puste → `+` obok → **Name:** `0`, **Value:** `20:00`
6. wróć na poziom `config` → `+` → **Name:** `profil`, **Value:** `tydzien`
7. **`Add`**

Dwie rzeczy, na które warto spojrzeć po zatwierdzeniu:

* w `owners` **kluczem jest UID**, a wartością `true` — nie odwrotnie,
* `true` i `20:00` wpisz **bez cudzysłowów**; konsola sama zrobi z nich
  wartość logiczną i napis.

Wzorzec wartości, jeśli chcesz sobie sprawdzić, co ma gdzie stać, leży
w repo jako `firmware/PillBoxWeek/pillbox02-baza.json`. **To materiał do
czytania, nie do importowania.**

**Co to znaczy, linia po linii:**

| pole | znaczenie |
|---|---|
| `owner` | Ty. Zarządzasz pudełkiem. |
| `owners` | dziewczyna. Ma **te same prawa** co Ty do tego pudełka. |
| `deviceUid` | samo pudełko. Bez tego **nic nie zapisze** — reguły odrzucą każdy wpis. |
| `schedule` | **godziny przypomnień**, nie pory brania. Drugą dodasz tak samo: `1` → `22:30`. |
| `profil` | po tym aplikacja pozna, że ma pokazać pudełko tygodniowe, a nie ekrany Warfinu. |

Po wszystkim **sprawdź wzrokiem, że `pillbox01` nadal ma swoje dane.**

## 2d. Czego jej konto NIE widzi — i to jest celowe

Do `pillbox01` **nie dopisujemy jej nigdzie**. Twoje INR, dawki Warfinu
i raport dla lekarza to dane medyczne i zostają tylko Twoje. W aplikacji
zobaczy to pudełko na liście jako **wygaszone, z podpisem „to konto nie ma
dostępu"** — nie zniknie, bo znikające pudełko każe szukać usterki tam,
gdzie jej nie ma.

Ty widzisz oba.

**Jedna rzecz, którą trzeba powiedzieć wprost:** kalendarz każdy ma swój.
Otwarcia klapek pochodzą z pudełka, więc oboje widzicie **to samo** — ale
gdyby ktoś coś ręcznie poprawił w aplikacji, ta poprawka zostaje u niego.
Dane z urządzenia są wspólne, ręczne korekty nie.

---

# 3. `config.h`

Pobierz z GitHuba folder **`firmware/PillBoxWeek`** w całości. Oba pliki
muszą leżeć razem w folderze o nazwie **`PillBoxWeek`** — inaczej Arduino
IDE nie otworzy szkicu.

Trzy linie do wypełnienia:

```c
#define DEVICE_PASSWORD     "TUTAJ_WPISZ_HASLO"        // hasło konta z kroku 1a
#define WIFI_SSID           "TUTAJ_WPISZ_SIEC"         // nazwa Waszego WiFi
#define WIFI_PASS           "TUTAJ_WPISZ_HASLO_WIFI"   // hasło WiFi
```

To hasło **pudełka** z kroku 1a, nie jej. Jej hasło nie pojawia się
w żadnym pliku, nigdy.

**Wpisujesz je ostatni raz.** Przy pierwszym udanym logowaniu pudełko
przepisze je do własnej pamięci trwałej i od tej pory bierze je stamtąd.
Do repozytorium wraca placeholder i tak ma zostać.

**Sieć musi być 2,4 GHz.** ESP32-C3 nie widzi 5 GHz w ogóle. Jeśli router
rozgłasza obie pod jedną nazwą, zwykle jest dobrze — ale gdy pudełko
uparcie nie łapie sieci, to pierwsza rzecz do sprawdzenia.

---

# 4. Wgranie

Menu **Narzędzia** — identycznie jak przy pudełku dziennym:

| Ustawienie | Wartość |
|---|---|
| Board | **XIAO_ESP32C3** |
| USB CDC On Boot | **Enabled** |
| Partition Scheme | **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)** |
| Erase All Flash Before Sketch Upload | **Disabled** |

Program zajmuje **61%** pierwszej partycji. Podłącz kablem, wybierz port,
**Wgraj**.

**To jest ostatnie wgranie kablem, którego potrzebuje to pudełko.** Od
`0.4.0` umie się aktualizować przez WiFi — tak samo jak dzienne. Warunek
jest jeden i jest nie do obejścia: **hasło musi wylądować w jego pamięci
trwałej**, a trafia tam przy pierwszym udanym logowaniu do bazy. Dlatego
krok 5 („czy się zameldowało") nie jest formalnością — dopóki w logu nie
zobaczysz `haslo zapisane w pamieci`, aktualizacja przez WiFi odmówi
i słusznie: binarka z automatu hasła nie zna, więc wgranie jej pudełku,
które też go nie ma, odcięłoby je od bazy. Czyli od jedynej drogi naprawy
bez kabla.

Aktualizację zlecasz potem z aplikacji: **Ustawienia → Urządzenie →
Aktualizuj program pudełka**. Pudełko wykona ją przed zaśnięciem, przy
najbliższym połączeniu — w praktyce po otwarciu klapki. Usłyszysz dwa
piknięcia na start i trzy wznoszące tony, gdy nowa wersja wstanie
i przejdzie całą swoją drogę.

---

# 4a. Zmiana WiFi bez kabla — ukryty przycisk

Od `0.5.0` **nie musisz już wpisywać sieci w `config.h`**. Wpisz ją raz
przy pierwszym wgraniu albo zostaw placeholder i ustaw wszystko
przyciskiem — pudełko zapamiętuje sieć w swojej pamięci trwałej.

**Trzy drogi do portalu:**

| kiedy | co zrobić |
|---|---|
| pudełko śpi, chcesz zmienić sieć | **naciśnij ukryty przycisk** — pudełko budzi się od razu |
| pudełko właśnie startuje (reset, kabel) | **trzymaj przycisk ponad 3 sekundy** |
| pudełko nie zna żadnej sieci | otwiera portal **samo**, przy zimnym starcie |

Krótkie naciśnięcie przy starcie to dalej **autotest** — rozróżnia je
czas trzymania, więc nie musisz pamiętać, kiedy puścić.

**Co się dzieje dalej:**

1. Trzy piknięcia = jestem w trybie konfiguracji
2. W telefonie wybierz sieć **`Pudelko-na-leki`**, hasło **`pudelko123`**
3. Strona otworzy się sama (jeśli nie — wpisz `http://192.168.4.1`)
4. Wybierz swoją sieć z listy, wpisz hasło, **Połącz**
5. Jedno piknięcie = połączone i zapisane

**Sieć zapisuje się dopiero po udanym połączeniu.** Literówka w haśle nie
skasuje więc tej, która działa — usłyszysz dwa sygnały błędu i stara sieć
zostaje. Portal zamkniesz w każdej chwili **drugim naciśnięciem
przycisku** (dwa opadające tony); sam zamknie się po 5 minutach.

Pole **„hasło urządzenia"** pojawia się tylko wtedy, gdy pudełko nie ma
go w pamięci — czyli praktycznie nigdy. Jeśli je zobaczysz, znaczy to, że
pamięć przepadła (np. „Erase All Flash" przy wgrywaniu) i trzeba wpisać
hasło konta `pillbox02@device.local`. Jeśli baza go nie przyjmie, pudełko
je skasuje i będzie można spróbować jeszcze raz.

---

# 4b. Czujnik baterii MAX17048 — podłączenie

Opcjonalny. Pudełko bez niego działa tak samo jak dotąd — po prostu wraca
do dzielnika na płytce, a gdy i ten nie daje sensownego odczytu, uczciwie
mówi „nie wiem". **Ten sam program chodzi z czujnikiem i bez.**

Moduł Adafruit MAX17048 ma dwa złącza JST-PH (ogniwo), dwa STEMMA QT
(I²C) i rząd pinów: `VIN GND SCL SDA INT QStart`.

## Osiem przewodów

Moduł staje się **punktem, w którym ląduje ogniwo**: bateria wchodzi w niego,
a zasilanie idzie z niego dalej do płytki. Dwa złącza JST na module są
połączone równolegle właśnie po to.

| # | od | do | uwaga |
|---|---|---|---|
| 1 | **ogniwo `+`** | moduł **`Bat`** | pady są podpisane na SPODZIE modułu |
| 2 | **ogniwo `−`** | moduł **`GND`** (ten w środku płytki) | |
| 3 | moduł **`Bat`** | XIAO **`BAT+`** (spód) | min. 26 AWG |
| 4 | moduł **`GND`** | XIAO **`BAT−`** (spód) | min. 26 AWG |
| 5 | moduł **`VIN`** | XIAO **`3V3`** | |
| 6 | moduł **`GND`** | XIAO **`GND`** | ta sama masa co wiersz 4 — zbędny, ale zostaw |
| 7 | moduł **`SDA`** | XIAO **`D4`** | |
| 8 | moduł **`SCL`** | XIAO **`D5`** | |
| — | `INT`, `QStart` | nigdzie | niepotrzebne |

```
   ogniwo LiPo
      +  −
      │  │
   ┌──┴──┴──────────────┐
   │   MAX17048         │   VIN ──── 3V3  ┐
   │  (oba złącza JST   │   GND ──── GND  │  XIAO
   │   są równoległe)   │   SDA ──── D4   │  ESP32-C3
   │                    │   SCL ──── D5   ┘
   └──┬──┬──────────────┘
      │  │
     BAT+ BAT−   (pady na spodzie XIAO)
```

**D4 i D5 są wolne** — pudełko używa D0, D1, D2 i D3.

**NIC NIE PRZECINASZ.** MAX17048 tylko *mierzy* napięcie ogniwa, nie jest
licznikiem kulombów. Można go równie dobrze podpiąć po prostu równolegle
do ogniwa i zostawić zasilanie tak, jak jest — tabelka wyżej opisuje
wariant, w którym moduł jest punktem zbornym, bo tak jest porządniej.

## Polaryzacja — nazwy są na SPODZIE modułu

W środku płytki, od spodu, są dwa okrągłe otwory podpisane **`GND`** i
**`Bat`**:

- **`Bat` → plus ogniwa**
- **`GND` → minus ogniwa**

Spód nosi też napis **„Either JST 2 PH for Batt/Load"** — czyli oba złącza
JST są równorzędne: w jedno wchodzi bateria, z drugiego wychodzi zasilanie
do płytki. Można więc użyć złączy zamiast lutowania do padów.

Kontrola, jeśli chcesz pewności: brzęczyk między środkowym `GND`
a `GND` w dolnym rzędzie pinów musi zapiszczeć.

**Odwrotna polaryzacja zabija moduł w sekundę** i jest jedynym
nieodwracalnym błędem w całej tej operacji — sprawdź dwa razy.

## Dwie zworki na spodzie

**`Vin` / `VDD` / `Bat`** — wybiera, skąd zasilany jest sam układ
pomiarowy. **Nie ruszaj.** Skoro `VIN` idzie z 3V3, działa w każdym
ustawieniu. Wracamy do niej tylko wtedy, gdy monitor powie „czujnik nie
odpowiada".

**`LED`** (prawy górny róg) — rozłącza diodę zasilania. **Na pudełku
bateryjnym to nie jest drobiazg:** dioda świecąca całą dobę zjada rząd
miliampera, czyli więcej niż całe pudełko w normalnej pracy. Po podaniu
zasilania zobacz, czy coś się świeci — jeśli tak, przetnij zworkę nożykiem
między padami.

Spód podaje jeszcze dwie rzeczy warte sprawdzenia przy kłopotach:
**`i2c addr: 0x36`** (zgadza się z firmware) i **`VLogic/Vcc: 3-5VDC`**
(3,3 V z XIAO jest w zakresie).

## Wiersze 3 i 4 to CAŁE zasilanie pudełka

Przez tę parę płynie prąd ESP, z szarpnięciami do pół ampera przy
nadawaniu WiFi. Dlatego: nie cieniutkie żyłki (26 AWG albo grubiej,
krótko) i **porządne luty**. Zimny lut na tej drodze daje restarty przy
każdym połączeniu z siecią, a objaw wygląda wtedy jak błąd w programie,
nie jak lut — i szuka się go w złym miejscu.

## Kolejność

1. **USB odłączone**, ogniwo odpięte, jeśli się da.
2. Najpierw cztery cienkie przewody I²C (wiersze 5–8).
3. Potem grube: moduł → XIAO (wiersze 3–4).
4. **Na końcu ogniwo** (wiersze 1–2), po zmierzeniu polaryzacji.
5. Przed podaniem prądu: brzęczyk na `3V3`↔`GND` i `SDA`↔`SCL` — żadne
   nie może piszczeć.
6. **Przyklej moduł** na gorąco albo dwustronną taśmą. Wiszący na
   przewodach urwie pady po kilku otwarciach pudełka.

**Zanim zakleisz obudowę:** w monitorze portu (115200) po wgraniu
zobaczysz jedną z dwóch linii:

```
[BAT] czujnik: 87%  3.98 V          ← czujnik gada, gotowe
[BAT] czujnik nie odpowiada - biore odczyt z dzielnika
```

Druga linia znaczy, że coś jest nie tak z SDA/SCL albo z zasilaniem
modułu. To samo widać potem w aplikacji: **Ustawienia → Urządzenie →
Pomiar baterii** mówi wprost, czy liczba przyszła z czujnika, z dzielnika,
czy nie przyszła wcale.

---

# 5. Czy się zameldowało

Otwórz **Monitor portu szeregowego, 115200**:

```
===== PillBoxWeek 0.5.0  (wybudzenie 1) =====
[BAT] 87%  4.08 V
[   ] zimny start
[NET] lacze z 'TwojaSiec'
[NET] polaczono, -56 dBm
[NET] czas zsynchronizowany
[FB ] zalogowano
[FB ] haslo zapisane w pamieci
[FB ] zdarzenie OK: {"ts":...,"type":"boot",...}
[SEN] spie na ... min (kolejka: 0)
```

**`[FB ] haslo zapisane w pamieci`** to ta linia, na którą czekasz — od niej
pudełko radzi sobie samo i kolejne wgrania mogą iść z placeholderem.

| co widzisz | co to znaczy |
|---|---|
| `[FB ] logowanie HTTP 400` | hasło w `config.h` ≠ hasło konta `pillbox02@device.local` |
| `[FB ] zdarzenie HTTP 401` albo `403` | `deviceUid` w bazie nie zgadza się z UID konta pudełka |
| `[FB ] zdarzenie HTTP 400` | reguły odrzuciły wpis — przyślij mi linię `baza: ...` spod spodu |
| `[NET] brak sieci` | WiFi: nazwa, hasło albo 5 GHz |
| cisza, żadnego logu | zły port albo **USB CDC On Boot** na Disabled |

Na koniec zajrzyj w bazę: w `devices/pillbox02` powinny dojść `events`
i `status`. To dowód, że cała droga działa.

---

# 6. Na jej telefonie

1. Otwiera aplikację, loguje się **swoim** adresem i hasłem.
2. Aplikacja sama wybierze pudełko tygodniowe — bo do drugiego nie ma
   dostępu i wie o tym, zanim cokolwiek pokaże.
3. Dodaje do ekranu głównego (Safari → Udostępnij → Do ekranu początkowego).

U Ciebie: **Ustawienia → Pudełko** i przełączasz między dwoma. Wybór siedzi
w pamięci **tego telefonu**, nie w bazie — więc Ty możesz patrzeć na
dzienne, ona na tygodniowe, na tym samym ekranie ustawień.

---

# Jak tego używać

**Otwarcie klapki = tabletka wzięta.** Nic nie trzeba potwierdzać. Pudełko
pika tyle razy, która to komora — poniedziałek raz, wtorek dwa, ...
niedziela siedem. To potwierdzenie, że wie, którą klapkę otworzyła.

**Jeśli do godziny z `schedule` klapka nie została otwarta** — pudełko
przypomina. Nie odpuszcza po jednym piknięciu: wraca **trzy razy, co 10
minut**. Otwarcie klapki w trakcie alarmu ucina go natychmiast **i zapisuje
dawkę**.

**Kilka klapek naraz to napełnianie**, nie dawka — pudełko rozpoznaje to po
napięciu (jeden długi, niski dźwięk) i niczego nie zapisuje. Można spokojnie
otworzyć wszystkie siedem i wsypać tabletki na tydzień.

**Klapkę zamknąć.** Zostawiona otwarta trzyma pudełko w czuwaniu — po
minucie samo pójdzie spać, ale będzie się budzić co dwie minuty, dopóki
jest otwarta. Bateria tego nie lubi.

**Dwa krótkie piknięcia to ostrzeżenie o baterii** (poniżej 15%).

## Autotest bez komputera

Trzymaj **przycisk** przy podłączaniu zasilania. Pudełko:

1. piknie trzy razy — „jestem",
2. zmierzy drabinkę i piknie numerem komory, którą widzi jako otwartą
   (albo jednym długim dźwiękiem, jeśli otwarta jest więcej niż jedna),
3. piknie dwa razy niżej — koniec.

Otwórz przy tym jedną klapkę i policz piknięcia. Sprawdza **całą** drogę:
rezystor, mikroprzełącznik, lutowanie i progi.

---

---

# 7. RATUNEK — gdy gałąź `devices` zniknęła

Stało się: `Import JSON` zrobiony na `devices` zamiast na `pillbox02`
skasował **oba pudełka**. Poniżej dokładnie, co przeżyło i jak wrócić.

## Co NIE zginęło — i to jest większość

**Cała historia leków jest cała.** Dawki, pomiary INR, rozpisania lekarza,
znaczniki dni i kopie zapasowe leżą pod `users/<Twój uid>/…` — a skasowana
została gałąź `devices`. To dwie różne gałęzie i wipe dotknął tylko jednej.

**Pudełko dalej dzwoni.** Harmonogram, strefa czasowa, stan opakowania
i termin INR siedzą w jego własnej pamięci trwałej. `fetchConfig()` przy
braku konfiguracji w bazie loguje *„brak config — zostaje lokalny"* i nie
rusza tego, co ma. Twoje 20:00 działa.

**Żadna dawka nie przepadnie.** Reguły bazy wymagają `deviceUid`, więc bez
tego węzła pudełko nie ma prawa pisać — dostaje HTTP 401. A 401 **nie jest**
odrzuceniem trwałym (`trwaleOdrzucony()` zdejmuje wpis tylko przy 400/413),
więc dawki czekają w kolejce w pudełku i pójdą, gdy węzeł wróci. Kolejka ma
120 miejsc.

## Co zginęło na dobre

**Surowa historia otwarć pudełka** (`devices/pillbox01/events`) — nie ma jej
skąd wziąć. Ale kalendarz dawek jest z tych zdarzeń **już policzony**
i nietknięty, więc historia leczenia została; zginął tylko zapis źródłowy.

`status`, `scan` i dziennik nieudanych zapisów pudełko nadpisze samo przy
pierwszym wybudzeniu.

## Jak wrócić — po kolei

### Krok 1: dwa pola, żeby pudełko znowu mogło pisać

Najpilniejsze, bo od tego zależy, czy aplikacja cokolwiek widzi.

Potrzebujesz dwóch UID-ów z
https://console.firebase.google.com/project/pudelko-na-leki/authentication/users
— swojego (przy Twoim adresie) i konta `pillbox01@device.local`.

W bazie, **polami przez `+`** (nigdy importem):

```
devices
└── pillbox01
    ├── owner      ← Twój UID
    ├── deviceUid  ← UID konta pillbox01@device.local
    └── config
        └── schedule
            └── 0  ← Twoja godzina przypomnienia, np. 20:00
```

`config/schedule` zakładasz od razu, bo reguły wymagają, żeby konfiguracja
— jeśli w ogóle istnieje — miała harmonogram.

Po `Add` otwórz aplikację. Powinna zobaczyć pudełko, a przy najbliższym
wybudzeniu pudełko dośle wszystko, co czekało w kolejce.

### Krok 2: reszta ustawień z kopii zapasowej

Twoje kopie leżą w bazie pod `users/<Twój uid>/backup/` — trzy najnowsze,
robione automatycznie raz na dzień przy otwarciu aplikacji. Zajrzyj tam
i zobacz `cfg` w najnowszej: to jest Twój harmonogram, dawkowanie, nazwa
leku, odstęp INR i stan opakowania.

**W aplikacji: Ustawienia → Kopia zapasowa → odtwórz z kopii w bazie.**
Od wersji `2026-09-27.83` odtwarzanie przywraca **także ustawienia
pudełka** — dokłada tylko te pola, których w bazie nie ma, więc niczego
nie nadpisze. Potwierdzenie przed zapisem powie, ile ustawień wróci.

Zanim to zrobisz, sprawdź w aplikacji **dawkowanie** (Ustawienia → Lek).
Przy pustej konfiguracji aplikacja pokazuje wartości domyślne — `defaultDose`
wraca na 1 — więc dopóki ustawienia nie wrócą, liczba tabletek w kalendarzu
może być liczona nie Twoją dawką.

### Krok 3: pudełko tygodniowe od nowa

Wróć do kroku **2b** wyżej i zbuduj `pillbox02` polami. Nic z niego nie
zginęło, bo nic w nim jeszcze nie było.

### Czego NIE robić

* **Nie klikaj „Import JSON"** — ani teraz, ani nigdy w tym projekcie.
* Nie kasuj niczego z `users/` „żeby posprzątać". Tam siedzi cała historia
  leczenia i Twoje kopie zapasowe.
* Nie kasuj pełnej pamięci pudełka („Erase All Flash") — z nią zniknie
  hasło do bazy i lista sieci WiFi, czyli jedyna droga do pudełka bez kabla.

# Czego to jeszcze nie ma

* **Nie było uruchomione na płytce.** Program się kompiluje i sprzęt jest
  zmierzony osobnym szkicem testowym — ale ten kod nie chodził w pudełku
  ani minuty. To pierwszy raz.
* **Nie ma aktualizacji przez WiFi.** Każda poprawka = kabel.
* **Nie ma portalu do zmiany sieci.** Inne WiFi = kabel i `config.h`.
* **Nie ma powiadomień na telefon.** Pudełko piszczy, aplikacja pokazuje.
* **Ekrany aplikacji to na razie te same co przy Warfinie.** Wybór pudełka
  i dostęp działają, dane są właściwe i nie mieszają się — ale INR
  i raport dla lekarza z ekranów jeszcze nie zniknęły. To następny krok.
