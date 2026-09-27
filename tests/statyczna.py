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
# Katalog repo liczymy z POLOZENIA TEGO PLIKU, nie z katalogu wywolania.
# Stala '..' dzialala wylacznie przy uruchomieniu z tests/ - czyli tak, jak
# robi to run_all.sh - a `python3 tests/statyczna.py` z naglowka tego pliku
# konczylo sie wyjatkiem FileNotFoundError na database.rules.json.
# Kontrola, ktorej nie da sie uruchomic osobno, jest po polowie bezuzyteczna:
# szukajac jednej usterki trzeba wtedy odpalac caly zestaw.
root = pathlib.Path(__file__).resolve().parent.parent
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

# KAZDA NAZWA Z `KOPIA_CFG` MUSI ISTNIEC W `cfg`.
#
# Whitelist kopii zapasowej nie ma jak zglosic literowki: nazwa spoza `cfg`
# po prostu nic nie kopiuje i milczy. Tak przepadl odstep miedzy pomiarami
# INR - lista wolala go `inrInterval`, a pole nazywa sie `inrEveryDays`.
# Kopia wygladala na kompletna az do dnia, w ktorym trzeba bylo z niej
# odtworzyc; wtedy odstep wracal do domyslnych 21 dni.
_m_kopia = re.search(r'const KOPIA_CFG = \[(.*?)\];', js, re.S)
if not _m_kopia:
    bad += 1; print('  BLAD nie znaleziono listy KOPIA_CFG')
else:
    _pola = re.findall(r'"([\w]+)"', _m_kopia.group(1))
    _uzywane = set(re.findall(r'cfg\.(\w+)', js)) | set(re.findall(r'cfg\[\"(\w+)\"\]', js))
    _widma = [p for p in _pola if p not in _uzywane]
    if _widma:
        bad += 1
        print('  BLAD KOPIA_CFG wymienia pola, ktorych nie ma w cfg:', _widma)
    else:
        print(f'  OK   wszystkie {len(_pola)} pola z KOPIA_CFG istnieja w cfg')

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
    # Tlo paska idzie od D129 przez zmienna (--nav-bg), zeby motyw jasny mogl
    # je odwrocic. Kontrola musi wiec ROZWINAC zmienna i sprawdzic KAZDA jej
    # postac - w :root i w kazdym motywie. Szukanie slowa "rgba" wprost
    # w regule paska przestaloby cokolwiek pilnowac w chwili, w ktorej tlo
    # stalo sie zmienna, i nikt by tego nie zauwazyl: wypis dalej mowilby OK.
    _tlo = _re.search(r'background:([^;}]+)', _n)
    _wart = [_tlo.group(1)] if _tlo else []
    _zm = _re.match(r'\s*var\((--[\w-]+)\)\s*$', _wart[0]) if _wart else None
    if _zm:
        _wart = _re.findall(_zm.group(1) + r'\s*:\s*([^;}]+)', _css)
    if 'padding-bottom:env(safe-area-inset-bottom)' not in _n:
        bad += 1; print('  BLAD pasek nawigacji nie rezerwuje miejsca nad wcieciem ekranu')
    elif _zm and not _wart:
        bad += 1; print('  BLAD tlo paska uzywa zmiennej', _zm.group(1), '- nikt jej nie definiuje')
    elif any('rgba' in _w for _w in _wart) and 'backdrop-filter' not in _n:
        bad += 1; print('  BLAD polprzezroczysty pasek bez rozmycia tla')
    elif 'backdrop-filter' in _n and '-webkit-backdrop-filter' not in _n:
        bad += 1; print('  BLAD brak prefiksu -webkit-backdrop-filter (iOS nie rozmyje, B26)')
    else:
        print('  OK   pasek nawigacji: rezerwa na wciecie i spojne tlo')

