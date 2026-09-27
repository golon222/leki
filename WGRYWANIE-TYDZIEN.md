# Pudełko tygodniowe — od zera do działania

Osobne urządzenie, osobny program, osobne konta w bazie. Pudełka dziennego
to nie dotyka w żadnym miejscu — ani jednego pliku, ani jednego wpisu.

Całość to jakieś 25 minut:

1. konto **pudełka** i konto **dziewczyny** — to dwie różne rzeczy (10 min)
2. gałąź `pillbox02` w bazie (5 min)
3. `config.h` — trzy linie (2 min)
4. wgranie kablem z Arduino IDE (5 min)
5. sprawdzenie, czy się zameldowało

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

## 2b. Utwórz `pillbox02`

Najedź na węzeł **`devices`** — po prawej pojawią się ikonki. Kliknij **`+`**.

* **Name:** `pillbox02`
* **Value:** zostaw **puste** i kliknij **`+`** jeszcze raz — pojawi się
  wcięty wiersz. W nim:
  * **Name:** `owner`
  * **Value:** wklej **swój** UID

**Kliknij `Add`.**

(Po co ten jeden ręczny wpis, skoro zaraz wklejamy całość: konsola pozwala
wklejać JSON tylko do węzła, **który już istnieje**. Ten krok robi
`pillbox02` bytem, w który da się celować.)

## 2c. Wgraj resztę jednym wklejeniem

Najedź na węzeł **`pillbox02`** i kliknij **trzy kropki `⋮`** →
**`Import JSON`**.

> **Jedyne miejsce, w którym można tu narobić szkody:** upewnij się, że
> trzy kropki klikasz na **`pillbox02`**, a nie na `devices`. Import
> **zastępuje** zawartość węzła — zrobiony na `devices` skasowałby
> `pillbox01`, czyli całą historię Warfinu. Na `pillbox02` jest zupełnie
> bezpieczny, bo tam i tak nic jeszcze nie ma.

```json
{
  "owner": "TWOJ_UID",
  "owners": { "JEJ_UID": true },
  "deviceUid": "UID_KONTA_pillbox02",
  "config": {
    "schedule": ["20:00"],
    "profil": "tydzien"
  }
}
```

(ten sam plik leży w repo jako `firmware/PillBoxWeek/pillbox02-baza.json`)

Uwaga: w `owners` **kluczem jest UID**, a wartością `true`. Nie odwrotnie.

**Co to znaczy, linia po linii:**

| pole | znaczenie |
|---|---|
| `owner` | Ty. Zarządzasz pudełkiem. |
| `owners` | dziewczyna. Ma **te same prawa** co Ty do tego pudełka. |
| `deviceUid` | samo pudełko. Bez tego **nic nie zapisze** — reguły odrzucą każdy wpis. |
| `schedule` | **godziny przypomnień**, nie pory brania. Możesz dać dwie: `["20:00","22:30"]` — wtedy pudełko przypomni jeszcze raz, jeśli klapka nadal nie została otwarta. |
| `profil` | po tym aplikacja pozna, że ma pokazać pudełko tygodniowe, a nie ekrany Warfinu. |

Po imporcie **sprawdź wzrokiem, że `pillbox01` nadal ma swoje dane.**

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

Program zajmuje **58%** pierwszej partycji. Podłącz kablem, wybierz port,
**Wgraj**.

---

# 5. Czy się zameldowało

Otwórz **Monitor portu szeregowego, 115200**:

```
===== PillBoxWeek 0.1.0  (wybudzenie 1) =====
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
