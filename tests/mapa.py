#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generuje MAPA.md - spis tresci trzech duzych plikow z numerami linii.
Powod: index.html to ~130 tys. tokenow, PillBox.ino ~89 tys.  Wczytanie
ktoregokolwiek w calosci zjada wiecej kontekstu niz cale zadanie.  Majac
zakres linii czyta sie 'sed -n 620,700p' zamiast szukac po omacku.
Mapa jest GENEROWANA (run_all.sh robi to sam) - nie poprawiaj jej recznie."""
import re, os, sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
out = []
W = out.append


def czytaj(p):
    return open(p, encoding='utf-8', errors='replace').read().split('\n')


def zakresy(punkty, koniec):
    """[(linia, etykieta)] -> [(od, do, etykieta)]"""
    r = []
    for i, (n, e) in enumerate(punkty):
        do = punkty[i + 1][0] - 1 if i + 1 < len(punkty) else koniec
        r.append((n, do, e))
    return r


# ---------------------------------------------------------------- index.html
L = czytaj('index.html')
KRESKA = '\u2550'   # ═ , z ktorej zrobione sa naglowki sekcji w CSS i JS


def naglowek_ramkowy(L, i):
    """Tytul sekcji z ramki /* ═══ ... — w tej samej linii albo w nastepnej."""
    l = L[i - 1]
    if not l.lstrip().startswith('/*') or KRESKA not in l: return None
    t = l.strip('/* \t').strip(KRESKA + ' */')
    if t.strip(): return t.strip()
    if i < len(L):
        t = L[i].lstrip(' *\t').strip()
        if t and KRESKA not in t: return t
    return None


sekcje = []
for i, l in enumerate(L, 1):
    m = re.search(r'<section id="(tab-[\w-]+)"', l)
    if m: sekcje.append((i, m.group(1)))
    elif re.match(r'\s*<style', l): sekcje.append((i, 'CSS — poczatek'))
    elif re.match(r'\s*<script', l) and 'src=' not in l: sekcje.append((i, 'JS — poczatek'))
    else:
        t = naglowek_ramkowy(L, i)
        if t: sekcje.append((i, t))
sekcje.sort()

# funkcje JS: deklaracje najwyzszego poziomu
fun = []
for i, l in enumerate(L, 1):
    m = (re.match(r'(?:async )?function (\w+)', l)
         or re.match(r'const (\w+) = (?:async )?\(', l)
         or re.match(r'window\.(\w+) = (?:async )?(?:function|\()', l))
    if m: fun.append((i, m.group(1)))

W('## `index.html` — %d linii, ~130 tys. tokenow\n' % len(L))
W('Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja')
W('pozycjom w pasku nawigacji i podekranom Ustawien.\n')
W('| od | do | co |')
W('|---|---|---|')
for a, b, e in zakresy(sekcje, len(L)):
    W(f'| {a} | {b} | {e} |')

W('\n**Funkcje** (%d) — nazwa i linia deklaracji:\n' % len(fun))
biez = None
for a, b, e in zakresy(sekcje, len(L)):
    moje = [f'`{n}`&nbsp;{i}' for i, n in fun if a <= i <= b]
    if moje:
        W(f'*{e}* — ' + ', '.join(moje) + '\n')

# --------------------------------------------------------------- PillBox.ino
for sciezka in ('firmware/PillBox/PillBox.ino', 'firmware/PillBoxTest/PillBoxTest.ino'):
    L = czytaj(sciezka)
    blok = []
    for i, l in enumerate(L, 1):
        if l.startswith('/* ====') and i < len(L):
            tyt = L[i].lstrip(' *').strip()
            blok.append((i, tyt))
    fun = []
    for i, l in enumerate(L, 1):
        m = re.match(r'(?:static )?(?:void|bool|int|uint\d+_t|int\d+_t|long|unsigned|float|size_t|String|const char\s*\*)\s+\*?(\w+)\s*\(', l)
        if m and not l.rstrip().endswith(';'): fun.append((i, m.group(1)))
    W(f'\n---\n\n## `{sciezka}` — {len(L)} linii\n')
    W('| od | do | blok |')
    W('|---|---|---|')
    for a, b, e in zakresy(blok, len(L)):
        W(f'| {a} | {b} | {e} |')
    W(f'\n**Funkcje** ({len(fun)}):\n')
    for a, b, e in zakresy(blok, len(L)):
        moje = [f'`{n}`&nbsp;{i}' for i, n in fun if a <= i <= b]
        if moje: W(f'*{e}* — ' + ', '.join(moje) + '\n')

# ------------------------------------------------------------------ config.h
L = czytaj('firmware/PillBox/config.h')
grupy = [(i, l.lstrip(' *').strip()) for i, l in enumerate(L, 1)
         if l.startswith('/* ---') or l.startswith('// ---')]
W(f'\n---\n\n## `firmware/PillBox/config.h` — {len(L)} linii\n')
if grupy:
    W('| linia | grupa |')
    W('|---|---|')
    for i, e in grupy[:60]:
        W(f'| {i} | {e} |')

naglowek = """# Mapa kodu — gdzie co stoi

**Plik jest generowany** (`python3 tests/mapa.py`, robi to tez `tests/run_all.sh`).
Nie poprawiaj recznie — zmiany przepadna przy najblizszym przebiegu testow.

Po co: `index.html` i `PillBox.ino` maja razem ~220 tys. tokenow. Wczytanie
ktoregokolwiek w calosci kosztuje wiecej niz cale zadanie, ktore go dotyczy.
Majac zakres linii czyta sie fragment:

```bash
sed -n '2800,2960p' index.html          # jeden obszar
grep -n "nazwaFunkcji" index.html       # gdy znasz nazwe
```

"""
open('MAPA.md', 'w', encoding='utf-8').write(naglowek + '\n'.join(out) + '\n')
print('MAPA.md: %d znakow' % os.path.getsize('MAPA.md'))
