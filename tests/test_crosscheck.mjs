/* =====================================================================
 *  TEST ZGODNOSCI PUDELKA I APLIKACJI
 *
 *  Bierze te same znaczniki czasu i przepuszcza je przez DWIE niezalezne
 *  implementacje tej samej reguly:
 *     - localDayNumber() z PillBox.ino  (skompilowany kod C++)
 *     - devKey() z index.html           (prawdziwy kod aplikacji)
 *
 *  Jesli kiedykolwiek sie rozjada, tabletka trafi w kalendarzu na inny
 *  dzien niz zapisalo ja pudelko. Ten blad nie daje o sobie znac od razu:
 *  wyglada jak "pominieta dawka" tydzien pozniej. Dlatego sprawdzamy go
 *  na dziesiatkach tysiecy chwil z calego roku, ze szczegolnym naciskiem
 *  na granice doby, zmiane czasu i przelom roku.
 *      node test_crosscheck.mjs
 * ===================================================================== */
import * as A from "./app_module.mjs";
import { execFileSync } from "node:child_process";
import { readFileSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const here = dirname(fileURLToPath(import.meta.url));
const BIN  = join(here, "crosscheck_bin");

let PASS = 0, FAIL = 0;
const check = (c, m) => c ? PASS++ : (FAIL++, console.log("  FAIL  " + m));
const head  = t => console.log("\n=== " + t + " ===");

/* Numer doby wedlug APLIKACJI, w tym samym formacie co firmware (YYYYMMDD). */
function dzienAplikacji(ts) {
  const k = A.devKey(ts);                      // "2026-08-01"
  return +k.slice(0, 4) * 10000 + +k.slice(5, 7) * 100 + +k.slice(8, 10);
}

function dzienPudelka(lista, offsetMin) {
  const wej = "dzien\n" + offsetMin + "\n" + lista.join("\n") + "\n";
  const out = execFileSync(BIN, { input: wej, encoding: "utf8" });
  return out.trim().split("\n").map(Number);
}

/* Numer DAWKI wedlug pudelka - prawdziwe matchSlot() z PillBox.ino. */
function slotPudelka(lista, offsetMin, harmonogram) {
  const wej = `slot\n${offsetMin}\n${harmonogram.join("|")}\n` + lista.join("\n") + "\n";
  const out = execFileSync(BIN, { input: wej, encoding: "utf8" });
  return out.trim().split("\n").map(Number);
}

/* Numer DAWKI wedlug aplikacji. Nie da sie tego wywolac wprost - regula
   siedzi w srodku doReconcile() - wiec puszczamy PRAWDZIWA sciezke:
   zdarzenie bez numeru dawki wchodzi, a my patrzymy, pod ktory slot
   aplikacja zapisala je w bazie. -1 = nie przypisala do zadnej.      */
async function slotAplikacji(ts, offsetMin, harmonogram) {
  A.__resetDb();
  A.__setState({
    cfg: { schedule: harmonogram, tzOffsetMin: offsetMin, defaultDose: 5 },
    doses: {},
    events: [{ ts, type: "open" }]          // bez pola slot - niech liczy sama
  });
  await A.doReconcile(true);
  const zapisy = A.__db.writes.flatMap(w => Object.keys(w.val || {}));
  const sciezka = zapisy.find(k => k.includes("/doses/"));
  return sciezka ? Number(sciezka.split("/").pop()) : -1;
}

function porownaj(nazwa, lista, offsetMin) {
  A.__setState({ cfg: { tzOffsetMin: offsetMin } });
  const pud = dzienPudelka(lista, offsetMin);
  const app = lista.map(dzienAplikacji);

  let zle = 0, pierwszy = null;
  for (let i = 0; i < lista.length; i++)
    if (pud[i] !== app[i]) {
      zle++;
      if (!pierwszy)
        pierwszy = `ts=${lista[i]} (${new Date(lista[i] * 1000).toISOString()}) ` +
                   `pudelko=${pud[i]} aplikacja=${app[i]}`;
    }
  check(zle === 0, `${nazwa}: ${zle} z ${lista.length} rozbieznosci` +
                   (pierwszy ? `\n        pierwsza: ${pierwszy}` : ""));
}

/* ═══════════ 1. CALY ROK, CO 7 MINUT ═══════════ */
head("Rok pracy minuta po minucie");
{
  const start = Math.floor(Date.UTC(2026, 0, 1) / 1000);
  const lista = [];
  for (let t = start; t < start + 365 * 86400; t += 7 * 60) lista.push(t);
  porownaj("caly rok co 7 minut", lista, 120);
  console.log(`        sprawdzono ${lista.length} chwil`);
}

/* ═══════════ 2. GRANICA DOBY, SEKUNDA PO SEKUNDZIE ═══════════ */
head("Okolice granicy doby lekowej");
{
  /* Najbardziej podatne miejsce w calym projekcie: przejscie 02:59:59 -> 03:00:00.
     Sprawdzamy kazda sekunde przez cztery godziny wokol granicy, przez tydzien. */
  const lista = [];
  for (let d = 0; d < 7; d++) {
    const polnoc = Math.floor(Date.UTC(2026, 7, 1 + d, 0, 0, 0) / 1000) - 120 * 60;
    for (let s = -3600; s < 3 * 3600; s++) lista.push(polnoc + s);
  }
  porownaj("kazda sekunda wokol granicy", lista, 120);
  console.log(`        sprawdzono ${lista.length} sekund`);
}

/* ═══════════ 3. PRZELOM MIESIACA, ROKU I ROK PRZESTEPNY ═══════════ */
head("Przelomy dat");
{
  const punkty = [];
  const daty = [
    [2026, 0, 1], [2026, 1, 28], [2026, 2, 1], [2026, 6, 31], [2026, 7, 1],
    [2026, 11, 31], [2027, 0, 1], [2028, 1, 28], [2028, 1, 29], [2028, 2, 1]
  ];
  for (const [y, m, d] of daty)
    for (let g = 0; g < 24; g++)
      for (const mi of [0, 1, 29, 30, 31, 59])
        punkty.push(Math.floor(Date.UTC(y, m, d, g, mi, 0) / 1000) - 120 * 60);
  porownaj("przelomy miesiecy, lat i 29 lutego", punkty, 120);
  console.log(`        sprawdzono ${punkty.length} chwil`);
}

/* ═══════════ 4. ROZNE STREFY CZASOWE ═══════════ */
head("Inne strefy czasowe");
{
  for (const off of [-720, -300, -60, 0, 60, 120, 330, 540, 840]) {
    const start = Math.floor(Date.UTC(2026, 5, 1) / 1000);
    const lista = [];
    for (let t = start; t < start + 40 * 86400; t += 23 * 60) lista.push(t);
    porownaj(`offset ${off} min`, lista, off);
  }
}

/* ═══════════ 5. ZMIANA CZASU LETNI/ZIMOWY ═══════════ */
head("Zmiana czasu");
{
  /* Pudelko dostaje z aplikacji sztywne przesuniecie, wiec w dniu zmiany
     obie strony musza uzywac TEJ SAMEJ liczby. Sprawdzamy oba warianty
     wokol ostatniej niedzieli marca i pazdziernika.                     */
  for (const [nazwa, y, m, d, off] of [
    ["ostatnia niedziela marca (przed zmiana)",      2026, 2, 29,  60],
    ["ostatnia niedziela marca (po zmianie)",        2026, 2, 29, 120],
    ["ostatnia niedziela pazdziernika (przed)",      2026, 9, 25, 120],
    ["ostatnia niedziela pazdziernika (po)",         2026, 9, 25,  60]
  ]) {
    const lista = [];
    const baza = Math.floor(Date.UTC(y, m, d - 1, 0, 0, 0) / 1000);
    for (let s = 0; s < 3 * 86400; s += 60) lista.push(baza + s);
    porownaj(nazwa, lista, off);
  }
}

/* ═══════════ 6. WARTOSCI SKRAJNE ═══════════ */
head("Wartosci skrajne");
{
  const lista = [
    1,                                        // poczatek epoki
    86399, 86400, 86401,                      // pierwsza doba
    Math.floor(Date.UTC(2026, 7, 2, 0, 59, 59) / 1000),
    Math.floor(Date.UTC(2038, 0, 19, 3, 14, 7) / 1000),   // granica 32 bitow
    2147483647
  ];
  porownaj("skrajne znaczniki czasu", lista, 120);
}

/* ═══════════ 6b. DOPASOWANIE OTWARCIA DO DAWKI ═══════════
   Druga regula liczona niezaleznie po obu stronach: pudelko ma
   matchSlot(), aplikacja ma wlasny skan po cfg.schedule w doReconcile().
   Obie uzywaja okna +/-90 min i obie musza wskazac TE SAMA dawke.
   Rozjazd oznacza tabletke wpisana pod inna pore niz zostala wzieta -
   a przy dwoch dawkach dziennie to jedna pokazana jako wzieta i druga
   jako pominieta, tego samego dnia.                                  */
head("Zgodnosc dopasowania otwarcia do dawki");
{
  const OFF = 120;
  for (const plan of [["08:00","20:00"], ["06:00","14:00","22:00"], ["00:30","12:30"]]) {
    /* Cala doba co 10 minut - z naciskiem na krawedzie okna +/-90 min
       i na przejscia przez polnoc.                                    */
    const lista = [];
    const polnoc = Math.floor(Date.UTC(2026, 7, 1) / 1000) - OFF * 60;
    for (let s = 0; s < 86400; s += 600) lista.push(polnoc + s);

    const pud = slotPudelka(lista, OFF, plan);
    let zle = 0, pierwszy = null;
    for (let i = 0; i < lista.length; i++) {
      const app = await slotAplikacji(lista[i], OFF, plan);
      if (app !== pud[i]) {
        zle++;
        if (!pierwszy)
          pierwszy = `ts=${lista[i]} (${new Date(lista[i]*1000).toISOString()}) ` +
                     `pudelko=${pud[i]} aplikacja=${app}`;
      }
    }
    check(zle === 0, `harmonogram ${plan.join(", ")}: ${zle} z ${lista.length} rozbieznosci` +
                     (pierwszy ? `\n        pierwsza: ${pierwszy}` : ""));
  }
  console.log(`        sprawdzono 3 harmonogramy po 144 chwile`);
}

head("Krawedzie okna dopasowania (+/-90 min)");
{
  const OFF = 120, plan = ["20:00"];
  /* Przy JEDNEJ dawce dziennie obie strony maja te sama furtke:
     "slot < 0 && jedna pora -> slot 0". Dlatego okno badamy na
     harmonogramie dwudawkowym, gdzie furtka nie dziala.              */
  const plan2 = ["08:00", "20:00"];
  const oDwudziestej = Math.floor(Date.UTC(2026, 7, 1, 20, 0) / 1000) - OFF * 60;
  for (const [dt, opis] of [[-90*60, "dokladnie 90 min przed"],
                            [-91*60, "91 min przed - juz poza oknem"],
                            [ 90*60, "dokladnie 90 min po"],
                            [ 91*60, "91 min po - juz poza oknem"],
                            [ 0,     "co do sekundy"]]) {
    const ts = oDwudziestej + dt;
    const pud = slotPudelka([ts], OFF, plan2)[0];
    const app = await slotAplikacji(ts, OFF, plan2);
    check(pud === app, `${opis}: pudelko=${pud} aplikacja=${app}`);
  }
  void plan;
}

head("Jedna dawka dziennie: obie strony maja te sama furtke");
{
  /* Przy jednej porze otwarcie o dowolnej godzinie ma trafic do slotu 0,
     nawet jesli wypadlo daleko od pory leku. Pudelko robi to poza
     matchSlot(), wiec porownujemy z sama aplikacja.                   */
  const OFF = 120;
  const polnoc = Math.floor(Date.UTC(2026, 7, 1) / 1000) - OFF * 60;
  let zle = 0;
  for (let s = 0; s < 86400; s += 3600) {
    const app = await slotAplikacji(polnoc + s, OFF, ["20:00"]);
    if (app !== 0) zle++;
  }
  check(zle === 0, `kazde otwarcie trafia do jedynej dawki (${zle} wyjatkow)`);
  check(/slot < 0 && slotCount == 1\) slot = 0/.test(
          readFileSync(join(here, "..", "firmware", "PillBox", "PillBox.ino"), "utf8")),
        "pudelko ma te sama furtke w setup()");
}

