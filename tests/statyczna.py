#!/usr/bin/env python3
"""Kontrola statyczna repo - krok 4/10 zestawu testow.

Do 2026-08-17 ten kod byl WKLEJONY w run_all.sh jako heredoc. Wyjety, bo
skryptu wklejonego w runnera nie da sie ani uruchomic osobno przy szukaniu
jednej usterki, ani sprawdzic mutacja bez odpalania calego zestawu.

Sprawdza rzeczy, ktorych nie widac w testach jednostkowych, a ktore psuja
aplikacje na telefonie: brakujace elementy HTML, niesparowane znaczniki,
tresc nad naglowkiem (pasek statusu iOS), handlery bez definicji,
niezbalansowane nawiasy w firmware, pola konfiguracji nieznane regulom bazy.

Uzycie:  python3 tests/statyczna.py
"""
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

# Nad <header> nie moze stac NIC. Naglowek jest position:sticky i sam
# rezerwuje miejsce na pasek statusu iOS (env(safe-area-inset-top)), wiec
# element wstawiony przed nim laduje pod zegarkiem systemu - poza zasiegiem
# palca - i spycha caly uklad w dol. Tak zniknal przycisk powrotu.
_m = _re.search(r'<div id="app"[^>]*>(.*?)<header>', html, _re.S)
if not _m:
    bad += 1; print('  BLAD nie znaleziono naglowka aplikacji')
else:
    _miedzy = _re.sub(r'<!--.*?-->', '', _m.group(1), flags=_re.S).strip()
    if _miedzy:
        bad += 1
        print('  BLAD miedzy <div id="app"> a <header> stoi tresc: ' + _miedzy[:60])
    else:
        print('  OK   nic nie stoi nad naglowkiem (pasek statusu iOS)')

# OSLONA RYSOWANIA obejmuje WYLACZNIE rysowanie. Polkniety wyjatek jest
# ratunkiem dla ekranu i katastrofa dla zapisu: zapis, ktory sie nie udal,
# ma krzyczec. Gdyby ktos kiedys owinal nia doReconcile(), settlePills()
# albo zapiszPewnie() "zeby nie wywalalo", dawka gubilaby sie po cichu -
# dokladnie ten ksztalt bledu, ktory ta oslona ma zamykac (B23/D28).
_zakazane = ['doReconcile', 'settlePills', 'zapiszPewnie', 'zapiszCfg',
             'oczekWyslij', 'zapiszReconcile']
_zle_rys = [f for f in _zakazane
            if re.search(rf'rysuj\w*\((?:[^()]|\([^()]*\))*\b{f}\b', js)]
if _zle_rys:
    bad += 1
    print('  BLAD oslona rysowania owija zapis:', _zle_rys)
else:
    print('  OK   oslona rysowania nie owija zadnego zapisu do bazy')

# Kazdy render wolany z renderAll() ma miec WLASNA oslone - inaczej jeden
# wysypany ekran znow zabiera ze soba pozostale.
_all = js[js.index('function renderAll(){') + len('function renderAll(){'):]
_all = _all[:_all.index('\n}')]
_gole = re.findall(r'(?<![\w"])(render[A-Z]\w*)\(\)', _all)
if _gole:
    bad += 1; print('  BLAD render bez oslony w renderAll():', sorted(set(_gole)))
else:
    print('  OK   kazdy ekran w renderAll() ma wlasna oslone')

# Kazdy plik z listy service workera MUSI istniec w repo. Brakujacy plik
# na tej liscie to blad B12: instalacja workera przestaje sie konczyc,
# a razem z nia znika CALY mechanizm aktualizacji aplikacji - telefon
# zostaje na starej wersji i nic o tym nie mowi.
_sw = (root/'sw.js').read_text(encoding='utf-8')
_shell = re.findall(r'"\.\/([\w.\-]*)"', _sw[_sw.index('const SHELL'):_sw.index('];', _sw.index('const SHELL'))])
_brak = [f for f in _shell if f and not (root/f).exists()]
if _brak:
    bad += 1; print('  BLAD service worker cache\'uje nieistniejacy plik:', _brak)
else:
    print(f'  OK   wszystkie {len(_shell)} pozycji z listy service workera istnieja')

# Obrazek tabletki: lekka wersja WEBP musi byc animowana i naprawde lzejsza,
# a GIF ma zostac jako zapas dla przegladarki bez WEBP.
_webp = root/'tabletka.webp'
_gif  = root/'tabletka.gif'
if not _webp.exists() or not _gif.exists():
    bad += 1; print('  BLAD brakuje ktoregos z obrazkow tabletki')
