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
  const wej = offsetMin + "\n" + lista.join("\n") + "\n";
  const out = execFileSync(BIN, { input: wej, encoding: "utf8" });
  return out.trim().split("\n").map(Number);
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

console.log("\n──────────────────────────────────────");
console.log(`  ZALICZONE: ${PASS}    BLEDY: ${FAIL}`);
console.log("──────────────────────────────────────");
process.exit(FAIL ? 1 : 0);