# Kolor w tej aplikacji NIESIE ZNACZENIE: zielony/zolty/czerwony naleza do
# stanu dawki. Wartosci szesnastkowe wpisane wprost w style omijaja ten
# system - i wlasnie tak rodzi sie interfejs, w ktorym pieć odcieni zieleni
# znaczy pieć roznych rzeczy. Progi sa z pomiaru stanu po przebudowie.
#
# PALETY POMIJAMY, i to nie jest poblazanie: `:root` i motyw pudelka
# tygodniowego to MIEJSCA, W KTORYCH kolor ma byc wpisany wprost - one
# definiuja zmienne, ktorych ma uzywac reszta. Liczac je razem z reszta,
# kontrola mierzyla wielkosc palety zamiast tego, o co jej chodzi, i rosla
# przy kazdym nowym motywie az do falszywego alarmu.
_css_reguly = re.sub(r':root\{.*?\n\}', '', _css, flags=re.S)
_css_reguly = re.sub(r'body\[data-profil="[^"]+"\]\{.*?\n\}', '', _css_reguly, flags=re.S)
_hex_css = re.findall(r'#[0-9a-fA-F]{3,8}\b', _css_reguly)
# Stan po przebudowie to 12. Prog trzymamy tuz nad nim, zeby kazdy nowy
# kolor wpisany wprost w regule byl widoczny, a nie ginal w zapasie.
if len(_hex_css) > 13:
    bad += 1
    print(f'  BLAD za duzo kolorow wpisanych wprost w regulach CSS ({len(_hex_css)}) - uzyj zmiennych')
else:
    print(f'  OK   kolory ida przez zmienne ({len(_hex_css)} wyjatkow w CSS)')

# Skala odstepow: cztery piksele i jej wielokrotnosci. "Jeszcze dwa piksele"
# w jednym miejscu to poczatek ukladu zlozonego z poprawek.
# `else` nalezy tu do `if`, a NIE do `for`. Wczesniej stalo przy petli,
# a `for/else` w Pythonie wykonuje sie zawsze, gdy nie bylo `break` - wiec
# przy brakujacej zmiennej wypisywalo sie BLAD i zaraz pod nim OK. Sama
# kontrola dzialala (bad rosl), ale wypis mowil dwie sprzeczne rzeczy naraz,
# a to jest dokladnie ten nawyk, ktory uczy nie czytac wynikow testow.
_brak_skali = [z for z in ('--s1:4px', '--s2:8px', '--s3:12px', '--s4:16px')
               if z not in _css]
if _brak_skali:
    bad += 1; print('  BLAD brak zmiennych skali odstepow:', _brak_skali)
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

# LICZBY, KTORE MUSZA ZNACZYC TO SAMO PO OBU STRONACH.
#
# Pudelko i aplikacja czytaja te same dane, ale kazde ma wlasna kopie
# granicy. Rozjazd nie wywala niczego - po prostu jedna strona wie o dniu,
# o ktorym druga nie wie, i to widac dopiero w dniu, w ktorym to ma
# znaczenie. Tu chodzi o wyjatki dawkowania na daty: pamiec RTC miesci
# DOSE_EX_MAX, `fetchConfig()` bierze tyle NAJBLIZSZYCH, a reszte pomija
# po cichu. Aplikacja o tej granicy mowi wprost - ale tylko dopoty, dopoki
# obie liczby sa te same (D102).
# NOTATKA: limit trzymaja REGULY BAZY, aplikacja musi go znac.
# `note` przy dawce i przy pomiarze INR ma w regulach `length <= 300`,
# a wpis dawki ma obok `$other: false` - czyli za dluga notatka nie jest
# obcinana, tylko odrzuca CALY wpis o dawce. Aplikacja przycina ja sama
# (NOTATKA_MAX), ale tylko dopoki obie liczby sa te same (D104).
_lim_regul = set(re.findall(r'newData\.isString\(\) && newData\.val\(\)\.length <= (\d+)',
                            (root/'database.rules.json').read_text(encoding='utf-8')))
_m_not = re.search(r'const\s+NOTATKA_MAX\s*=\s*(\d+)', js)
if not _m_not:
    bad += 1; print('  BLAD nie znalazlem NOTATKA_MAX w aplikacji')
