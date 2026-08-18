#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Symulacja czasu pracy PillBoxa na baterii.

    python3 narzedzia/bateria.py            # trzy scenariusze + porownanie
    python3 narzedzia/bateria.py --szczegoly  # rozbicie kazdej pozycji

CZYM TO JEST, A CZYM NIE JEST
-----------------------------
To jest MODEL, nie pomiar. Zaden prad w tym pliku nie zostal zmierzony na
tej plytce - wszystkie pochodza z kart katalogowych (Seeed XIAO ESP32-C3,
Espressif ESP32-C3) albo z wyliczenia wprost z config.h. Dlatego kazda
wartosc ma trzy warianty (min / typ / max) i kazdy wynik podawany jest
jako WIDELKI, a nie jako jedna liczba.

Jedyne zaczepienie w rzeczywistosci: komentarz przy BATT_STEP_DOWN
w config.h mowi "realne zuzycie to okolo 1 punkt na dobe". Przy 470 mAh
znaczy to grubo ponad kwartal pracy - i model to potwierdza. To NIE jest
pomiar poboru, tylko obserwacja wskaznika, ale zgadza sie co do rzedu.

CO ZROBIC, ZEBY TO PRZESTALO BYC ZGADYWANIEM
--------------------------------------------
Wystarczy jeden pomiar: miliamperomierz (albo USB tester z rozdzielczoscia
uA) w szereg z ogniwem, odczyt w glebokim snie przy ZAMKNIETYM wieczku.
Ta jedna liczba zastepuje dwie najgrubsze niewiadome naraz (I_SEN + kontaktron)
i w scenariuszu 1 odpowiada za wiekszosc zuzycia. Wpisz ja w PRAD["sen_plyta"]
i ustaw PRAD["kontaktron"] na 0 - pomiar obejmuje juz obie pozycje.

ZALOZENIA ZACHOWANIA (wyczytane z PillBox.ino / config.h, nie wymyslone)
-----------------------------------------------------------------------
- W spoczynku uklad jest w deep sleep. Budzi go kontaktron albo timer.
- USE_INTERNAL_PULLS = 1, wiec przy ZAMKNIETYM wieczku kontaktron zwiera
  wewnetrzne podciagniecie ~45 kOhm do masy: ~73 uA non stop, przez cala
  dobe. To jest podane wprost w config.h i w scenariuszu 1 wychodzi
  DROZSZE niz sam uklad w glebokim snie.
- planNextSleep() budzi pudelko: minute po granicy doby lekowej (3:00,
  MIDNIGHT_CHECK) i przy kazdym slocie harmonogramu. Slot, przy ktorym
  dawka jest juz wzieta, konczy sie cisza - ale i tak robi pelny meldunek
  (galaz "else" w WAKE_TIMER: wifiConnect + fetchConfig + flushQueue +
  pushStatus).
- Otwarcie wieczka: sygnal, zapis dawki, reportEvent, czekanie na
  zamkniecie z RADIEM WLACZONYM (RADIO_OTWARTE_S = 600 s), po zamknieciu
  fetchConfig + pushLidState + pushStatus.
- Token Firebase zyje godzine i przezywa deep sleep (NVS + rtcTokenExp),
  wiec logowanie haslem idzie najwyzej raz na ~55 minut, a nie przy kazdym
  wybudzeniu. Przy 96 wybudzeniach na dobe to nie jest drobiazg.
