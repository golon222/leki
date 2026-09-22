# Pudełko tygodniowe — jak je uruchomić

Osobne urządzenie, osobny program, **osobne konto w bazie**. Pudełka dziennego
to nie dotyka w żadnym miejscu — ani jednego pliku, ani jednego wpisu.

Kolejność jest ważna: najpierw baza, potem `config.h`, potem wgranie.
Wgrane bez konta w bazie pudełko będzie działać i piszczeć, ale wszystko, co
zapisze, zostanie w jego pamięci zamiast dojechać do telefonu.

---

## 1. Załóż konto urządzenia (Firebase Console)

**Authentication → Users → Add user**

| pole | wartość |
|---|---|
| e-mail | `pillbox02@device.local` |
| hasło | wymyśl i **zapisz sobie** — wpiszesz je za chwilę w `config.h` |

Po dodaniu skopiuj **UID** tego konta (długi ciąg znaków obok e-maila).

## 2. Załóż gałąź urządzenia (Realtime Database)

W `devices` dodaj `pillbox02` i wklej:

```json
{
  "owner": "TWOJ_UID_CZLOWIEKA",
  "deviceUid": "UID_KONTA_pillbox02",
  "config": {
    "schedule": ["20:00"],
    "profil": "tydzien"
  }
}
```

* `owner` — UID **Twojego** konta, tego samego co przy `pillbox01`. Skopiuj
  je wprost z `devices/pillbox01/owner`, żeby nie było literówki.
* `deviceUid` — UID konta `pillbox02@device.local` z kroku 1.
* `schedule` — **godziny przypomnień**, nie pory brania. Możesz dodać drugą,
  np. `["20:00","22:30"]` — wtedy pudełko przypomni jeszcze raz, jeśli do
  tej pory klapka nie została otwarta.
* `profil: "tydzien"` — to po tym aplikacja pozna, że ma pokazać pudełko
  tygodniowe, a nie ekrany Warfinu.

Bez `owner` i `deviceUid` **reguły bazy odrzucą każdy zapis** — i dobrze,
bo to one pilnują, żeby cudze pudełko nie pisało do Twojego.

## 3. Uzupełnij `config.h`

Pobierz folder `firmware/PillBoxWeek` w całości — oba pliki muszą leżeć razem
w folderze o nazwie **`PillBoxWeek`**, inaczej Arduino IDE nie otworzy szkicu.

Trzy linie do wypełnienia:

```c
#define DEVICE_PASSWORD     "TUTAJ_WPISZ_HASLO"        // hasło z kroku 1
#define WIFI_SSID           "TUTAJ_WPISZ_SIEC"         // nazwa WiFi
#define WIFI_PASS           "TUTAJ_WPISZ_HASLO_WIFI"   // hasło WiFi
```

**Hasło do bazy wpisujesz ostatni raz.** Przy pierwszym udanym logowaniu
pudełko przepisze je do własnej pamięci i od tej pory bierze je stamtąd —
tak samo jak pudełko dzienne. W repozytorium ma zostać placeholder.

## 4. Ustawienia płytki

Menu **Narzędzia** — identycznie jak przy pudełku dziennym:

| Ustawienie | Wartość |
|---|---|
| Board | **XIAO_ESP32C3** |
| USB CDC On Boot | **Enabled** |
| Partition Scheme | **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)** |
| Erase All Flash Before Sketch Upload | **Disabled** |

Program zajmuje **58%** pierwszej partycji.

## 5. Wgraj i sprawdź

Otwórz **Monitor portu szeregowego, 115200**. Po wgraniu zobaczysz:

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

`[FB ] haslo zapisane w pamieci` to ta linia, na którą czekasz — od niej
pudełko radzi sobie samo.

**Jeśli zamiast tego jest `[FB ] logowanie HTTP 400`** — hasło w `config.h`
nie zgadza się z tym z Firebase. `HTTP -1` albo `brak sieci` to WiFi, nie baza.

---

## Jak tego używać

**Otwarcie klapki = tabletka wzięta.** Nic nie trzeba potwierdzać, nic
klikać. Pudełko pika tyle razy, która to komora — poniedziałek raz,
wtorek dwa, ... niedziela siedem. To jest potwierdzenie, że wie, którą
klapkę otworzyłaś.

**Jeśli do godziny z `schedule` klapka nie została otwarta** — pudełko
przypomina. Nie odpuszcza po jednym piknięciu: wraca **trzy razy, co 10
minut**. Otwarcie klapki w trakcie alarmu ucina go natychmiast.

**Kilka klapek naraz to napełnianie**, nie dawka — pudełko rozpoznaje to po
napięciu (jeden długi niski dźwięk) i niczego nie zapisuje. Możesz spokojnie
otworzyć wszystkie siedem i wsypać tabletki na tydzień.

**Klapkę zamknij.** Zostawiona otwarta trzyma pudełko w stanie czuwania —
po minucie czekania samo pójdzie spać, ale będzie się budzić co dwie minuty,
dopóki jest otwarta. Bateria tego nie lubi.

**Dwa krótkie piknięcia to ostrzeżenie o baterii** (poniżej 15%).

### Autotest bez komputera

Trzymaj **przycisk** przy podłączaniu zasilania. Pudełko:

1. piknie trzy razy — „jestem",
2. zmierzy drabinkę i piknie numerem komory, którą widzi jako otwartą
   (albo jednym długim dźwiękiem, jeśli otwarta jest więcej niż jedna),
3. piknie dwa razy niżej — koniec testu.

Otwórz przy tym jedną klapkę i policz piknięcia. To sprawdza **całą** drogę:
rezystor, mikroprzełącznik, lutowanie i progi.

### W aplikacji

**Ustawienia → Pudełko → Tygodniowe.** Aplikacja się przeładuje i od tej
pory patrzy na `pillbox02`. Powrót do Warfinu tym samym kafelkiem.

---

## Czego to jeszcze nie ma

Mówię wprost, żeby nie było niespodzianek:

* **Nie było uruchomione na płytce.** Program się kompiluje i sprzęt jest
  zmierzony osobnym szkicem testowym, ale ten konkretny kod nie chodził
  jeszcze w pudełku ani minuty.
* **Nie ma aktualizacji przez WiFi.** Każda poprawka = kabel. Pudełko
  dzienne dorobiło się OTA dopiero po kilkunastu wersjach.
* **Nie ma portalu do zmiany sieci.** Inne WiFi = kabel i `config.h`.
* **Nie ma powiadomień na telefon.** Pudełko piszczy, aplikacja pokazuje —
  Telegram przyjdzie później, jeśli będzie potrzebny.
* **Ekrany aplikacji to na razie te same co przy Warfinie.** Wybór pudełka
  działa i dane są właściwe, ale INR i raport dla lekarza jeszcze z nich
  nie zniknęły. To następny krok.
