#!/usr/bin/env bash
# =====================================================================
#  PRAWDZIWA KOMPILACJA FIRMWARE toolchainem Arduino
#
#  Do tej pory w CLAUDE.md stalo "firmware nigdy nie bylo kompilowane".
#  Testy sprawdzaja wyciete funkcje na zaslepkach - to lapie bledy
#  logiki, ale nie zlapie literowki w nazwie z Arduino API, zlego typu
#  w wywolaniu esp_sleep_* ani funkcji, ktora w rdzeniu 3.x zmienila
#  sygnature. Ten skrypt lapie.
#
#  NIE jest czescia run_all.sh - wymaga sieci i ~500 MB toolchainu.
#  Uruchamiasz go, gdy zmieniasz cokolwiek w firmware.
#
#      bash tests/kompiluj_firmware.sh
#
#  Pierwsze uruchomienie sciaga toolchain (kilka minut), kolejne sa szybkie.
# =====================================================================
set -e
cd "$(dirname "$0")/.."
ROOT="$(pwd)"

# Gdy ustawione, gotowa binarka PillBox.ino lezy po kompilacji w tym
# katalogu. Uzywa tego automat na GitHubie (.github/workflows/firmware.yml),
# zeby plik do aktualizacji przez WiFi powstal z DOKLADNIE tymi ustawieniami
# plytki co reszta skryptu - jedno zrodlo prawdy zamiast drugiej kopii.
OTA_OUT="${OTA_OUT:-}"

ACLI_DIR="${ACLI_DIR:-$HOME/.pillbox-arduino}"
ACLI="$ACLI_DIR/arduino-cli"
CORE_VER="${CORE_VER:-3.3.11}"        # ta wersja siedzi w pudelku

# USTAWIENIA PLYTKI - musza byc DOKLADNIE te, ktore Kuba wybiera w Arduino IDE.
#
#   Kompilacja na domyslnych ustawieniach mierzy inna rzecz niz to, co
#   naprawde ląduje w pudelku. Domyslny podzial pamieci daje programowi
#   1,2 MB, a Kuba wgrywa z podzialem "Huge APP" - 3 MB. Ten sam plik to
#   raz 92% zajetosci, raz 38%. Pierwsza liczba wyglada jak alarm, ktorego
#   nie ma.
#
#   Nazwy opcji sa mylace i latwo je wziac odwrotnie:
#     CDCOnBoot=default  ->  USB CDC WLACZONE   (tak ma byc)
#     CDCOnBoot=cdc      ->  USB CDC WYLACZONE
#   Zrodlo prawdy to naglowek PillBox.ino, ktory te ustawienia wypisuje.
#   Ponizej sprawdzamy, czy nadal sie zgadzaja.
#   Od 1.38.0 podzial to min_spiffs, a nie huge_app (D59): aktualizacja
#   przez WiFi potrzebuje DWOCH partycji programu, a huge_app ma jedna.
#   Program ma teraz do dyspozycji 1,875 MB zamiast 3 MB, wiec ten skrypt
#   jest jedynym miejscem, ktore zauwazy, gdy firmware zacznie sie w tym
#   nie miescic.
PART="${PART:-min_spiffs}"
PART_OPIS="Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)"
FQBN="esp32:esp32:XIAO_ESP32C3:PartitionScheme=$PART,CDCOnBoot=default"

# OBA szkice, nie tylko glowny. Naglowek PillBoxTest.ino zapowiadal
# "Huge APP" jeszcze dlugo po tym, jak projekt przeszedl na min_spiffs -
# czyli mowil Kubie co innego, niz ten skrypt buduje i sprawdza. Szkic
# diagnostyczny wgrywa sie recznie, wiec to jego naglowek jest instrukcja.
for _szkic in "$ROOT/firmware/PillBox/PillBox.ino" \
              "$ROOT/firmware/PillBoxTest/PillBoxTest.ino"; do
  if ! grep -qF "$PART_OPIS" "$_szkic"; then
    echo "BLAD: naglowek $(basename "$_szkic") nie zapowiada podzialu '$PART_OPIS'."
    echo "      Kompilowanie na innym podziale mierzy nie to urzadzenie."
    exit 1
  fi
done
ESP32_INDEX="https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"

mkdir -p "$ACLI_DIR"

# ── 1. arduino-cli ────────────────────────────────────────────────────
if [ ! -x "$ACLI" ]; then
  echo "── pobieram arduino-cli"
  curl -sSL --max-time 180 -o "$ACLI_DIR/acli.tgz" \
    "https://github.com/arduino/arduino-cli/releases/download/v1.3.1/arduino-cli_1.3.1_Linux_64bit.tar.gz"
  tar xzf "$ACLI_DIR/acli.tgz" -C "$ACLI_DIR"
