/* Wyciaga <script type="module"> z index.html, podmienia importy
   Firebase na atrapy i dokleja eksport wewnetrznych funkcji, zeby testy
   dzialaly na PRAWDZIWYM kodzie aplikacji, a nie na jego kopii.        */
import { readFileSync, writeFileSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const here = dirname(fileURLToPath(import.meta.url));
const html = readFileSync(join(here, "..", "index.html"), "utf8");

const m = html.match(/<script type="module">([\s\S]*?)<\/script>/);
if (!m) { console.error("Nie znaleziono modulu w index.html"); process.exit(1); }

let js = m[1];

const before = js.length;
js = js.replace(/^import\s+\{[\s\S]*?\}\s+from\s+"https:\/\/www\.gstatic\.com[^"]*";/gm, "");
if (js.length === before) { console.error("Nie usunieto importow Firebase"); process.exit(1); }

const header = `
import { initializeApp, getAuth, signInWithEmailAndPassword, onAuthStateChanged, signOut,
         setPersistence, indexedDBLocalPersistence, browserLocalPersistence,
         getDatabase, ref, onValue, set, update, remove, query, orderByChild, limitToLast,
         goOnline, get,
         __db, __resetDb } from "./firebase_stub.mjs";
import "./dom_stub.mjs";
`;

const footer = `
export { brakujePokrycia, doReconcile, opisLadowan, minutyDoPelna, opisLadowania, renderAll, toast, tablet3D, cieniuj, TAB3D_SEGMENTOW, busy, seriaDni, doNastepnej, opisCzasu,
         cfg, doses, inr, events, tabletSVG, doseGraphic,
         dayDose, dayStatus, devKey, devHM, devDate, inrState,
         inrOdstep, inrTerminKey, inrDoTerminu, dniTxt, renderOstrzezenia, ostrzStraty,
         collectRows, inrChart, nf, esc, renderCalendar, renderToday,
         renderInr, renderPills, renderSheet, settlePills, relTime, slotMin,
         yesterdayKey, pillColors, todayKey, dateKey, DAY_START_HOUR, medDate,
         inNightWindow, renderOpenWarn, renderDiag, devDayMon, renderBoxLog, renderTesty, TEST_PL, WAKE_PL,
         renderAnalysis, TZ_LIST, TZ_DEFAULT, tzOffsetFor, tzOffsetTxt, tzLabel,
         tzName, trackingSince, beforeTracking, analyze, inrContext, openTimeOf,
         openMinutes, hm, dev2, renderStatus };
export function __setState(o){
  if (o.cfg)    Object.assign(cfg, o.cfg);
  if (o.doses)  { for (const k of Object.keys(doses)) delete doses[k]; Object.assign(doses, o.doses); }
  if (o.inr)    { for (const k of Object.keys(inr))   delete inr[k];   Object.assign(inr, o.inr); }
  if (o.events) { events.length = 0; events.push(...o.events); }
  if (o.boxLog) { boxLog = o.boxLog; }
}
export { __db, __resetDb };
`;

/* doses / inr / cfg sa deklarowane przez let - zamieniamy na obiekty
   stale, zeby testy mogly nimi manipulowac przez referencje.          */
js = js.replace("let doses = {};", "const doses = {};")
       .replace("let inr   = {};", "const inr   = {};")
       .replace("let events = [];", "const events = [];");

/* uid ustawiamy recznie w testach */
js = js.replace("let uid = null;", "let uid = 'testuid';");

writeFileSync(join(here, "app_module.mjs"), header + js + footer, "utf8");
console.log(`Zbudowano app_module.mjs (${js.split("\n").length} linii prawdziwego kodu)`);