elif _m_not.group(1) not in _lim_regul:
    bad += 1
    print(f'  BLAD NOTATKA_MAX={_m_not.group(1)}, a reguly bazy nie maja takiego '
          f'limitu dlugosci (maja: {sorted(_lim_regul)})')
else:
    # OBA pola notatki, nie "gdziekolwiek w pliku". Pierwsza wersja tej
    # kontroli szukala napisu `maxlength="300"` w calym HTML - a pole dnia
    # ma tam `maxlength="${NOTATKA_MAX}"`, wiec wystarczalo, ze limit
    # zostal przy notatce INR. Zdjecie go z notatki dnia (czyli z tego
    # pola, ktore odrzuca CALY wpis o dawce) przechodzilo bez slowa.
    # Zlapane mutacja, nie okiem.
    _bez = []
    for _id in ('inrNote', 'dayNote'):
        _m_pole = _re.search(rf'<input[^>]*id="{_id}"[^>]*>', html, _re.S)
        if not _m_pole:
            _bez.append(f'{_id}: nie ma takiego pola')
        elif 'maxlength=' not in _m_pole.group(0):
            _bez.append(f'{_id}: bez maxlength')
    if _bez:
        bad += 1
        print('  BLAD pole notatki bez ograniczenia dlugosci:', '; '.join(_bez))
    else:
        print(f'  OK   limit notatki ({_m_not.group(1)}) zgodny z regulami bazy '
              f'i pilnowany w obu polach')

_pary = [('DOSE_EX_MAX', 'PUDELKO_WYJATKOW_MAX')]
for _fw, _app in _pary:
    _m1 = re.search(rf'#\s*define\s+{_fw}\s+(\d+)', cfg)
    _m2 = re.search(rf'const\s+{_app}\s*=\s*(\d+)', js)
    if not _m1 or not _m2:
        bad += 1; print(f'  BLAD nie znalazlem {_fw} albo {_app}')
    elif _m1.group(1) != _m2.group(1):
        bad += 1
        print(f'  BLAD {_fw}={_m1.group(1)} w firmware, a {_app}={_m2.group(1)} '
              f'w aplikacji - aplikacja klamie o granicy pudelka')
    else:
        print(f'  OK   {_fw} i {_app} zgodne ({_m1.group(1)})')

# ── AUTOMAT BUDUJACY BINARKE TRZYMA KOPIE LICZB Z FIRMWARE ──────────────
#
# `.github/workflows/firmware.yml` jest jedynym plikiem w tym repo, ktorego
# nie dotyka zaden test - a to on produkuje binarke, ktora Kuba wgrywa do
# pudelka. Trzyma przy tym WLASNE kopie czterech rzeczy z config.h. Kazda
# z nich moze sie rozjechac po cichu, a objaw wychodzi dopiero na plytce:
#
#   * granice rozmiaru - automat wypuscilby plik, ktory pudelko odrzuci
#     komunikatem "opis wersji na serwerze jest niepoprawny";
#   * nazwy plikow - pudelko pobieraloby adres, ktorego tam nie ma;
#   * placeholder hasla - to jest ten guard, ktory zatrzymuje budowe, gdyby
#     ktos wrzucil do repo config.h z PRAWDZIWYM haslem. Szuka konkretnego
#     napisu; zmiana tego napisu po stronie firmware rozbraja go w ciszy.
# ── BINARKA MUSI BYC POWTARZALNA ────────────────────────────────────────
#
# ZMIERZONE: dwie kompilacje byte-identycznego zrodla dawaly binarki
# roznione 65 bajtami (app_elf_sha256 w naglowku obrazu - suma pliku ELF,
# a ELF nosi bezwzgledne sciezki budowania). Winny byl `mktemp -d`: inny
# katalog przy kazdym uruchomieniu.
#
# Skutek widac az u Kuby: automat publikuje binarke przy kazdej zmianie
# w firmware/**, wiec md5 bylo zawsze inne, wlasny guard automatu
# ("binarka bez zmian") nie mogl sie odezwac ani razu, a aplikacja pisala
# "jest nowa wersja" o tej samej wersji. Pudelko sciagalo 1,2 MB przez
# WiFi z baterii i restartowalo sie na to samo.
_ksh = (root/'tests/kompiluj_firmware.sh').read_text(encoding='utf-8')
_ksh_kod = _re.sub(r'#[^\n]*', '', _ksh)          # komentarze opisuja ten blad
if 'mktemp -d' in _ksh_kod:
    bad += 1
    print('  BLAD kompiluj_firmware.sh buduje w mktemp -d - binarka przestaje '
          'byc powtarzalna, a automat publikuje ja przy kazdym przebiegu')
