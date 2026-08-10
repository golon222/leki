#!/usr/bin/env python3
"""
Odtwarza to, co Arduino IDE robi ze szkicem .ino PRZED kompilacja:
generuje prototypy wszystkich funkcji i wstawia je TUZ PRZED PIERWSZA
DEFINICJA FUNKCJI w pliku.

PO CO TO ISTNIEJE
-----------------
tests/kompiluj_firmware.sh kompiluje szkic jako .cpp, zeby ominac
generator prototypow (patrz D17: latana wersja ctags od Arduino kontra
zwykly exuberant-ctags). To bylo swiadome - ale kosztowalo miesiac.

Przez ten omijany etap ZADNA wersja firmware od 1.23.0 do 1.28.0 nie
dala sie wgrac z Arduino IDE, a skrypt kompilacji przez caly ten czas
meldowal sukces. Naprawa B5 dolozyla nvsPutStr() na samej gorze pliku
i przesunela punkt wstawiania prototypow PONAD `enum WakeReason` -
wygenerowany prototyp `const char* wakeName(WakeReason w);` odwolywal sie
do typu, ktorego kompilator jeszcze nie znal.

Objaw u uzytkownika:
    error: 'WakeReason' was not declared in this scope
przy linii, ktora wyglada na zupelnie poprawna.

Ten plik nie potrzebuje ctags. Prototypy wypisuje sam, wedlug tych samych
regul co Arduino:
  - prototyp powstaje dla kazdej definicji funkcji na poziomie pliku,
  - `static` zostaje w prototypie (inaczej byloby "declared extern and
    later static"),
  - wartosci domyslne argumentow sa obcinane (C++ nie pozwala podac ich
    dwa razy).
"""
import re
import sys

# Definicja funkcji na poziomie pliku: typ, nazwa, nawiasy, otwierajaca klamra.
DEF = re.compile(r'^[A-Za-z_][A-Za-z0-9_ *&]*[ *&]([A-Za-z_][A-Za-z0-9_]*)\([^;]*\)\s*\{')
# Slowa kluczowe, ktore wygladaja jak wywolanie funkcji, a nia nie sa.
NIE_FUNKCJA = ("if", "for", "while", "switch", "else", "return", "do", "catch")


def bez_domyslnych(sygnatura):
    """Obcina wartosci domyslne argumentow: (int a, String* r = nullptr) -> (int a, String* r)"""
    i = sygnatura.index("(")
    glowa, ogon = sygnatura[:i + 1], sygnatura[i + 1:sygnatura.rindex(")")]
    out, poziom, pomijaj = [], 0, False
    for c in ogon:
        if c in "([{":
            poziom += 1
        elif c in ")]}":
            poziom -= 1
        if c == "=" and poziom == 0:
            pomijaj = True
        elif c == "," and poziom == 0:
            pomijaj = False
        if not pomijaj:
            out.append(c)
    return glowa + "".join(out).rstrip() + ")"


def przetworz(zrodlo, nazwa_pliku="szkic.ino"):
    """Zwraca tresc .cpp taka, jaka zobaczy kompilator po obrobce Arduino IDE."""
    linie = zrodlo.split("\n")

    definicje = []
    for i, l in enumerate(linie):
        m = DEF.match(l)
        if m and m.group(1) not in NIE_FUNKCJA:
            definicje.append((i, l[:l.rindex("{")].rstrip()))

    if not definicje:
        return "#include <Arduino.h>\n" + zrodlo

    pierwsza = definicje[0][0]

    proto = []
    for i, sygnatura in definicje:
        proto.append('#line %d "%s"' % (i + 1, nazwa_pliku))
        proto.append(bez_domyslnych(sygnatura) + ";")
    proto.append('#line %d "%s"' % (pierwsza + 1, nazwa_pliku))

    return "\n".join(
        ["#include <Arduino.h>", '#line 1 "%s"' % nazwa_pliku]
        + linie[:pierwsza] + proto + linie[pierwsza:]
    )


def pierwsza_funkcja(zrodlo):
    """Numer linii (od 1) pierwszej definicji funkcji - tam ida prototypy."""
    for i, l in enumerate(zrodlo.split("\n")):
        m = DEF.match(l)
        if m and m.group(1) not in NIE_FUNKCJA:
            return i + 1
    return None


if __name__ == "__main__":
    src = open(sys.argv[1], encoding="utf8").read()
    out = przetworz(src, sys.argv[3] if len(sys.argv) > 3 else "szkic.ino")
    open(sys.argv[2], "w", encoding="utf8").write(out)
