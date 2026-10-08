/* =====================================================================
 *  PODGLAD APLIKACJI W PRZEGLADARCE
 *
 *  Zasada 14 mowi: "Wyglad sprawdzaj NA RENDERZE, nie w wyobrazni".
 *  Do tej pory nie bylo czym - testy sprawdzaja logike i tekst w HTML,
 *  ale nikt nie widzial ekranu. Ten plik buduje z index.html strone,
 *  ktora otwiera sie BEZ Firebase i BEZ logowania, z podstawionym stanem.
 *
 *      node tests/podglad.mjs                 -> tests/podglad.html
 *      node tests/podglad.mjs --zrzut         -> dodatkowo zrzuty ekranu
 *
 *  Podglad jest NARZEDZIEM, nie czescia aplikacji: nie wchodzi do
 *  run_all.sh i nie jest publikowany. Sluzy do popatrzenia.
 * ===================================================================== */
import { readFileSync, writeFileSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const here = dirname(fileURLToPath(import.meta.url));
let html = readFileSync(join(here, "..", "index.html"), "utf8");

/* Atrapa Firebase wprost w module - zadnych importow z sieci.
   Nasluchy NIE oddzwaniaja: boot() ma sie nie uruchomic, bo stan
   podstawiamy sami. Inaczej podglad zalezalby od kolejnosci zdarzen,
   ktorej w przegladarce bez sieci i tak nie ma jak odtworzyc.        */
const atrapa = `
const initializeApp = () => ({});
const getAuth = () => ({ currentUser: { uid:"podglad", email:"podglad@przyklad.pl" } });
const getDatabase = () => ({});
const signInWithEmailAndPassword = async () => ({});
const onAuthStateChanged = () => {};
const signOut = async () => {};
const setPersistence = async () => {};
const indexedDBLocalPersistence = {}, browserLocalPersistence = {};
const ref = (_d, path = "") => ({ path });
const onValue = () => {};
const set = async () => {}, update = async () => {}, remove = async () => {};
const query = r => r, orderByChild = () => ({}), limitToLast = () => ({});
const goOnline = () => {};
const get = async () => ({ val: () => null, forEach: () => false });
const runTransaction = async () => ({ committed:true });
`;

const przed = html.length;
html = html.replace(/^\s*import\s+\{[\s\S]*?\}\s+from\s+"https:\/\/www\.gstatic\.com[^"]*";/gm, "");
if (html.length === przed) { console.error("Nie usunieto importow Firebase"); process.exit(1); }

/* Atrape wstawiamy na POCZATEK modulu, a wejscie dla podgladu na koniec -
   musi widziec wszystko, co modul zdefiniowal.                        */
html = html.replace('<script type="module">', '<script type="module">' + atrapa);

const wejscie = `
/* Wejscie podgladu: sam sie uruchamia po zaladowaniu, wedlug ?profil= i
   ?ekran= w adresie. Dzieki temu wystarczy otworzyc plik - albo kazac
   przegladarce zrobic zrzut - i nie trzeba niczym sterowac z zewnatrz. */
function __podglad(stan = {}) {
  Object.assign(cfg, stan.cfg || {});
  doses = stan.doses || {};
  events = stan.events || [];
  uid = "podglad";
  pudelkaWidoczne = stan.pudelka || PUDELKA.map(p => p.id);
  if (stan.urzadzenie) DEVICE_ID = stan.urzadzenie;
  document.getElementById("splash")?.classList.add("hide");
  document.getElementById("login")?.classList.add("hide");
  document.getElementById("app")?.classList.remove("hide");
  const d = new Date(); viewYear = d.getFullYear(); viewMonth = d.getMonth();
  ustawProfil();
  renderAll();
  /*  STATUS PUDELKA, bo bez niego ekran Urzadzenie jest pusty - a to
      wlasnie tam ladujemy diagnostyke, ktorej Kuba nie przeczyta
      w monitorze portu (D142).                                      */
  if (stan.status) renderStatus(stan.status);
  showTab(stan.ekran || "cal");
}
window.__podglad = __podglad;

/* Dane pokazowe. Kilkanascie dni wstecz, zeby kalendarz i pierscien
   skutecznosci mialy co rysowac, a nie swiecily pustka.             */
function __dane(profil){
  const doses = {}, events = [];
  const dzis = new Date();
  for (let i = 25; i >= 0; i--){
    const d = new Date(dzis); d.setDate(dzis.getDate() - i);
    const k = \`\${d.getFullYear()}-\${String(d.getMonth()+1).padStart(2,"0")}-\${String(d.getDate()).padStart(2,"0")}\`;
    const ts = Math.floor(d.getTime()/1000) + 20*3600;
    if (i === 0) continue;                       // dzis zostawiamy nietkniete
    if (i % 9 === 4) { doses[k] = { 0:{ status:"missed", dose:0, source:"device", ts } }; continue; }
    doses[k] = { 0:{ status:"taken", dose: profil === "tydzien" ? 1 : 1.5,
                     source:"device", ts, openTs: ts } };
    events.push({ id:"e"+i, ts, type:"open", slot: profil === "tydzien" ? (d.getDay()+6)%7 : 0 });
  }
  const cfg = profil === "tydzien"
    ? { profil:"tydzien", schedule:["20:00"], drugName:"Escitalopram", defaultDose:1 }
    : { profil:"warfin",  schedule:["20:00"], drugName:"Warfin", drugStrength:5, defaultDose:1.5,
        pillsBase:100, pillsBaseFrom:"2026-08-25", inrMin:2, inrMax:3, inrEveryDays:21 };
  const teraz = Math.floor(Date.now()/1000);
  /*  Stan pudelka podstawiamy taki, jaki NAPRAWDE moze przyjsc: tygodniowe
      z czujnikiem baterii i z pomiarem porownawczym z dzielnika (D141),
      dzienne po staremu.                                              */
  const status = profil === "tydzien"
    ? { lastSeen: teraz - 120, battery: 87, volt: 3.98, voltDz: 2.32,
        battSrc: "max17048", gauge: "ok", fw: "0.12.0", rssi: -56, queue: 0,
        boots: 134, ssid: "Dom", boxOpen: false, tg: true }
    : { lastSeen: teraz - 300, battery: 64, volt: 3.86, battSrc: "dzielnik",
        fw: "1.53.0", rssi: -61, queue: 0, boots: 2041, ssid: "Dom", boxOpen: false };
  return { cfg, doses, events, status,
           urzadzenie: profil === "tydzien" ? "pillbox02" : "pillbox01" };
}

addEventListener("DOMContentLoaded", () => {
  const q = new URLSearchParams(location.search);
  const profil = q.get("profil") === "tydzien" ? "tydzien" : "warfin";
  const stan = __dane(profil);
  stan.ekran = q.get("ekran") || "cal";
  /*  ?czujnik=ok|czeka|cichy pozwala zobaczyc KAZDY z trzech stanow
      czujnika baterii (D142) - rozniace sie kolorem i czynnoscia, wiec
      sprawdzalne tylko okiem.                                        */
  const cz = q.get("czujnik");
  if (cz && stan.status) {
    stan.status.gauge = cz;
    if (cz !== "ok") {
      /* Bez czujnika procent przychodzi z dzielnika - inna liczba i inne
         zrodlo, inaczej podglad pokazywalby stan, ktory nie moze zajsc. */
      stan.status.battSrc = "dzielnik";
      stan.status.battery = 48;
      stan.status.volt    = 3.80;
      delete stan.status.voltDz;
    }
  }
  __podglad(stan);
});
</script>`;
html = html.replace(/<\/script>(?![\s\S]*<\/script>)/, wejscie);

/* Obrazki leza obok index.html, a podglad w tests/ - inaczej tabletka
   w medalionie jest ikona zepsutego pliku i podglad klamie o wygladzie. */
html = html.replace(/(src|srcset)="(tabletka\.[a-z]+)"/g, '$1="../$2"');

writeFileSync(join(here, "podglad.html"), html, "utf8");
console.log("Zbudowano tests/podglad.html");