/* ═══════════ 6c. LICZNIK TABLETEK: KONTRAKT MIEDZY STRONAMI ═══════════
   Tu nie ma dwoch implementacji jednej reguly - jest kontrakt. Aplikacja
   LICZY pillsLeft i zapisuje do config, pudelko tylko go CZYTA i piszczy
   "konczy sie opakowanie". Zeby ten pisk mial sens, liczba zapisana przez
   aplikacje musi znaczyc dla pudelka to samo.                        */
head("Licznik tabletek: aplikacja liczy, pudelko piszczy");
{
  const fw  = readFileSync(join(here, "..", "firmware", "PillBox", "PillBox.ino"), "utf8");
  const cfgH = readFileSync(join(here, "..", "firmware", "PillBox", "config.h"), "utf8");
  const PROG = +cfgH.match(/#define\s+LOW_STOCK_WARN\s+(\d+)/)[1];

  check(/rtcPillsLeft = doc\["pillsLeft"\]\.as<int>\(\)/.test(fw),
        "pudelko czyta dokladnie to pole, ktore zapisuje aplikacja");
  check(/rtcPillsLeft >= 0 && rtcPillsLeft < LOW_STOCK_WARN/.test(fw),
        `pudelko piszczy przy zapasie ponizej ${PROG}`);

  /* Liczba zapisana przez PRAWDZIWE settlePills(). Pudelko czyta ja
     jako int, wiec musi byc nieujemna i bez ogona, ktory by uciello. */
  const dzien = n => new Date(Date.UTC(2026, 0, 1 + n)).toISOString().slice(0, 10);

  async function policz(baza, dni, dawka) {
    const doses = {};
    for (let i = 0; i < dni; i++) doses[dzien(i)] = { 0: { status: "taken", dose: dawka } };
    A.__resetDb();
    A.__setState({
      cfg: { schedule: ["20:00"], defaultDose: dawka, tzOffsetMin: 120,
             pillsBase: baza, pillsBaseFrom: dzien(0) },
      doses
    });
    await A.settlePills();
    const wpis = A.__db.writes.flatMap(w => Object.entries(w.val || {}))
                              .find(([k]) => k.endsWith("/config/pillsLeft"));
    return wpis ? wpis[1] : undefined;
  }

  const cale = await policz(30, 12, 1);
  check(cale === 18, `30 tabletek minus 12 dni po jednej = ${cale}`);

  const polowki = await policz(30, 13, 0.5);
  check(polowki === 23.5, `30 minus 13 dni po pol tabletki = ${polowki}`);
  check(Number.isInteger(polowki * 100),
        "wartosc bez ogona po przecinku, ktory pudelko i tak by uciello");

  const naZero = await policz(10, 40, 1);
  check(naZero === 0, `licznik nie schodzi ponizej zera (${naZero})`);
  check(naZero >= 0, "ujemna wartosc oznaczalaby dla pudelka 'nie wiem' i wylaczyla pisk");

  /* Kluczowe: prog pudelka jest w TABLETKACH, a ostrzezenie w aplikacji
     w DNIACH ZAPASU. Przy jednej dawce dziennie to ta sama liczba -
     i tak dziala pudelko Kuby. Przy dwoch dawkach dziennie juz nie.  */
  const dniZapasu = (left, perDay) => Math.floor(left / perDay);
  let zgodne = 0, rozjazd = 0;
  for (let left = 0; left <= 30; left++) {
    const pudelkoPiszczy = left >= 0 && left < PROG;
    const aplikacjaOstrzega = dniZapasu(left, 1) < 7;
    if (pudelkoPiszczy === aplikacjaOstrzega) zgodne++; else rozjazd++;
  }
  check(rozjazd === 0,
        `jedna dawka dziennie: pisk i ostrzezenie na tym samym progu (${zgodne}/31)`);

  /* ZNANE OGRANICZENIE (B7 w DECYZJE). Przy dwoch dawkach dziennie
     aplikacja mowi "czas po recepte" juz przy 13 tabletkach, a pudelko
     milczy az do szostej. Test PRZYPINA ten stan, zeby ewentualna
     zmiana progu byla swiadoma, a nie przypadkowa.                   */
  let cichePudelko = 0;
  for (let left = 0; left <= 30; left++)
    if (dniZapasu(left, 2) < 7 && !(left < PROG)) cichePudelko++;
  check(cichePudelko === 7,
        `dwie dawki dziennie: przez ${cichePudelko} wartosci aplikacja ostrzega, ` +
        `a pudelko milczy - znane ograniczenie B7`);
}

/* ═══════════ 7. LICZNIKI STRAT ═══════════ */
head("Liczniki strat: pudelko wysyla, aplikacja czyta");
{
  const fw  = readFileSync(join(here, "..", "firmware", "PillBox", "PillBox.ino"), "utf8");
  const app = readFileSync(join(here, "..", "index.html"), "utf8");
  /* Nazwy pol musza sie zgadzac po obu stronach. Rozjazd oznacza
     ostrzezenie, ktore nigdy sie nie zapali - czyli ciche straty wracaja. */
  for (const pole of ["dropped", "nvsFail"]) {
    check(fw.includes(`doc["${pole}"]`), `pudelko wysyla w statusie: ${pole}`);
    check(new RegExp(`st\\.${pole}`).test(app), `aplikacja czyta ze statusu: ${pole}`);
  }
}

console.log("\n──────────────────────────────────────");
console.log(`  ZALICZONE: ${PASS}    BLEDY: ${FAIL}`);
console.log("──────────────────────────────────────");
process.exit(FAIL ? 1 : 0);