elif 'BUILD_DIR' not in _ksh_kod:
    bad += 1
    print('  BLAD kompiluj_firmware.sh nie ma stalego katalogu budowania')
else:
    print('  OK   binarka budowana w stalym katalogu (powtarzalne md5)')

_wf_p = root/'.github/workflows/firmware.yml'
if not _wf_p.exists():
    bad += 1; print('  BLAD brak .github/workflows/firmware.yml')
else:
    _wf = _wf_p.read_text(encoding='utf-8')
    _def = lambda n: (re.search(rf'#\s*define\s+{n}\s+"?([^"\s]+)"?', cfg) or [None, None])[1] \
                     if re.search(rf'#\s*define\s+{n}\s+', cfg) else None
    _rozjazd = []
    for _n in ('OTA_MIN_BIN_SIZE', 'OTA_MAX_BIN_SIZE'):
        _v = _def(_n)
        if not _v:            _rozjazd.append(f'{_n}: nie ma w config.h')
        elif _v not in _wf:   _rozjazd.append(f'{_n}={_v} nie wystepuje w workflow')
    for _n in ('OTA_BIN_FILE', 'OTA_JSON_FILE'):
        _v = _def(_n)
        if not _v:            _rozjazd.append(f'{_n}: nie ma w config.h')
        elif _v not in _wf:   _rozjazd.append(f'{_n}={_v} nie wystepuje w workflow')
    # Placeholder hasla stoi w TRZECH miejscach: jako wartosc DEVICE_PASSWORD
    # w config.h, jako PASSWORD_PLACEHOLDER w PillBox.ino (po tym firmware
    # poznaje binarke z automatu) i jako wzorzec w guardzie workflow.
    # Rozjazd miedzy dwoma pierwszymi jest grozny: NVS z dawnym
    # placeholderem zostalby uznany za prawdziwe haslo, `otaWolno()`
    # przepuscilby aktualizacje, a nowy program nie zalogowalby sie do bazy -
    # czyli pudelko traci jedyna zdalna droge naprawy (D59).
    _ph_ino = re.search(r'#\s*define\s+PASSWORD_PLACEHOLDER\s+"([^"]+)"', ino)
    _ph_cfg = re.search(r'#\s*define\s+DEVICE_PASSWORD\s+"([^"]+)"', cfg)
    if not _ph_ino or not _ph_cfg:
        _rozjazd.append('nie znalazlem PASSWORD_PLACEHOLDER albo DEVICE_PASSWORD')
    else:
        if _ph_ino.group(1) != _ph_cfg.group(1):
            _rozjazd.append(f'placeholder hasla: .ino ma "{_ph_ino.group(1)}", '
                            f'config.h ma "{_ph_cfg.group(1)}"')
        if _ph_ino.group(1) not in _wf:
            _rozjazd.append(f'workflow nie pilnuje placeholdera "{_ph_ino.group(1)}"')
    if _rozjazd:
        bad += 1
        print('  BLAD automat budujacy binarke rozjechal sie z firmware:')
        for _r in _rozjazd: print('       ' + _r)
    else:
        print('  OK   automat budujacy binarke zgodny z config.h (rozmiary, nazwy, placeholder)')
defined = set(re.findall(r'#\s*define\s+(\w+)', cfg))
unused = [d for d in defined if d not in ino and d not in
          ('LOG','LOGLN','REED_MODE','BUTTON_MODE')]
