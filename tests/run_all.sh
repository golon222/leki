#!/usr/bin/env bash
# Uruchamia caly zestaw testow.  Uzycie:  bash tests/run_all.sh
set -e
cd "$(dirname "$0")"

echo "════════ 1/10  Testy logiki firmware (C++) ════════"
python3 extract.py
g++ -O0 -std=c++17 test_firmware.cpp -o /tmp/pillbox_tests
/tmp/pillbox_tests

echo
echo "════════ 1b/10  Testy odpornosci firmware (C++) ════════"
g++ -O0 -std=c++17 test_firmware_stress.cpp -o /tmp/pillbox_stress
/tmp/pillbox_stress

echo
echo "════════ 2/10  Testy logiki aplikacji (Node) ════════"
node build_app_module.mjs
node test_app.mjs

echo
echo "════════ 2b/10  Aplikacja o roznych porach doby ════════"
# Granica doby lekowej (DAY_START_HOUR) sprawia, ze aplikacja zachowuje sie
# inaczej miedzy polnoca a 3:00 niz w ciagu dnia. Bledy w tym oknie sa
# niewidoczne, jesli testy zawsze uruchamiaja sie po poludniu - dlatego
# przesuwamy strefe czasowa tak, zeby przejsc przez cala dobe.
UTC_H=$((10#$(date -u +%H)))
for TARGET in 1 2 3 8 20 23; do
  OFF=$(( (TARGET - UTC_H + 24) % 24 ))
  if [ "$OFF" -le 12 ]; then ZONE="Etc/GMT-$OFF"; else ZONE="Etc/GMT+$((24-OFF))"; fi
  printf "  %02d:xx czasu lokalnego (%s)  " "$TARGET" "$ZONE"
  TZ="$ZONE" node test_app.mjs > /tmp/pillbox_tz.log 2>&1 \
    && grep -o "ZALICZONE: [0-9]*" /tmp/pillbox_tz.log \
    || { echo "BLAD"; grep "FAIL" /tmp/pillbox_tz.log; exit 1; }
done

echo
echo "════════ 2c/10  Testy odpornosci aplikacji (Node) ════════"
node test_stress.mjs

echo
echo "════════ 2d/10  Kolejka zapisow w telefonie (Node) ════════"
# Najgrozniejszy tryb awarii aplikacji: Firebase offline nie odrzuca
# obietnicy, tylko wisi. Te testy URUCHAMIAJA kolejke, zamiast czytac jej
# kod wyrazeniami regularnymi.
node test_kolejka.mjs

echo
echo "════════ 2e/10  Zgodnosc pudelka i aplikacji ════════"
# Ten sam znacznik czasu przepuszczony przez PRAWDZIWY kod obu stron.
# Rozjazd tutaj oznacza tabletke zapisana na innym dniu w kalendarzu niz
# w pudelku - blad, ktory ujawnia sie dopiero tydzien pozniej.
g++ -O2 -std=c++17 crosscheck_days.cpp -o crosscheck_bin
node test_crosscheck.mjs

echo
echo "════════ 2f/10  Reguly bazy kontra to, co naprawde wysylamy ════════"
# "$other": false odrzuca CALY wpis, gdy trafi w nim nieznane pole.
# Sprawdzamy prawdziwy database.rules.json przeciw prawdziwym payloadom.
node test_rules.mjs

echo
echo "════════ 3/10  Kontrola firmware (audyt) ════════"
python3 audit_firmware.py

echo
echo "════════ 4/10  Kontrola statyczna ════════"
python3 - <<'PY'
import re, json, pathlib, sys
root = pathlib.Path(__file__).resolve().parent.parent if '__file__' in dir() else pathlib.Path('.')
root = pathlib.Path('..')
bad = 0

for f in ['database.rules.json', 'manifest.json']:
    json.load(open(root/f, encoding='utf-8'))
    print(f'  OK   {f} — poprawny JSON')

html = (root/'index.html').read_text(encoding='utf-8')
js = re.search(r'<script type="module">(.*?)</script>', html, re.S).group(1)
missing = sorted(set(re.findall(r'getElementById\("([\w-]+)"\)', js))
                 - set(re.findall(r'id="([\w-]+)"', html)))
if missing: bad += 1; print('  BLAD id bez odpowiednika w HTML:', missing)
else: print('  OK   wszystkie getElementById maja swoj element')

# Kazda sekcja ekranu musi miec sparowane znaczniki. Ekranow jest teraz
# kilkanascie i powstaja przez przenoszenie blokow miedzy nimi - jeden
# zgubiony </div> rozjezdza uklad dopiero na telefonie i niczego nie wywala.
import re as _re
_body = html[html.index('<div id="app"'):html.index('<script type="module">')]
_zle = []
for _m in _re.finditer(r'<section id="(tab-[\w-]+)"', _body):
    _a = _m.start(); _b = _body.index('</section>', _a)
    _sec = _body[_a:_b]
    for _t in ('div', 'details', 'button', 'span'):
        _o = len(_re.findall(rf'<{_t}[\s>]', _sec))
        _c = len(_re.findall(rf'</{_t}>', _sec))
        if _o != _c:
            _zle.append(f'{_m.group(1)}: <{_t}> {_o}/{_c}')
if _zle:
    bad += 1; print('  BLAD niesparowane znaczniki w sekcjach: ' + '; '.join(_zle))
else:
    print('  OK   kazda sekcja ekranu ma sparowane znaczniki')

handlers = set(re.findall(r'on(?:click|change)="(\w+)\(', html)) - {'if'}
orphan = [h for h in handlers if f'window.{h}' not in js]
if orphan: bad += 1; print('  BLAD handlery bez definicji:', orphan)
else: print(f'  OK   {len(handlers)} handlerow onclick/onchange ma definicje')

# Nawiasy liczymy TYM SAMYM dokladnym skanerem co dla programu
# diagnostycznego. Wyrazenia regularne dawaly tu falszywe alarmy na
# apostrofach w HTML-u portalu i w polskich komentarzach.
def zbalansowany(src):
    i, n, depth, par, st = 0, len(src), 0, 0, "code"
    while i < n:
        c = src[i]
        if st == "code":
            if c == "/" and i+1 < n and src[i+1] == "*": st = "blk"; i += 2; continue
            if c == "/" and i+1 < n and src[i+1] == "/": st = "ln";  i += 2; continue
            if c == '"': st = "str"; i += 1; continue
            if c == "'": st = "chr"; i += 1; continue
            if   c == "{": depth += 1
            elif c == "}": depth -= 1
            elif c == "(": par += 1
            elif c == ")": par -= 1
        elif st == "blk":
            if c == "*" and i+1 < n and src[i+1] == "/": st = "code"; i += 2; continue
        elif st == "ln":
            if c == "\n": st = "code"
        elif st in ("str", "chr"):
            if c == "\\": i += 2; continue
            if (st == "str" and c == '"') or (st == "chr" and c == "'"): st = "code"
        i += 1
    return depth == 0 and par == 0 and st == "code"

for f in ['firmware/PillBox/PillBox.ino', 'firmware/PillBox/config.h',
          'firmware/PillBoxTest/PillBoxTest.ino']:
    s = (root/f).read_text(encoding='utf-8')
    ifs = len(re.findall(r'^\s*#\s*if', s, re.M)); ends = len(re.findall(r'^\s*#\s*endif', s, re.M))
    if not zbalansowany(s) or ifs != ends:
        bad += 1; print(f'  BLAD {f}: nawiasy/#if niezbalansowane')
    else:
        print(f'  OK   {f} — nawiasy i #if/#endif zbalansowane')

cfg = (root/'firmware/PillBox/config.h').read_text(encoding='utf-8')
ino = (root/'firmware/PillBox/PillBox.ino').read_text(encoding='utf-8')
defined = set(re.findall(r'#\s*define\s+(\w+)', cfg))
unused = [d for d in defined if d not in ino and d not in
          ('LOG','LOGLN','REED_MODE','BUTTON_MODE')]
if unused: print('  UWAGA nieuzywane ustawienia w config.h:', unused)
else: print('  OK   kazde ustawienie z config.h jest uzywane')

for pl in ['pillsLeft','inrMin','inrMax','drugName','drugStrength']:
    if f'"{pl}"' not in (root/'database.rules.json').read_text(encoding='utf-8'):
        bad += 1; print(f'  BLAD reguly bazy nie znaja pola {pl}')
print('  OK   reguly bazy pokrywaja pola konfiguracji')

t = root/"firmware/PillBoxTest/PillBoxTest.ino"
if t.exists():
    src = t.read_text(encoding="utf-8")
    if src.count("void setup()") == 1 and src.count("void loop()") == 1:
        print("  OK   program diagnostyczny - jedno setup() i loop()")
    else:
        bad += 1; print("  BLAD PillBoxTest.ino: zle setup()/loop()")
    if "gpio_hold_en(" in src and "gpio_deep_sleep_hold_en()" in src:
        print("  OK   program diagnostyczny testuje sen tak samo jak firmware")
    else:
        bad += 1; print("  BLAD PillBoxTest.ino: brak zatrzasku pinu przed snem")

sys.exit(1 if bad else 0)
PY

echo
echo "✔  Wszystkie testy zakonczone powodzeniem."
