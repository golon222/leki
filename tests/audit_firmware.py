#!/usr/bin/env python3
"""Kontrola firmware pod katem bledow, ktorych testy jednostkowe nie widza:
kolejnosc definicji, parowanie Preferences.begin/end, sciezki wyjscia z setup(),
petle bez ogranicznika, uzycie makr. Uruchom: python3 audit_firmware.py"""
import re, sys, pathlib

ROOT = pathlib.Path(__file__).parent.parent / "firmware" / "PillBox"
ino = (ROOT / "PillBox.ino").read_text(encoding="utf-8")
cfg = (ROOT / "config.h").read_text(encoding="utf-8")

PASS, FAIL, WARN = [], [], []
def ok(c, m):  (PASS if c else FAIL).append(m)
def warn(c, m):
    if c: WARN.append(m)

def strip(src):
    """Usuwa komentarze i literaly, zeby zostal sam kod.

    UWAGA na apostrofy. W kodzie strony portalu jest HTML w rodzaju
    autocapitalize='off' - pojedynczy apostrof w NAPISIE. Dawny wzorzec
    traktowal go jak poczatek literalu znakowego i zjadal wszystko do
    nastepnego apostrofu, czyli kawal pliku razem z definicjami funkcji.
    Audyt sprawdzal potem tekst, ktorego w kodzie nie ma - i cicho
    przepuszczal bledy. W C literal znakowy to DOKLADNIE jeden znak albo
    sekwencja z ukosnikiem, i tak to teraz zapisujemy.
    """
    s = re.sub(r"/\*.*?\*/", "", src, flags=re.S)
    s = re.sub(r"//[^\n]*", "", s)
    s = re.sub(r'"(\\.|[^"\\])*"', '""', s)
    return re.sub(r"'(\\.|[^'\\])'", "''", s)

code = strip(ino)

def cialo(naglowek):
    """Cialo funkcji od jej DEFINICJI, a nie od deklaracji zapowiadajacej.

    Deklaracje w rodzaju 'bool syncTimeNTP();' stoja na poczatku pliku,
    wiec zwykle wyszukanie nazwy trafialo w nie i zwracalo kawalek
    zupelnie innego kodu. Kontrola sprawdzala wtedy nie to, co trzeba."""
    i = code.find(naglowek + " {")
    if i < 0: i = code.find(naglowek + "{")
    if i < 0: return ""
    reszta = code[i:]
    k = reszta.find("\n}")
    return reszta[:k] if k > 0 else reszta


# ---------- 1. Struktura ----------
def zbalansowany(src):
    """Liczy nawiasy znak po znaku, z prawdziwym sledzeniem napisow i komentarzy.

    Wyrazenia regularne sie tu nie nadaja: w kodzie strony portalu jest HTML
    z apostrofami wewnatrz napisow, a w komentarzach polskie slowa. Kazde
    uproszczenie konczylo sie falszywym alarmem albo - gorzej - cichym
    przepuszczeniem prawdziwego bledu.
    """
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
    return depth, par, st

d, pr, stan = zbalansowany(ino)
ok(d == 0 and pr == 0 and stan == "code",
   f"nawiasy zbilansowane (klamry {d}, okragle {pr}, stan {stan})")
ok(len(re.findall(r"^\s*#\s*if", ino, re.M)) == len(re.findall(r"^\s*#\s*endif", ino, re.M)),
   "kazdy #if ma swoj #endif")
ok(ino.count("void setup()") == 1 and ino.count("void loop()") == 1,
   "dokladnie jedno setup() i jedno loop()")
ok('#include "config.h"' in ino and "PILLBOX_CONFIG_VERSION" in cfg,
   "config.h dolaczony i oznaczony znacznikiem")

# ---------- 2. Kolejnosc definicji (C++ wymaga definicji przed uzyciem) ----------
defs = {}
for m in re.finditer(r"^(?:void|bool|int|uint\d+_t|int\d+_t|String|WakeReason|static\s+\w+)\s+"
                     r"(\w+)\s*\([^;{]*\)\s*\{", ino, re.M):
    defs.setdefault(m.group(1), m.start())
fwd = set(re.findall(r"^\s*(?:bool|void|int|String)\s+(\w+)\s*\([^)]*\)\s*;", ino, re.M))

# Pracujemy na kodzie bez komentarzy, zeby slowa w opisach nie udawaly wywolan.
# setup/loop wola Arduino, wiec ich kolejnosc nie ma znaczenia.
code_defs = {}
for m in re.finditer(r"^(?:void|bool|int|uint\d+_t|int\d+_t|String|WakeReason|static\s+\w+)\s+"
                     r"(\w+)\s*\([^;{]*\)\s*\{", code, re.M):
    code_defs.setdefault(m.group(1), m.start())

bad_order = []
for m in re.finditer(r"\b(\w+)\s*\(", code):
    callee = m.group(1)
    if callee in ("setup", "loop") or callee in fwd or callee not in code_defs:
        continue
    if m.start() < code_defs[callee] and m.start() != code_defs[callee]:
        # czy to wywolanie, czy sama definicja?
        if not re.match(r"\s*\{", code[m.end():m.end()+200].split(")")[-1][:5] or ""):
            bad_order.append(f"{callee} (poz. {m.start()}) przed definicja ({code_defs[callee]})")
ok(not bad_order, "wszystkie funkcje zdefiniowane przed uzyciem"
   + (f" — PROBLEM: {set(bad_order)}" if bad_order else ""))

# ---------- 2b. Typy zdefiniowane przed uzyciem ----------
# Kompilator czyta plik z gory na dol. Zmienna typu, ktory poznaje dopiero
# nizej, konczy sie bledem "does not name a type" - a to jest blad, ktory
# widac dopiero przy wgrywaniu, czyli najdrozej.
# Zmienne globalne tez: uzyte w funkcji zdefiniowanej WYZEJ = blad kompilacji.
glob = {}
for m in re.finditer(r"^(?:RTC_DATA_ATTR\s+)?(?:static\s+)?"
                     r"(?:bool|int|float|uint\d+_t|int\d+_t|String|Gest|WakeReason|Preferences)\s+"
                     r"(\w+)\s*(?:=|;|\[)", code, re.M):
    glob.setdefault(m.group(1), m.start())
zle_glob = []
for nazwa, gdzie in glob.items():
    if len(nazwa) < 4: continue
    for u in re.finditer(rf"\b{nazwa}\b", code):
        if u.start() < gdzie:
            zle_glob.append(nazwa); break
ok(not zle_glob, "zmienne globalne uzywane dopiero po deklaracji"
   + (f" — PROBLEM: {sorted(set(zle_glob))}" if zle_glob else ""))

