# PillBox — instrukcje dla Claude Code

Inteligentne pudełko na leki (Seeed XIAO ESP32-C3) + PWA na iPhone.
Użytkownik: **Kuba**, Warfin 5 mg raz dziennie o 20:00. Lek przeciwzakrzepowy —
pominięta albo podwójna dawka to nie jest drobiazg.

**Rozmawiaj po polsku.**

> **Przeczytaj `PROJEKT-PillBox-kontekst.md`** przed pierwszą zmianą.
> Są tam przyczyny dziewięciu błędów, które kosztowały godziny szukania.
> Nie cofaj poprawki, nie znając powodu jej powstania.
>
> **Przeczytaj `DECYZJE.md`** — dziennik decyzji: co jest tymczasowe i kiedy
> to usunąć, co zostało zrobione dlaczego, co już raz **cofnięto** i czego
> nie próbować drugi raz. Każdą własną decyzję dopisz tam od razu, nie potem.

---

## Zanim cokolwiek zmienisz

```bash
bash tests/run_all.sh
```

Musi przejść przed zmianą i po zmianie. Stan wyjściowy:
**220 + 51 firmware, 482 + 81 aplikacja (×6 pór doby), 17 zgodności,
190 kontroli audytu — 0 błędów.**

Testy pracują na **prawdziwym kodzie**, nie na kopii: `tests/extract.py` wycina
funkcje z `PillBox.ino`, `tests/build_app_module.mjs` buduje moduł z `index.html`.
Jeśli zmieniasz nazwę wyciąganej funkcji, popraw też `extract.py`.

---

## Twarde ograniczenia — NIE ŁAMAĆ

1. **Firmware to dokładnie dwa pliki**: `PillBox.ino` + `config.h`. Scalanie odrzucone.
2. **Żadnych zmian sprzętowych.** Płytka jest zlutowana i docelowo zaklejona.
3. **`config.h` JEST w repo — celowo, i tak ma zostać.**
   Trzyma wyłącznie placeholder `TUTAJ_WPISZ_HASLO`, nigdy prawdziwego hasła.
   Powód: Kuba pracuje tak, że pobiera folder `firmware/` z GitHuba i otwiera
   go wprost w Arduino IDE. Bez `config.h` w komplecie szkic się nie otwiera,
   a on musi zmieniać nazwy plików na telefonie albo MacBooku przed wyjazdem.
   Krótka próba trzymania tu tylko `config.example.h` **została cofnięta na
   jego wyraźną prośbę** — nie przywracaj jej.
   Prawdziwe hasło żyje wyłącznie na jego dysku i **nigdy nie wraca do repo**.
4. **`DAY_START_HOUR = 3`** identycznie w firmware i aplikacji.
5. **Każdy zapis użytkownika w aplikacji idzie przez `zapiszPewnie()`.**
   Nigdy gołe `set()`. Firebase offline nie odrzuca obietnicy, tylko wisi —
   ekran pokazuje sukces, dane nie docierają. Test tego pilnuje.
6. **Nic nie kasujemy z pamięci pudełka przed potwierdzonym 2xx.**
   Kolejka, flagi statusu, dziennik wieczka. Rodzina błędu 3.5.
7. **Do gałęzi `events` nie dokładamy pól.** Reguła `$other: false` odrzuca
   **cały** wpis, gdy trafi w nim nieznane pole — czyli otwarcie pudełka
   przepada w całości. Nowe dane idą do nowej gałęzi.
8. **Narzędzie diagnostyczne nie może uszkodzić danych o leku.**
   Stąd dziennik wieczka ma własny bufor zamiast kolejki dawek.

Blok pomiaru napięcia **wolno** zmieniać (zakaz zniesiony). Audyt nie blokuje —
zgłasza tylko uwagę, żeby zmiana przypadkowa nie wyglądała jak świadoma.

---

## Struktura

```
firmware/PillBox/PillBox.ino     główny kod (~2600 linii)
firmware/PillBox/config.h        ustawienia (w repo, bez hasła)
firmware/PillBoxTest/            osobny szkic diagnostyczny
index.html                   cała PWA w jednym pliku
sw.js, tabletka.gif      service worker + tabletka na ekranie głównym
tests/                           testy + audyt
database.rules.json              reguły Firebase
```

Po zmianie w plikach aplikacji **podbij `APP_VERSION` i `CACHE` w `sw.js`** — inaczej
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
