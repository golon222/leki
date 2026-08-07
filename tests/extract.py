#!/usr/bin/env python3
"""Wycina wskazane funkcje z PillBox.ino do logic.inc, zeby testy
uruchamialy sie na PRAWDZIWYM kodzie, a nie na jego kopii."""
import re, sys, pathlib

SRC  = pathlib.Path(__file__).parent.parent / "firmware" / "PillBox" / "PillBox.ino"
DEST = pathlib.Path(__file__).parent / "logic.inc"

WANTED = [
    # Pomocniki NVS musza byc PRZED wszystkim, co z nich korzysta.
    "nvsPutStr", "nvsPutU16",
    "readBatteryRaw", "battPercentFromCurve", "resetBatteryFilter",
    "battSmooth", "readBattery", "zapiszKoniecLadowania", "trackCharging",
    "parseSchedule", "slotMinutes", "localDayNumber", "localMinutesOfDay",
    "matchSlot", "secondsToDayBoundary", "secondsToNextSlot",
    "openWarnSecondsLeft",
    "wakeName", "lidLogAdd", "lidLogCount", "lidLogJson", "lidLogClear",
    "trackBoxOpen",
    # awakeTooLong musi byc przed flushQueue, ktore go wola.
    "awakeTooLong", "extendAwake",
    "przesunZnaczniki", "queuePush", "queueCount", "queuePeek", "queuePop", "queueDrop",
    "queueShiftTimestamps", "rekordKompletny", "trwaleOdrzucony", "flushQueue",
    "makeRecordAt", "makeRecord",
    "loadDayMarkers", "setTakenDay", "setRolloverDay",
    "checkDayRollover",
    "wartoZapisac", "logbookAdd", "jsonEscape", "logbookJson",
]

def grab(src, name):
    m = re.search(r"^[A-Za-z_][\w:<>\* ]*\b" + name + r"\s*\([^;{]*\)\s*\{", src, re.M)
    if not m:
        sys.exit(f"BLAD: nie znaleziono funkcji {name}")
    i, depth = m.end() - 1, 0
    while i < len(src):
        if src[i] == "{": depth += 1
        elif src[i] == "}":
            depth -= 1
            if depth == 0: return src[m.start():i+1]
        i += 1
    sys.exit(f"BLAD: niezbalansowane nawiasy w {name}")

src = SRC.read_text(encoding="utf-8")

# Bloki oznaczone znacznikami - dla rzeczy, ktore nie sa funkcjami
# (struktury, tablice stalych). Musza trafic do testow przed funkcjami,
# ktore z nich korzystaja.
blocks = re.findall(r"/\* @extract-begin \*/(.*?)/\* @extract-end \*/", src, re.S)

out = ["/* WYGENEROWANE przez extract.py - nie edytowac recznie */\n"]
out.extend(b.strip() + "\n" for b in blocks)
for n in WANTED:
    out.append(grab(src, n) + "\n")
DEST.write_text("\n".join(out), encoding="utf-8")
print(f"Wyciagnieto {len(WANTED)} funkcji i {len(blocks)} blokow do {DEST.name} "
      f"({len(DEST.read_text(encoding='utf-8').splitlines())} linii)")