for m in re.finditer(r"^(?:enum|struct)\s+(\w+)\s*\{", code, re.M):
    typ, gdzie = m.group(1), m.start()
    uzycia = [u.start() for u in re.finditer(rf"^{typ}\s+\w+", code, re.M)]
    wczesniej = [u for u in uzycia if u < gdzie]
    ok(not wczesniej,
       f"typ {typ} zdefiniowany przed uzyciem"
       + (f" — UZYTY WCZESNIEJ w {len(wczesniej)} miejscach" if wczesniej else ""))

# ---------- 3. Preferences: kazde begin() ma swoje end() ----------
for fn, pos in defs.items():
    end = ino.find("\n}", pos)
    body = strip(ino[pos:end])
    b, e = body.count("prefs.begin("), body.count("prefs.end()")
    ok(b == e, f"{fn}(): prefs.begin/end sparowane ({b}/{e})") if b or e else None
ok(code.count("prefs.begin(") == code.count("prefs.end()"),
   f"globalnie prefs.begin = prefs.end ({code.count('prefs.begin(')})")

# zagniezdzenie: begin bez end w tej samej funkcji przed kolejnym begin
nested = re.search(r"prefs\.begin\([^)]*\)(?:(?!prefs\.end).)*?prefs\.begin\(", code, re.S)
ok(nested is None, "brak zagniezdzonych prefs.begin() (NVS na to nie pozwala)")

# ---------- 4. Kazda petla oczekiwania ma ogranicznik ----------
def while_conditions(src):
    out = []
    for m in re.finditer(r"\bwhile\s*\(", src):
        i, depth = m.end() - 1, 0
        while i < len(src):
            if src[i] == "(": depth += 1
            elif src[i] == ")":
                depth -= 1
                if depth == 0:
                    out.append(src[m.end():i]); break
            i += 1
    return out

loops = while_conditions(code)
BOUNDED = ("millis()", "guard", "queuePeek", "queueCount", "start <", "i <", "provDone")
unbounded = [" ".join(l.split()) for l in loops if not any(b in l for b in BOUNDED)]
ok(not unbounded, "kazda petla ma ogranicznik czasu lub licznik"
   + (f" — PODEJRZANE: {unbounded}" if unbounded else ""))

# ---------- 5. setup() zawsze konczy sie snem ----------
si = ino.find("void setup()")
setup_body = ino[si:ino.find("\nvoid loop()")]
ok("goToSleep(planNextSleep());" in setup_body, "setup() konczy sie goToSleep()")
ok("return;" not in strip(setup_body), "brak przedwczesnego return w setup() (ominalby sen)")
ok(strip(setup_body).count("goToSleep(") >= 3,
   f"sciezki awaryjne tez zasypiaja ({strip(setup_body).count('goToSleep(')} wywolan)")

# ---------- 6. Deep sleep: wybudzanie ----------
gs = ino[ino.find("void goToSleep("):]
gs = gs[:gs.find("\n}")]
# Liczymy na kodzie BEZ komentarzy - opis bledu w komentarzu wymienia nazwe
# funkcji i inaczej sam podbijalby licznik wywolan.
gs_code = strip(gs)
# DOKLADNIE JEDNO wywolanie. esp_deep_sleep_enable_gpio_wakeup() nadpisuje
# maske przy kazdym wywolaniu, wiec drugie kasowalo pierwsze - i kontaktron
# nigdy nie budzil pudelka. Ten test pilnuje, zeby blad nie wrocil.
ok(gs_code.count("esp_deep_sleep_enable_gpio_wakeup(") == 1,
   f"tylko jedno uzbrojenie GPIO ({gs_code.count('esp_deep_sleep_enable_gpio_wakeup(')}) "
   "- kolejne nadpisuja poprzednie")
ok("GPIO_BUTTON" not in gs_code.split("esp_deep_sleep_enable_gpio_wakeup(")[-1][:200],
   "uzbrajany jest kontaktron, nie przycisk")
ok("ESP_GPIO_WAKEUP_GPIO_LOW" in gs_code and "ESP_GPIO_WAKEUP_GPIO_HIGH" in gs_code,
   "oba poziomy uzywane (otwarcie i zamkniecie wieczka)")
ok("esp_sleep_enable_timer_wakeup(" in gs, "timer wybudzania ustawiony")
ok("rtcArmedForClose" in gs, "obsluga pudelka juz otwartego przy zasypianiu")
ok("portalRequested" in ino, "portal WiFi ma zastepczy gest (przytrzymanie przy resecie)")

# Gesty serwisowe: pudelko bedzie sklejone, wiec musza dzialac bez RESETU.
ok("czekajNaZamkniecieIGest" in code,
   "przycisk obserwowany w oknie po otwarciu wieczka")
ok(code.count("czekajNaZamkniecieIGest(CZEKAJ_ZAMKNIECIE_MS)") >= 3,
   f"gest dziala po KAZDYM otwarciu "
   f"({code.count('czekajNaZamkniecieIGest(CZEKAJ_ZAMKNIECIE_MS)')} miejsc)")

# Pudelko ma czuwac przy otwartym wieczku, a nie zasypiac - inaczej
# aplikacja dowiaduje sie o zamknieciu z opoznieniem.
czek = code[code.find("Gest czekajNaZamkniecieIGest("):]
czek = czek[:czek.find("\n}")]
ok("extendAwake(limitMs + 30000)" in czek,
   "bezpiecznik czasowy odsuniety na czas czuwania")
ok("pushLidState();" in czek,
   "otwarcie zglaszane od razu z KAZDEJ sciezki czekania, nie po zamknieciu")
ok("!rtcOpenReported" in czek,
   "ale bez powtarzania tego, co poszlo juz przy zwyklym otwarciu")
ok("WiFi.reconnect();" in czek,
   "lacze odbudowywane w czasie czekania, nie dopiero przy wysylce")
ok("msZamkniecia = millis();" in czek,
   "zapamietujemy chwile zamkniecia - inaczej opoznienia nie da sie zmierzyc")
ok("zamkn->wyslane" in ino,
   "czas od zamkniecia do potwierdzenia trafia do czarnej skrzynki")
ok("wifiUspij();" in czek and "radioZgaszone" in czek,
   "radio uspione po dluzszym czekaniu - zjadaloby kilka razy wiecej pradu")
ok("RADIO_OTWARTE_S * 1000UL" in czek,
   "prog uspienia radia bierze sie z config.h, nie z liczby w kodzie")
ok("#define RADIO_OTWARTE_S" in cfg, "config.h ma RADIO_OTWARTE_S")
import re as _re
_m = _re.search(r"#define\s+RADIO_OTWARTE_S\s+(\d+)", cfg)
ok(_m and 60 <= int(_m.group(1)) <= int(_re.search(r"#define\s+OPEN_WARN_FIRST_S\s+(\d+)", cfg).group(1)),
   "radio gasnie pozniej niz po minucie, ale nie pozniej niz pierwsze ostrzezenie")