fi
"$ACLI" version

# ── 2. ctags ──────────────────────────────────────────────────────────
# arduino-cli generuje prototypy funkcji dla plikow .ino i wola do tego
# ctags z pakietu "builtin" Arduino. My kompilujemy .cpp, wiec zadnych
# prototypow nie potrzeba - ale sciezka do ctags musi byc rozwiazywalna,
# inaczej builder przerywa jeszcze przed kompilacja.
#
# ATRAPA, gdy ctags nie ma w systemie (swiezy kontener, czysta maszyna).
# Wolno ja podstawic, bo OBIE sciezki kompilacji ponizej podaja arduino-cli
# gotowy plik .cpp i pusty .ino obok - generator prototypow dostaje wiec
# pusty plik i nie ma z czego niczego wyciagac. Prototypy dla sciezki
# "tak jak wgrywa Arduino IDE" robi nasz proto_arduino.py, nie ctags.
# Bez tego builder przerywa PRZED kompilacja z "fork/exec ctags", czyli
# skrypt nie zdazy sprawdzic tego, po co istnieje.
if ! command -v ctags >/dev/null 2>&1; then
  echo "── brak ctags w systemie, podstawiam atrape (prototypow i tak nie generujemy)"
  mkdir -p "$ACLI_DIR/ctagsbin"
  printf '#!/bin/sh\nexit 0\n' > "$ACLI_DIR/ctagsbin/ctags"
  chmod +x "$ACLI_DIR/ctagsbin/ctags"
fi
CTAGS_DIR="$(dirname "$(command -v ctags || echo "$ACLI_DIR/ctagsbin/ctags")")"

# ── 3. rdzen ESP32 ────────────────────────────────────────────────────
# Rdzen 3.x ma jedna zaleznosc spoza pakietu esp32: dfu-util z pakietu
# "arduino". Sluzy WYLACZNIE do wgrywania przez USB - przy kompilacji
# nikt go nie wola. Gdy oryginalny indeks Arduino jest nieosiagalny,
# podstawiamy atrape, zeby rozwiazywanie zaleznosci przeszlo.
if ! "$ACLI" core list 2>/dev/null | grep -q "esp32:esp32 *$CORE_VER"; then
  URLS="$ESP32_INDEX"
  if ! curl -sSf --max-time 20 -o /dev/null https://downloads.arduino.cc/packages/package_index.tar.bz2; then
    echo "── oryginalny indeks Arduino nieosiagalny, podstawiam atrape dfu-util"
    STUB="$ACLI_DIR/stub"; mkdir -p "$STUB/dfu-util-dummy"
    echo "atrapa - dfu-util sluzy tylko do wgrywania, nie do kompilacji" \
      > "$STUB/dfu-util-dummy/README"
    ( cd "$STUB" && tar czf dfu.tar.gz dfu-util-dummy )
    python3 - "$STUB" <<'PY'
import json, sys, hashlib, os
stub = sys.argv[1]
blob = open(os.path.join(stub, "dfu.tar.gz"), "rb").read()
sysent = [{"host": h, "url": "http://127.0.0.1:8765/dfu.tar.gz",
           "archiveFileName": "dfu.tar.gz", "size": str(len(blob)),
           "checksum": "SHA-256:" + hashlib.sha256(blob).hexdigest()}
          for h in ["x86_64-pc-linux-gnu", "i686-pc-linux-gnu", "aarch64-linux-gnu",
                    "arm-linux-gnueabihf", "x86_64-apple-darwin", "i686-mingw32"]]
json.dump({"packages": [{"name": "arduino", "maintainer": "Arduino",
    "websiteURL": "https://arduino.cc", "email": "-", "platforms": [],
    "tools": [{"name": "dfu-util", "version": "0.11.0-arduino5", "systems": sysent}]}]},
    open(os.path.join(stub, "package_dfustub_index.json"), "w"))
