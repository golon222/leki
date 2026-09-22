#!/usr/bin/env bash
# =====================================================================
#  Odcisk ZRODLA, z ktorego powstaje PillBox.bin.
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
#  nie ma prawa kazac pudelku sciagac 1,2 MB przez WiFi.
# =====================================================================
set -e
cd "$(dirname "$0")/.."
{
  cat firmware/PillBox/PillBox.ino firmware/PillBox/config.h
  grep -E '^(CORE_VER|PART|FQBN)=|--branch v' tests/kompiluj_firmware.sh
} | sha256sum | cut -d' ' -f1