# esp_wifi_stop() ubija sterownik i powrot bywa zawodny, a zaraz po
# zamknieciu wieczka MUSIMY sie polaczyc. Stad lagodne uspienie.
ok("esp_wifi_stop" not in czek,
   "podczas czuwania NIE zatrzymujemy sterownika radia")
usp = code[code.find("void wifiUspij("):]
usp = usp[:usp.find("\n}")]
ok("esp_wifi_stop" not in usp, "wifiUspij tylko gasi nadajnik, nie ubija sterownika")
push = code[code.find("bool pushStatus("):]
push = push[:push.find("\n}")]
ok("bool pushStatus(" in code, "pushStatus melduje, czy wysylka sie udala")
ok("code >= 200 && code < 300" in push, "sukces = potwierdzony kod HTTP, nie sam fakt wyslania")
ok(push.find("rtdbSend") < push.find("rtcOpenReported ="),
   "rtcOpenReported ustawiane PO wyslaniu, nie przed")
ok("rtcStatusDirty" in push, "nieudana wysylka zostawia slad w rtcStatusDirty")
plan = code[code.find("uint32_t planNextSleep("):]
plan = plan[:plan.find("\n}")]
ok("rtcStatusDirty" in plan,
   "nieaktualny stan w aplikacji wymusza wczesniejsze wybudzenie")
ok("&& pushStatus()" in code,
   "komunikat o powiadomieniu aplikacji tylko po potwierdzonej wysylce")
# ── Token Firebase miedzy wybudzeniami ──
# idToken jest zwykla zmienna, wiec deep sleep go kasuje. Przy budzeniu co
# minute (podglad ladowania) dawaloby to 1440 logowan haslem na dobe -
# Google potrafi za to czasowo zablokowac konto.
ok("bool tokenZPamieci(" in code, "token przezywa deep sleep")
sig = code[code.find("bool firebaseSignIn("):]
sig = sig[:sig.find("\n}")]
ok("tokenZPamieci()" in sig, "logowanie haslem tylko gdy nie ma waznego tokenu")
ok("TOKEN_MARGIN_S" in code, "token odswiezany z zapasem przed wygasnieciem")
snd = code[code.find("int rtdbSend("):]
snd = snd[:snd.find("\n}")]
ok("zapomnijToken()" in snd, "odmowa z bazy uniewaznia zapamietany token")
ok(code.find("void zapomnijToken(") < code.find("int rtdbSend("),
   "zapomnijToken zdefiniowane przed uzyciem w rtdbSend")
# ── Ostrzezenie o powtorce musi byc PIERWSZE ──
# Ten sygnal ma zatrzymac reke przed druga dawka. Kazde 100 ms zwloki
# jest realne, bo wieczko odkreca sie szybciej, niz plytka wstaje.
_su = code[code.find("void setup() {"):]
_i_beep = _su.find("beepAlreadyTaken();")
_i_ser  = _su.find("Serial.begin(")
_i_bat  = _su.find("readBattery()")
_i_wake = _su.find("wakeReason = detectWakeReason();")
ok(_i_beep > 0 and _i_ser > 0 and _i_beep < _i_ser,
   "ostrzezenie o powtorce brzmi PRZED startem portu szeregowego")
ok(_i_bat < 0 or _i_beep < _i_bat,
   "i przed pomiarem baterii")
_i_load = _su.find("loadDayMarkers();")
ok(0 < _i_load < _i_beep,
   "znacznik dawki odtworzony z pamieci PRZED sygnalem - po resecie RTC jest puste")
ok(_su.count("loadDayMarkers();") == 1, "znacznik odtwarzany dokladnie raz")
ok(0 < _i_wake < _i_beep,
   "powod wybudzenia ustalany przed ostrzezeniem, nie po")
ok("if (!juzOstrzezono) beepAlreadyTaken();" in code,
   "sygnal nie odzywa sie dwa razy przy jednym otwarciu")
ok(code.count("wakeReason = detectWakeReason();") == 1,
   "powod wybudzenia ustalany dokladnie raz")
_cfgi = _su.find("configureInputs();")
ok(0 < _cfgi < _i_beep,
   "wejscia skonfigurowane zanim czytamy kontaktron i wlaczamy buzzer")

# ── Jedno polaczenie TLS na wybudzenie ──
# Uzgodnienie TLS na ESP32-C3 trwa okolo sekundy. Jedno otwarcie pudelka
# to szesc zapytan - wlasny klient na kazde oznaczal szesc uzgodnien.
_snd = cialo("int rtdbSend(const char* method, const String& path, const String& body, String* resp)")
if not _snd: _snd = cialo("int rtdbSend(const char* method, const String& path, const String& body, String* resp = nullptr)")
ok("WiFiClientSecure rtdbClient;" in code,
   "klient TLS jest jeden na cale wybudzenie, nie jeden na zapytanie")
ok("WiFiClientSecure client;" not in _snd,
   "rtdbSend nie tworzy nowego klienta przy kazdym wywolaniu")
ok("http.setReuse(true);" in _snd,
   "kanal nie jest zrywany po zapytaniu - kolejne ida gotowym polaczeniem")
ok("rtcLogWyslanyIdx" in code,
   "czarna skrzynka wysylana tylko gdy przybylo wpisow")

# ── Stan wieczka wysylany osobno i natychmiast ──
ok("bool pushLidState(" in code, "stan wieczka ma wlasna, szybka wysylke")
lid = cialo("bool pushLidState()")
ok("PATCH" in ino[ino.find("bool pushLidState("):][:600],
   "stan wieczka idzie PATCH-em - nie nadpisuje reszty statusu")
ok("logbookJson" not in lid, "szybka wysylka nie ciagnie za soba czarnej skrzynki")
rep = cialo("void reportEvent(const char* type, int slot)")
ok(rep.find("pushLidState()") < rep.find("fetchConfig()"),
   "przy otwarciu stan wieczka leci PRZED pobraniem ustawien i kolejka")
ok("pushLidState() && pushStatus()" in code,
   "przy zamknieciu tez najpierw krotki pakiet, potem pelny status")

# ── Procent baterii przy ladowaniu ──
# Uklad ladujacy trzyma na ogniwie swoje napiecie, wiec kazdy pomiar
# wychodzi 100%. Pokazywanie tego byloby klamstwem.
_pr = ino[ino.find("bool pushStatus() {"):]
_pr = _pr[:_pr.find("\n}")]
ok('doc["chargeSince"]' in _pr and 'doc["chargeFromPct"]' in _pr,
   "pudelko podaje moment i punkt startowy ladowania - z tego liczy sie czas")
