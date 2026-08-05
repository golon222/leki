# PillBox — instrukcje dla Claude Code

Inteligentne pudełko na leki (Seeed XIAO ESP32-C3) + PWA na iPhone.
Użytkownik: **Kuba**, Warfin 5 mg raz dziennie o 20:00. Lek przeciwzakrzepowy —
pominięta albo podwójna dawka to nie jest drobiazg.

**Rozmawiaj po polsku.**

> **Przeczytaj `PROJEKT-PillBox-kontekst.md`** przed pierwszą zmianą.
> Są tam przyczyny dziewięciu błędów, które kosztowały godziny szukania.
> Nie cofaj poprawki, nie znając powodu jej powstania.

---

## Zanim cokolwiek zmienisz

```bash
bash tests/run_all.sh
```

Musi przejść przed zmianą i po zmianie. Stan wyjściowy:
**201 + 51 firmware, 442 + 81 aplikacja (×6 pór doby), 17 zgodności,
190 kontroli audytu — 0 błędów.**

Testy pracują na **prawdziwym kodzie**, nie na kopii: `tests/extract.py` wycina
funkcje z `PillBox.ino`, `tests/build_app_module.mjs` buduje moduł z `index.html`.
Jeśli zmieniasz nazwę wyciąganej funkcji, popraw też `extract.py`.

---

## Twarde ograniczenia — NIE ŁAMAĆ

1. **Firmware to dokładnie dwa pliki**: `PillBox.ino` + `config.h`. Scalanie odrzucone.
2. **Żadnych zmian sprzętowych.** Płytka jest zlutowana i docelowo zaklejona.
3. **`config.h` nigdy w repo** — jest w `.gitignore`. Wzorzec: `config.example.h`.
4. **Blok pomiaru napięcia zostaje dosłownie taki, jaki jest**
   (`CALIBRATION_FACTOR = 0.921`). Wolno zmieniać tylko przeliczenie na procent.
5. **`DAY_START_HOUR = 3`** identycznie w firmware i aplikacji.

---

## Struktura

```
firmware/PillBox/PillBox.ino     główny kod (~2600 linii)
firmware/PillBox/config.example.h  ustawienia — skopiuj do config.h
firmware/PillBoxTest/            osobny szkic diagnostyczny
app/index.html                   cała PWA w jednym pliku
app/sw.js, app/tabletka.gif      service worker + tabletka na ekranie głównym
tests/                           testy + audyt
database.rules.json              reguły Firebase
```

Po zmianie w `app/` **podbij `APP_VERSION` i `CACHE` w `sw.js`** — inaczej
telefon zostanie na starej wersji. Po zmianie firmware podbij `FW_VERSION`.

---

## Czego nie zweryfikowano

- **Firmware nigdy nie było kompilowane** toolchainem Arduino — testy kompilują
  wycięte funkcje na zaślepkach. Nie twierdź, że „się skompiluje".
- Prąd ładowania 350 mA to wartość katalogowa, nie pomiar.

---

## Jak pracować z Kubą

- Konkretnie, bez asekuracji. Po polsku.
- **Jego opisy objawów są cenniejsze niż twoje hipotezy.** W poprzedniej sesji
  kilka razy naprawiano rzeczy, które nie były przyczyną; przełomem zawsze był
  jego konkretny opis z liczbami.
- Gdy coś jest niepewne — powiedz wprost, że to hipoteza, i **dołóż pomiar**
  zamiast zgadywać kolejny raz.
- Powiadomienia push: czeka na hasło **„dawaj kod"**. Rekomendacja: bot Telegram.
