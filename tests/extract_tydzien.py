#!/usr/bin/env python3
"""Wycina funkcje z PillBoxWeek.ino do logic_tydzien.inc.

Pudelko tygodniowe nie mialo ANI JEDNEGO testu jednostkowego do 0.15.0 -
stala za nim tylko kontrola statyczna, ktora czyta kod, a nie uruchamia go.
Kosztowalo to B31 i D145: dwa bledy w logice, ktora da sie sprawdzic
na biurku w milisekundy.

Tak samo jak extract.py dla pudelka dziennego: testy chodza na PRAWDZIWYM
kodzie, a nie na jego kopii. Zmieniasz nazwe funkcji - popraw tutaj.
"""
import re, sys, pathlib

SRC  = pathlib.Path(__file__).parent.parent / "firmware" / "PillBoxWeek" / "PillBoxWeek.ino"
DEST = pathlib.Path(__file__).parent / "logic_tydzien.inc"

WANTED = [
    # Drabinka: ktora klapka, kilka naraz, wszystko zamkniete.
    "ktoraKomora", "opisKomory",
    # Doba lekowa - granica DAY_START_HOUR, ta sama co w aplikacji.
    "dniOdEry", "numerDoby", "poczatekDoby",
    # Przypomnienia. minutyDnia musi byc przed slotTeraz - ono go wola.
    "minutyDnia", "slotTeraz",
    # Kolejka (kolejkaIle wola nvs) - potrzebna sekundomDoNastepnego.
    "kolejkaIle",
    "sekundDoNastepnego",
]


def grab(src, name):
    m = re.search(r"^[A-Za-z_][\w:<>\* ]*\b" + name + r"\s*\([^;{]*\)\s*\{", src, re.M)
    if not m:
        sys.exit(f"BLAD: nie znaleziono funkcji {name} w PillBoxWeek.ino")
    i, depth = m.end() - 1, 0
    while i < len(src):
        if src[i] == "{":
            depth += 1
        elif src[i] == "}":
            depth -= 1
            if depth == 0:
                return src[m.start():i + 1]
        i += 1
    sys.exit(f"BLAD: niezbalansowane nawiasy w {name}")


src = SRC.read_text(encoding="utf-8")
out = ["/* WYGENEROWANE przez extract_tydzien.py - nie edytowac recznie */\n"]
for n in WANTED:
    out.append(grab(src, n) + "\n")
DEST.write_text("\n".join(out), encoding="utf-8")
print(f"Wyciagnieto {len(WANTED)} funkcji pudelka tygodniowego do {DEST.name} "
      f"({len(DEST.read_text(encoding='utf-8').splitlines())} linii)")
