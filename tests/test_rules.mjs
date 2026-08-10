/* =====================================================================
 *  TESTY REGUL BAZY
 *
 *  Do tej pory database.rules.json byl sprawdzany jako poprawny JSON
 *  i jednym skanem, czy piec nazw pol wystepuje gdziekolwiek w pliku.
 *  Nie bylo NICZEGO, co sprawdzaloby, czy zapis wysylany przez pudelko
 *  albo aplikacje w ogole przez te reguly przejdzie.
 *
 *  Konkretny scenariusz, ktorego caly zestaw testow nie lapal:
 *  ktos dopisuje pole do zdarzenia w firmware, "$other": false odrzuca
 *  CALY wpis kodem 400, a trwaleOdrzucony(400) po naprawie B3 KASUJE
 *  dawke z kolejki pudelka. Testy zielone, dawka przepadla.
 *
 *      node test_rules.mjs
 * ===================================================================== */
import { wczytajReguly, sprawdzZapis, opiszBledy, nieobslugiwane } from "./rules_engine.mjs";
import { readFileSync } from "node:fs";

const R = wczytajReguly();
const ino = readFileSync(new URL("../firmware/PillBox/PillBox.ino", import.meta.url), "utf8");

let PASS = 0, FAIL = 0;
const check = (c, m) => c ? PASS++ : (FAIL++, console.log("  FAIL  " + m));
const head  = t => console.log("\n=== " + t + " ===");

const przejdzie = (sciezka, val) => sprawdzZapis(R, sciezka, val).ok;
const czemu     = (sciezka, val) => opiszBledy(sprawdzZapis(R, sciezka, val).bledy);

const ok = (nazwa, sciezka, val) =>
  check(przejdzie(sciezka, val), `${nazwa} POWINNO przejsc, a nie przechodzi: ${czemu(sciezka, val)}`);
const odrzuc = (nazwa, sciezka, val) =>
  check(!przejdzie(sciezka, val), `${nazwa} POWINNO zostac odrzucone, a przeszlo`);

/* ═══════════ 1. SILNIK MA ZEBY ═══════════
   Walidator, ktory przepuszcza wszystko, jest gorszy niz jego brak:
   daje falszywe poczucie pokrycia. Zanim uwierzymy w wyniki ponizej,
   sprawdzamy, ze silnik naprawde odrzuca to, co baza odrzuci.        */
head("Silnik regul odrzuca to, co baza odrzuci");
odrzuc("nieznane pole w zdarzeniu ($other:false)",
       "devices/pillbox1/events/e1", { ts: 1, type: "open", openTs: 1 });
odrzuc("nieznane pole w dawce ($other:false)",
       "users/u1/doses/2026-08-01/0", { status: "taken", dose: 1, cokolwiek: 7 });
odrzuc("zdarzenie bez wymaganego type",  "devices/pillbox1/events/e1", { ts: 1 });
odrzuc("dawka bez wymaganego status",    "users/u1/doses/2026-08-01/0", { dose: 1 });
odrzuc("status dawki spoza slownika",    "users/u1/doses/2026-08-01/0", { status: "wziete" });
odrzuc("zrodlo dawki spoza slownika",    "users/u1/doses/2026-08-01/0", { status: "taken", source: "bot" });
odrzuc("ts jako napis, nie liczba",      "devices/pillbox1/events/e1", { ts: "1750000000", type: "open" });
odrzuc("pillsLeft ujemny",               "devices/pillbox1/config/pillsLeft", -1);
odrzuc("tzOffsetMin poza zakresem",      "devices/pillbox1/config/tzOffsetMin", 999);
odrzuc("INR poza skala pomiaru",         "users/u1/inr/2026-08-01", { value: 99 });
odrzuc("INR jako napis",                 "users/u1/inr/2026-08-01", { value: "2.5" });
odrzuc("pusty harmonogram",              "devices/pillbox1/config/schedule", []);
odrzuc("zapis poza drzewem regul",       "cokolwiek/tam", { a: 1 });
odrzuc("notatka dluzsza niz limit",
       "users/u1/doses/2026-08-01/0", { status: "taken", note: "x".repeat(301) });

head("Silnik regul przepuszcza to, co poprawne");
ok("zdarzenie z pudelka",  "devices/pillbox1/events/e1",
   { ts: 1750000000, type: "open", battery: 80, volt: 4.02, slot: 0, fw: "1.22.0" });
ok("dawka z urzadzenia",   "users/u1/doses/2026-08-01/0",
   { status: "taken", dose: 5, source: "device", ts: 1750000000, openTs: 1750000000 });
ok("dawka poprawiona recznie", "users/u1/doses/2026-08-01/0",
   { status: "missed", dose: 0, source: "manual", ts: 1750000000, note: "zapomnialem" });
ok("pomiar INR",           "users/u1/inr/2026-08-01", { value: 2.5, ts: 1750000000, note: "" });
ok("harmonogram",          "devices/pillbox1/config/schedule", ["20:00"]);
ok("znacznik to-ja",       "users/u1/lidMarks/2026-08-01-20", true);
ok("paczka dziennika wieczka", "devices/pillbox1/lidlog/1750000000", { wpisy: "x" });