tc2 = cialo("bool trackCharging(float v, float prev)")
ok("if (!rtcCharging) {" in tc2 and "rtcChargeSinceTs =" in tc2,
   "moment startu zapisywany TYLKO przy wejsciu w tryb, nie co pomiar")
ok("rtcChargeSinceTs" in cialo("void przesunZnaczniki(int32_t delta)"),
   "korekta zegara obejmuje takze start ladowania")
ok("void zapiszKoniecLadowania(" in code, "koniec ladowania jest zapisywany")
zk = cialo("void zapiszKoniecLadowania()")
_zkRaw = ino[ino.find("void zapiszKoniecLadowania() {"):]
_zkRaw = _zkRaw[:_zkRaw.find("\n}")]
ok('"chgPrev"' in _zkRaw and '"chgEnd"' in _zkRaw,
   "trzymamy dwie ostatnie daty, nie jedna")
ok("prefs.begin" in zk and "prefs.end()" in zk,
   "data ladowania idzie do pamieci NIEULOTNEJ - RTC przepada przy pustym ogniwie")
ok("zapiszKoniecLadowania();" in tc2, "zapis nastepuje w chwili odlaczenia kabla")
_pl = cialo("void petlaLadowania()")
ok("CHARGE_SETTLE_S" in _pl,
   "po odlaczeniu czekamy na ustabilizowanie napiecia przed pomiarem")
ok("resetBatteryFilter();" in _pl and "batteryPercentage = batteryRawPercentage;" in _pl,
   "i dopiero wtedy przyjmujemy swiezy odczyt jako punkt wyjscia")

# ── Czuwanie na ladowarce ──
ok("void petlaLadowania(" in code, "na ladowarce pudelko czuwa zamiast usypiac")
petla = cialo("void petlaLadowania()")
ok("CHARGE_AWAKE_MAX_S" in petla,
   "jest bezpiecznik czasowy - nieudane wykrycie odlaczenia nie zjada ogniwa")
ok("goToSleep(1)" in petla,
   "otwarcie wieczka podczas ladowania wraca do normalnej obslugi dawki")
ok("boxIsOpen()" in petla,
   "wieczko pilnowane programowo - czuwajaca plytka nie dostanie przerwania")
ok("extendAwake(" in petla, "bezpiecznik czuwania nie przerywa ladowania")
ok("secondsToNextSlot(" in petla and "goToSleep(1)" in petla,
   "pora dawki podczas ladowania nie zostaje polknieta")
ok("secondsToDayBoundary(" in petla, "koniec doby lekowej tez nie")
# Korekta zegara musi objac wszystkie znaczniki, nie tylko kolejke
ok("void przesunZnaczniki(" in code, "korekta zegara ma wlasna funkcje")
ntp = cialo("bool syncTimeNTP()")
ok("przesunZnaczniki(delta)" in ntp and "queueShiftTimestamps(delta)" in ntp,
   "korekta zegara poprawia I kolejke, I znaczniki RTC")
prz = code[code.find("void przesunZnaczniki("):]
prz = prz[:prz.find("\n}")]
for zn in ["rtcNextWarnTs", "rtcOpenSinceTs", "rtcTokenExp", "rtcLastPushTs", "rtcLastOpenTs"]:
    ok(zn in prz, f"korekta zegara obejmuje {zn}")
ok("fetchConfig();" in petla,
   "na ladowarce zmiany ustawien z telefonu wchodza od razu")
ok("flushQueue();" in petla, "i zaleglosci z kolejki tez sie doslaja")
ok("batterySaver" in petla, "przy chronionym ogniwie nie zostajemy na lacza")
ok(code.find("void petlaLadowania(") > code.find("void goToSleep("),
   "petlaLadowania zdefiniowana po funkcjach, ktorych uzywa")
sig2 = code[code.find("bool firebaseSignIn("):]
sig2 = sig2[:sig2.find("\n}")]
ok("TOKEN_MARGIN_S" in sig2,
   "przy wielogodzinnym czuwaniu token jest odswiezany zanim wygasnie")
ok("meldunekLadowania" in code and "CHARGE_PUSH_MAX_S" in code,
   "przy ladowaniu meldunek idzie na zmianie procentu, nie co minute")

# ── Podglad ladowania ──
ok("bool trackCharging(" in code, "stan ladowania jest osobna, testowalna funkcja")
ok("rtcCharging && CHARGE_POLL_S < s" in plan,
   "na ladowarce pudelko melduje sie czesto - prad jest za darmo")
pushRaw = ino[ino.find("bool pushStatus("):]
pushRaw = pushRaw[:pushRaw.find("\n}")]
ok('doc["charging"] = rtcCharging' in pushRaw, "stan ladowania trafia do aplikacji")
ok("|| meldunekLadowania ||" in code, "ladowanie samo wymusza wyslanie statusu")
tc = code[code.find("bool trackCharging("):]
tc = tc[:tc.find("\n}")]
ok("CHARGE_IDLE_MAX" in tc,
   "jest bezpiecznik - zaklinowane wykrycie nie zjada ogniwa budzeniem co minute")
ok("zWysokiego" in tc and "rtcWysokieZRzedu" in tc,
   "kabel podpiety przed startem tez jest wykrywany, nie tylko wzrost")
ok("rtcBlokWysokie" in tc,
   "po odpieciu tryb nie wraca sam na wysokim napieciu")
ok("CHARGE_FULL_V" in tc,
   "plateau przy pelnym ogniwie nie jest mylone z odlaczeniem kabla")
ok("rtcVoltMax - CHARGE_DROP_V" in tc, "odlaczenie wykrywane po spadku od szczytu")
ok("byloOtwarte" in code,
   "pudelko pamieta, ze wieczko bylo otwarte w tym wybudzeniu")
ok("|| byloOtwarte" in code,
   "po zamknieciu stan jest wysylany ZAWSZE, bez dodatkowych warunkow")
ok("OPEN_WARN_FIRST_S * 1000UL" in cfg,
   "czuwanie trwa dokladnie do pierwszego ostrzezenia o otwartym wieczku")
ok("while (boxIsOpen() && millis()" not in code,
   "stare petle czekania zastapione - inaczej gest bylby ignorowany")
ok("autoTest();" in code and "GEST_TEST" in code, "trzy klikniecia uruchamiaja autotest")
ok("GEST_PORTAL" in code and code.count("startWifiPortal();") >= 2,
   "przytrzymanie otwiera portal WiFi")

# Punkt dostepowy to najdrozszy tryb pracy - zamkniecie wieczka musi go konczyc.
ap = code[code.find("void startWifiPortal("):]
ap = ap[:ap.find("\n}")]
ok("bylOtwarty" in ap and "zamknietoWieczko" in ap,
   "zamkniecie wieczka konczy parowanie")
