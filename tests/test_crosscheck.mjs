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
  /* Zapis idzie transakcja na sciezke (D35) - `path` jest wlasnym polem
     wpisu, nie kluczem wewnatrz `val` jak przy dawnym zbiorczym update(). */
  const sciezka = A.__db.writes.map(w => w.path).find(p => p && p.includes("/doses/"));
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

/* ═══════════ 6b. ZADNA DAWKA NIE MOZE PRZEPASC ═══════════
   Godziny w cfg.schedule to PRZYPOMNIENIA, nie pory brania leku. Kuba
   bierze tabletke kiedy chce - o 10, o 14, o 21, czasem o 2 w nocy -
   a pudelko ma tylko przypomniec, jesli do danej godziny jeszcze jej
   nie wzial. Dawka jest JEDNA dziennie (ONE_DOSE_PER_DAY w firmware).

   TU BYL BLAD, ktory ujawnil sie dopiero przy DRUGIM przypomnieniu.
   doReconcile() mial furtke "jedna pora -> slot 0"; przy dwoch porach
   furtka znikala, otwarcie o 14:00 nie pasowalo do zadnej z nich
   (okno +/-90 min), slot zostawal -1 i `continue` wyrzucalo zdarzenie
   do kosza. Dawka wzieta poza pora przypomnienia po prostu nie trafiala
   do kalendarza i wygladala jak pominieta.

   Niezmiennik jest teraz prosty i tego pilnujemy: KAZDE otwarcie zostaje
   zapisane, w dobie wskazanej przez devKey(), zawsze w slocie 0.       */
head("Zadna dawka nie przepada, niezaleznie od liczby przypomnien");
{
  const OFF = 120;
  for (const plan of [["20:00"], ["20:00","23:00"], ["06:00","14:00","22:00"]]) {
    const polnoc = Math.floor(Date.UTC(2026, 7, 1) / 1000) - OFF * 60;
    let zgubione = 0, zlySlot = 0, pierwszy = null;
    for (let s = 0; s < 86400; s += 600) {
      const ts = polnoc + s;
      const slot = await slotAplikacji(ts, OFF, plan);
      if (slot < 0) { zgubione++; if (!pierwszy) pierwszy = new Date(ts*1000).toISOString(); }
      else if (slot !== 0) zlySlot++;
    }
    check(zgubione === 0,
          `${plan.length} przypomnien: ${zgubione} zgubionych dawek` +
          (pierwszy ? ` (pierwsza: ${pierwszy})` : ""));
    check(zlySlot === 0,
          `${plan.length} przypomnien: ${zlySlot} dawek pod numerem innym niz 0 ` +
          `- arkusz dnia czyta doses[data][0], wiec inny numer = dawka niewidzialna`);
  }
  console.log(`        sprawdzono 3 uklady przypomnien po 144 chwile doby`);
}

head("Dawka trafia do WLASCIWEJ doby, nie tylko do wlasciwego slotu");
{
  /* Numer slotu to polowa sprawy - drugie tyle warta jest doba. Godziny
     nocne sa tu najwazniejsze: otwarcie o 02:00 nalezy do doby
     POPRZEDNIEJ (DAY_START_HOUR = 3).                                 */
  const OFF = 120;
  for (const [g, m] of [[0,30],[2,0],[2,59],[3,0],[3,1],[13,0],[23,59]]) {
    const ts = Math.floor(Date.UTC(2026, 7, 2, g, m) / 1000) - OFF * 60;
    A.__resetDb();
    A.__setState({ cfg: { schedule: ["20:00","23:00"], tzOffsetMin: OFF, defaultDose: 1 },
                   doses: {}, events: [{ ts, type: "open" }] });
    await A.doReconcile(true);
    /* Transakcja (D35): sciezka jest polem `path` wpisu, nie kluczem `val`. */
    const klucz = A.__db.writes[0]?.path || "";
    const oczekiwana = `users/testuid/doses/${A.devKey(ts)}/0`;
    check(klucz === oczekiwana,
          `${String(g).padStart(2,"0")}:${String(m).padStart(2,"0")} -> ${klucz || "NIC"} (oczekiwano ${oczekiwana})`);
  }
}

head("Pudelko nadal poprawnie rozpoznaje, ktore przypomnienie dzwoni");
{
  /* matchSlot() przestal decydowac o zapisie, ale dalej decyduje o tym,
     KTORE przypomnienie trwa - a od tego zalezy drzemka alarmu. Okna
     +/-90 min pilnujemy wiec po stronie pudelka osobno.               */
  const OFF = 120, plan = ["08:00", "20:00"];
  const oDwudziestej = Math.floor(Date.UTC(2026, 7, 1, 20, 0) / 1000) - OFF * 60;
  for (const [dt, opis, oczek] of [[-90*60, "dokladnie 90 min przed", 1],
                                   [-91*60, "91 min przed - poza oknem", -1],
                                   [ 90*60, "dokladnie 90 min po", 1],
                                   [ 91*60, "91 min po - poza oknem", -1],
                                   [ 0,     "co do sekundy", 1]]) {
    const pud = slotPudelka([oDwudziestej + dt], OFF, plan)[0];
    check(pud === oczek, `${opis}: pudelko=${pud} (oczekiwano ${oczek})`);
  }
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