if unused: print('  UWAGA nieuzywane ustawienia w config.h:', unused)
else: print('  OK   kazde ustawienie z config.h jest uzywane')

# Tak samo tutaj: "OK" szlo bezwarunkowo, zaraz za wypisanym bledem.
_reguly = (root/'database.rules.json').read_text(encoding='utf-8')
_brak_pol = [pl for pl in ['pillsLeft','inrMin','inrMax','drugName','drugStrength']
             if f'"{pl}"' not in _reguly]
if _brak_pol:
    bad += 1; print('  BLAD reguly bazy nie znaja pol konfiguracji:', _brak_pol)
else:
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

# ── Motyw moze przemalowac znaczenia, ale MUSZA zostac rozroznialne ──
#
# Pierwsza wersja tej kontroli zabraniala motywowi ruszac --ok/--warn/--bad
# w ogole. Zakaz byl za szeroki: Kuba poprosil wprost, zeby w pudelku
# tygodniowym "odejsc od neonowych i dac pudrowe", a przygaszenie koloru
# NIE zmienia jego znaczenia - zielony dalej znaczy "wziete".
#
# Chronimy wiec to, o co naprawde chodzi w zasadzie 14:
#   1. przypisanie zostaje - zielony jest zielony, czerwony czerwony,
#   2. kolory daja sie od siebie odroznic JEDNYM SPOJRZENIEM.
#
# Drugi punkt mierzymy, zamiast oceniac na oko: odleglosc w przestrzeni
# Lab (CIE76). Ponizej ~25 dwa kolory zaczynaja byc mylone przy malych
# plamach, a kratka kalendarza to wlasnie mala plama.
def _lab(hx):
    r, g, b = (int(hx[i:i+2], 16)/255 for i in (1, 3, 5))
    def li(c): return c/12.92 if c <= .04045 else ((c+.055)/1.055)**2.4
    r, g, b = li(r), li(g), li(b)
    X = (r*.4124 + g*.3576 + b*.1805)/.95047
    Y =  r*.2126 + g*.7152 + b*.0722
    Z = (r*.0193 + g*.1192 + b*.9505)/1.08883
    def f(t): return t**(1/3) if t > .008856 else 7.787*t + 16/116
    fx, fy, fz = f(X), f(Y), f(Z)
    return (116*fy - 16, 500*(fx-fy), 200*(fy-fz))

def _odleglosc(a, b):
    la, lb = _lab(a), _lab(b)
    return sum((x-y)**2 for x, y in zip(la, lb)) ** .5

def _odcien(hx):
    r, g, b = (int(hx[i:i+2], 16)/255 for i in (1, 3, 5))
    import colorsys
    return colorsys.rgb_to_hls(r, g, b)[0]*360

_html = (root/'index.html').read_text(encoding='utf-8')
_m = re.search(r'body\[data-profil="tydzien"\]\{(.*?)\n\}', _html, re.S)
_rt = re.search(r':root\{(.*?)\n\}', _html, re.S)
if not _m or not _rt:
    bad += 1; print('  BLAD brak motywu tygodniowego albo :root')
else:
    def _token(nazwa):
        for blok in (_m.group(1), _rt.group(1)):       # motyw ma pierwszenstwo
            t = re.search(r'(?<![\w-])' + nazwa + r'\s*:\s*(#[0-9a-fA-F]{6})', blok)
            if t: return t.group(1)
        return None
    _p = {n: _token('--' + n) for n in ('ok', 'warn', 'bad', 'acc')}
    if None in _p.values():
        bad += 1; print('  BLAD nie znalazlem koloru:', [k for k, v in _p.items() if not v])
    else:
        # 1. przypisanie: zielony/zolty/czerwony zostaja w swoich rodzinach
        _rodziny = {'ok': (75, 190), 'warn': (20, 75), 'bad': (320, 20)}
        _zle = []
        for n, (a, b) in _rodziny.items():
            h = _odcien(_p[n])
            w_zakresie = (a <= h <= b) if a < b else (h >= a or h <= b)
            if not w_zakresie: _zle.append(f'{n}={_p[n]} ({h:.0f} st)')
        # 2. rozroznialnosc kazdej pary
        _pary = [(x, y) for i, x in enumerate(_p) for y in list(_p)[i+1:]]
        _blisko = [(x, y, _odleglosc(_p[x], _p[y])) for x, y in _pary
                   if _odleglosc(_p[x], _p[y]) < 25]
        if _zle:
            bad += 1; print('  BLAD motyw zmienil ZNACZENIE koloru, nie tylko odcien:', _zle)
        elif _blisko:
            bad += 1
            print('  BLAD w motywie kolory sa zbyt podobne, beda mylone:',
                  [f'{x}~{y} ({d:.0f})' for x, y, d in _blisko])
        else:
            _naj = min(_odleglosc(_p[x], _p[y]) for x, y in _pary)
            print(f'  OK   motyw tygodniowy: znaczenia na miejscu, najblizsza para {_naj:.0f}')