PY
    ( cd "$STUB" && python3 -m http.server 8765 >/dev/null 2>&1 & echo $! > "$STUB/pid" )
    trap '[ -f "$STUB/pid" ] && kill "$(cat "$STUB/pid")" 2>/dev/null || true' EXIT
    sleep 1
    URLS="$ESP32_INDEX http://127.0.0.1:8765/package_dfustub_index.json"
  fi
  "$ACLI" config init --overwrite >/dev/null 2>&1 || true
  "$ACLI" config set board_manager.additional_urls $URLS >/dev/null 2>&1 || true
  "$ACLI" core update-index 2>&1 | grep -v "Error initializing" || true
  echo "── instaluje rdzen esp32:esp32@$CORE_VER (to potrwa)"
  "$ACLI" core install "esp32:esp32@$CORE_VER" 2>&1 | grep -v "Error initializing" | tail -3
fi

# ── 4. ArduinoJson ────────────────────────────────────────────────────
LIB="$ACLI_DIR/libraries/ArduinoJson"
if [ ! -d "$LIB/src" ]; then
  echo "── pobieram ArduinoJson"
  mkdir -p "$ACLI_DIR/libraries"
  git -c advice.detachedHead=false clone --quiet --depth 1 --branch v7.2.1 \
    https://github.com/bblanchon/ArduinoJson.git "$LIB"
fi

# ── 5. Kompilacja ─────────────────────────────────────────────────────
# Kompilujemy jako .cpp, nie .ino. Powod: generator prototypow w
# arduino-cli uzywa LATANEJ wersji ctags od Arduino; zwykly exuberant-ctags
# wypluwa dla "static void etapTestu(...)" prototyp z typem int i
# kompilacja pada na sprzecznej deklaracji. Nasz kod definiuje wszystko
# w kolejnosci uzycia, wiec zadne prototypy nie sa potrzebne.
# Dyrektywa #line sprawia, ze numery linii w bledach wskazuja .ino.
zbuduj() {
  local nazwa="$1" zrodlo="$2" katalog="$3" opis="$4" dodatkoweH="${5:-}"
  local tmp; tmp="$(mktemp -d)/$nazwa"
  mkdir -p "$tmp"
  printf '#include <Arduino.h>\n#line 1 "%s"\n' "$(basename "$zrodlo")" > "$tmp/$nazwa.cpp"
  cat "$zrodlo" >> "$tmp/$nazwa.cpp"
  # config.h i reszta plikow obok szkicu
  find "$katalog" -maxdepth 1 -name '*.h' -exec cp {} "$tmp/" \;
  [ -n "$dodatkoweH" ] && find "$dodatkoweH" -maxdepth 1 -name '*.h' -exec cp {} "$tmp/" \;
  touch "$tmp/$nazwa.ino"

  echo
  echo "════════ $opis ════════"
  "$ACLI" compile --fqbn "$FQBN" --library "$LIB" \
      --build-property "runtime.tools.ctags.path=$CTAGS_DIR" "$tmp" 2>&1 \
    | grep -vE "^Error initializing instance|Downloading index|^Multiple libraries|^  (Used|Not used):" \
    | grep -E "error:|warning:|Sketch uses|Global variables|Error during" || true

  "$ACLI" compile --fqbn "$FQBN" --library "$LIB" \
      --build-property "runtime.tools.ctags.path=$CTAGS_DIR" "$tmp" >/dev/null 2>&1
}

zbuduj pillbox "$ROOT/firmware/PillBox/PillBox.ino" "$ROOT/firmware/PillBox" \
       "PillBox.ino  (rdzen $CORE_VER, XIAO ESP32-C3, $PART_OPIS)"
# --- PillBoxTest budujemy DWA RAZY, i to nie jest nadmiar ------------
#
# TU BYLA DZIURA, znaleziona przy pelnym przegladzie kodu. Katalog
# firmware/PillBoxTest/ nie zawiera config.h, wiec `__has_include` dawalo
# MAM_CONFIG = 0 i CALA czesc sieciowo-bazodanowa szkicu (logowanie do
# Firebase, zapis wyniku testu) nie byla kompilowana ANI RAZU. Skrypt
# meldowal "PillBoxTest.ino kompiluje sie", pomijajac polowe, ktora Kuba
# naprawde uruchamia - bo naglowek szkicu kaze mu polozyc obok KOPIE
# swojego config.h.
#
# Dokladnie ta sama lekcja co B21/D26: sprawdzalismy sciezke budowania,
# ktorej nikt nie uzywa. Teraz budujemy obie: bez config.h (sam sprzet,
# tak jak obiecuje naglowek) i z config.h z repo - placeholder w srodku
# w zupelnosci wystarczy, kompilatorowi chodzi o to, ze pola istnieja.
zbuduj pillboxtest "$ROOT/firmware/PillBoxTest/PillBoxTest.ino" "$ROOT/firmware/PillBoxTest" \
       "PillBoxTest.ino  (szkic diagnostyczny, BEZ config.h)"