ok("bylOtwarty && !boxIsOpen()" in ap,
   "warunek dziala tylko przy przejsciu otwarte -> zamkniete")
ok(re.search(r"if \(!boxIsOpen\(\)\) \{ zamknietoWieczko = true", ap) is not None,
   "stan potwierdzany dwa razy - odbicie styku nie przerywa wpisywania hasla")

# Autotest ma dawac znac po KAZDYM etapie - inaczej przy sklejonym pudelku
# nie wiadomo, czy w ogole ruszyl.
ok("void wynikEtapu" in code, "kazdy etap testu ma wspolne zakonczenie")
# Pikniecia numeruja etapy, zeby przy sklejonym pudelku bylo wiadomo,
# na czym stoimy i czy trzeba zareagowac.
ok("void pikNumer" in code, "etapy numerowane piknieciami")
numery = [int(m) for m in re.findall(r"pikNumer\((\d)\)", code)]
ok(numery == list(range(1, len(numery) + 1)),
   f"numery ida po kolei bez luk: {numery}")
ok(len(numery) == 2,
   f"tylko dwa sygnaly w trakcie: start i koniec czesci z Toba ({len(numery)})")
ok("pikEtapOK" not in code and "pikEtapZle" not in code,
   "brak piknięc po kazdym etapie - wyniki ogląda sie w aplikacji")
ok("void pikBrakSieci" in code and "buzzerTonCicho" in code,
   "jedyny sygnal bledu - brak sieci - jest lagodny (male wypelnienie)")
ok("pikBrakSieci();" in code and code.count("pikBrakSieci();") == 1,
   "i odzywa sie tylko wtedy, gdy wyniku naprawde nie da sie zobaczyc")
etapy = ino.count('wynikEtapu("')
ok(etapy >= 6, f"autotest ma {etapy} etapow")
# Przycisk celowo NIE jest testowany: test odpala sie trzema klknieciami,
# wiec zanim ruszy, przycisk juz sie udowodnil.
ok('wynikEtapu("przycisk"' not in ino,
   "nie sprawdzamy przycisku - jego dzialanie wynika z uruchomienia testu")
_at = ino[ino.find("void autoTest() {"):]
_at = _at[:_at.find("\n}")]
ok(_at.find("pikNumer(1);") < _at.find('wynikEtapu("kontaktron"'),
   "test zaczyna sie piknieciem - to jest zarazem dowod, ze buzzer dziala")
ok(_at.find('wynikEtapu("kontaktron"') < _at.find("pikNumer(2);"),
   "kontaktron jest PIERWSZYM etapem, bo tylko on wymaga Twojego udzialu")
ok(_at.find("pikNumer(2);") < _at.find('wynikEtapu("bateria"'),
   "dwa pikniecia oddzielaja czesc z Toba od samodzielnej")
ok(_at.count('etapTestu("') >= 6,
   "kazdy etap testu melduje sie w logu - inaczej 'przerwalo sie' jest nie do zdiagnozowania")
ok("extendAwake(120000)" in code,
   "zaden etap testu nie moze paść przez bezpiecznik czasowy")
ok("rtcLogWyslanyIdx = 0xFFFF;" in _at,
   "wyniki testu sa wysylane od razu, a nie czekaja na kolejna okazje")
ok('logbookAdd("test:START")' in ino and 'test:KONIEC' in ino,
   "przebieg ma poczatek i koniec, zeby aplikacja wiedziala, co pokazac")
ok("void pikKoniecTestu" in code, "koniec testu ma wlasna sygnature")
ok("pikKoniecTestu" in code.split("void pikNumer")[-1], "rozna od numerow etapow")
ok("esp_deep_sleep_start();" in gs, "faktyczne wejscie w deep sleep")

# Bez tych dwoch linii kontaktron NIE MA JAK obudzic pudelka: rdzen wlacza
# wewnetrzny rezystor 10k do masy, ktory wygrywa z zewnetrznym 100k
# podciagajacym i pin zostaje na ~0,4 V (espressif/esp-idf#12183).
ok("gpio_hold_en(" in gs_code, "podciagniecie kontaktronu zatrzasniete przed snem")
ok("gpio_deep_sleep_hold_en()" in gs_code, "zatrzask aktywny takze w deep sleep")
hold_pos = gs_code.find("gpio_hold_en(")
arm_pos  = gs_code.find("esp_deep_sleep_enable_gpio_wakeup(")
start_pos = gs_code.find("esp_deep_sleep_start()")
ok(arm_pos < hold_pos < start_pos,
   "kolejnosc: uzbrojenie -> zatrzask -> sen (inna nie zadziala)")
ok("gpio_hold_dis(" in code and "gpio_deep_sleep_hold_dis()" in code,
   "zatrzask zwalniany po wybudzeniu, inaczej pin zostaje zamrozony")
si_hold = code.find("gpio_deep_sleep_hold_dis()")
si_cfg  = code.find("configureInputs();")
ok(si_hold >= 0 and si_cfg >= 0 and si_hold < si_cfg,
   "zwolnienie zatrzasku przed configureInputs()")

# ---------- 6b. Alarm: gdzie zapada decyzja o dawce (B1) ----------
# Decyzja "otwarte wieczko = dawka wzieta" jest wydzielona z petli alarmu
# wylacznie po to, zeby dala sie przetestowac. Cala wartosc tego zabiegu
# znika, gdy petla zacznie pytac o pin z pominieciem tej funkcji: naprawa
# B1 wejdzie wtedy w jedno miejsce, a dzialac bedzie drugie.
alarm = cialo("bool runAlarmWindow()")
ok("bool alarmPotwierdzony(" in code,
   "decyzja o potwierdzeniu dawki jest osobna, testowalna funkcja")
ok(alarm.count("alarmPotwierdzony(") == 2,
   f"obie sciezki wyjscia z alarmu pytaja przez nia ({alarm.count('alarmPotwierdzony(')})")
ok("if (boxIsOpen()) { buzzerOff(); return true; }" not in alarm,
   "petla alarmu nie omija funkcji decyzyjnej skrotem do pinu")
ok("const bool byloOtwarteNaStarcie = boxIsOpen();" in alarm,
   "stan wieczka sprzed dzwonienia zapamietany, zanim cokolwiek zapiszczy")
ok(alarm.find("byloOtwarteNaStarcie = boxIsOpen()") < alarm.find("buzzerInit()"),
   "i zapamietany PRZED pierwszym pikiem, nie w trakcie")

