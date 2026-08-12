# Jak wgrać firmware (1.38.x)

To wgranie jest **jednorazowo inne** niż wszystkie poprzednie. Zmienia się
jedno ustawienie w menu Arduino IDE. Po nim aktualizacje idą już przez WiFi,
przyciskiem w aplikacji.

**Wgrywasz te same dwa pliki co zawsze: `PillBox.ino` i `config.h`.**
Nic więcej nie pobierasz i nic więcej nie otwierasz. `PillBox.bin` z repo jest
dla pudełka, nie dla Ciebie — Arduino IDE go nie potrzebuje.

---

## 1. Pobierz folder `firmware/PillBox`

Tak jak zawsze — oba pliki muszą leżeć razem w folderze o nazwie `PillBox`.

## 2. Wpisz hasło w `config.h`

Znajdź linię:

```c
#define DEVICE_PASSWORD     "TUTAJ_WPISZ_HASLO"
```

i wstaw prawdziwe hasło konta `pillbox01@device.local`.

**To ostatni raz, kiedy to robisz.** Przy pierwszym udanym połączeniu pudełko
przepisze hasło do własnej pamięci i od tej pory będzie go stamtąd używać.
Dlatego binarki budowane automatem — bez hasła — będą działać.

Linii poniżej (`PASSWORD_PLACEHOLDER`) **nie ruszaj**. To wzorzec, po którym
pudełko poznaje, że hasła nie ma.

## 3. Ustawienia płytki w Arduino IDE

Menu **Narzędzia**:

| Ustawienie | Wartość |
|---|---|
| Board | **XIAO_ESP32C3** |
| USB CDC On Boot | **Enabled** |
| **Partition Scheme** | **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)** |
| **Erase All Flash Before Sketch Upload** | **Disabled** |

Dwie ostatnie pozycje są tu całą różnicą wobec poprzednich wgrań.

### Partition Scheme — dlaczego się zmienia

Do 1.37.0 było `Huge APP (3MB No OTA/1MB SPIFFS)` — **jedna** partycja
programu. Aktualizacja przez WiFi zapisuje nowy program do **drugiej**
partycji i dopiero potem się na nią przełącza. Przy jednej nie ma dokąd pisać.

Nowy podział daje dwie po 1,875 MB. Program zajmuje 62%, więc mieści się
z zapasem 730 kB.

### Erase All Flash — dlaczego MUSI być wyłączone

Ta opcja kasuje **całą** pamięć, także tę, której nie widać w programie.
Zginęłyby wtedy:

- niewysłane dawki czekające w kolejce,
- lista znanych sieci WiFi (razem z hotspotem na wyjazd),
- znacznik „dzisiejsza dawka już wzięta" — pudełko pozwoliłoby zapisać ją
  drugi raz,
- historia ładowań i czarna skrzynka.

Pamięć trwała leży pod tym samym adresem w obu podziałach, więc przy
**wyłączonej** opcji zmiana podziału jest dla tych danych niewidoczna.

## 4. Wgraj

Zwykłą strzałką, tak jak zawsze.

## 5. Sprawdź, czy się udało

W aplikacji, ekran **Urządzenie**:

- **Firmware** zgadza się z numerem, który stoi w `config.h` przy
  `FW_VERSION` (dziś 1.38.1). Jeśli widnieje stara wersja, wgranie nie doszło.
- Nie ma czerwonego ostrzeżenia o braku hasła. Jeśli jest — pudełko nie
  zdążyło jeszcze zapisać hasła w pamięci. Poczekaj na jedno połączenie
  (otwórz i zamknij wieczko) i sprawdź ponownie.

Od tej chwili na tym ekranie pojawia się przycisk **„Zaktualizuj pudełko"**,
gdy tylko automat zbuduje nowszą wersję.

---

## Jak wygląda aktualizacja przez WiFi

1. Naciskasz przycisk w aplikacji — to **zleca** aktualizację, nic więcej.
2. Pudełko śpi, więc nic nie dzieje się od razu. Zobaczy zlecenie przy
   najbliższym połączeniu i wykona przed zaśnięciem: **po zamknięciu wieczka
   albo na ładowarce**.

   **Samo otwarcie nie wystarczy.** Przy otwartym wieczku pudełko czeka na
   zamknięcie (do 15 minut) i dopiero potem bierze się za aktualizację —
   dokładnie tak samo, jak przy zwykłym zapisie dawki. Otwórz, weź tabletkę,
   zamknij i zostaw je w spokoju na minutę.
3. Zanim zacznie pobierać, **piknie dwa razy**. Potem milczy przez około
   minutę i restartuje się — to nie jest zawieszenie.
   Po restarcie, gdy nowa wersja wstanie i przejdzie całą swoją drogę,
   zagra **fanfarę w górę** (sześć rosnących tonów i jeden długi). To jest
   słyszalny dowód, że aktualizacja się udała — nie da się jej pomylić
   z przypomnieniem o leku (krótkie, powtarzalne) ani z ostrzeżeniem
   „już dziś brałeś" (opadające). **Cisza po restarcie też coś znaczy:**
   wersja, która nie wstała, nigdy do tej fanfary nie dojdzie i po trzech
   próbach pudełko samo wróci do poprzedniej.
4. Dawka zapisuje się **przed** aktualizacją, zawsze. Jeśli w kolejce wiszą
   niewysłane dawki, pudełko odłoży aktualizację, dopóki nie dojadą.

Ekran w aplikacji pokazuje, co się dzieje — w tym wtedy, gdy **nie** wychodzi:
czy pudełko jeszcze się nie łączyło, czy łączyło się i odmówiło (i dlaczego),
ile było nieudanych prób i czy przestało próbować.

---

## Gdyby coś poszło nie tak

**Pudełko nie łączy się z aplikacją po wgraniu.**
Najpewniej hasło w `config.h` było błędne albo puste. Wgraj ponownie
z poprawnym. Pamięć pudełka nadal jest pusta, więc nic nie przykrywa.

**Nowa wersja nie wstaje po aktualizacji przez WiFi.**
Pudełko liczy własne starty i po trzech nieudanych **samo wraca na poprzednią
wersję**, a tamtej już nigdy nie pobierze. Aplikacja to pokaże. Nie musisz nic
robić — poza wypuszczeniem poprawionej wersji.

**Pamięć pudełka została skasowana** (przypadkowe „Erase All Flash").
Nie potrzebujesz kabla. Przytrzymaj przycisk przy resecie — pudełko postawi
własną sieć **PillBox-setup** (hasło `pillbox123`). Na stronie, która się
otworzy, poza siecią WiFi pojawi się wtedy dodatkowe pole **„Hasło urządzenia
(Firebase)"**. To pole widać **tylko wtedy**, gdy pudełko naprawdę nie ma
hasła — jeśli go nie widzisz, znaczy że hasło jest na miejscu.