zbuduj pillboxtestcfg "$ROOT/firmware/PillBoxTest/PillBoxTest.ino" "$ROOT/firmware/PillBoxTest" \
       "PillBoxTest.ino  (szkic diagnostyczny, Z config.h - czesc Firebase)" \
       "$ROOT/firmware/PillBox"

# ── 6. Kompilacja przez PRAWDZIWA sciezke .ino ────────────────────────
# Kompilacja jako .cpp powyzej odpowiada na pytanie "czy kod sie buduje".
# NIE odpowiada na pytanie "czy Kuba to wgra" - a to dwie rozne rzeczy.
#
# Arduino IDE przed kompilacja dopisuje prototypy wszystkich funkcji i
# wstawia je TUZ PRZED PIERWSZA DEFINICJA FUNKCJI. Jesli jakikolwiek typ
# wlasny (enum, struct) uzyty w SYGNATURZE stoi nizej niz ten punkt,
# wygenerowany prototyp odwoluje sie do typu, ktorego kompilator jeszcze
# nie zna - i wgrywanie pada, choc `.cpp` buduje sie bez uwag.
#
# TO SIE NAPRAWDE STALO. Wersje 1.23.0-1.28.0 nie daly sie wgrac z
# Arduino IDE przez ponad miesiac, a ten skrypt przez caly ten czas
# meldowal sukces. Dlatego ten etap juz tu zostaje.
#
# EKSPORT BINARKI (piaty argument, opcjonalny) - to wlasnie ta sciezka
# produkuje plik do aktualizacji przez WiFi (D59). Swiadomie ta, a nie
# kompilacja jako .cpp: pudelko ma dostac dokladnie ten program, ktory
# wgralby Arduino IDE, a nie jego bliskiego kuzyna.
zbudujIno() {
  local nazwa="$1" zrodlo="$2" katalog="$3" opis="$4" wynik="$5" dodatkoweH="${6:-}"
  local tmp; tmp="$(mktemp -d)/$nazwa"
  mkdir -p "$tmp"
  python3 "$ROOT/tests/proto_arduino.py" "$zrodlo" "$tmp/$nazwa.cpp" "$(basename "$zrodlo")"
  find "$katalog" -maxdepth 1 -name '*.h' -exec cp {} "$tmp/" \;
  [ -n "$dodatkoweH" ] && find "$dodatkoweH" -maxdepth 1 -name '*.h' -exec cp {} "$tmp/" \;
  touch "$tmp/$nazwa.ino"

  echo
  echo "════════ $opis ════════"
  "$ACLI" compile --fqbn "$FQBN" --library "$LIB" \
      --build-property "runtime.tools.ctags.path=$CTAGS_DIR" "$tmp" 2>&1 \
    | grep -E "error:|Sketch uses|Error during" || true

  if [ -n "$wynik" ]; then
    mkdir -p "$wynik"
    "$ACLI" compile --fqbn "$FQBN" --library "$LIB" \
        --build-property "runtime.tools.ctags.path=$CTAGS_DIR" \
        --output-dir "$wynik" "$tmp" >/dev/null 2>&1
    echo "   binarka: $wynik/$nazwa.ino.bin"
  else
    "$ACLI" compile --fqbn "$FQBN" --library "$LIB" \
        --build-property "runtime.tools.ctags.path=$CTAGS_DIR" "$tmp" >/dev/null 2>&1
  fi
}

zbudujIno pbino "$ROOT/firmware/PillBox/PillBox.ino" "$ROOT/firmware/PillBox" \
       "PillBox.ino  Z PROTOTYPAMI, tak jak wgrywa Arduino IDE" "$OTA_OUT"
zbudujIno pbtino "$ROOT/firmware/PillBoxTest/PillBoxTest.ino" "$ROOT/firmware/PillBoxTest" \
       "PillBoxTest.ino  Z PROTOTYPAMI (bez config.h)" ""
zbudujIno pbtinocfg "$ROOT/firmware/PillBoxTest/PillBoxTest.ino" "$ROOT/firmware/PillBoxTest" \
       "PillBoxTest.ino  Z PROTOTYPAMI (z config.h)" "" "$ROOT/firmware/PillBox"

echo
echo "✔  Firmware kompiluje sie toolchainem Arduino (rdzen $CORE_VER)."
echo
echo "   UWAGA: to dowodzi, ze kod sie BUDUJE - nie, ze dziala na plytce."
echo "   Kompilacja nie wgrywa niczego i niczego nie uruchamia."
