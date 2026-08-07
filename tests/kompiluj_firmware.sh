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

ACLI_DIR="${ACLI_DIR:-$HOME/.pillbox-arduino}"
ACLI="$ACLI_DIR/arduino-cli"
CORE_VER="${CORE_VER:-3.3.11}"        # ta wersja siedzi w pudelku
FQBN="esp32:esp32:XIAO_ESP32C3"
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
CTAGS_DIR="$(dirname "$(command -v ctags || echo /usr/bin/ctags)")"

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
  local nazwa="$1" zrodlo="$2" katalog="$3" opis="$4"
  local tmp; tmp="$(mktemp -d)/$nazwa"
  mkdir -p "$tmp"
  printf '#include <Arduino.h>\n#line 1 "%s"\n' "$(basename "$zrodlo")" > "$tmp/$nazwa.cpp"
  cat "$zrodlo" >> "$tmp/$nazwa.cpp"
  # config.h i reszta plikow obok szkicu
  find "$katalog" -maxdepth 1 -name '*.h' -exec cp {} "$tmp/" \;
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
       "PillBox.ino  (rdzen $CORE_VER, plytka XIAO ESP32-C3)"
zbuduj pillboxtest "$ROOT/firmware/PillBoxTest/PillBoxTest.ino" "$ROOT/firmware/PillBoxTest" \
       "PillBoxTest.ino  (szkic diagnostyczny)"

echo
echo "✔  Firmware kompiluje sie toolchainem Arduino (rdzen $CORE_VER)."
echo
echo "   UWAGA: to dowodzi, ze kod sie BUDUJE - nie, ze dziala na plytce."
echo "   Kompilacja nie wgrywa niczego i niczego nie uruchamia."