else:
    _b = _webp.read_bytes()
    _ramek = _b.count(b'ANMF')
    if _b[:4] != b'RIFF' or _b[8:12] != b'WEBP' or _ramek < 2:
        bad += 1; print(f'  BLAD tabletka.webp nie jest animowanym WEBP (ramek: {_ramek})')
    elif _webp.stat().st_size >= _gif.stat().st_size:
        bad += 1; print('  BLAD tabletka.webp nie jest lzejsza od GIF-a')
    else:
        print(f'  OK   tabletka.webp — {_ramek} klatek, '
              f'{_webp.stat().st_size//1024} kB zamiast {_gif.stat().st_size//1024} kB')
    if 'tabletka.gif' not in js:
        bad += 1; print('  BLAD zniknal zapas w GIF-ie - przegladarka bez WEBP zostanie bez obrazka')
    else:
        print('  OK   zapas w GIF-ie na miejscu')

# ── SYSTEM WIZUALNY (D74) ───────────────────────────────────────────────
_css = re.search(r'<style>(.*?)</style>', html, re.S).group(1)
# Komentarze WYCINAMY przed analiza. Pierwsza wersja tej kontroli zglosila
# blad, bo trafila we wlasny komentarz opisujacy regule paska nawigacji -
# kontrola czytajaca opis kodu zamiast kodu mierzy nie to, co trzeba.
_css = re.sub(r'/\*.*?\*/', '', _css, flags=re.S)

# Pasek nawigacji WOLNO zmieniac (zakaz zdjety 2026-08-21 na prosbe Kuby).
# Zostaja dwa niezmienniki, ktore nie sa kwestia gustu:
#   1. rezerwa na wciecie ekranu - bez niej pasek wchodzi pod pasek gestow
#      iPhone'a i dolny rzad przyciskow przestaje byc klikalny;
#   2. polprzezroczyste tlo TYLKO razem z rozmyciem - polprzezroczysty pasek
#      bez blura to nieczytelna szyba z prleswitujacym kalendarzem.
_nav = _re.search(r'\bnav\{([^}]*)\}', _css)
if not _nav:
    bad += 1; print('  BLAD nie znaleziono regul paska nawigacji')
else:
    _n = _nav.group(1)
    if 'padding-bottom:env(safe-area-inset-bottom)' not in _n:
        bad += 1; print('  BLAD pasek nawigacji nie rezerwuje miejsca nad wcieciem ekranu')
    elif _re.search(r'background:rgba', _n) and 'backdrop-filter' not in _n:
        bad += 1; print('  BLAD polprzezroczysty pasek bez rozmycia tla')
    elif 'backdrop-filter' in _n and '-webkit-backdrop-filter' not in _n:
        bad += 1; print('  BLAD brak prefiksu -webkit-backdrop-filter (iOS nie rozmyje, B26)')
    else:
        print('  OK   pasek nawigacji: rezerwa na wciecie i spojne tlo')

# Kolor w tej aplikacji NIESIE ZNACZENIE: zielony/zolty/czerwony naleza do
# stanu dawki. Wartosci szesnastkowe wpisane wprost w style omijaja ten
# system - i wlasnie tak rodzi sie interfejs, w ktorym pieć odcieni zieleni
# znaczy pieć roznych rzeczy. Progi sa z pomiaru stanu po przebudowie.
_hex_css = re.findall(r'#[0-9a-fA-F]{3,8}\b', _css)
if len(_hex_css) > 40:
    bad += 1
    print(f'  BLAD za duzo kolorow wpisanych wprost w CSS ({len(_hex_css)}) - uzyj zmiennych')
else:
    print(f'  OK   kolory ida przez zmienne ({len(_hex_css)} wyjatkow w CSS)')

# Skala odstepow: cztery piksele i jej wielokrotnosci. "Jeszcze dwa piksele"
# w jednym miejscu to poczatek ukladu zlozonego z poprawek.
for _zm in ('--s1:4px', '--s2:8px', '--s3:12px', '--s4:16px'):
    if _zm not in _css:
        bad += 1; print(f'  BLAD brak zmiennej skali odstepow: {_zm}')
else:
    print('  OK   skala odstepow zdefiniowana')

# Cele dotykowe: przycisk ponizej 44 px to cel wielkosci litery, a nie palca
# (wytyczne Apple). Sprawdzamy sama regule bazowa - warianty moga byc mniejsze
# swiadomie, ale domyslny przycisk nie.
_btn = re.search(r'(?<![\w.#-])button\{([^}]*)\}', _css)
if not _btn or 'min-height:44px' not in _btn.group(1):
    bad += 1; print('  BLAD domyslny przycisk nie ma minimalnej wysokosci 44 px')
