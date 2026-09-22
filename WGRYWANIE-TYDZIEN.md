# Pudełko tygodniowe — od zera do działania

Osobne urządzenie, osobny program, **osobne konto w bazie**. Pudełka dziennego
to nie dotyka w żadnym miejscu — ani jednego pliku, ani jednego wpisu w bazie.

Całość to jakieś 20 minut. Kolejność jest ważna:

1. konto urządzenia w Firebase (5 min)
2. gałąź `pillbox02` w bazie (5 min)
3. `config.h` — trzy linie (2 min)
4. wgranie kablem z Arduino IDE (5 min)
5. sprawdzenie, czy się zameldowało

Wgrane bez kroków 1–2 pudełko **będzie działać i piszczeć**, ale wszystko, co
zapisze, zostanie w jego pamięci zamiast dojechać do telefonu. Nic nie przepada
— dojedzie później, jak konto powstanie. Ale lepiej zrobić to po kolei.

---

# 1. Konto urządzenia

Każde pudełko loguje się do bazy **własnym kontem**. Nie Twoim — własnym.
Dzięki temu reguły bazy mogą powiedzieć „to urządzenie pisze wyłącznie do
swojej gałęzi". `pillbox01` ma takie konto od początku, `pillbox02` musi
dostać swoje.

**Wejdź:**
https://console.firebase.google.com/project/pudelko-na-leki/authentication/users

Zobaczysz listę, a na niej `pillbox01@device.local` i swój adres.

**Kliknij `Add user`** (niebieski przycisk, prawy górny róg listy).

| pole | co wpisać |
|---|---|
| Email | `pillbox02@device.local` |
| Password | **to samo hasło co przy `pillbox01`** |

> **To samo hasło to nie lenistwo — to jedno hasło mniej do zgubienia.**
> Oba konta są w Twoim prywatnym projekcie, oba służą wyłącznie pudełkom,
> a to hasło i tak masz już zapisane. Jeśli wolisz nowe — wymyśl i **zapisz
> sobie teraz**, bo za chwilę będzie potrzebne, a Firebase go nie pokaże
> drugi raz.

**Kliknij `Add user`.** Konto pojawi się na liście.

**Teraz skopiuj jego UID.** To ten długi ciąg znaków w ostatniej kolumnie
(`User UID`), coś w rodzaju `k3Jx9mQ2pVfR7dN8sT1yZ4wB6aC0`. Najedź na niego
myszką — pokaże się ikonka kopiowania. **Wklej go sobie gdzieś na boku**,
w notatniku. Będzie potrzebny w kroku 2.

---

# 2. Gałąź `pillbox02` w bazie

**Wejdź:**
https://console.firebase.google.com/project/pudelko-na-leki/database/pudelko-na-leki-default-rtdb/data

Zobaczysz drzewo. Rozwiń `devices` — jest tam `pillbox01`.

## 2a. Najpierw skopiuj SWÓJ UID

Rozwiń `devices` → `pillbox01`. Pierwsze pole to **`owner`** — to jest UID
**Twojego** konta (tego, którym logujesz się w aplikacji).

**Skopiuj tę wartość.** Nie przepisuj ręcznie, skopiuj — jedna pomylona
litera i baza odrzuci wszystko, co pudełko wyśle, a błąd będzie wyglądał
jak awaria sieci.

Masz teraz dwa UID-y na boku:

* **UID właściciela** — z `devices/pillbox01/owner` (Twój)
* **UID urządzenia** — z kroku 1 (konta `pillbox02@device.local`)

To są **dwie różne rzeczy** i najłatwiej je pomylić. Właściciel to Ty,
urządzenie to pudełko.

## 2b. Utwórz `pillbox02`

Najedź na węzeł **`devices`** — po prawej pojawią się ikonki. Kliknij
**`+`**.

Pojawi się pusty wiersz na nazwę i wartość:

* **Name:** `pillbox02`
* **Value:** zostaw **puste** i kliknij **`+`** jeszcze raz — pojawi się
  wcięty, zagnieżdżony wiersz. W nim:
  * **Name:** `owner`
  * **Value:** wklej **UID właściciela**

**Kliknij `Add`.** W drzewie powstanie `devices/pillbox02/owner`.