/* config ma $other: true - to swiadoma decyzja (D10, D12), zeby dodanie
   pola nie wymagalo od Kuby publikowania nowych regul. Gdyby ktos to
   kiedys zmienil, inrEveryDays i pillsBase przestalyby sie zapisywac. */
head("Nowe pola konfiguracji nie wymagaja publikowania regul (D10/D12)");
ok("nieznane pole w config przechodzi", "devices/pillbox1/config",
   { schedule: ["20:00"], inrEveryDays: 21, cosZupelnieNowego: 1 });
ok("nieznane pole w statusie przechodzi", "devices/pillbox1/status",
   { battery: 80, cosZupelnieNowego: 1 });

/* ═══════════ 2. TO, CO NAPRAWDE WYSYLA PUDELKO ═══════════ */
head("Zapisy pudelka przechodza przez reguly");

/* Pola wyciagniete z PRAWDZIWEGO zrodla. Gdy ktos dopisze pole do
   zdarzenia, ta lista urosnie i test od razu powie, ze reguly go nie
   znaja - zamiast ciszy i skasowanej dawki u Kuby.                  */
function polaJson(nazwaFunkcji) {
  const i = ino.indexOf(nazwaFunkcji);
  if (i < 0) throw new Error("nie znaleziono " + nazwaFunkcji);
  /* koniec funkcji: pierwsza linia zaczynajaca sie od "}" w kolumnie 0 */
  const koniec = ino.indexOf("\n}", i);
  const cialo = ino.slice(i, koniec < 0 ? undefined : koniec);
  return [...cialo.matchAll(/doc\["(\w+)"\]/g)].map(m => m[1]);
}

const POLA_ZDARZENIA = polaJson("int pushEventRecord(");
check(POLA_ZDARZENIA.length > 0, "udalo sie wyciagnac pola zdarzenia ze zrodla");

/* Wartosci dobrane tak, jak realnie wysyla pudelko. */
const PRZYKLAD_ZDARZENIA = {
  ts: 1750000000, type: "open", battery: 80, volt: 4.02, slot: 0, fw: "1.22.0"
};
check(POLA_ZDARZENIA.every(p => p in PRZYKLAD_ZDARZENIA) &&
      Object.keys(PRZYKLAD_ZDARZENIA).every(p => POLA_ZDARZENIA.includes(p)),
      "pola zdarzenia w firmware zgadzaja sie z lista w tescie " +
      `(firmware: ${POLA_ZDARZENIA.join(",")} | test: ${Object.keys(PRZYKLAD_ZDARZENIA).join(",")}) ` +
      "- jesli dodales pole, dopisz je TAKZE do database.rules.json");
ok("zdarzenie z pushEventRecord()", "devices/pillbox1/events/e1", PRZYKLAD_ZDARZENIA);

/* Kazde pole osobno - zeby komunikat wskazywal winowajce, a nie caly wpis. */
for (const p of POLA_ZDARZENIA)
  check(przejdzie("devices/pillbox1/events/e1",
                  { ts: 1, type: "open", [p]: PRZYKLAD_ZDARZENIA[p] }),
        `pole "${p}" ze zdarzenia jest znane regulom`);