# ── Motyw jasny: chrom odwrocony i atrament czytelny (D129) ─────────
#
# Prosba Kuby: "przebuduj caly design dla tego konta, zeby nie wygladalo to
# jak psychiatryk". Odwrocenie motywu ma jedna pulapke, ktora zobaczylem
# dopiero NA ZRZUCIE, a nie w zadnym tescie: kilkanascie regul mialo kolor
# ciemnego tla wpisany wprost. Naglowek zostal czarny, wiec tytul ekranu byl
# czarny na czarnym, a liczby w kalendarzu - pastelowe na bieli.
#
# Te dwie kontrole MIERZA to, co wtedy bylo zle, zamiast wyliczac nazwy
# zmiennych: jasnosc powierzchni i kontrast atramentu wzgledem tla, po
# ktorym naprawde jezdzi oko (kratka dnia to `--X-soft` polozone na karcie).
def _skladowe(w):
    w = w.strip()
    t = re.match(r'#([0-9a-fA-F]{6})$', w)
    if t: return tuple(int(t.group(1)[i:i+2], 16) for i in (0, 2, 4)) + (1.0,)
    t = re.match(r'rgba?\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*(?:,\s*([\d.]+))?\s*\)$', w)
    if t: return (int(t.group(1)), int(t.group(2)), int(t.group(3)),
                  float(t.group(4)) if t.group(4) else 1.0)
    t = re.match(r'(\d+)\s*,\s*(\d+)\s*,\s*(\d+)$', w)      # sam zestaw skladowych
    if t: return (int(t.group(1)), int(t.group(2)), int(t.group(3)), 1.0)
    return None

def _jasnosc(rgb):
    def li(c):
        c /= 255
        return c/12.92 if c <= .04045 else ((c+.055)/1.055)**2.4
    r, g, b = (li(x) for x in rgb[:3])
    return .2126*r + .7152*g + .0722*b

def _nalozone(wierzch, spod):
    a = wierzch[3]
    return tuple(wierzch[i]*a + spod[i]*(1-a) for i in range(3))

def _kontrast(a, b):
    ja, jb = _jasnosc(a), _jasnosc(b)
    return (max(ja, jb) + .05) / (min(ja, jb) + .05)