else:
    print('  OK   przyciski maja cel dotykowy 44 px')

# ── ILE TEKSTU NA EKRANIE (D75) ─────────────────────────────────────────
# Zgloszenie Kuby: "ta aplikacja ma strasznie duzo tekstu". Mial racje -
# wyjasnienia staly tam, gdzie sie ich uzywa, wiec czytal je codziennie
# ktos, kto zna je na pamiec. Od D75 tlumaczenia mieszkaja w Instrukcji,
# a na ekranach zostaje to, czego brak prowadzi do ZLEJ DECYZJI o leku.
#
# Ta kontrola pilnuje, zeby nie wrocily po cichu. Zwiniete sekcje
# (<details>) sa poza pomiarem - one z definicji nie zajmuja ekranu,
# dopoki ktos ich sam nie otworzy.
_PROG_TEKST = 200
_body = html[html.index('<div id="app"'):html.index('<script type="module">')]
_dlugie = []
for _m in _re.finditer(r'<section id="(tab-[\w-]+)"', _body):
    _a = _m.start(); _b = _body.index('</section>', _a)
    if _m.group(1) == 'tab-help':      # Instrukcja to jedyne miejsce na wyjasnienia
        continue
    _sec = _re.sub(r'<details[\s\S]*?</details>', '', _body[_a:_b])
    for _t in _re.finditer(r'<(p|div)\b[^>]*class="(?:muted|dim2)"[^>]*>(.*?)</\1>', _sec, _re.S):
        _txt = ' '.join(_re.sub(r'<[^>]+>', '', _t.group(2)).split())
        if len(_txt) > _PROG_TEKST:
            _dlugie.append(f'{_m.group(1)}: {len(_txt)} zn. — „{_txt[:60]}…"')
if _dlugie:
    bad += 1
    print(f'  BLAD akapit dluzszy niz {_PROG_TEKST} zn. poza Instrukcja:')
    for _d in _dlugie: print('       ' + _d)
else:
    print(f'  OK   zadne wyjasnienie na ekranie nie przekracza {_PROG_TEKST} zn.')

# Instrukcja musi istniec i byc osiagalna z Ustawien - inaczej przeniesione
# tam wyjasnienia po prostu znikaja z aplikacji.
if 'id="tab-help"' not in html:
    bad += 1; print('  BLAD brak ekranu Instrukcji')
elif "showTab('help')" not in html:
    bad += 1; print('  BLAD do Instrukcji nie da sie wejsc z Ustawien')
else:
    print('  OK   Instrukcja istnieje i ma wejscie z Ustawien')

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

# Dziennik decyzji jest podzielony na indeks (DECYZJE.md) i pelne wpisy
# w decyzje/*.md - zeby wejscie w zadanie kosztowalo 5 tys. tokenow zamiast
# 60 tys.  Podzial dziala tylko dopoty, dopoki indeks nie klamie: wpis bez
# linijki w indeksie jest niewidoczny, a linijka bez wpisu prowadzi donikad.
ind = (root/'DECYZJE.md').read_text(encoding='utf-8')
w_indeksie = dict(re.findall(r'^\| \*\*([DN]\d+[a-z]?)\*\* \|.*\| `(\w+)` \|$', ind, re.M))
w_plikach = {}
for f in sorted((root/'decyzje').glob('*.md')):
    for nr in re.findall(r'^\| \*\*([DN]\d+[a-z]?)\*\* \|', f.read_text(encoding='utf-8'), re.M):
        if nr in w_plikach:
            bad += 1; print(f'  BLAD decyzja {nr} stoi w dwoch plikach: {w_plikach[nr]}, {f.stem}')
        w_plikach[nr] = f.stem
osierocone = sorted(set(w_indeksie) - set(w_plikach))
nieznane = sorted(set(w_plikach) - set(w_indeksie))
zlyplik = sorted(n for n in set(w_indeksie) & set(w_plikach) if w_indeksie[n] != w_plikach[n])
if osierocone: bad += 1; print('  BLAD w indeksie, bez wpisu w decyzje/:', osierocone)
if nieznane:   bad += 1; print('  BLAD wpis w decyzje/ bez linijki w indeksie:', nieznane)
if zlyplik:    bad += 1; print('  BLAD indeks wskazuje zly plik dla:', zlyplik)
if not (osierocone or nieznane or zlyplik):
    print(f'  OK   indeks decyzji zgadza sie z decyzje/ ({len(w_plikach)} wpisow)')

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
