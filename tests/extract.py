#!/usr/bin/env python3
"""Wycina wskazane funkcje z PillBox.ino do logic.inc, zeby testy
uruchamialy sie na PRAWDZIWYM kodzie, a nie na jego kopii."""
import re, sys, pathlib

SRC  = pathlib.Path(__file__).parent.parent / "firmware" / "PillBox" / "PillBox.ino"
DEST = pathlib.Path(__file__).parent / "logic.inc"

WANTED = [
    # Pomocniki NVS musza byc PRZED wszystkim, co z nich korzysta.
    # zanotujNvsFail przed nvsPutStr/nvsPutU16 - to one je woluja.
    "zanotujNvsFail", "nvsPutStr", "nvsPutU16",
    "readBatteryRaw", "battPercentFromCurve", "resetBatteryFilter",
    "battSmooth", "readBattery", "zapiszKoniecLadowania", "trackCharging",
    "parseSchedule", "slotMinutes", "localDayNumber", "localMinutesOfDay",
    # Dni bez leku - musza byc przed checkDayRollover, ktore je wola.
    "localWeekday", "dateKeyToNum", "dawkaNaDobe", "dzisBezLeku",
    "parseDoseWeek", "parseDoseEx", "saveDosing",
    "matchSlot", "secondsToDayBoundary", "secondsToNextSlot",
    "openWarnSecondsLeft",
    "jsonEscape",   # przed lidLogJson - ta go wola
    "wakeName", "lidLogAdd", "lidLogCount", "lidLogJson", "lidLogClear",
    # Historia nieudanych zapisow NVS - tez wola jsonEscape.
    "nvsFailLogDoWyslania", "nvsFailLogJson", "nvsFailLogOznaczWyslany",
    "trackBoxOpen",
    # awakeTooLong musi byc przed flushQueue, ktore go wola.
    "awakeTooLong", "extendAwake",
    # Lista znanych sieci WiFi. netKlucz musi byc pierwszy - reszta go wola.
    "netKlucz", "wifiSieciCount", "wifiSiecSsid", "wifiSiecPass",
    "wifiListeZapisz", "wifiListeCzytaj",
    "wifiSiecDodaj", "wifiSiecUsun", "wifiSiecPriorytet",
    "tokenZPamieci", "zapomnijToken",
    # Haslo urzadzenia w pamieci trwalej i decyzja o aktualizacji (D59).
    # hasloJestPrawdziwe musi byc pierwsze - reszta go wola.
    "hasloJestPrawdziwe", "hasloZPamieci", "hasloWPamieci", "hasloUtrwal",
    "hasloDoLogowania", "otaDecyzja", "otaOpisDecyzji",
    "przesunZnaczniki", "queuePush", "queueCount", "queuePeek", "queuePop", "queueDrop",
    "queueShiftTimestamps", "rekordKompletny", "trwaleOdrzucony", "flushQueue",
    "makeRecordAt", "makeRecord",
    "loadDayMarkers", "setTakenDay", "setRolloverDay",
    "zapiszDawke", "oznaczAlarmObsluzony", "alarmJuzObsluzony", "juzDzisBrane",
    "ostatniSlotDoby",
    "checkDayRollover",
    "wartoZapisac", "logbookAdd", "logbookJson",
    "alarmPotwierdzony",
    # planNextSleep na koncu - wola prawie wszystko powyzej.
    "planNextSleep",
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
