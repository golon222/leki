#!/usr/bin/env bash
# =====================================================================
#  Odcisk ZRODLA, z ktorego powstaje binarka do aktualizacji przez WiFi.
#
#  Uzycie:  bash tests/zrodlo_firmware.sh [dzienne|tygodniowe]
#           (bez argumentu: dzienne - tak, jak bylo)
#
#  PO CO TO ISTNIEJE
#  -----------------
#  Binarka NIE MOZE byc powtarzalna i to nie jest do naprawienia:
#  rdzen ESP32 wkompilowuje w nia __DATE__ i __TIME__ ("Compile
#  Date/Time" w Esp.cpp), a do naglowka obrazu trafia `app_elf_sha256`,
#  czyli odcisk pliku ELF. Dwie kompilacje tego samego zrodla w rozne
#  dni roznia sie wiec zawsze - zmierzone, nie przypuszczone (D123).
#
#  Dlatego automat nie pyta "czy binarka jest inna", tylko "czy jest
#  z czego zrobic inna binarke". Odpowiedz daje ten odcisk.
#
#  Co wchodzi do sumy: oba pliki szkicu i te linie skryptu kompilacji,
#  ktore zmieniaja WYNIK - wersja rdzenia, podzial pamieci, FQBN
#  i wersja ArduinoJson. Komentarz dopisany w kompiluj_firmware.sh
#  nie ma prawa kazac pudelku sciagac megabajta przez WiFi.
#
#  KAZDE PUDELKO MA WLASNY ODCISK i to jest cala tresc tego argumentu.
#  Gdyby byl jeden wspolny, poprawka w pudelku dziennym kazalaby
#  pudelku dziewczyny sciagnac swoja binarke bez powodu - i odwrotnie.
# =====================================================================
set -e
cd "$(dirname "$0")/.."

case "${1:-dzienne}" in
  dzienne)    KATALOG="firmware/PillBox";     SZKIC="PillBox.ino" ;;
  tygodniowe) KATALOG="firmware/PillBoxWeek"; SZKIC="PillBoxWeek.ino" ;;
  *) echo "uzycie: $0 [dzienne|tygodniowe]" >&2; exit 2 ;;
esac

{
  cat "$KATALOG/$SZKIC" "$KATALOG/config.h"
  grep -E '^(CORE_VER|PART|FQBN)=|--branch v' tests/kompiluj_firmware.sh
} | sha256sum | cut -d' ' -f1