"""

import sys
import math

SZCZEGOLY = "--szczegoly" in sys.argv

# =====================================================================
#  1. PRADY  [mA]
#     Zrodla:
#       sen_plyta   - Seeed: XIAO ESP32-C3 w deep sleep ~43 uA (cala
#                     plytka: rdzen 5 uA + stabilizator + uklad ladowania)
#       kontaktron  - wyliczone z config.h: 3,3 V / 45 kOhm = 73 uA,
#                     plynie TYLKO przy zamknietym wieczku
#       procesor    - ESP32-C3 modem-sleep, CPU 160 MHz
#       wifi_assoc  - skan + asocjacja + DHCP, srednia z pikow
#       wifi_ruch   - TLS + HTTP do Firebase, mieszanka RX/TX
#       radio_czuwa - CPU aktywny + WiFi w modem-sleep (WiFi.setSleep(true)),
#                     czyli stan przy otwartym wieczku i miedzy zapytaniami
#       buzzer      - piezo 23 mm, wartosc srednia w czasie trwania tonu
# =====================================================================
PRAD = {
    "sen_plyta":   {"min": 0.040, "typ": 0.043, "max": 0.055},
    "kontaktron":  {"min": 0.066, "typ": 0.073, "max": 0.082},
    "procesor":    {"min": 18.0,  "typ": 22.0,  "max": 26.0},
    "wifi_assoc":  {"min": 85.0,  "typ": 110.0, "max": 140.0},
    "wifi_ruch":   {"min": 70.0,  "typ": 90.0,  "max": 115.0},
    "radio_czuwa": {"min": 22.0,  "typ": 30.0,  "max": 42.0},
    "buzzer":      {"min": 6.0,   "typ": 12.0,  "max": 20.0},
}

# =====================================================================
#  2. CZASY  [s]  - ile trwa ktory kawalek wybudzenia
# =====================================================================
CZAS = {
    "rozruch":      {"min": 0.8, "typ": 1.5, "max": 2.5},   # setup, ADC, NVS
    "assoc":        {"min": 2.0, "typ": 3.5, "max": 7.0},   # polaczenie z AP + NTP
    "logowanie":    {"min": 1.5, "typ": 2.5, "max": 4.5},   # signInWithPassword (TLS)
    "meldunek":     {"min": 2.5, "typ": 4.0, "max": 7.0},   # fetchConfig+flush+pushStatus
    "zapis_dawki":  {"min": 2.0, "typ": 3.0, "max": 5.0},   # reportEvent
    "wieczko":      {"min": 8.0, "typ": 20.0, "max": 45.0}, # ile wieczko jest otwarte
    "meldunek_kon": {"min": 3.0, "typ": 5.0, "max": 8.0},   # fetchConfig+pushLid+pushStatus
}

TOKEN_ZYJE_S = 3300.0     # godzina minus TOKEN_MARGIN_S = 300 s

# =====================================================================
#  3. OGNIWO
#     pojemnosc_uzyteczna: LiPo oddaje do progu BATT_CUTOFF_V = 3,20 V
#       prawie cala pojemnosc, ale ogniwo sie starzeje, a przy 3,35 V
#       (BATT_SAFE_V) pudelko i tak przestaje wlaczac radio.
#     samorozladowanie: LiPo, %/miesiac od pojemnosci znamionowej.
# =====================================================================
OGNIWO = {
    "uzyteczne":        {"min": 0.80, "typ": 0.90, "max": 0.95},
    "samorozladowanie": {"min": 0.010, "typ": 0.020, "max": 0.030},
}

# Widelki liczymy uczciwie: wariant "pesymistyczny" bierze NAJWIEKSZE prady
# i NAJMNIEJ pojemnosci, "optymistyczny" odwrotnie. Nazwy wynikow dotyczą
# LICZBY DNI, wiec sa odwrocone wzgledem nazw pradow.
WARIANTY = {"ostroznie": "max", "typowo": "typ", "dobrze": "min"}


def p(nazwa, w):
    return PRAD[nazwa][w]


def c(nazwa, w):
    return CZAS[nazwa][w]


def mAh(prad_mA, sekundy):
    """Prad przez czas -> mAh."""
    return prad_mA * sekundy / 3600.0


def ogniwo(w, pojemnosc):
    """Ile mAh naprawde da sie wyjac i ile ucieka samo z siebie na dobe."""
    frakcja = OGNIWO["uzyteczne"]["min" if w == "max" else ("typ" if w == "typ" else "max")]
    samo = OGNIWO["samorozladowanie"][w]
    return pojemnosc * frakcja, pojemnosc * samo / 30.0


# =====================================================================
#  4. KLOCKI WYBUDZEN
# =====================================================================
def koszt_sesji_sieciowej(w, udzial_logowan=1.0, meldunek="meldunek"):
    """Jedno wybudzenie z radiem: rozruch, asocjacja, ewentualne logowanie,
       wymiana z baza. udzial_logowan < 1 = token z NVS jeszcze zyje."""
    return (mAh(p("procesor", w), c("rozruch", w))
            + mAh(p("wifi_assoc", w), c("assoc", w))
            + udzial_logowan * mAh(p("wifi_ruch", w), c("logowanie", w))
            + mAh(p("wifi_ruch", w), c(meldunek, w)))


def udzial_logowan(wybudzen_na_dobe):
    """Token zyje ~55 min i przezywa deep sleep. Przy rzadkich wybudzeniach
       logujemy sie za kazdym razem; przy czestych - raz na godzine."""
    logowan = 86400.0 / TOKEN_ZYJE_S            # ~26 na dobe
    return min(1.0, logowan / max(1.0, wybudzen_na_dobe))


# =====================================================================
#  5. SCENARIUSZE
#     Kazdy zwraca liste (nazwa pozycji, mAh na dobe).
# =====================================================================
def scenariusz_obecny(w, przypomnien=2, alarmow_na_tydzien=1.0):
    """Tak, jak urzadzenie dziala dzisiaj: deep sleep + wybudzenia z
       planNextSleep(). Wybudzenia timerem: granica doby (3:01) + kazdy
       slot harmonogramu. Do tego jedno otwarcie wieczka na dobe."""
    wybudzen = 1 + przypomnien + 1            # granica doby + sloty + wieczko
    ul = udzial_logowan(wybudzen)
    poz = []

    # Sen. Kontaktron plynie tylko przy zamknietym wieczku - odejmujemy
    # czas, przez ktory wieczko jest otwarte (i tak jest go tyle co nic).
    sekund_snu = 86400.0 - c("wieczko", w)
    poz.append(("deep sleep (plytka)", mAh(p("sen_plyta", w), 86400.0)))
    poz.append(("podciagniecie kontaktronu", mAh(p("kontaktron", w), sekund_snu)))

    poz.append((f"meldunki z timera ({1 + przypomnien}/dobe)",
                (1 + przypomnien) * koszt_sesji_sieciowej(w, ul)))

    # Otwarcie wieczka: sesja + zapis dawki + czekanie z radiem + status koncowy.
    wieczko = (koszt_sesji_sieciowej(w, ul, meldunek="zapis_dawki")
               + mAh(p("radio_czuwa", w), c("wieczko", w))
               + mAh(p("wifi_ruch", w), c("meldunek_kon", w)))
    poz.append(("otwarcie wieczka (dawka)", wieczko))

    # Dzwonienie: ALARM_WINDOW_S = 120 s czuwania, ton okolo 13% tego czasu
    # (3 x BEEP_MS 180 ms na BURST_GAP_MS 4 s).
    alarm = mAh(p("procesor", w), 120.0) + mAh(p("buzzer", w), 120.0 * 0.13)
    poz.append(("dzwonienie (%.1f x tydzien)" % alarmow_na_tydzien,
                alarm * alarmow_na_tydzien / 7.0))
    return poz


def scenariusz_bez_snu(w, radio_podtrzymane=True, co_minut=15):
    """1600 mAh i uklad NIGDY nie zasypia. Co `co_minut` robi pelne
       sprawdzenie. Reszte doby stoi wybudzony."""
    sprawdzen = 24 * 60 / co_minut
    poz = []
    if radio_podtrzymane:
        # Radio zostaje zalogowane w sieci (modem-sleep). Sprawdzenie to
        # samo zapytanie - nie ma sie do czego laczyc od nowa.
        czas_ruchu = sprawdzen * c("meldunek", w)
        ul = udzial_logowan(sprawdzen)
        czas_ruchu += sprawdzen * ul * c("logowanie", w)
        poz.append(("czuwanie z radiem w sieci",
                    mAh(p("radio_czuwa", w), 86400.0 - czas_ruchu)))
        poz.append((f"sprawdzenia ({sprawdzen:.0f}/dobe)",
                    mAh(p("wifi_ruch", w), czas_ruchu)))
    else:
        # Radio gasi sie miedzy sprawdzeniami, procesor dalej nie spi.
        czas_radia = sprawdzen * (c("assoc", w) + c("meldunek", w))
        poz.append(("czuwanie, radio zgaszone",
                    mAh(p("procesor", w), 86400.0 - czas_radia)))
        poz.append((f"laczenie i sprawdzenia ({sprawdzen:.0f}/dobe)",
                    sprawdzen * (mAh(p("wifi_assoc", w), c("assoc", w))
                                 + mAh(p("wifi_ruch", w), c("meldunek", w)))))
    return poz


def scenariusz_cykl(w, co_minut=15, czuwanie_s=60.0):
    """1600 mAh, deep sleep, ale wybudzenie co `co_minut` na `czuwanie_s`
       sekund pelnej pracy (siec, baza, wszystko) - i tak w kolo."""
    cykli = 24 * 60 / co_minut
    ul = udzial_logowan(cykli)
    sekund_czuwania = cykli * czuwanie_s
    sekund_snu = 86400.0 - sekund_czuwania

    # Ile z tej minuty naprawde zajmuje robota, a ile jest samym staniem.
    robota = c("rozruch", w) + c("assoc", w) + ul * c("logowanie", w) + c("meldunek", w)
    reszta = max(0.0, czuwanie_s - robota)

    poz = [
        ("deep sleep (plytka)", mAh(p("sen_plyta", w), sekund_snu)),
        ("podciagniecie kontaktronu", mAh(p("kontaktron", w), 86400.0)),
        (f"robota w kazdym cyklu ({cykli:.0f}/dobe)", cykli * koszt_sesji_sieciowej(w, ul)),
        ("reszta minuty czuwania", cykli * mAh(p("radio_czuwa", w), reszta)),
    ]
    return poz


# =====================================================================
#  6. LICZENIE I WYPIS
# =====================================================================
def policz(fn, pojemnosc, **kw):
    wynik = {}
    for etykieta, w in WARIANTY.items():
        poz = fn(w, **kw)
        zuzycie = sum(x for _, x in poz)
        uzyteczne, samo = ogniwo(w, pojemnosc)
        dni = uzyteczne / (zuzycie + samo)
        wynik[etykieta] = {
            "pozycje": poz, "zuzycie": zuzycie, "samo": samo,
            "uzyteczne": uzyteczne, "dni": dni,
        }
    return wynik


def dni_slownie(d):
    if d < 4:
        return f"{d * 24:.0f} godzin"
    if d < 60:
        return f"{d:.0f} dni"
    return f"{d:.0f} dni (~{d / 30.4:.1f} miesiaca)"


def wypisz(tytul, opis, pojemnosc, wynik):
    print()
    print("=" * 72)
    print(tytul)
    print("=" * 72)
    for linia in opis.strip().splitlines():
        print("  " + linia.strip())
    print(f"\n  ogniwo: {pojemnosc} mAh znamionowo")

    t = wynik["typowo"]
    if SZCZEGOLY:
        print("\n  rozbicie doby (wariant typowy):")
        for nazwa, ile in t["pozycje"]:
            udzial = 100.0 * ile / t["zuzycie"]
            print(f"    {nazwa:<42} {ile:8.3f} mAh/dobe  {udzial:5.1f}%")
        print(f"    {'samorozladowanie ogniwa':<42} {t['samo']:8.3f} mAh/dobe")
    else:
        naj = max(t["pozycje"], key=lambda x: x[1])
        print(f"  najwieksza pozycja: {naj[0]} ({100.0 * naj[1] / t['zuzycie']:.0f}% doby)")

    print(f"\n  zuzycie:   {t['zuzycie'] + t['samo']:.3f} mAh na dobe (typowo)")
    print(f"  do wziecia: {t['uzyteczne']:.0f} mAh z {pojemnosc} mAh")
    print(f"\n  >>> CZAS PRACY: {dni_slownie(t['dni'])}   (typowo)")
    print(f"      widelki:   {dni_slownie(wynik['ostroznie']['dni'])}"
          f"  ...  {dni_slownie(wynik['dobrze']['dni'])}")


def main():
    print("""