if _m and _rt:
    def _wartosc(blok, nazwa):
        t = re.search(r'(?<![\w-])' + nazwa + r'\s*:\s*([^;}\n]+)', blok)
        return _skladowe(t.group(1)) if t else None

    # 1. CHROM. Kazda z tych powierzchni w motywie podstawowym jest ciemna;
    #    w jasnym MUSI byc jasna, bo tekst na niej bierze kolor z --txt.
    _chrom = ('--aura', '--hdr-rgb', '--nav-bg', '--nav-pier', '--toast-bg')
    _zle = []
    for _t in _chrom:
        _c, _j = _wartosc(_rt.group(1), _t), _wartosc(_m.group(1), _t)
        if _c is None: _zle.append(f'{_t}: brak w :root'); continue
        if _j is None: _zle.append(f'{_t}: motyw jasny go nie zmienia'); continue
        if _jasnosc(_c) > .25: _zle.append(f'{_t}: w palecie nie jest ciemny')
        if _jasnosc(_j) < .60: _zle.append(f'{_t}: w motywie jasnym nadal ciemny')
    if _zle:
        bad += 1; print('  BLAD chrom motywu jasnego:', _zle)
    else:
        print(f'  OK   motyw jasny odwraca caly chrom ({len(_chrom)} powierzchni)')

    # 2. ATRAMENT STANU na kratce kalendarza. Prog 4.5 to wymaganie WCAG AA
    #    dla zwyklego tekstu - a liczba dnia jest mala i czyta sie ja
    #    w przelocie, wiec ponizej tego progu kalendarz przestaje mowic.
    _slabe = []
    for _nazwa, _blok in (('paleta', _rt.group(1)), ('motyw jasny', _m.group(1))):
        _karta = _wartosc(_blok, '--card') or _wartosc(_rt.group(1), '--card')
        for _st in ('ok', 'warn', 'bad'):
            _txt = _wartosc(_blok, f'--{_st}-txt') or _wartosc(_rt.group(1), f'--{_st}-txt')
            _tlo = _wartosc(_blok, f'--{_st}-soft') or _wartosc(_rt.group(1), f'--{_st}-soft')
            if not (_txt and _tlo and _karta):
                _slabe.append(f'{_nazwa}/{_st}: brak koloru'); continue
            _k = _kontrast(_txt[:3], _nalozone(_tlo, _karta))
            if _k < 4.5: _slabe.append(f'{_nazwa}/{_st}: kontrast {_k:.1f}')
    if _slabe:
        bad += 1; print('  BLAD liczba w kalendarzu za slabo widoczna:', _slabe)
    else:
        print('  OK   atrament stanu czytelny w obu motywach (WCAG AA)')

    # 3. TEKST DRUGORZEDNY. Zgloszenie Kuby po pierwszej wersji motywu
    #    jasnego: "nie widac dni tygodnia w ogole". Skrot PN/WT/SR ma 10 px
    #    i wersaliki - to najmniejszy tekst w aplikacji, a stoi w kolorze
    #    `--dim2`. Na bialej karcie wychodzilo 3,1 i po prostu znikal.
    #    Prog 4.0 nie jest wziety z sufitu: tyle ma `--dim2` w palecie
    #    podstawowej, ktora Kuba czyta codziennie. Motyw nie moze byc
    #    gorszy od tego, co kopiuje.
    _blade = []
    for _nazwa, _blok in (('paleta', _rt.group(1)), ('motyw jasny', _m.group(1))):
        _karta = _wartosc(_blok, '--card') or _wartosc(_rt.group(1), '--card')
        for _t in ('--dim', '--dim2'):
            _c = _wartosc(_blok, _t) or _wartosc(_rt.group(1), _t)
            if not (_c and _karta): _blade.append(f'{_nazwa}/{_t}: brak koloru'); continue
            _k = _kontrast(_c[:3], _karta[:3])
            if _k < 4.0: _blade.append(f'{_nazwa}/{_t}: kontrast {_k:.1f}')
    if _blade:
        bad += 1; print('  BLAD tekst drugorzedny za blady:', _blade)
    else:
        print('  OK   tekst drugorzedny czytelny w obu motywach')

# ── Dwa pudelka: dane czlowieka nie moga sie mieszac (D124) ──────────
#
# TU BYL NAJGROZNIEJSZY BLAD, jaki ta aplikacja moze miec. Dawki, INR,
# rozpisania i kopie leza pod `users/<uid>/...`, czyli pod CZLOWIEKIEM -
# w sciezce nie bylo numeru pudelka. Wybor urzadzenia zmienial tylko to,
# skad czytane sa ZDARZENIA; kalendarz zapisywal sie dalej w to samo
# miejsce, wiec jedno przelaczenie na pudelko tygodniowe wpisywaloby jego
# otwarcia jako dawki do kalendarza WARFINU.
#
# Naprawa jest jednym miejscem - korzenDanych(). Ta kontrola pilnuje, zeby
# nikt go nie obszedl, dopisujac kiedys `users/${uid}/cokolwiek` wprost.
html = (root/'index.html').read_text(encoding='utf-8')
# Samo cialo korzenDanych() te sciezki oczywiscie zawiera - wycinamy je,
# zeby kontrola patrzyla na WOLAJACYCH, a nie na definicje.
bez_korzenia = re.sub(r'function korzenDanych\(\)\{[\s\S]*?\n\}', '', html)
obejscia = re.findall(r'`users/\$\{uid\}[^`]*`', bez_korzenia)
if obejscia:
    bad += 1
    print('  BLAD sciezka danych czlowieka omija korzenDanych():', obejscia[:3])