# ---------- 6c. Ochrona przed druga dawka bez internetu ----------
# Zgloszenie Kuby z wyjazdu: bez sieci pudelko nie ostrzegalo o powtorce.
# Cala ochrona byla bramkowana rtcTimeValid, ktore ustawia WYLACZNIE NTP.
# Wycinanie galezi z setup() ma wlasna funkcje, bo recznie pomylilem sie
# przy tym TRZY RAZY. Wszystkie nazwy "case WAKE_*" wystepuja rowniez
# w wakeName(), ktore stoi WYZEJ - szukanie od poczatku pliku dawalo raz
# wycinek 61-znakowy, raz 105 000-znakowy, a kontrole na nim przechodzily
# nie sprawdzajac niczego. Teraz kotwiczymy od setup() i sprawdzamy
# rozmiar, wiec kolejna taka pomylka od razu sie zglosi.
def galaz(nazwa, konczy_sie):
    poczatek = ino.find("void setup()")
    i = ino.find(nazwa, poczatek)
    if i < 0: return ""
    j = ino.find(konczy_sie, i)
    return ino[i:j] if j > i else ino[i:]

reed = galaz("case WAKE_REED:", "case WAKE_BUTTON:")
ok(1000 < len(reed) < 20000,
   f"galaz kontaktronu wycieta poprawnie ({len(reed)} znakow)")
ok("void zapiszDawke()" in code,
   "zapis dawki jest osobna funkcja, dzialajaca takze bez zegara")
ok("bool juzDzisBrane()" in code,
   "decyzja 'juz dzis brane' jest osobna, testowalna funkcja")
ok("rtcTakenTs" in cialo("void zapiszDawke()"),
   "zapis dawki odnotowuje CZAS takze wtedy, gdy zegar jest nieznany")
ok("if (rtcTimeValid) setTakenDay(localDayNumber(time(nullptr)));" not in reed,
   "galaz kontaktronu nie bramkuje juz zapisu dawki zegarem")
ok("zapiszDawke();" in reed, "otwarcie odnotowuje dawke bezwarunkowo")
ok(reed.count("juzDzisBrane()") >= 1, "i pyta o powtorke przez wspolna funkcje")
# Bez zegara ostrzezenie to tylko PODEJRZENIE - wolno piknac, ale nie
# wolno wyrzucic otwarcia, bo prawdziwa dawka przepadlaby bez sladu.
_jd = reed[reed.find("if (juzDzisBrane())"):]
_jd = _jd[:_jd.find("\n      }")]
ok("if (rtcTimeValid) {" in _jd,
   "pominiecie zapisu tylko przy WIARYGODNYM zegarze, nie na podejrzenie")
ok("ONE_DOSE_WINDOW_S" in code, "okno podejrzenia jest nazwana stala z config.h")

# ---------- 6d. Alarm nie moze dzwonic w kolko w oknie dopasowania ----------
# Zgloszenie Kuby: "pika 3 raz od 18:30, o 19:30 tez". matchSlot() dopasowuje
# pore leku w oknie +/-MATCH_WINDOW_MIN, czyli dla 20:00 przez trzy godziny.
# Bez znacznika "odzwonione" kazde wybudzenie w tym oknie zaczyna alarm od nowa,
# a bez sieci pudelko budzi sie tam wielokrotnie (ponawianie kolejki).
tim = galaz("case WAKE_TIMER:", "gestPoOtwarciu == GEST_TEST")
ok(1000 < len(tim) < 20000,
   f"galaz timera wycieta poprawnie ({len(tim)} znakow)")
ok("void oznaczAlarmObsluzony()" in code,
   "istnieje znacznik 'alarm na te dobe juz sie odbyl'")
ok("bool alarmJuzObsluzony()" in code, "i funkcja, ktora go czyta")
ok("alarmJuzObsluzony()" in tim,
   "galaz alarmu sprawdza go PRZED zadzwonieniem")
_po = tim[tim.find('reportEvent("missed"'):]
ok("oznaczAlarmObsluzony();" in _po[:400],
   "po wyczerpaniu prob alarm zostawia slad, ze juz dzwonil")
# Tu szukamy NAPISU, wiec na surowym zrodle - strip() zamienia literaly na "".
_ldm = ino[ino.find("void loadDayMarkers()"):]
ok('prefs.getULong("almDay"' in _ldm[:_ldm.find("\n}")],
   "znacznik przezywa reset - odtwarzany z pamieci trwalej")

# ---------- 7. Znaczniki dobowe trwale ----------
ok("loadDayMarkers();" in setup_body, "znaczniki dobowe odtwarzane przy starcie")
allowed_fns = ("loadDayMarkers", "setTakenDay", "setRolloverDay", "clearDayMarkers")
def in_allowed(pos):
    for fn in allowed_fns:
        if fn in code_defs:
            start = code_defs[fn]
            end = code.find("\n}", start)
            if start <= pos <= end: return True
    return False
stray = [m.start() for m in re.finditer(r"\brtcTakenDay\s*=[^=]", code)
         if not in_allowed(m.start()) and "RTC_DATA_ATTR" not in code[max(0,m.start()-40):m.start()]]
ok(not stray, f"rtcTakenDay zmieniany wylacznie przez setTakenDay() ({len(stray)} obejsc)")
stray2 = [m.start() for m in re.finditer(r"\brtcRolloverDay\s*=[^=]", code)
          if not in_allowed(m.start()) and "RTC_DATA_ATTR" not in code[max(0,m.start()-45):m.start()]]
ok(not stray2, f"rtcRolloverDay zmieniany wylacznie przez setRolloverDay() ({len(stray2)} obejsc)")
ok("putULong(\"takenDay\"" in ino, "znacznik dawki zapisywany w pamieci nieulotnej")

# ---------- 8. Makra z config.h faktycznie uzywane ----------
declared = set(re.findall(r"#\s*define\s+(\w+)", cfg))
ignore = {"LOG", "LOGLN", "REED_MODE", "BUTTON_MODE", "PILLBOX_CONFIG_VERSION"}
unused = sorted(d for d in declared - ignore if d not in ino)
warn(bool(unused), f"nieuzywane ustawienia w config.h: {unused}")
ok(True, f"config.h definiuje {len(declared)} ustawien")

# ---------- 9. Spojnosc wartosci ----------
def val(name, src=cfg):
    m = re.search(rf"#\s*define\s+{name}\s+([0-9.]+)", src)
    return float(m.group(1)) if m else None
ok(val("BATT_CUTOFF_V") < val("BATT_SAFE_V") < val("BATT_RECOVER_V"),
   "progi napiecia rosnaco: cutoff < safe < recover")
ok(val("BATT_CRIT_PCT") < val("BATT_WARN_PCT"), "prog krytyczny ponizej ostrzegawczego")
ok(val("RETRY_BASE_S") < val("RETRY_MAX_S"), "backoff ma sens")
ok(val("PORTAL_TIMEOUT_S") * 1000 > val("AWAKE_LIMIT_MS"),
   "portal dluzszy niz bazowy limit czuwania (dlatego limit jest ruchomy)")
