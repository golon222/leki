# PillBox — pełny kontekst projektu

**Dokument przekazania.** Wgraj go do wiedzy projektu w Claude razem z plikami
z katalogu `firmware/` i `app/`. Wtedy Claude na dowolnym urządzeniu wie to,
co wiedział Claude prowadzący ten projekt — łącznie z przyczynami decyzji,
które kosztowały godziny szukania.

Stan na: **5 sierpnia 2026** · firmware **1.21.1** · aplikacja **2026-08-05.3**

---

## 1. Co to jest

Inteligentne pudełko na leki dla Kuby. Warfin 5 mg, **raz dziennie**,
przypomnienie domyślnie o 20:00. Aplikacja śledzi też wyniki INR — to lek
przeciwzakrzepowy, więc pominięta albo podwójna dawka to nie jest drobiazg.

**Sprzęt** (zlutowany, docelowo zaklejony — nie do zmiany):

| element | pin | uwagi |
|---|---|---|
| Seeed XIAO ESP32-C3 | — | ogniwo LiPo 470 mAh |
| pomiar baterii | D0 / GPIO2 | dzielnik 100k/100k, ADC1_CH2 |
| kontaktron | D1 / GPIO3 | do GND, podciągnięcie do 3V3; **otwarte = HIGH** |
| buzzer piezo | D2 / GPIO4 | pasywny, LEDC PWM, rezonans ~2700 Hz |
| ukryty przycisk | D3 / GPIO5 | do GND; **wciśnięty = LOW** |

**Aplikacja**: PWA na iPhone, GitHub Pages + Firebase Realtime Database.
Bez konta Apple Developer, wszystko darmowe.

---

## 2. Twarde ograniczenia — NIE ŁAMAĆ

1. **Firmware to dokładnie dwa pliki**: `PillBox.ino` + `config.h`.
   Scalanie zostało wprost odrzucone: *„nie nie zostanimy przy dwoch plikach"*.
2. **Żadnych zmian sprzętowych**: *„ja nie będę zmieniał ani dodawał rezystorów"*.
   Każde rozwiązanie musi działać na powyższym schemacie.
3. **`config.h` nigdy nie trafia na GitHuba** — zawiera `DEVICE_PASSWORD`.
   Na stronę idzie wyłącznie zawartość `app/`.
4. **Blok pomiaru napięcia zostaje dosłownie taki, jaki jest**:
   `CALIBRATION_FACTOR = 0.921`, `(rawValue / 4095.0) * 3.3`, `pinVoltage * 2.0`.
   Wolno było zmienić wyłącznie przeliczenie napięcia na procent. Audyt tego pilnuje.
5. **`DAY_START_HOUR = 3`** musi być identyczne w firmware i w aplikacji.
   Osobny test przepuszcza ~177 000 znaczników czasu przez kod obu stron.

---

## 3. Błędy, które kosztowały najwięcej — i ich przyczyny

To jest najcenniejsza część dokumentu. Każdy z tych błędów wyglądał z zewnątrz
zupełnie inaczej, niż wyglądał w kodzie.

