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

    # ── DWIE KOPIE DECYZJI O AKTUALIZACJI MUSZA BYC IDENTYCZNE ──────
    #
    # `otaDecyzja()` stoi w obu szkicach, bo ograniczenie 1 z CLAUDE.md
    # zabrania wspolnego pliku: zmiana w pudelku dziewczyny nie ma prawa
    # dotykac kodu pilnujacego Warfinu. Cena za to jest znana z D111 -
    # dwie kopie rozjada sie przy pierwszej poprawce, a ta decyduje
    # o tym, czy pudelko pobierze 1,1 MB i zrestartuje sie na nowy program.
    #
    # Porownujemy SAM KOD, bez komentarzy i bez odstepow: oryginal ma
    # w srodku kilkadziesiat linii opisu, ktorych nie ma po co dublowac.
    # Testy C++ uruchamiaja te funkcje raz - identycznosc rozciaga ich
    # wynik na oba pudelka i to jest caly sens tej kontroli.
    def _cialo(tekst, nazwa):
        i = tekst.find(nazwa + '(')
        if i < 0: return None
        i = tekst.index('{', i)
        gl, j = 0, i
        while j < len(tekst):
            if tekst[j] == '{': gl += 1
            elif tekst[j] == '}':
                gl -= 1
                if gl == 0: return tekst[i:j+1]
            j += 1
        return None

    def _sameKod(t):
        t = re.sub(r'/\*.*?\*/', '', t, flags=re.S)
        t = re.sub(r'//[^\n]*', '', t)
        return ' '.join(t.split())

    _a = _cialo(ino, 'OtaDecyzja otaDecyzja')
    _b = _cialo(src, 'OtaDecyzja otaDecyzja')
    if not _a or not _b:
        bad += 1; print('  BLAD nie znalazlem otaDecyzja() w obu szkicach')
    elif _sameKod(_a) != _sameKod(_b):
        bad += 1
        print('  BLAD otaDecyzja() w obu pudelkach ROZJECHALA SIE - jedno z nich')
        print('       zdecyduje inaczej o pobraniu programu. Wyrownaj kod.')
    else:
        print('  OK   decyzja o aktualizacji identyczna w obu pudelkach')

    # ── KAZDE PUDELKO POBIERA SWOJ PLIK ─────────────────────────────
    #
    # Wgranie pudelku tygodniowemu programu dziennego (albo odwrotnie)
    # konczy sie dwiema ceglami: piny, drabinka i cala logika sa inne,
    # a OTA nie ma jak tego rozpoznac - sprawdza sume pliku, ktory sam
    # wskazal. Rozroznienie jest WYLACZNIE w nazwie pliku, wiec ta nazwa
    # jest czescia bezpieczenstwa, a nie kosmetyka.
    _cfgW = (root/"firmware/PillBoxWeek/config.h").read_text(encoding="utf-8")
    def _defW(n, tekst):
        m = re.search(rf'#\s*define\s+{n}\s+"?([^"\s]+)"?', tekst)
        return m.group(1) if m else None
    _zle = []
    for _n in ('OTA_BIN_FILE', 'OTA_JSON_FILE'):
        _d, _t = _defW(_n, cfg), _defW(_n, _cfgW)
        if not _t:      _zle.append(f'{_n}: nie ma w config.h pudelka tygodniowego')
        elif _t == _d:  _zle.append(f'{_n}: oba pudelka pobieraja "{_t}"')
        elif _wf_p.exists() and _t not in _wf_p.read_text(encoding='utf-8'):
            _zle.append(f'{_n}="{_t}" - automat go nie buduje')
    for _n in ('OTA_MIN_BIN_SIZE', 'OTA_MAX_BIN_SIZE', 'OTA_MAX_FAILS', 'OTA_BOOT_TRIES'):
        _d, _t = _defW(_n, cfg), _defW(_n, _cfgW)
        if _d != _t: _zle.append(f'{_n}: dzienne {_d}, tygodniowe {_t}')
    if 'TUTAJ_WPISZ_HASLO' not in _cfgW:
        _zle.append('config.h pudelka tygodniowego nie ma placeholdera hasla')
    # ── PROG BATERII CHODZI RAZEM Z POMIAREM BATERII ────────────────
    #
    # W pudelku tygodniowym prog jest ZNIESIONY (D138), bo pomiar baterii
    # jest zepsuty i nie wiemy dlaczego (D136). Prog oparty na liczbie,
    # ktorej nie rozumiemy, nie jest zabezpieczeniem, tylko loteria -
    # a stawka jest asymetryczna: zle zablokowana aktualizacja odcina
    # JEDYNA zdalna droge naprawy, zle przepuszczona kosztuje 1-2 mAh.
    #
    # Ta kontrola pilnuje, ze te dwie rzeczy chodza razem: prog wraca
    # dopiero wtedy, gdy ktos naprawi pomiar. Zmiana jednego bez drugiego
    # zatrzyma sie tutaj i kaze przeczytac, o co chodzilo.
    _prog = _defW('OTA_MIN_BATT_PCT', _cfgW)
    if _prog is None:
        _zle.append('OTA_MIN_BATT_PCT: nie ma w config.h pudelka tygodniowego')
    elif _prog != '0':
        _zle.append(f'prog baterii dla OTA to {_prog}, a pomiar baterii w tym '
                    f'pudelku nadal jest niewyjasniony (D136/D138)')
    # Ekran nie ma prawa obiecywac progu, ktorego pudelko nie stosuje.
    _ota_txt = re.search(r'const warunki = profilTydzien\(\) \? `(.*?)` : `', _html, re.S)
    if not _ota_txt:
        _zle.append('nie znalazlem warunkow aktualizacji osobnych dla pudelek')
    elif '% baterii' in _ota_txt.group(1):
        _zle.append('ekran obiecuje prog baterii, ktorego pudelko tygodniowe nie stosuje')

    if _zle:
        bad += 1
        print('  BLAD aktualizacja pudelka tygodniowego:')
        for _z in _zle: print('       ' + _z)
    else:
        print('  OK   pudelko tygodniowe pobiera wlasny plik, bez progu baterii')

    # ── APLIKACJA I PUDELKO MUSZA WSKAZYWAC TEN SAM PLIK ────────────
    #
    # Aplikacja pokazuje "jest nowa wersja" na podstawie opisu, ktory
    # pobiera SAMA; pudelko pobiera program na podstawie opisu, ktory
    # pobiera SAMO. Gdyby te dwie nazwy sie rozjechaly, ekran mowilby
    # o jednej wersji, a pudelko sciagaloby inna - w najgorszym razie
    # program DRUGIEGO pudelka, czyli dwie cegly.
    _pud = dict(re.findall(r'id:"(pillbox\d+)",[^}]*?fw:"([^"]+)"', _html, re.S))
    _kfg = {'pillbox01': cfg, 'pillbox02': _cfgW}
    _zle = []
    for _id, _tekst in _kfg.items():
        _wFirmware = _defW('OTA_JSON_FILE', _tekst)
        _wAplikacji = _pud.get(_id)
        _devId = _defW('DEVICE_ID', _tekst)
        if _devId != _id:
            _zle.append(f'{_id}: config.h mowi DEVICE_ID="{_devId}"')
        if not _wAplikacji:
            _zle.append(f'{_id}: aplikacja nie zna pliku opisu wersji')
        elif _wAplikacji != _wFirmware:
            _zle.append(f'{_id}: aplikacja czyta "{_wAplikacji}", pudelko "{_wFirmware}"')
    if _zle:
        bad += 1
        print('  BLAD aplikacja i pudelko o roznych programach:')
        for _z in _zle: print('       ' + _z)
    else:
        print(f'  OK   aplikacja i pudelka wskazuja te same programy ({len(_kfg)})')

    # ── OKABLOWANIE AKTUALIZACJI W PUDELKU TYGODNIOWYM ──────────────
    #
    # Audyt firmware (krok 3/10) czyta wylacznie pudelko dzienne, a to sa
    # te jego reguly, ktorych zlamanie w pudelku tygodniowym kosztowaloby
    # dokladnie tyle samo. Kazda byla w pudelku dziennym BLEDEM, nie
    # przewidywaniem: minuta radia miedzy otwarciem a zapisem (zasada 11),
    # ufanie pamieci sprzed wybudzenia (D62), licznik prob podniesiony po
    # pobraniu zamiast przed (aktualizacja wieszajaca plytke probowalaby
    # w kolko), i dobra wersja cofajaca sie, bo nikt jej nie potwierdzil.
    _bez = lambda t: re.sub(r'//[^\n]*', '', re.sub(r'/\*.*?\*/', '', t, flags=re.S))
    _kod = _bez(src)
    _sen = _cialo(src, 'void idzSpac') or ''
    _sen = _bez(_sen)
    _setup = _bez(_cialo(src, 'void setup') or '')
    _spr = _bez(_cialo(src, 'void otaSprobuj') or '')
    _zle = []

    if len(re.findall(r'\botaSprobuj\s*\(\s*\)', _kod)) != 2:   # definicja + jedno wolanie
        _zle.append('otaSprobuj() nie jest wolane z dokladnie jednego miejsca')
    elif 'otaSprobuj()' not in _sen:
        _zle.append('otaSprobuj() nie jest wolane z idzSpac()')
    _p, _s2 = _sen.find('otaPotwierdzDzialanie()'), _sen.find('otaSprobuj()')
    if _p < 0 or _s2 < 0 or _p > _s2:
        _zle.append('otaPotwierdzDzialanie() nie idzie PRZED otaSprobuj()')
    if 'otaZlecenieWBazie' not in _spr:
        _zle.append('otaSprobuj() ufa pamieci sprzed wybudzenia zamiast dopytac baze')
    # Licznik proby musi rosnac MIEDZY decyzja a pobraniem. Pierwsze
    # wystapienie `otaZanotujProbe` w tej funkcji lezy na sciezce "nie
    # udalo sie pobrac opisu" i jest PRZED `otaWgraj()` zawsze - wiec
    # szukanie po nim nie pilnowaloby niczego. Patrzymy dokladnie w okno
    # miedzy zerowaniem licznika a pobraniem.
    _wg  = _spr.find('otaWgraj(')
    _dec = _spr.find('OTA_MAX_FAILS')
    if _wg < 0 or _dec < 0 or _dec > _wg:
        _zle.append('nie rozpoznaje sciezki pobierania w otaSprobuj()')
    elif 'otaZanotujProbe' not in _spr[_dec:_wg]:
        _zle.append('licznik proby podnoszony PO pobraniu, nie przed')
    if 'otaSprawdzPoStarcie()' not in _setup:
        _zle.append('setup() nie sprawdza, czy swiezo wgrana wersja w ogole wstaje')
    if _zle:
        bad += 1
        print('  BLAD okablowanie aktualizacji w pudelku tygodniowym:')
        for _z in _zle: print('       ' + _z)
    else:
        print('  OK   aktualizacja tygodniowego rusza tylko ze snu, po zapisie zdarzenia')

    # ── PORTAL WiFi: CZTERY RZECZY, KTORE ODCINAJA PUDELKO ──────────
    #
    # Portal jest jedyna droga do pudelka, ktora nie potrzebuje ani sieci,
    # ani bazy, ani kabla (zasada 9). Kazda z tych kontroli pilnuje bledu,
    # ktory by te droge zamknal - albo zamienil pudelko w cegle piszczaca
    # przez petle wybudzen (B30, tylko z drugiego pinu).
    _portal = _bez(_cialo(src, 'void startPortalWifi') or '')
    _zle = []

    # 1. Siec zapisujemy DOPIERO po udanym polaczeniu. Odwrotna kolejnosc
    #    kasuje dzialajaca siec na rzecz literowki - i nie ma jak wrocic.
    _polaczono = _portal.find('WiFi.status() != WL_CONNECTED')
    _zapis     = _portal.find('nvs.putString("ssid"')
    if _zapis < 0:
        _zle.append('portal nie zapisuje sieci do pamieci trwalej')
    elif _polaczono < 0 or _zapis < _polaczono:
        _zle.append('siec zapisywana PRZED potwierdzeniem polaczenia (zasada 9)')

    # 2. Haslo urzadzenia podane w portalu jest KANDYDATEM: gdy baza je
    #    odrzuci, musi zniknac. Zostawione blokuje to poprawne z config.h.
    if 'nvs.remove("haslo")' not in _portal:
        _zle.append('portal nie kasuje hasla, ktorego baza nie przyjela')

    # 3. Przycisku WCISNIETEGO nie uzbrajamy - obudzilby uklad w tej samej
    #    milisekundzie, w ktorej zasnal.
    if 'przyciskWolny' not in _sen or 'gpio_pullup_en((gpio_num_t)PIN_PRZYCISK)' not in _sen:
        _zle.append('przycisk uzbrajany bez podciagniecia albo bez sprawdzenia stanu')
    if 'gpio_hold_en((gpio_num_t)PIN_PRZYCISK)' not in _sen:
        _zle.append('podciagniecie przycisku nie przezyje snu (brak zatrzasku)')

    # 4. Placeholder sieci musi zgadzac sie z config.h z repozytorium -
    #    inaczej pudelko wgrane "jak jest" nie otworzy portalu nigdy.
    _ph_ino = re.search(r'#\s*define\s+SSID_PLACEHOLDER\s+"([^"]+)"', src)
    _ph_cfg = re.search(r'#\s*define\s+WIFI_SSID\s+"([^"]+)"', _cfgW)
    if not _ph_ino or not _ph_cfg:
        _zle.append('brak SSID_PLACEHOLDER albo WIFI_SSID')
    elif _ph_ino.group(1) != _ph_cfg.group(1):
        _zle.append(f'placeholder sieci: kod ma "{_ph_ino.group(1)}", '
                    f'config.h "{_ph_cfg.group(1)}"')
    # 5. Nazwa sieci i haslo portalu stoja W INSTRUKCJI W APLIKACJI -
    #    na prosbe Kuby ("zapisz gdzies to haslo i nazwe sieci"). Telefon
    #    ma sie przy sobie zawsze, kartki nie. Instrukcja, ktora podaje
    #    inne haslo niz pudelko, jest gorsza niz jej brak: czlowiek stoi
    #    nad pudelkiem, wpisuje i nie rozumie, dlaczego nie wchodzi.
    for _n, _co in (('AP_SSID', 'nazwa sieci'), ('AP_PASS', 'haslo')):
        _v = _defW(_n, _cfgW)
        if not _v:
            _zle.append(f'{_n}: nie ma w config.h')
        elif _v not in _html:
            _zle.append(f'{_co} portalu ("{_v}") nie stoi w Instrukcji w aplikacji')
    if _zle:
        bad += 1
        print('  BLAD portal WiFi pudelka tygodniowego:')
        for _z in _zle: print('       ' + _z)
    else:
        print('  OK   portal WiFi: siec po potwierdzeniu, przycisk bez petli, haslo w Instrukcji')

    # ── POWIADOMIENIA NA TELEFON: ZASADA 12 W CALOSCI ──────────────
    #
    # Token bota jest sekretem tej samej klasy co haslo do WiFi: kto go ma,
    # pisze w imieniu bota. Zasada 12 z CLAUDE.md mowi o nim szesc rzeczy
    # i kazda z nich ma tu swoja kontrole - w pudelku dziennym pilnuje ich
    # audyt firmware, ktory czyta wylacznie tamten szkic.
    _zle = []
    _tgSlij = _bez(_cialo(src, 'void tgWyslijZalegle') or '')
    _tgTekst = _bez(_cialo(src, 'bool tgWyslijTekst') or '')
    _status  = _bez(_cialo(src, 'void wyslijStatus') or '')
    _ustaw   = _bez(_cialo(src, 'void pobierzUstawienia') or '')

    # 1. Ta sama decyzja co w pudelku dziennym, znak w znak.
    _a = _cialo(ino, 'TgDecyzja tgDecyzja')
    _b = _cialo(src, 'TgDecyzja tgDecyzja')
    if not _a or not _b:
        _zle.append('nie znalazlem tgDecyzja() w obu szkicach')
    elif _sameKod(_a) != _sameKod(_b):
        _zle.append('tgDecyzja() rozjechala sie z pudelkiem dziennym')

    # 2-3. Wysylka rusza WYLACZNIE z idzSpac() i jako PIERWSZA z rzeczy
    #      przed snem: udana aktualizacja konczy sie restartem, wiec
    #      wiadomosc za nia nie poszlaby wcale.
    if len(re.findall(r'\btgWyslijZalegle\s*\(\s*\)', _kod)) != 2:
        _zle.append('tgWyslijZalegle() nie jest wolane z dokladnie jednego miejsca')
    elif 'tgWyslijZalegle()' not in _sen:
        _zle.append('tgWyslijZalegle() nie jest wolane z idzSpac()')
    else:
        _t, _o = _sen.find('tgWyslijZalegle()'), _sen.find('otaSprobuj()')
        if _o >= 0 and _t > _o:
            _zle.append('wiadomosc idzie PO aktualizacji - restart by ja zjadl')

    # 4. Przy pustej skrzynce wychodzimy PRZED wlaczeniem radia.
    _dec, _radio = _tgSlij.find('tgDecyzja('), _tgSlij.find('wifiPolacz()')
    if _dec < 0:
        _zle.append('tgWyslijZalegle() nie pyta tgDecyzja()')
    elif _radio >= 0 and _radio < _dec:
        _zle.append('radio wlaczane przed sprawdzeniem, czy jest co wysylac')

    # 5. Powiadomienie za stare KASUJEMY zamiast wysylac - jedyny wyjatek
    #    od zasady 6 w tym obszarze, i nie dotyczy danych o leku.
    _i = _tgSlij.find('TG_ZA_STARE')
    if _i < 0 or 'rtcTgSlot = -1' not in _tgSlij[_i:_i+300].replace('rtcTgSlot   = -1', 'rtcTgSlot = -1'):
        _zle.append('za stare powiadomienie nie jest kasowane')

    # 6. TOKEN NIE TRAFIA ANI DO LOGU, ANI DO STATUSU. Aplikacja dostaje
    #    wylacznie "jest/nie ma" i powod - token czyta caly dostep do bazy.
    for _l in _kod.split('\n'):
        if 'tgTok' in _l and ('LOG(' in _l or 'snprintf(rtcTg' in _l):
            _zle.append('token bota trafia do logu albo do statusu')
            break
    if 'tgTok' in _status or 'token' in _status:
        _zle.append('token bota jest w statusie wysylanym do bazy')
    if re.search(r'LOG\([^)]*\btoken\b', _tgTekst):
        _zle.append('adres z tokenem trafia do logu')

    # 7. Token kasujemy z bazy DOPIERO po potwierdzonym zapisie w NVS
    #    (zasada 9, ta sama co przy hasle WiFi).
    _u = _ustaw.find('tgUtrwal(')
    _d2 = _ustaw.find('config/tgNowy.json')
    if _u < 0 or _d2 < 0 or _u > _d2:
        _zle.append('token kasowany z bazy bez potwierdzonego zapisu w NVS')

    if _zle:
        bad += 1
        print('  BLAD powiadomienia pudelka tygodniowego:')
        for _z in _zle: print('       ' + _z)
    else:
        print('  OK   Telegram: token tylko w NVS, wysylka pierwsza i tylko ze snu')

    # ── STATUS MUSI NIESC POLA, KTORE APLIKACJA NAPRAWDE CZYTA ──────
    #
    # Zgloszenie Kuby: "w aplikacji nie pokazuje sie, ze jest otwarte, jak
    # jest otwarte". Przyczyny byly DWIE i obie tego samego rodzaju:
    # `boxOpen` nie bylo wysylane w ogole, a chwile "ostatnio widziane"
    # pudelko slalo jako `ts` - pole, ktorego aplikacja nie czyta nigdzie.
    # Czyta `lastSeen`, i to w kilkunastu miejscach naraz.
    #
    # Jedno zle nazwane pole daje tyle objawow, ile miejsc je czyta - i ani
    # jednego bledu po drodze. Dlatego ta kontrola porownuje NAZWY: bierze
    # pola, po ktore aplikacja siega w statusie, i sprawdza, ze pudelko
    # tygodniowe je wysyla.
    _st = _bez(_cialo(src, 'bool wyslijStatus') or '')
    _brak = [f for f in ('lastSeen', 'boxOpen', 'fw', 'rssi', 'queue')
             if f'doc["{f}"]' not in _st]
    # Pole czytane przez aplikacje, ktorego nikt nie wysyla, to ekran
    # mowiacy "nigdy" bez zadnego powodu.
    # `st.pole` i `st?.pole` to to samo odczytanie - wzorzec bez `?`
    # przegapial polowe aplikacji i mowil "nie czyta" o polu, ktore czyta.
    _czytane = set(re.findall(r'st\??\.([A-Za-z][A-Za-z0-9]*)', _html))
    if 'lastSeen' not in _czytane:
        _brak.append('aplikacja nie czyta juz lastSeen - sprawdz, czy ta kontrola ma sens')
    # Diagnostyka czujnika baterii ma dojechac DO TELEFONU, nie do logu
    # przez kabel (D142): "nie bede patrzyl na monitor, zobacze
    # w aplikacji". Pole wyslane pod nazwa, ktorej aplikacja nie czyta,
    # to dokladnie blad D137 - wtedy kosztowal baner o otwartej klapce.
    for _f in ('gauge', 'voltDz', 'charging', 'crate'):
        if f'doc["{_f}"]' not in _st:
            _brak.append(f'pudelko nie wysyla {_f}')
        elif _f not in _czytane:
            _brak.append(f'{_f} jedzie do bazy, ale aplikacja go nie czyta')
    # LADOWANIA NIE ZGADUJEMY (D143). `charging` wolno wyslac TYLKO wtedy,
    # gdy czujnik podal tempo - bez niego pudelko tygodniowe nie ma czym
    # poznac kabla. Falszywe "laduje sie" kazaloby czlowiekowi odejsc od
    # pudelka, ktore sie nie laduje.
    _tz, _ch = _st.find('battTempoZnane'), _st.find('doc["charging"]')
    if _ch >= 0 and (_tz < 0 or _tz > _ch):
        _brak.append('charging jedzie bez sprawdzenia, czy czujnik podal tempo')
    if _brak:
        bad += 1
        print('  BLAD status pudelka tygodniowego bez pol, ktore czyta aplikacja:', _brak)
    else:
        print('  OK   status tygodniowego niesie pola czytane przez aplikacje')

    # ── CZUJNIK BATERII JEST OPCJONALNY, NIE WYMAGANY (D139) ────────
    #
    # Kuba dolozyl MAX17048, bo dzielnik na tej plytce melduje 2,32 V
    # i nie wiemy dlaczego (D136). Czujnik ma PIERWSZENSTWO, ale ten sam
    # program musi chodzic takze na plytce bez niego - inaczej jedno
    # odejscie przewodu zamienia dzialajace pudelko w martwe.
    #
    # Trzy rzeczy, kazda byla do pomylenia: kolejnosc zrodel, zapas przy
    # milczacym czujniku i to, ze aplikacja widzi, KTORE zrodlo dalo
    # liczbe. Bez tego trzeciego "62%" z czujnika i z dzielnika wygladaja
    # identycznie, a to one rozstrzygaja, czy czujnik w ogole gada.
    #
    # Mierzymy to po ZAPISIE do battVolt, nie po samym wywolaniu ADC
    # (D141). Od 0.11.0 dzielnik czytamy TAKZE w galezi czujnika - jako
    # punkt porownania do logu - wiec "pierwszy analogRead w funkcji"
    # przestal znaczyc "zapas". Liczy sie to, co nadpisuje wynik.
    _bat = _bez(_cialo(src, 'void czytajBaterie') or '')
    _zle = []
    if 'float dzielnikVolt' not in _bez(src):
        _zle.append('nie ma osobnego dzielnikVolt() - pomiar stoi w dwoch kopiach')
    _g = _bat.find('gaugeCzytaj(')
    _d = _bat.find('battVolt = dzielnikVolt()')
    _r = _bat.find('return;', _g) if _g >= 0 else -1
    if _g < 0:
        _zle.append('czytajBaterie() nie pyta czujnika w ogole')
    elif _d < 0:
        _zle.append('czytajBaterie() nie ma juz zapasu w dzielniku')
    elif _g > _d:
        _zle.append('dzielnik ma pierwszenstwo przed czujnikiem - odwrotnie niz trzeba')
    elif _r < 0 or _r > _d:
        _zle.append('odczyt z czujnika nie konczy funkcji - dzielnik go nadpisze')
    if 'doc["battSrc"]' not in _st:
        _zle.append('status nie mowi, ktore zrodlo dalo pomiar')
    if 'const OPIS_BATT_SRC' not in _html or 'wiersz("Pomiar baterii"' not in _html:
        _zle.append('aplikacja nie pokazuje zrodla pomiaru w Urzadzeniu')
    # Odczyt dwoch bajtow w JEDNYM wyrazeniu ma w C++ nieokreslona
    # kolejnosc - bajty potrafia wyjsc odwrotnie przy zmianie rdzenia.
    # (na KODZIE bez komentarzy - komentarz obok tej linii pokazuje
    #  wlasnie ten zly zapis jako przestroge i zglaszalby sam siebie)
    if re.search(r'Wire\.read\(\)[^;]*Wire\.read\(\)', _kod):
        _zle.append('dwa Wire.read() w jednym wyrazeniu - kolejnosc nieokreslona')
    if _zle:
        bad += 1
        print('  BLAD czujnik baterii w pudelku tygodniowym:')
        for _z in _zle: print('       ' + _z)
    else:
        print('  OK   czujnik baterii ma pierwszenstwo, dzielnik zostaje zapasem')

    # ------------------------------------------------------------------
    # ILE PRZYPOMNIEN POZWALA USTAWIC APLIKACJA, A ILE PUDELKO ZAPAMIETA
    # (D145). Do 0.14.0 pudelko tygodniowe mialo SLOTOW_MAX 4, a aplikacja
    # MAX_PRZYPOMNIEN 12: pokazywala szesc godzin, zapisywala szesc do
    # bazy, a pudelko po cichu znalo cztery. Cicha rozbieznosc miedzy
    # ekranem a urzadzeniem to ta sama rodzina co B9/B10.
    _zle = []
    _m = re.search(r'#define\s+SLOTOW_MAX\s+(\d+)', _cfgW)
    _a = re.search(r'const MAX_PRZYPOMNIEN = (\d+)', _html)
    _d = re.search(r"String slots\[(\d+)\]", ino)
    if not _m:
        _zle.append('SLOTOW_MAX nie stoi w config.h pudelka tygodniowego')
    if not _a:
        _zle.append('nie znalazlem MAX_PRZYPOMNIEN w aplikacji')
    if _m and _a and int(_a.group(1)) > int(_m.group(1)):
        _zle.append(f'aplikacja daje ustawic {_a.group(1)} przypomnien, '
                    f'a tygodniowe zapamieta {_m.group(1)}')
    if _d and _a and int(_a.group(1)) > int(_d.group(1)):
        _zle.append(f'aplikacja daje ustawic {_a.group(1)} przypomnien, '
                    f'a dzienne zapamieta {_d.group(1)}')
    # Maska wyczerpanych slotow musi miescic KAZDY slot - przy uint8_t
    # dwunasty bit wypadal poza typ i slot nigdy by sie nie zamknal.
    _mask = re.search(r'RTC_DATA_ATTR\s+(\w+)\s+rtcAlarmMaska', _kod)
    _bity = {'uint8_t': 8, 'uint16_t': 16, 'uint32_t': 32}
    if not _mask:
        _zle.append('nie znalazlem rtcAlarmMaska')
    elif _m and _bity.get(_mask.group(1), 0) < int(_m.group(1)):
        _zle.append(f'maska alarmow jest {_mask.group(1)}, a slotow jest {_m.group(1)}')
    if _zle:
        bad += 1
        print('  BLAD liczba przypomnien rozjechala sie miedzy aplikacja a pudelkiem:')
        for _z in _zle: print('       ' + _z)
    else:
        print('  OK   tyle przypomnien, ile aplikacja daje ustawic, pudelka zapamietaja')

    # ------------------------------------------------------------------
    # „OTWARTE" MELDUJEMY ZAWSZE, NIE TYLKO PRZY NOWEJ DAWCE (B31).
    #
    # Status z `boxOpen` wychodzil wylacznie ze sciezki zapisu zdarzenia,
    # a ta ma trzy wyjscia, ktore jej nie dotykaja: ta sama komora juz
    # dzis zgloszona, kilka klapek naraz (napelnianie) i komora
    # nierozpoznana. Pudelko wiedzialo, ze klapka jest otwarta, i nie
    # mowilo o tym nikomu - a przy drugim otwarciu tej samej komory tego
    # samego dnia baner nie mial jak sie zapalic ani razu.
    #
    # Odrzucenie powtorki dotyczy DANYCH o leku, `boxOpen` dotyczy STANU
    # urzadzenia. Galaz "obudzila nas klapka" musi zglosic stan sama.
    # Granice bierzemy z KODU, nie z komentarzy - `_kod` jest bez nich.
    _b = _kod.find('powod == ESP_SLEEP_WAKEUP_TIMER')
    _a = _kod.rfind('powod == ESP_SLEEP_WAKEUP_GPIO', 0, _b) if _b > 0 else -1
    if _a < 0 or _b < 0 or _b < _a:
        bad += 1
        print('  BLAD nie znalazlem galezi wybudzenia w setup()')
    elif 'zglosKlapki(' not in _kod[_a:_b]:
        bad += 1
        print('  BLAD wybudzenie klapka nie melduje stanu klapki:')
        print('       "otwarte" wyszloby tylko przy nowej dawce (B31)')
    else:
        print('  OK   otwarcie melduje sie takze bez nowej dawki')

    # ------------------------------------------------------------------
    # ZADNEGO DNIA TYGODNIA PRZY KOMORZE (D140).
    #
    # Do 0.10.0 drabinka nadawala komorom dni: komora 0 byla "PON", a
    # powiadomienie na telefon pisalo "nikt nie otworzyl klapki PON".
    # Kuba: "chce zrezygnowac z tych dni, bo to i tak bez sensu, jak sie
    # zmienia polaczenie". Przypisanie zylo WYLACZNIE w kolejnosci
    # rezystorow - czego pudelko nie umie sprawdzic, a mowilo jak fakt.
    # Wiadomosc wskazujaca ZLA przegrodke jest gorsza od tej, ktora nie
    # wskazuje zadnej: czlowiek bierze dawke z innego dnia.
    #
    # Numer komory ZOSTAJE (rozroznialnosc jest potrzebna: jedna klapka
    # kontra kilka naraz, i ta sama komora drugi raz tego samego dnia) -
    # odpada tylko jego tlumaczenie na dzien.
    _zle = []
    if re.search(r'"(PON|WT|SR|CZW|PT|SOB|ND)"', _kod):
        _zle.append('skroty dni tygodnia wrocily do kodu')
    for _f in ('komoraDoby', 'rtcTgKomora'):
        if _f in _kod:
            _zle.append(f'{_f} wrocilo - to byla droga od numeru komory do dnia')
    if 'tgTekstNieodebrane(int slot)' not in _kod:
        _zle.append('tgTekstNieodebrane() bierze znowu wiecej niz numer przypomnienia')
    if _zle:
        bad += 1
        print('  BLAD komora nie moze znowu znaczyc dnia tygodnia:')
        for _z in _zle: print('       ' + _z)
    else:
        print('  OK   komora ma numer, nie dzien tygodnia')

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