ok('"' in re.search(r'#define AP_PASS\s+("[^"]*")', cfg).group(1)
   and len(re.search(r'#define AP_PASS\s+"([^"]*)"', cfg).group(1)) >= 8,
   "haslo punktu dostepowego spelnia wymog WPA2 (min. 8 znakow)")

# ---------- 9b. Granica doby: firmware i aplikacja musza sie zgadzac ----------
app = (pathlib.Path(__file__).parent.parent / "index.html").read_text(encoding="utf-8")
m_fw  = re.search(r"#\s*define\s+DAY_START_HOUR\s+(\d+)", cfg)
m_app = re.search(r"const\s+DAY_START_HOUR\s*=\s*(\d+)", app)
ok(bool(m_fw),  "config.h definiuje DAY_START_HOUR")
ok(bool(m_app), "aplikacja definiuje DAY_START_HOUR")
ok(bool(m_fw and m_app and m_fw.group(1) == m_app.group(1)),
   f"granica doby taka sama po obu stronach (firmware={m_fw and m_fw.group(1)}, "
   f"aplikacja={m_app and m_app.group(1)})")
ok(bool(m_fw and 0 <= int(m_fw.group(1)) <= 6),
   "granica doby w rozsadnym zakresie 0-6 godz.")
# Granica nie moze wpasc w pore brania leku - inaczej dawka wziete tuz przed
# nia i tuz po niej wygladalyby jak dwie rozne doby.
sched = re.search(r'#define DEFAULT_SCHEDULE\s+"([^"]*)"', cfg)
if sched and m_fw:
    hours = [int(h.split(":")[0]) for h in sched.group(1).split("|") if ":" in h]
    ok(all(abs(h - int(m_fw.group(1))) >= 2 for h in hours),
       f"granica doby ({m_fw.group(1)}:00) z dala od pory leku ({sched.group(1)})")

# ---------- 9c. Ostrzeganie o otwartym wieczku ----------
ok(val("OPEN_WARN_FIRST_S") > 0 and val("OPEN_WARN_REPEAT_S") > 0,
   "interwaly ostrzegania o otwartym pudelku sa dodatnie")
ok(val("OPEN_WARN_REPEAT_S") <= val("HOUSEKEEP_MAX_S"),
   "przerwa miedzy sygnalami nie dluzsza niz maksymalny sen")
ok(val("OPEN_WARN_MS") < 2000, "sygnal jest krotki, a nie syrena")
ok("trackBoxOpen()" in code and code.index("trackBoxOpen()") < code.index("switch (wakeReason)"),
   "trackBoxOpen wolane przed rozgalezieniem na powod wybudzenia")
ok("WAKE_CLOSED" in code and "rtcArmedForClose" in code,
   "zamkniecie wieczka ma wlasny powod wybudzenia (nie udaje przycisku)")
ok(code.count("rtcArmedForClose = false") >= 2,
   "flaga czekania na zamkniecie jest kasowana, a nie tylko ustawiana")
ok('doc["boxOpen"]' in ino, "status wysylany do aplikacji zawiera stan wieczka")
# Bez tego warunku aplikacja nie dowiadywala sie o ZAMKNIECIU wieczka po
# zwyklym otwarciu na lek - baner "otwarte" wisial godzinami.
ok("rtcOpenReported != boxIsOpen()" in code,
   "rozjazd miedzy prawda a obrazem w aplikacji wymusza wyslanie statusu")
# Stan zapamietujemy z chwili WYSYLKI (stanWyslany), nie z chwili zapisu -
# wieczko mogloby sie w miedzyczasie ruszyc i zapisalibysmy nieprawde.
ok("const bool stanWyslany = boxIsOpen();" in push
   and "rtcOpenReported = stanWyslany;" in push,
   "pushStatus zapamietuje dokladnie ten stan, ktory poszedl do bazy")
warn(val("OPEN_WARN_MAX") * val("OPEN_WARN_REPEAT_S") > 24 * 3600,
     "pudelko moze pikac dluzej niz dobe - rozwaz mniejszy OPEN_WARN_MAX")

# ---------- 9d. Wskaznik baterii ----------
ok("BATT_CURVE" in ino and "battPercentFromCurve" in ino,
   "procent liczony z krzywej LiPo, a nie z prostej")
ok("(realBatteryVoltage - 3.3) / (4.2 - 3.3)" not in ino,
   "stare liniowe przeliczanie usuniete")
# Blok pomiaru napiecia: zakaz zmian ZNIESIONY przez Kube 2026-08-05
# (patrz DECYZJE.md, sekcja "Cofniete"). Wolno go modyfikowac.
#
# Zostaje jednak TRIPWIRE zamiast zakazu: zmiana nie wywala audytu, ale zglasza
# sie jako uwaga. Powod - te liczby byly kalibrowane na sprzecie, wiec zmiana
# przypadkowa i zmiana swiadoma wygladaja w diffie identycznie. Uwaga kosztuje
# sekunde uwagi, a odroznia jedno od drugiego.
for frag in ["CALIBRATION_FACTOR = 0.921", "(rawValue / 4095.0) * 3.3",
             "pinVoltage * 2.0", "rawBatteryVoltage * CALIBRATION_FACTOR"]:
    warn(frag not in ino,
         f"zmieniony blok pomiaru napiecia: {frag} - swiadomie? (wolno, patrz DECYZJE.md)")
ok(val("BATT_STEP_DOWN") > val("BATT_STEP_UP"),
   "wskazanie schodzi szybciej niz rosnie")
ok(val("BATT_STEP_UP") >= 1,
   "dopuszczony ruch w gore, zeby chwilowe zaklocenie samo sie zagoilo")
# Tabela musi byc uporzadkowana malejaco po napieciu - inaczej interpolacja klamie.
curve = re.findall(r"\{\s*(\d\.\d+)f\s*,\s*(\d+)\s*\}", ino)
volts = [float(a) for a, _ in curve]
pcts  = [int(b) for _, b in curve]
ok(len(curve) >= 10, f"krzywa ma {len(curve)} punktow")
ok(volts == sorted(volts, reverse=True), "punkty krzywej uporzadkowane malejaco po napieciu")
ok(pcts == sorted(pcts, reverse=True), "procenty tez maleja razem z napieciem")
ok(pcts and pcts[0] == 100 and pcts[-1] == 0, "krzywa zaczyna sie od 100% i konczy na 0%")