/* Typ zdarzenia musi miescic sie w limicie 16 znakow. */
for (const t of [...ino.matchAll(/reportEvent\("(\w+)"/g)].map(m => m[1]))
  check(przejdzie("devices/pillbox1/events/e1", { ts: 1, type: t }),
        `typ zdarzenia "${t}" przechodzi przez reguly`);

head("Status pudelka przechodzi przez reguly");
const POLA_STATUSU = polaJson("bool pushStatus(");
const PRZYKLAD_STATUSU = {
  battery: 80, battRaw: 82, volt: 4.02, lastSeen: 1750000000, rssi: -60,
  ssid: "dom", fw: "1.22.0", boots: 12, queued: 0, dropped: 0, nvsFail: 0,
  nvsFree: 512,
  charging: false, chargeSince: 0, chargeFromPct: -1, lastCharge: 0,
  prevCharge: 0, boxOpen: false, openSince: 0
};
check(POLA_STATUSU.every(p => p in PRZYKLAD_STATUSU),
      `pola statusu nieopisane w tescie: ${POLA_STATUSU.filter(p => !(p in PRZYKLAD_STATUSU))}`);
ok("pelny status z pushStatus()", "devices/pillbox1/status", PRZYKLAD_STATUSU);

/* pushLidState() lepi JSON recznie w snprintf, wiec pola bierzemy stamtad. */
{
  const i = ino.indexOf("bool pushLidState(");
  const cialo = ino.slice(i, ino.indexOf("\n}", i));
  const pola = [...cialo.matchAll(/\\"(\w+)\\":/g)].map(m => m[1]);
  check(pola.length === 3, `pushLidState wysyla 3 pola (${pola.join(",")})`);
  const przyklad = {};
  for (const p of pola) przyklad[p] = (p === "boxOpen") ? true : 1750000000;
  ok("PATCH stanu wieczka", "devices/pillbox1/status", przyklad);
}

/* Bateria w zdarzeniu bywa 0-100, a napiecie ulamkowe - reguly nie moga
   sie na tym wylozyc przy skrajnych, ale prawdziwych pomiarach.       */
head("Skrajne, ale prawdziwe pomiary z pudelka");
for (const [b, v] of [[0, 3.20], [100, 4.20], [1, 3.31], [99, 4.19]])
  ok(`bateria ${b}%, ${v}V`, "devices/pillbox1/events/e1",
     { ts: 1750000000, type: "open", battery: b, volt: v, slot: 0, fw: "1.22.0" });

/* ═══════════ 3. TO, CO NAPRAWDE WYSYLA APLIKACJA ═══════════ */
head("Zapisy aplikacji przechodza przez reguly");
ok("recznie oznaczona dawka", "users/u1/doses/2026-08-01/0",
   { status: "taken", dose: 5, source: "manual", ts: 1750000000 });
ok("dawka pominieta recznie", "users/u1/doses/2026-08-01/0",
   { status: "missed", dose: 0, source: "manual", ts: 1750000000 });
ok("dawka pominieta swiadomie", "users/u1/doses/2026-08-01/0",
   { status: "skipped", dose: 0, source: "manual", ts: 1750000000 });
ok("uzupelnienie z pudelka (doReconcile)", "users/u1/doses/2026-08-01/0",
   { status: "taken", dose: 1, source: "device", ts: 1750000000, openTs: 1750000000 });
ok("ustawienia polami (zapiszCfg)", "devices/pillbox1/config/pillsBase", 30);
ok("data bazy licznika", "devices/pillbox1/config/pillsBaseFrom", "2026-08-01");
ok("odstep INR (D10)", "devices/pillbox1/config/inrEveryDays", 21);
ok("skasowanie znacznika to-ja", "users/u1/lidMarks/2026-08-01-20", null);

/* pillsBaseFrom i pillsCountedUntil to daty "YYYY-MM-DD" = 10 znakow.
   Regula dopuszcza <= 10, wiec kazdy dluzszy format ja zlamie.       */
odrzuc("data dluzsza niz YYYY-MM-DD",
       "devices/pillbox1/config/pillsBaseFrom", "2026-08-01T20:00:00Z");

/* ═══════════ 4. STRAZNIK D13: co baza odrzuci NA STALE ═══════════
   trwaleOdrzucony() kasuje wpis przy 400. Zdarzenie, ktore reguly
   odrzucaja, wraca wlasnie 400 - wiec kazde takie pole to nie
   "zapis sie nie udal", tylko "dawka przepadla".                    */
head("Odrzucone zdarzenie = skasowana dawka (D13)");
check(/return code == 400 \|\| code == 413;/.test(ino),
      "trwaleOdrzucony nadal kasuje przy 400 - dlatego reguly musza sie zgadzac");
odrzuc("zdarzenie z polem spoza regul zostaloby skasowane",
       "devices/pillbox1/events/e1",
       { ...PRZYKLAD_ZDARZENIA, nowePole: 1 });

/* ═══════════ 5. GRANICE SAMEGO SILNIKA ═══════════
   Silnik nie liczy wyrazen z auth/data/root - i slusznie, bo wymagalyby
   symulowania logowania i stanu calej bazy. Wazne jest, zeby taka regula
   NIE zamienila sie w falszywy alarm "reguly odrzucily zapis", bo wtedy
   komunikat wskazywalby na dane zamiast na silnik. Zamiast tego
   przepuszczamy - i pilnujemy tutaj, czy w pliku cos takiego przybylo. */
head("Granice silnika sa jawne, a nie zamieniaja sie w falszywy alarm");
{
  const walidacje = [];
  (function zbierz(n){
    if (!n || typeof n !== "object") return;
    for (const [k, v] of Object.entries(n)) {
      if (k === ".validate") walidacje.push(v);
      else zbierz(v);
    }
  })(R);
  check(walidacje.length > 0, `znaleziono ${walidacje.length} regul .validate`);

  const poza = walidacje.filter(nieobslugiwane);
  check(poza.length === 0,
        `${poza.length} regul .validate uzywa auth/data/root - silnik ich NIE ` +
        `sprawdza, wiec te pola sa poza pokryciem testow: ${JSON.stringify(poza)}`);

  /* Regula, ktorej silnik nie umie policzyc, ma PRZEPUSCIC zapis. */
  const udawana = { ".validate": "root.child('x').val() === true" };
  check(nieobslugiwane(udawana[".validate"]),
        "wyrazenie z root jest rozpoznawane jako nieobslugiwane");
}

console.log("\n──────────────────────────────────────");
console.log(`  ZALICZONE: ${PASS}    BLEDY: ${FAIL}`);
console.log("──────────────────────────────────────");
process.exit(FAIL ? 1 : 0);