else:
    print(f'  OK   dane czlowieka ida przez korzenDanych() ({html.count("korzenDanych()")} miejsc)')

# Wspolwlasciciel pudelka: dziewczyna ma wlasne konto i wlasne haslo, a nie
# loginy Kuby. Wyrazenia z auth/data/root silnik testowy swiadomie pomija,
# wiec ich KSZTALTU nie pilnuje nic poza ta kontrola.
reg = json.loads((root/'database.rules.json').read_text(encoding='utf-8'))['rules']
dev = reg['devices']['$deviceId']
braki = [k for k in ('.read', '.write') if "owners" not in dev[k]]
if braki or '$uid' not in dev.get('owners', {}):
    bad += 1; print('  BLAD reguly nie znaja wspolwlascicieli pudelka:', braki)
else:
    print('  OK   reguly znaja wspolwlascicieli pudelka (owners)')

# ── Pudelko tygodniowe: pin wraca do trybu cyfrowego przed snem (B30) ──
#
# analogRead...() zostawia pad w trybie ANALOGOWYM, a to wylacza bufor
# wejscia cyfrowego - ten sam, ktorym komparator wybudzania czyta stan
# pinu. Wylaczony daje stale zero, czyli "stan niski" spelniony od razu
# po zasnieciu: pudelko budzi sie w kolko i piszczy bez konca. Zmierzone
# u Kuby, nie wydedukowane.
w = root/"firmware/PillBoxWeek/PillBoxWeek.ino"
if w.exists():
    src = w.read_text(encoding="utf-8")
    sen = src[src.index("void idzSpac()"):src.index("void setup()")]
    # Wersja w naglowku i wersja w kodzie MUSZA sie zgadzac. Rozjazd
    # sprawia, ze pudelko meldujac "0.2.0" moze chodzic na starym kodzie -
    # i tak sie wlasnie stalo (B30, druga runda).
    import re as _r2
    _fw  = _r2.search(r'#define\s+FW_VERSION\s+"([^"]+)"',
                      (root/"firmware/PillBoxWeek/config.h").read_text(encoding="utf-8"))
    _kod = _r2.search(r'#define\s+KOD_WERSJA\s+"([^"]+)"', src)
    if not _fw or not _kod:
        bad += 1; print("  BLAD PillBoxWeek: brak FW_VERSION albo KOD_WERSJA")
    elif _fw.group(1) != _kod.group(1):
        bad += 1; print(f"  BLAD PillBoxWeek: config.h mowi {_fw.group(1)}, kod {_kod.group(1)}")
    else:
        print(f"  OK   pudelko tygodniowe - naglowek i kod tej samej wersji ({_fw.group(1)})")

    braki = [x for x in ("pinMode(PIN_KLAPKI, INPUT)", "gpio_hold_en(",
                         "gpio_deep_sleep_hold_en()") if x not in sen]
    if braki:
        bad += 1; print("  BLAD PillBoxWeek usypia bez powrotu pinu do trybu cyfrowego:", braki)
    elif "gpio_hold_dis(" not in src or "gpio_deep_sleep_hold_dis()" not in src:
        bad += 1; print("  BLAD PillBoxWeek nie zdejmuje zatrzasku po wybudzeniu")
    else:
        print("  OK   pudelko tygodniowe usypia z pinem w trybie cyfrowym")

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