(Po co ten jeden ręczny wpis, skoro za chwilę wkleimy całość: konsola
pozwala wklejać JSON tylko do węzła, **który już istnieje**. Ten krok
robi `pillbox02` bytem, w który da się celować.)

## 2c. Wgraj resztę jednym wklejeniem

Teraz najedź na węzeł **`pillbox02`** (ten świeżo utworzony) i kliknij
**trzy kropki `⋮`** → **`Import JSON`**.

> **Uwaga, jedyne miejsce, w którym można tu narobić szkody:** upewnij się,
> że trzy kropki klikasz na **`pillbox02`**, a nie na `devices`. Import
> **zastępuje** zawartość węzła — zrobiony na `devices` skasowałby
> `pillbox01`, czyli całą historię Warfinu. Na `pillbox02` jest zupełnie
> bezpieczny, bo tam i tak nic jeszcze nie ma.

Wklej to, podmieniając oba `WKLEJ_TU_...` na UID-y z notatnika:

```json
{
  "owner": "WKLEJ_TU_UID_SWOJEGO_KONTA",
  "deviceUid": "WKLEJ_TU_UID_KONTA_pillbox02",
  "config": {
    "schedule": ["20:00"],
    "profil": "tydzien"
  }
}
```

(ten sam plik leży w repo jako `firmware/PillBoxWeek/pillbox02-baza.json`,
jeśli wygodniej Ci go edytować w edytorze i wybrać z dysku)

**Co to znaczy, linia po linii:**

| pole | znaczenie |
|---|---|
| `owner` | Ty. Bez tego aplikacja nie zobaczy pudełka. |
| `deviceUid` | pudełko. Bez tego **nic nie zapisze** — reguły odrzucą każdy wpis. |
| `schedule` | **godziny przypomnień**, nie pory brania. Możesz dać dwie: `["20:00","22:30"]` — wtedy pudełko przypomni jeszcze raz, jeśli do tej pory klapka nie została otwarta. |
| `profil` | po tym aplikacja pozna, że ma pokazać pudełko tygodniowe, a nie ekrany Warfinu. |

Kliknij **`Import`**. Drzewo powinno wyglądać tak:

```
devices
├── pillbox01
│   └── ... (nietknięte)
└── pillbox02
    ├── owner      "..."
    ├── deviceUid  "..."
    └── config
        ├── schedule
        │   └── 0   "20:00"
        └── profil  "tydzien"
```

**Sprawdź wzrokiem, że `pillbox01` nadal ma swoje dane.** Jedno spojrzenie,
a spokój na całą resztę.

---

# 3. `config.h`

Pobierz z GitHuba folder **`firmware/PillBoxWeek`** w całości. Oba pliki
muszą leżeć razem w folderze o nazwie **`PillBoxWeek`** — inaczej Arduino IDE
nie otworzy szkicu.

Otwórz `config.h` i wypełnij trzy linie:

```c
#define DEVICE_PASSWORD     "TUTAJ_WPISZ_HASLO"        // hasło konta z kroku 1
#define WIFI_SSID           "TUTAJ_WPISZ_SIEC"         // nazwa Waszego WiFi
#define WIFI_PASS           "TUTAJ_WPISZ_HASLO_WIFI"   // hasło do WiFi
```

**Hasło do bazy wpisujesz ostatni raz.** Przy pierwszym udanym logowaniu
pudełko przepisze je do własnej pamięci trwałej i od tej pory bierze je
stamtąd — dokładnie tak jak pudełko dzienne. Do repozytorium wraca
placeholder i tak ma zostać.

**Sieć musi być 2,4 GHz.** ESP32-C3 nie widzi 5 GHz w ogóle — jeśli router
rozgłasza obie pod tą samą nazwą, zwykle jest dobrze, ale gdy pudełko
uparcie nie łapie sieci, to jest pierwsza rzecz do sprawdzenia.

---

# 4. Wgranie

Menu **Narzędzia** — identycznie jak przy pudełku dziennym:

| Ustawienie | Wartość |
|---|---|
| Board | **XIAO_ESP32C3** |
| USB CDC On Boot | **Enabled** |
| Partition Scheme | **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)** |
| Erase All Flash Before Sketch Upload | **Disabled** |

Program zajmuje **58%** pierwszej partycji.

Podłącz kablem, wybierz port, **Wgraj**.

