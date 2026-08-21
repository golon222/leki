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
         goOnline, get, runTransaction,
         __db, __resetDb } from "./firebase_stub.mjs";
import "./dom_stub.mjs";
`;

const footer = `
export { renderSkan, brakujePokrycia, doReconcile, reconcileDecyzja, opisLadowan, minutyDoPelna, opisLadowania, renderAll, toast, tablet3D, cieniuj, TAB3D_SEGMENTOW, busy, seriaDni, doNastepnej, opisCzasu,
         cfg, doses, inr, events, tabletSVG, doseGraphic,
         dayDose, dayStatus, devKey, devHM, devDate, inrState,
         dawkaNaDzien, tydzienDawek, dzienBezLeku, wyjatekNaDzien, opisDawkowania,
         planNaDzien, dosePlans, zapiszPlanDnia, renderPlanList,
         dniZapasu, ZAPAS_HORYZONT, renderNetStan,
         EKRANY, renderEvents, renderKafelki, bezPokrycia, evPasuje, opisNvsFailKey,
         nvsFailLog, renderNvsFailLog,
         EV_STANY, EV_CZASY, EV_LIMIT,
         inrOdstep, inrTerminKey, inrDoTerminu, dniTxt, renderOstrzezenia, ostrzStraty,
         collectRows, inrChart, nf, esc, renderCalendar, renderToday,
         renderInr, renderPills, renderSheet, settlePills, relTime, slotMin,
         yesterdayKey, pillColors, todayKey, dateKey, DAY_START_HOUR, medDate,
         inNightWindow, renderOpenWarn, renderDiag, devDayMon, renderBoxLog, renderTesty, TEST_PL, WAKE_PL,
         renderAnalysis, rytmSVG, dniRytmu, poryWCzasieSVG, iskraSVG, dowSVG,
         skutecznoscTygodniami, komorkaRytmu,
         TZ_LIST, TZ_DEFAULT, tzOffsetFor, tzOffsetTxt, tzLabel,
         tzName, trackingSince, beforeTracking, analyze, inrContext, openTimeOf,
         openMinutes, hm, renderStatus,
         doRamyDoby, zRamyDoby, sredniaPora, kwantyl,
         dniMiedzy, odstepyZPunktow, godzTxt, trwanieTxt, MAX_PRZYPOMNIEN,
         ostatniaDawka, kiedyDawkaTxt, odswiezOdDawki, startTikOdDawki,
         zapiszPewnie, zapiszCfg, oczekWczytaj, oczekZapisz, oczekIle, oczekWyslij,
         magazyn, OCZEK_KEY, oczekOdmowy, bazaOdmowila,
         INR_ZAKRES_MIN, INR_ZAKRES_MAX, INR_ODSTEPY, inrKrokiZakresu, renderSettings,
         ostrzMilczy, MILCZY_PROG_H, stratyDotyczaLeku, stratyCicho,
         ostrzZatkana, ZATKANA_SWIEZOSC_H, nvsMalo, NVS_MALO, lastRec,
         rysuj, rysujWszystkie, rysBledy, ostrzRysowanie, RYS_BLEDY_LIMIT,
         askConfirm,
         renderOta, pobierzOpisFirmware,
         tgTokenPoprawny, renderTgStan, tgKodParowania };
/* Rozmowa z Telegramem idzie przez fetch(), ktorego w testach nie ma.
   Podstawiamy odpowiedzi getMe/getUpdates, zeby dalo sie sprawdzic
   PAROWANIE - w tym przypadek, w ktorym do bota pisze ktos obcy.
   Bez tego jedyna obrona przed wyslaniem powiadomien o leku obcej osobie
   nie mialaby ani jednego testu.                                       */
export function __setTelegram(odp){
  globalThis.fetch = async (url) => {
    const u = String(url);
    if (u.includes("/getMe"))
      return { ok:true, status:200, json: async () => odp.getMe
               ?? ({ ok:true, result:{ username:"pillbox_test_bot" } }) };
    if (u.includes("/getUpdates"))
      return { ok:true, status:200, json: async () => odp.getUpdates
               ?? ({ ok:true, result:[] }) };
    return { ok:false, status:404, json: async () => ({ ok:false, description:"brak" }) };
  };
}
/* Opis wersji firmware czytany jest przez fetch(), ktorego w testach nie
   ma. Podstawiamy go wprost, zeby dalo sie sprawdzic KAZDY stan ekranu
   aktualizacji - takze ten, w ktorym pudelko nie ma hasla.            */
export function __setOpisFirmware(o){
  opisFirmware = o; opisFirmwareBlad = o ? "" : "brak";
  /* Zaslepka sieci. Zlecenie pobiera opis PONOWNIE tuz przed zapisem
     (inaczej wysylaloby sume sprzed godziny), wiec testy musza miec czym
     odpowiedziec - inaczej sprawdzalyby sciezke bledu zamiast tej,
     o ktora chodzi. */
  globalThis.fetch = async (url) => String(url).includes("PillBox.json")
    ? { ok: !!o, status: o ? 200 : 404, json: async () => o }
    : { ok: false, status: 404, json: async () => ({}) };
}
export function __setState(o){
  if (o.cfg)    Object.assign(cfg, o.cfg);
  if (o.doses)  { for (const k of Object.keys(doses)) delete doses[k]; Object.assign(doses, o.doses); }
  if (o.inr)    { for (const k of Object.keys(inr))   delete inr[k];   Object.assign(inr, o.inr); }
  if (o.events) { events.length = 0; events.push(...o.events); }
  if (o.boxLog) { boxLog = o.boxLog; }
  if (o.dosePlans !== undefined) dosePlans = o.dosePlans || {};
  if (o.nvsFailLog) { nvsFailLog = o.nvsFailLog; }
}
/* Kalendarz domysla sie biezacego miesiaca, gdy viewYear/viewMonth nie sa
   jeszcze ustawione (patrz komentarz w renderCalendar()). Testy sprawdzajace
   konkretny dzien musza umiec wymusic miesiac inny niz "teraz" - inaczej
   test dla terminu INR liczonego "dzis + odstep dni" psulby sie caly czas,
   gdy "dzis" wypada blisko konca miesiaca i termin przeskakuje w przod.  */
/* Lista sieci widzianych przez pudelko przychodzi wlasna galezia bazy,
   a nie w statusie - podstawiamy ja wprost, tak jak opis firmware.   */
export function __setSkan(o){ skanSieci = o; renderSkan(); }
/* Osłona rysowania zbiera bledy przez cale zycie aplikacji - testy musza
   umiec zaczac od zera, inaczej kazdy nastepny sprawdza smieci po poprzednim. */
export function __resetRys(){ rysBledy.length = 0; rysBledyRazem = 0; }
export function __rysBledyRazem(){ return rysBledyRazem; }
export function __setView(rok, miesiac){ viewYear = rok; viewMonth = miesiac; }
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
