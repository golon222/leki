#!/usr/bin/env bash
# Uruchamia caly zestaw testow.  Uzycie:  bash tests/run_all.sh
#
# CICHO PRZY SUKCESIE, GLOSNO PRZY BLEDZIE - i to nie jest kosmetyka.
#
# Pelny wypis to bylo ~900 linii i ~39 000 znakow, z czego 533 linie to samo
# "OK" i naglowki sekcji. Czytal to glownie Claude, przy kazdym uruchomieniu,
# kilka razy dziennie - czyli placilismy tokenami za informacje "nic sie nie
# stalo". Teraz udany krok to JEDNA linijka z liczba zaliczonych kontroli.
#
# Zasada, ktorej nie wolno tu zlamac: oszczedzamy WYLACZNIE na sukcesie.
# Krok, ktory zawiedzie, wypluwa CALE swoje wyjscie - bo wtedy liczy sie
# dokladnie to, co zwykle jest szumem: ktora kontrola, z jakim komunikatem,
# w ktorej linii. Skrocony komunikat o bledzie kosztowalby wiecej niz
# oszczedza, bo za nim ida kolejne uruchomienia "a pokaz mi szczegoly".
#
#   SZCZEGOLY=1 bash tests/run_all.sh    - stary, pelny wypis
set -e
cd "$(dirname "$0")"

# Jednolinijkowe streszczenie udanego kroku: bierzemy liczbe, ktora ten krok
# sam o sobie podaje, zamiast zgadywac po liczbie linii.
podsumuj() {
  local s
  s=$(grep -oE "ZALICZONE: [0-9]+" "$1" | tail -1 || true)
  [ -n "$s" ] && { echo "$s"; return; }
  s=$(grep -oE "[0-9]+ kontroli zaliczonych" "$1" | tail -1 || true)
  [ -n "$s" ] && { echo "$s"; return; }
  s=$(grep -c "^  OK" "$1" || true)
  [ "${s:-0}" -gt 0 ] && { echo "$s kontroli OK"; return; }
  echo "OK"
}

# Ostrzezen kompilatora NIE chowamy bez sladu - liczymy je i mowimy, ze sa.
# Cicha zmiana "warning" w "nic" to dokladnie ten rodzaj oszczednosci,
# ktory kiedys kosztuje wieczor.
ostrzezenia() {
  local n; n=$(grep -c "warning:" "$1" || true)
  [ "${n:-0}" -gt 0 ] && printf '  (%s ostrzezen kompilatora)' "$n"
  return 0
}

krok() {
  local nazwa="$1"; shift
  local log; log="$(mktemp)"
  if "$@" >"$log" 2>&1; then
    if [ -n "${SZCZEGOLY:-}" ]; then
      echo "════════ $nazwa ════════"; cat "$log"; echo
    else
      printf '  ✓ %-44s %s%s\n' "$nazwa" "$(podsumuj "$log")" "$(ostrzezenia "$log")"
    fi
    rm -f "$log"
  else
    echo
    echo "✖  BLAD w kroku: $nazwa"
    echo "──────────────────────────────────────────────────────────────"
    # Same linie bledow, ale W CALOSCI - w tym projekcie kazda z nich niesie
    # plik, numer linii i nazwe kontroli, czyli komplet potrzebny do naprawy.
    # Gdy nic nie pasuje do wzorca (wysypka kompilatora, crash), NIE zgadujemy
    # co bylo wazne - pokazujemy wszystko.
    local trafienia
    trafienia=$(grep -nE "FAIL|BLAD|UWAGA|error:|Error|Traceback|Assertion" "$log" || true)
    if [ -n "$trafienia" ]; then
      echo "$trafienia"
      echo "──"
      tail -5 "$log"
      echo "──"
      echo "Pelny wypis tego kroku: $log"
      echo "Albo od nowa:  SZCZEGOLY=1 bash tests/run_all.sh"
    else
      cat "$log"
      rm -f "$log"
    fi
    exit 1
  fi
}

krok "1/10  logika firmware (C++)"        bash -c 'python3 extract.py && g++ -O0 -std=c++17 test_firmware.cpp -o /tmp/pillbox_tests && /tmp/pillbox_tests'
krok "1b/10 odpornosc firmware (C++)"     bash -c 'g++ -O0 -std=c++17 test_firmware_stress.cpp -o /tmp/pillbox_stress && /tmp/pillbox_stress'
krok "2/10  logika aplikacji (Node)"      bash -c 'node build_app_module.mjs && node test_app.mjs'

# Granica doby lekowej (DAY_START_HOUR) sprawia, ze aplikacja zachowuje sie
# inaczej miedzy polnoca a 3:00 niz w ciagu dnia. Bledy w tym oknie sa
# niewidoczne, jesli testy zawsze uruchamiaja sie po poludniu - dlatego
# przesuwamy strefe czasowa tak, zeby przejsc przez cala dobe.
pory_doby() {
  local utc_h; utc_h=$((10#$(date -u +%H)))
  local target off zone
  for target in 1 2 3 8 20 23; do
    off=$(( (target - utc_h + 24) % 24 ))
    if [ "$off" -le 12 ]; then zone="Etc/GMT-$off"; else zone="Etc/GMT+$((24-off))"; fi
    echo "── $(printf '%02d' "$target"):xx czasu lokalnego ($zone)"
    TZ="$zone" node test_app.mjs || return 1
  done
}
krok "2b/10 aplikacja o 6 porach doby"    pory_doby

krok "2c/10 odpornosc aplikacji (Node)"   node test_stress.mjs
# Najgrozniejszy tryb awarii aplikacji: Firebase offline nie odrzuca
# obietnicy, tylko wisi. Te testy URUCHAMIAJA kolejke, zamiast czytac jej
# kod wyrazeniami regularnymi.
krok "2d/10 kolejka zapisow w telefonie"  node test_kolejka.mjs
# Ten sam znacznik czasu przepuszczony przez PRAWDZIWY kod obu stron.
# Rozjazd tutaj oznacza tabletke zapisana na innym dniu w kalendarzu niz
# w pudelku - blad, ktory ujawnia sie dopiero tydzien pozniej.
krok "2e/10 zgodnosc pudelka i aplikacji" bash -c 'g++ -O2 -std=c++17 crosscheck_days.cpp -o crosscheck_bin && node test_crosscheck.mjs'
# "$other": false odrzuca CALY wpis, gdy trafi w nim nieznane pole.
# Sprawdzamy prawdziwy database.rules.json przeciw prawdziwym payloadom.
krok "2f/10 reguly bazy kontra payloady"  node test_rules.mjs
krok "3/10  kontrola firmware (audyt)"    python3 audit_firmware.py
krok "4/10  kontrola statyczna"           python3 statyczna.py

echo
echo "✔  Wszystkie testy zakonczone powodzeniem."