---

# 5. Czy się zameldowało

Otwórz **Monitor portu szeregowego, 115200**. Po wgraniu powinno pójść tak:

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

Jak coś nie gra:

| co widzisz | co to znaczy |
|---|---|
| `[FB ] logowanie HTTP 400` | hasło w `config.h` ≠ hasło konta `pillbox02@device.local` |
| `[FB ] zdarzenie HTTP 401` albo `403` | `deviceUid` w bazie nie zgadza się z UID konta (krok 2c) |
| `[FB ] zdarzenie HTTP 400` | reguły odrzuciły wpis — przyślij mi linię `baza: ...` spod spodu |
| `[NET] brak sieci` | WiFi: nazwa, hasło albo 5 GHz |
| cisza, żadnego logu | zły port albo **USB CDC On Boot** ustawione na Disabled |

Na koniec zajrzyj w bazę: w `devices/pillbox02` powinny dojść `events`
i `status`. To jest dowód, że cała droga działa.

---

# 6. W aplikacji

**Ustawienia → Pudełko → Tygodniowe.** Aplikacja się przeładuje i od tej
pory patrzy na `pillbox02`. Powrót do Warfinu tym samym kafelkiem.

Wybór siedzi w pamięci **tego telefonu**, nie w bazie — więc Ty możesz mieć
otwarte pudełko dzienne, a ona tygodniowe, na tym samym koncie.

---

# Jak tego używać

**Otwarcie klapki = tabletka wzięta.** Nic nie trzeba potwierdzać, nic
klikać. Pudełko pika tyle razy, która to komora — poniedziałek raz, wtorek
dwa, ... niedziela siedem. To jest potwierdzenie, że wie, którą klapkę
otworzyłaś.

**Jeśli do godziny z `schedule` klapka nie została otwarta** — pudełko
przypomina. Nie odpuszcza po jednym piknięciu: wraca **trzy razy, co 10
minut**. Otwarcie klapki w trakcie alarmu ucina go natychmiast **i zapisuje
dawkę**.

**Kilka klapek naraz to napełnianie**, nie dawka — pudełko rozpoznaje to po
napięciu (jeden długi, niski dźwięk) i niczego nie zapisuje. Możesz spokojnie
otworzyć wszystkie siedem i wsypać tabletki na tydzień.

**Klapkę zamknij.** Zostawiona otwarta trzyma pudełko w czuwaniu — po minucie
samo pójdzie spać, ale będzie się budzić co dwie minuty, dopóki jest otwarta.
Bateria tego nie lubi.

**Dwa krótkie piknięcia to ostrzeżenie o baterii** (poniżej 15%).

## Autotest bez komputera

Trzymaj **przycisk** przy podłączaniu zasilania. Pudełko:

1. piknie trzy razy — „jestem",
2. zmierzy drabinkę i piknie numerem komory, którą widzi jako otwartą
   (albo jednym długim dźwiękiem, jeśli otwarta jest więcej niż jedna),
3. piknie dwa razy niżej — koniec testu.

Otwórz przy tym jedną klapkę i policz piknięcia. To sprawdza **całą** drogę:
rezystor, mikroprzełącznik, lutowanie i progi.

---

# Czego to jeszcze nie ma

Mówię wprost, żeby nie było niespodzianek rano:

* **Nie było uruchomione na płytce.** Program się kompiluje i sprzęt jest
  zmierzony osobnym szkicem testowym — ale ten konkretny kod nie chodził
  w pudełku jeszcze ani minuty. To jest pierwszy raz.
* **Nie ma aktualizacji przez WiFi.** Każda poprawka = kabel. Pudełko dzienne
  dorobiło się OTA dopiero po kilkunastu wersjach.
* **Nie ma portalu do zmiany sieci.** Inne WiFi = kabel i `config.h`.
* **Nie ma powiadomień na telefon.** Pudełko piszczy, aplikacja pokazuje —
  Telegram przyjdzie później, jeśli będzie potrzebny.
* **Ekrany aplikacji to na razie te same co przy Warfinie.** Wybór pudełka
  działa i dane są właściwe, ale INR i raport dla lekarza jeszcze z nich nie
  zniknęły. To następny krok — biorę go, jak powiesz, że sprzęt gada z bazą.