PillBox - symulacja czasu pracy na baterii
Model, nie pomiar. Prady z kart katalogowych; widelki obejmuja niepewnosc.""")

    w1 = policz(scenariusz_obecny, 470)
    wypisz("1. TERAZ  -  470 mAh, deep sleep, wybudzenia jak dzis", """
        Deep sleep przez cala dobe. Budzi sie: minute po granicy doby lekowej
        (3:01, MIDNIGHT_CHECK), przy kazdym z 2 przypomnien harmonogramu
        i raz przy otwarciu wieczka. Dzwonienie srednio raz w tygodniu.
        """, 470, w1)

    w2 = policz(scenariusz_bez_snu, 1600, radio_podtrzymane=True)
    wypisz("2. BEZ SNU  -  1600 mAh, uklad nigdy nie zasypia", """
        Procesor wybudzony non stop, radio zalogowane w sieci (modem-sleep),
        pelne sprawdzenie co 15 minut. Deep sleep w ogole nie wystepuje.
        """, 1600, w2)

    w2b = policz(scenariusz_bez_snu, 1600, radio_podtrzymane=False)
    wypisz("2b. BEZ SNU, RADIO GASZONE  -  1600 mAh (wariant scenariusza 2)", """
        To samo, ale radio wlacza sie tylko na sprawdzenie i zaraz gasnie.
        Procesor dalej nie spi ani sekundy.
        """, 1600, w2b)

    w3 = policz(scenariusz_cykl, 1600)
    wypisz("3. CO 15 MINUT NA MINUTE  -  1600 mAh, deep sleep + cykl", """
        Deep sleep, wybudzenie co 15 minut, minuta pelnej pracy (siec, baza,
        wszystko), z powrotem spac. 96 takich cykli na dobe.
        """, 1600, w3)

    w1b = policz(scenariusz_obecny, 1600)
    wypisz("4. DLA PORZADKU  -  1600 mAh, zachowanie jak dzis", """
        Nie pytales o to, ale to jedyna uczciwa miara tego, ile z roznicy
        bierze sie z samego ogniwa, a ile z tego, co pudelko robi.
        """, 1600, w1b)

    print("\n" + "=" * 72)
    print("PORÓWNANIE (wariant typowy)")
    print("=" * 72)
    tabela = [
        ("1. teraz, 470 mAh, deep sleep", w1),
        ("2. 1600 mAh, nigdy nie spi (radio w sieci)", w2),
        ("2b. 1600 mAh, nigdy nie spi (radio gaszone)", w2b),
        ("3. 1600 mAh, co 15 min na minute", w3),
        ("4. 1600 mAh, zachowanie jak dzis", w1b),
    ]
    for nazwa, wyn in tabela:
        t = wyn["typowo"]
        print(f"  {nazwa:<44} {t['zuzycie'] + t['samo']:7.2f} mAh/dobe"
              f"   {dni_slownie(t['dni']):>22}")
    print()
    baza = w1["typowo"]["dni"]
    _ = baza
    for nazwa, wyn in tabela[1:]:
        d = wyn["typowo"]["dni"]
        print(f"  {nazwa:<44} {d / baza:5.2f} x tyle co dzis")

    print("\n" + "=" * 72)
    print("NA CO TO JEST CZULE  (scenariusz 1, wariant typowy)")
    print("=" * 72)
    for opis, zmiana in wrazliwosc():
        d = zmiana["typowo"]["dni"]
        print(f"  {opis:<44} {dni_slownie(d):>22}"
              f"   = {100.0 * d / baza:.0f}% dzisiejszego czasu")

    print("\n" + "=" * 72)
    print("LADOWANIE  (prad 350 mA katalogowo, ~250 mA netto - NIE zmierzone)")
    print("=" * 72)
    for poj in (470, 1600):
        print(f"  {poj} mAh od zera do pelna:  ok. {poj / 250.0:.1f} h")
    print()


def wrazliwosc():
    """Co naprawde rusza wynikiem w scenariuszu 1. Liczone tym samym modelem,
       przez podmiane jednej wartosci na czas obliczenia."""
    wyniki = []

    stare = PRAD["kontaktron"]
    PRAD["kontaktron"] = {"min": 0.002, "typ": 0.003, "max": 0.005}
    wyniki.append(("bez podciagniecia kontaktronu (1 MOhm)", policz(scenariusz_obecny, 470)))
    PRAD["kontaktron"] = stare

    wyniki.append(("jedno przypomnienie zamiast dwoch",
                   policz(scenariusz_obecny, 470, przypomnien=1)))
    wyniki.append(("cztery przypomnienia zamiast dwoch",
                   policz(scenariusz_obecny, 470, przypomnien=4)))
    wyniki.append(("dzwoni codziennie, nie raz w tygodniu",
                   policz(scenariusz_obecny, 470, alarmow_na_tydzien=7.0)))
    return wyniki


if __name__ == "__main__":
    main()