# ---------- 9b. Czy to sie W OGOLE wgra z Arduino IDE ----------
#
# Arduino IDE dopisuje prototypy wszystkich funkcji i wstawia je TUZ PRZED
# PIERWSZA DEFINICJA FUNKCJI w pliku. Typ wlasny uzyty w SYGNATURZE, ale
# zadeklarowany nizej niz ten punkt, daje przy wgrywaniu:
#     error: 'WakeReason' was not declared in this scope
# w miejscu, ktore wyglada na zupelnie poprawne.
#
# TO ZATRZYMALO PROJEKT NA MIESIAC. Naprawa B5 dolozyla nvsPutStr() na
# gorze pliku i przesunela punkt wstawiania prototypow ponad `enum
# WakeReason`. Od 1.23.0 do 1.28.0 zadna wersja nie dala sie wgrac, a
# kompilacja jako .cpp (D17) omija ten etap i przez caly czas meldowala
# sukces. Kuba zostal z firmware 1.22.0 w pudelku - nie dlatego, ze nie
# probowal, tylko dlatego, ze sie nie dalo.
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from proto_arduino import pierwsza_funkcja          # noqa: E402

_test_ino = (pathlib.Path(__file__).parent.parent / "firmware" / "PillBoxTest"
             / "PillBoxTest.ino").read_text(encoding="utf-8")

for nazwa, zrodlo in (("PillBox.ino", ino), ("PillBoxTest.ino", _test_ino)):
    granica = pierwsza_funkcja(zrodlo)
    ok(granica is not None, f"{nazwa} - znaleziono pierwsza definicje funkcji")
    if granica is None:
        continue
    linie = zrodlo.split("\n")
    # Typy wlasne zadeklarowane w tym pliku, z numerem linii.
    typy = {m.group(2): i + 1
            for i, l in enumerate(linie)
            for m in [re.match(r"^(enum|struct)\s+(\w+)", l)] if m}
    # Ktore z nich pojawiaja sie w SYGNATURZE jakiejkolwiek funkcji - bo
    # tylko takie trafiaja do wygenerowanych prototypow.
    for typ, gdzie in sorted(typy.items()):
        w_sygnaturze = re.search(
            r"^[A-Za-z_][\w \*&]*\**\w+\([^;{]*\b" + typ + r"\b[^;{]*\)\s*\{",
            zrodlo, re.M) or re.search(r"^" + typ + r"\s+\w+\([^;{]*\)\s*\{", zrodlo, re.M)
        if not w_sygnaturze:
            continue
        ok(gdzie < granica,
           f"{nazwa}: typ '{typ}' (linia {gdzie}) zadeklarowany przed pierwsza "
           f"funkcja (linia {granica}) - inaczej Arduino IDE nie wgra szkicu")

# ---------- 9c. Zagniezdzone prefs.begin() (B22) ----------
#
# Preferences to JEDEN globalny obiekt z JEDNYM uchwytem:
#     bool begin(...) { if (_started) return false; ... }
#     void end()      { nvs_close(_handle); _started = false; }
# begin() na juz otwartym uchwycie nic nie robi, ale end() zagniezdzonego
# wywolania ZAMYKA uchwyt funkcji nadrzednej - i wszystko, co ta funkcja
# zapisze pozniej, przepada.
#
# Tak zginela CALA czarna skrzynka: logbookAdd() wolalo queueCount()
# w argumentach snprintf, w srodku wlasnego bloku. Kazdy zapis po tym
# zawodzil, przy kazdym wybudzeniu, a pushStatus() nadpisywal historie
# w bazie pusta tablica.
_funkcje = {}          # nazwa -> (poczatek, koniec) w liniach
_def = re.compile(r'^[A-Za-z_][A-Za-z0-9_:<>, \*&]*?[ \*&]([A-Za-z_][A-Za-z0-9_]*)\s*\([^;]*\)\s*\{')
_linie = ino.split("\n")
_biezaca = None
for _i, _l in enumerate(_linie):
    _m = _def.match(_l)
    if _m:
        if _biezaca:
            _funkcje[_biezaca][1] = _i
        _biezaca = _m.group(1)
        _funkcje.setdefault(_biezaca, [_i, len(_linie)])
if _biezaca:
    _funkcje[_biezaca][1] = len(_linie)

def _bez_komentarzy(linie):
    """Zwraca linie z wyciętymi komentarzami /* */ i //.
       Nazwa funkcji wymieniona w komentarzu nie jest jej wywolaniem -
       bez tego kontrola nizej podnosi falszywy alarm na wlasnym opisie."""
    out, w_bloku = [], False
    for l in linie:
        buf, i = [], 0
        while i < len(l):
            if w_bloku:
                k = l.find("*/", i)
                if k < 0:
                    i = len(l)
                else:
                    w_bloku = False
                    i = k + 2
            elif l.startswith("/*", i):
                w_bloku = True
                i += 2
            elif l.startswith("//", i):
                break
            else:
                buf.append(l[i])
                i += 1
        out.append("".join(buf))
    return out


def _cialo(nazwa):
    a, b = _funkcje[nazwa]
    return _linie[a:b]

# Ktore funkcje otwieraja prefs - wprost albo przez inna taka funkcje.
_kod = {n: _bez_komentarzy(_cialo(n)) for n in _funkcje}

_otwiera = {n for n in _funkcje if any("prefs.begin(" in l for l in _kod[n])}
for _ in range(6):                       # domkniecie przechodnie
    _nowe = {n for n in _funkcje
             if any(re.search(r'\b' + f + r'\s*\(', l)
                    for l in _kod[n] for f in _otwiera if f != n)}
    if _nowe <= _otwiera:
        break
    _otwiera |= _nowe

_zagniezdzenia = []
for _n in sorted(_funkcje):
    _otwarty = False
    for _off, _goly in enumerate(_kod[_n]):
        if "prefs.begin(" in _goly:
            _otwarty = True
            continue
        if "prefs.end()" in _goly:
            _otwarty = False
            continue
        if not _otwarty:
            continue
        for _f in _otwiera:
            if _f != _n and re.search(r'\b' + _f + r'\s*\(', _goly):
                _zagniezdzenia.append(
                    f"{_n}() linia {_funkcje[_n][0] + _off + 1} wola {_f}(), "
                    f"ktore samo otwiera prefs")
ok(not _zagniezdzenia,
   "zaden blok prefs.begin()...end() nie wola funkcji otwierajacej prefs "
   f"(inaczej zapisy po niej przepadaja - B22){'; ' + '; '.join(_zagniezdzenia) if _zagniezdzenia else ''}")

# ---------- 10. Rzeczy do uzupelnienia przez uzytkownika ----------
todo = "TUTAJ_WPISZ_HASLO_C" in cfg
warn(todo, "DEVICE_PASSWORD nie jest jeszcze uzupelnione w config.h")

# ---------- wynik ----------
for m in PASS: print(f"  OK    {m}")
for m in WARN: print(f"  UWAGA {m}")
for m in FAIL: print(f"  BLAD  {m}")
print(f"\n  {len(PASS)} kontroli zaliczonych, {len(FAIL)} bledow, {len(WARN)} uwag")
sys.exit(1 if FAIL else 0)