### 3.1 Kontaktron nie budził pudełka
Pierwsza teoria (nadpisywanie maski wybudzania) była **błędna**.
Prawdziwa przyczyna: [espressif/esp-idf#12183](https://github.com/espressif/esp-idf/issues/12183) —
`esp_deep_sleep_start()` sam włącza wewnętrzne podciągnięcie ~10 kΩ **do masy**
na pinie czekającym na stan wysoki. Wygrywa z zewnętrznymi 100 kΩ, pin siedzi
na ~0,4 V i wybudzenie nigdy nie następuje.

Obejście: `gpio_hold_en(pin)` + `gpio_deep_sleep_hold_en()` **przed** snem,
zwolnienie przez `gpio_deep_sleep_hold_dis()` + `gpio_hold_dis(pin)` **po**
wybudzeniu. Potwierdzone działanie na sprzęcie.

### 3.2 „Synchronizacja się psuje" — ścieżka bez radia
Ścieżka *„już dziś brałeś"* (powtórne otwarcie w ciągu doby) **w ogóle nie
włączała WiFi**: pikała i szła czekać na zamknięcie. Pełny status leciał dopiero
po powrocie z czekania, czyli po zamknięciu wieczka. Użytkownik otwierał pudełko,
patrzył w telefon dwie minuty i nie widział nic — bo pudełko było **ciche**, nie wolne.

Naprawa: `pushLidState()` wywoływane na **wejściu** do `czekajNaZamkniecieIGest()`,
z każdej ścieżki, warunkowane `!rtcOpenReported`.

### 3.3 Każde zapytanie płaciło własne uzgodnienie TLS
`rtdbSend()` tworzył nowego `WiFiClientSecure` przy każdym wywołaniu. Uzgodnienie
TLS na ESP32-C3 to ~1–2 s, a jedno otwarcie pudełka to sześć zapytań.
Naprawa: jeden globalny `rtdbClient` na całe wybudzenie + `http.setReuse(true)`.

### 3.4 Token Firebase kasowany przez deep sleep
`idToken` był zwykłą zmienną. Przy kilku wybudzeniach dziennie bez znaczenia —
ale podgląd ładowania budzi co minutę, co dałoby **1440 logowań hasłem na dobę**.
Google potrafi za to czasowo zablokować konto. Token siedzi teraz w NVS razem
z czasem ważności (`rtcTokenExp`), odświeżany z zapasem `TOKEN_MARGIN_S`,
kasowany przy HTTP 401/403.

### 3.5 `pushStatus()` kłamał sam sobie
`rtcOpenReported` ustawiane **przed** wysyłką i bez patrzenia na wynik.
Nieudany pakiet oznaczał „aplikacja już wie" — i ponowienia nie było **nigdy**.
Teraz sukces = potwierdzony kod 2xx, porażka zostawia `rtcStatusDirty`,
a `planNextSleep()` planuje wcześniejsze wybudzenie.

### 3.6 Korekta zegara obejmowała tylko kolejkę
NTP potrafi przesunąć zegar o godziny. `queueShiftTimestamps()` poprawiało
zdarzenia — ale nie pięciu znaczników w RTC. Najgroźniejszy `rtcNextWarnTs`:
skok w przód wyrzucał termin sygnału w przeszłość i pudełko piszczałoby bez
powodu. Naprawa: `przesunZnaczniki()` wołane razem z korektą kolejki.

### 3.7 Ładowanie wykrywane tylko po wzroście napięcia
Zakładało pomiar przed podłączeniem kabla i po nim. Przy wgrywaniu firmware
z podpiętym kablem pamięć RTC jest kasowana, pierwszy pomiar to już 4,2 V
i wzrostu **nie będzie nigdy**. Dodany drugi dowód: dwa wysokie odczyty z rzędu
(≥ `CHARGE_FULL_V`), z blokadą `rtcBlokWysokie` przeciw migotaniu po odpięciu.

### 3.8 Procentu naładowania na kablu NIE DA SIĘ zmierzyć
Układ ładujący trzyma na ogniwie swoje napięcie niezależnie od stanu
naładowania. Każdy odczyt wychodzi 100%. Rozwiązanie: podczas ładowania
aplikacja pokazuje **wyszarzony odczyt sprzed ładowania** plus szacowany czas
do pełna liczony z `chargeSince` i `chargeFromPct`. Prawdziwy pomiar następuje
`CHARGE_SETTLE_S` = 20 s po odłączeniu kabla.

### 3.9 Audyt sprawdzał tekst, którego w kodzie nie ma — DWA RAZY
- Wzorzec na literały znakowe `'(\\.|[^'\\])*'` połykał wszystko od
  `autocapitalize='off'` w HTML portalu. Audyt cicho przepuszczał błędy.
  Naprawa: literał znakowy to **dokładnie jeden** znak.
- Wycinanie ciała funkcji trafiało w **deklarację zapowiadającą**
  (`bool syncTimeNTP();`) zamiast w definicję. Naprawa: funkcja `cialo()`
  szukająca nagłówka zakończonego `{`.

Morał: narzędziu weryfikującemu też trzeba nie ufać.

---

## 4. Zachowanie pudełka — stan obecny

- **Otwarcie** → natychmiastowy sygnał *„już dziś brałeś"* (jeśli dawka wzięta),
  wysyłany **przed** startem portu szeregowego i pomiarem baterii.
  `loadDayMarkers()` musi być wywołane wcześniej — po resecie RTC jest puste.
- **Przy otwartym wieczku pudełko czuwa**, nie śpi. Radio zostaje włączone przez
  `RADIO_OTWARTE_S` = 600 s, potem `wifiUspij()` (**nie** `wifiOff()` — `esp_wifi_stop()`
  ubija sterownik i powrót bywa zawodny). Łącze podtrzymywane co 5 s.
- **Zamknięcie** → natychmiastowy `pushLidState()` (PATCH, trzy pola),
  potem pełny `pushStatus()`. Czas mierzony i zapisywany jako `zamkn->wyslane N ms`.
- **Po 15 minutach** otwarcia → pierwszy sygnał, potem co 30 min (`OPEN_WARN_*`).
- **Na ładowarce** pudełko czuwa bez usypiania do 6 h (`CHARGE_AWAKE_MAX_S`),
  co 30 s mierzy i **pobiera ustawienia** (zmiana godziny wchodzi w ~30 s).
  Otwarcie wieczka albo pora dawki → `goToSleep(1)`, czyli oddanie sterowania
  normalnej ścieżce (kontaktron stoi w stanie wybudzenia → natychmiastowy WAKE_REED).
- **Autotest**: trzy kliknięcia przyciskiem. 1 piknięcie (= dowód, że buzzer działa)
  → ruch wieczkiem, 20 s → 2 piknięcia → reszta po cichu (bateria, pamięć, WiFi,
  zegar, baza). Przycisku **nie testujemy** — uruchomienie testu jest jego dowodem.
  Jedyny sygnał błędu to łagodne pulsowanie 5 s = brak sieci.
- **Portal WiFi**: przytrzymanie przycisku. Kończy się **zamknięciem wieczka**.

---

## 5. Aplikacja — stan obecny

- **Kolejka zapisów** w `localStorage`: każdy zapis użytkownika trafia najpierw
  do kolejki, znika po potwierdzeniu. Firebase offline **nie odrzuca** obietnicy,
  tylko trzyma ją w nieskończoność — stąd wyścig z 8-sekundowym limitem.
  Ustawienia zapisywane **polami**, żeby ponowienie nie skasowało cudzej zmiany.
- **Siatka bezpieczeństwa**: raz na minutę `brakujePokrycia()` sprawdza lokalnie,
  czy któreś otwarcie z pudełka nie ma wpisu w kalendarzu. Nie trzeba wiedzieć,
  co zawiodło — skutek naprawia się sam w ciągu minuty.
- **Ręczne wpisy wygrywają zawsze** (`cur.source === "manual"` → `continue`).
- **Otwarcia biją pominięcia**, a wśród otwarć wygrywa najwcześniejsze.
- Dni przed `trackingSince` to **brak danych**, nie „nie wzięte".
- Tabletka na ekranie głównym to `tabletka.gif` (58 px), wyszarzona do czasu dawki.

---

## 6. Testy — jak są zbudowane

**Zasada: testy pracują na PRAWDZIWYM kodzie, nie na kopii.**

- `tests/extract.py` wycina wskazane funkcje z `PillBox.ino` do `logic.inc`
  i kompiluje je g++ na zaślepkach Arduino (`arduino_shim.h`).
- `tests/build_app_module.mjs` buduje moduł z `index.html` i uruchamia go
  na zaślepkach Firebase/DOM.
- `tests/audit_firmware.py` — kontrole strukturalne: kolejność definicji,
  parowanie `prefs.begin/end`, pętle bez ogranicznika, kolejność operacji
  w `setup()`.
- `tests/crosscheck_days.cpp` + `test_crosscheck.mjs` — zgodność liczenia dób.
- `bash tests/run_all.sh` uruchamia całość.

**Stan: 201 + 51 firmware, 442 + 81 aplikacja (×6 pór doby), 17 zgodności,
190 kontroli audytu — 0 błędów.**

---

## 7. Czego NIE zweryfikowano

Uczciwie, bo to ma znaczenie przy ocenie ryzyka:

- **Firmware nigdy nie było skompilowane** toolchainem Arduino. Testy kompilują
  wycięte funkcje na zaślepkach. Pierwsza kompilacja u użytkownika jest pierwszą prawdziwą.
- **Prąd ładowania 350 mA** to wartość katalogowa XIAO, nie pomiar. Szacunek czasu
  ładowania (`minutyDoPelna`) opiera się na niej i na założeniu ~250 mA netto.
- **Model wykrywania ładowania** testowany na przebiegach, które sam wymyśliłem
  z tych samych założeń, z których napisałem kod. To realna cykliczność.

---

## 8. Otwarte sprawy

1. **Powiadomienia push na telefon** — odłożone do hasła **„dawaj kod"**.
   Rekomendacja: bot Telegram (jedno zapytanie HTTP z pudełka, darmowe,
   natywne powiadomienia na iPhonie).
2. **Tanie wybudzenie kontrolne co 30 min** bez włączania radia, żeby
   wykrywać podłączenie ładowarki szybciej niż po 12 h. Zaproponowane,
   **czeka na decyzję** — dokłada warunek obok ścieżki alarmu o dawce,
   czyli w najbardziej wrażliwym miejscu w całym kodzie.
3. **Gesty przyciskiem nie działają** podczas ładowania i podczas portalu.
   Świadomy kompromis.
4. **Reguły bazy** (`database.rules.json`) wymagają ponownej publikacji
   po dodaniu pól `tz` i `trackingSince`.
5. **Log z autotestu** — użytkownik zgłaszał przerywanie testu; w 1.21.1 dodano
   `etapTestu()` logujące każdy etap z czasem. Czeka na log z monitora portu.

---

## 9. Jak rozmawiać z Kubą o tym projekcie

- Pisze po polsku, bezpośrednio, często skrótowo. Oczekuje konkretów, nie asekuracji.
- **Jego opisy objawów są cenniejsze niż moje hipotezy.** Kilka razy w tej sesji
  naprawiałem rzeczy, które nie były przyczyną; przełomem zawsze był jego
  konkretny opis z liczbami („2,5 minuty", „40 sekund").
- Gdy coś jest niepewne — powiedzieć wprost, że to hipoteza, i dołożyć pomiar.
- Nie zgadywać po raz kolejny, gdy poprzednie zgadywanie zawiodło. Lepiej
  poprosić o log albo zbudować mechanizm, który naprawia się sam.
