/* =====================================================================
 *  TESTY ODPORNOSCI APLIKACJI
 *
 *  Nie sprawdzaja, czy dziala, gdy wszystko jest w porzadku - od tego sa
 *  testy w test_app.mjs. Tutaj celowo podajemy dane uszkodzone, niepelne,
 *  w zlej kolejnosci i w absurdalnych ilosciach. Aplikacja ma sie nie
 *  wywalic ANI nie sklamac - a klamstwo jest tu grozniejsze, bo raport
 *  z niej trafia do lekarza.
 *      node test_stress.mjs
 * ===================================================================== */
import * as A from "./app_module.mjs";

let PASS = 0, FAIL = 0;
const check = (c, m) => c ? PASS++ : (FAIL++, console.log("  FAIL  " + m));
const head  = t => console.log("\n=== " + t + " ===");

const bazowyCfg = { schedule:["20:00"], tz:"Europe/Warsaw", tzOffsetMin:120,
  trackingSince:"2000-01-01", defaultDose:1, drugName:"Warfin", drugStrength:5,
  inrMin:2.0, inrMax:3.0, pillsLeft:undefined, pillsCountedUntil:undefined,
  pillsBase:undefined, pillsBaseFrom:undefined };
const D = (o={}) => A.__setState({ cfg:{...bazowyCfg}, doses:{}, inr:{}, events:[], ...o });

/* Uruchamia funkcje i melduje, czy przezyla. */
function przezyje(nazwa, fn) {
  try { fn(); check(true, nazwa); }
  catch (e) { check(false, `${nazwa} -> WYJATEK: ${e?.message || e}`); }
}
async function przezyjeAsync(nazwa, fn) {
  try { await fn(); check(true, nazwa); }
  catch (e) { check(false, `${nazwa} -> WYJATEK: ${e?.message || e}`); }
}

const dzien = n => { const d = A.medDate(new Date()); d.setDate(d.getDate() - n);
                     return A.dateKey(d); };

/* ═══════════ 1. SMIECI Z BAZY ═══════════ */
head("Uszkodzone dane z bazy nie moga wywalic aplikacji");

const paskudztwa = [
  ["null zamiast dawki",        { doses:{ [dzien(1)]: null } }],
  ["dawka bez pol",             { doses:{ [dzien(1)]: { 0:{} } } }],
  ["status jako liczba",        { doses:{ [dzien(1)]: { 0:{ status:5, dose:1 } } } }],
  ["dawka jako tekst",          { doses:{ [dzien(1)]: { 0:{ status:"taken", dose:"duzo" } } } }],
  ["dawka ujemna",              { doses:{ [dzien(1)]: { 0:{ status:"taken", dose:-3 } } } }],
  ["dawka gigantyczna",         { doses:{ [dzien(1)]: { 0:{ status:"taken", dose:1e9 } } } }],
  ["klucz dnia to bzdura",      { doses:{ "nie-data": { 0:{ status:"taken", dose:1 } } } }],
  ["zdarzenie bez typu",        { events:[ { ts:1785600000 } ] }],
  ["zdarzenie z ts jako tekst", { events:[ { ts:"wczoraj", type:"open" } ] }],
  ["zdarzenie z ts ujemnym",    { events:[ { ts:-5, type:"open", slot:0 } ] }],
  ["slot poza zakresem",        { events:[ { ts:1785600000, type:"open", slot:999 } ] }],
  ["INR jako tekst",            { inr:{ [dzien(1)]: { value:"wysokie" } } }],
  ["INR null",                  { inr:{ [dzien(1)]: null } }],
  ["harmonogram pusty",         { cfg:{ ...bazowyCfg, schedule:[] } }],
  ["harmonogram to obiekt",     { cfg:{ ...bazowyCfg, schedule:{ 0:"20:00" } } }],
  ["godzina bez dwukropka",     { cfg:{ ...bazowyCfg, schedule:["2000"] } }],
  ["dawka domyslna zero",       { cfg:{ ...bazowyCfg, defaultDose:0 } }],
  ["dawka domyslna null",       { cfg:{ ...bazowyCfg, defaultDose:null } }],
  ["strefa czasowa jako tekst", { cfg:{ ...bazowyCfg, tzOffsetMin:"dwie godziny" } }],
  ["zakres INR odwrocony",      { cfg:{ ...bazowyCfg, inrMin:4, inrMax:2 } }]
];

for (const [nazwa, stan] of paskudztwa) {
  D(stan);
  przezyje(nazwa, () => {
    A.renderAll();
    A.renderCalendar();
    A.renderToday();
    A.renderInr();
    A.renderPills();
    A.analyze(30);
    A.collectRows(14);
    A.renderAnalysis();
    A.renderDiag();
    A.renderBoxLog();
    A.renderStatus({ battery: 80, volt: 4.0, lastSeen: 1785600000 });
  });
}

head("Uszkodzony stan pudelka");
for (const [nazwa, st] of [
  ["status null",            null],
  ["status pusty",           {}],
  ["bateria jako tekst",     { battery:"pelna", volt:"cztery" }],
  ["bateria ponad 100",      { battery:250, volt:9 }],
  ["bateria ujemna",         { battery:-40, volt:-1 }],
  ["lastSeen w przyszlosci", { battery:50, volt:3.9, lastSeen: 4e9 }],
  ["kolejka jako tekst",     { battery:50, volt:3.9, queued:"duzo" }],
  ["openSince bez boxOpen",  { battery:50, volt:3.9, openSince:1785600000 }]
]) przezyje(nazwa, () => A.renderStatus(st));

head("Uszkodzona historia pudelka");
for (const [nazwa, log] of [
  ["historia null",        null],
  ["historia jako obiekt", { a:"x" }],
  ["wpis pusty",           [""]],
  ["wpis bez separatorow", ["cokolwiek"]],
  ["wpis z nadmiarem pol", ["1|2|3|4|5|6|7|8"]],
  ["czas jako tekst",      ["kiedys|timer|wyslane|80|0"]],
  ["wpis z HTML",          ["1785600000|timer|<img src=x onerror=alert(1)>|80|0"]]
]) przezyje(nazwa, () => { A.__setState({ boxLog: log }); A.renderBoxLog(); });

/* ═══════════ 2. WSTRZYKNIECIE HTML ═══════════ */
head("Tresc od uzytkownika nie moze stac sie kodem");
{
  const zly = `<img src=x onerror="alert(1)">`;
  D({ cfg:{ ...bazowyCfg, drugName: zly },
      doses:{ [dzien(1)]: { 0:{ status:"taken", dose:1, note: zly } } },
      inr:{ [dzien(1)]: { value:2.5, note: zly } },
      boxLog:[`1785600000|timer|${zly}|80|0`] });
  A.renderAll(); A.renderBoxLog(); A.renderDiag();

  const caly = ["calGrid","todayWhen","hSub","inrList","boxLog","diagList","anaBody"]
    .map(id => document.getElementById(id).innerHTML).join("");
  check(!caly.includes("<img src=x"), "znaczniki HTML sa zamieniane na tekst");
  /* Atrybut zdarzenia jest grozny WYLACZNIE wewnatrz prawdziwego znacznika.
     Sam cudzyslow w tekscie nie znaczy nic - a przegladarka, wstawiajac tresc
     przez textContent, escapuje `&`, `<` i `>`, ale cudzyslowa NIE. Pytanie
     brzmi wiec: czy powstal wykonywalny atrybut w otwartym tagu, a nie: czy
     gdziekolwiek w tekscie wystepuje slowo onerror z cudzyslowem.        */
  /* Szukamy `onerror`, a nie dowolnego `on*`: aplikacja sama generuje
     onclick/onchange w swoich przyciskach i to jest jej wlasny, zaufany kod.
     `onerror` w otwartym znaczniku nie ma prawa powstac inaczej niz
     z wstrzyknietej tresci.                                             */
  check(!/<\s*[a-zA-Z][^>]*\bonerror\s*=/.test(caly),
        "atrybut zdarzenia nie jest wykonywalny (nie powstal otwarty znacznik)");
  check(caly.includes("&lt;img"), "znacznik zostal zamieniony na encje");
  check(A.esc(zly).includes("&lt;img"), "funkcja esc faktycznie ucieka znaki");
  check(A.esc(`"cudzyslow"`).includes("&quot;"), "cudzyslowy tez");
}

/* ═══════════ 2b. PRZYPOMNIENIA TO NIE DAWKI ═══════════
   cfg.schedule to godziny PRZYPOMNIEN. Kuba bierze tabletke kiedy chce;
   pudelko ma tylko przypomniec, jesli jeszcze jej nie wzial. Dawka jest
   jedna dziennie. Aplikacja liczyla to jednak jako defaultDose x liczba
   przypomnien, wiec dodanie drugiego przypomnienia zamienialo kazdy
   poprawny dzien w "czesciowy" i o polowe skracalo prognoze zapasu.  */
head("Drugie przypomnienie nie tworzy drugiej dawki");
{
  for (const plan of [["20:00"], ["20:00","23:00"], ["08:00","14:00","20:00"]]) {
    D({ cfg: { ...bazowyCfg, schedule: plan, defaultDose: 1 },
        doses: { [dzien(1)]: { 0: { status: "taken", dose: 1 } } } });
    check(A.dayStatus(dzien(1)) === "taken",
          `${plan.length} przypomnien: dzien z jedna wzieta dawka jest ZALICZONY ` +
          `(jest "${A.dayStatus(dzien(1))}")`);
  }
}

head("Prognoza zapasu nie zalezy od liczby przypomnien");
{
  const zapas = plan => {
    D({ cfg: { ...bazowyCfg, schedule: plan, defaultDose: 1,
               pillsLeft: 30, pillsBase: 30, pillsBaseFrom: dzien(0) }, doses: {} });
    A.renderPills();
    return document.getElementById("pillsSub").textContent;
  };
  const jedno = zapas(["20:00"]);
  check(jedno === zapas(["20:00","23:00"]),
        `dwa przypomnienia daja ten sam zapas co jedno ("${jedno}" vs "${zapas(["20:00","23:00"])}")`);
  check(jedno === zapas(["08:00","14:00","20:00"]), "trzy tak samo");
}

/* ═══════════ 3. STO DNI PRACY ═══════════ */
head("Sto dni pracy pod rzad");
{
  const doses = {}, events = [];
  let wziete = 0, pominiete = 0;
  for (let i = 100; i >= 1; i--) {
    const k = dzien(i);
    const ts = Math.floor(new Date(k + "T20:00:00Z").getTime() / 1000) - 120 * 60;
    if (i % 7 === 0) { doses[k] = { 0:{ status:"missed", dose:0, source:"device", ts } };
                       events.push({ ts, type:"missed", slot:0 }); pominiete++; }
    else             { doses[k] = { 0:{ status:"taken", dose:1, source:"device", ts, openTs:ts } };
                       events.push({ ts, type:"open", slot:0 }); wziete++; }
  }
  D({ doses, events });

  /* Okno analizy to DZIS i N-1 dni wstecz, a dzis nie ma jeszcze wpisu.
     Oczekiwania liczymy wiec z danych, ktore faktycznie w tym oknie leza -
     zamiast zakladac liczbe z gory i potem sie dziwic.                  */
  const wOknie = Object.keys(doses).filter(k => k > dzien(100));
  const oczTaken  = wOknie.filter(k => doses[k][0].status === "taken").length;
  const oczMissed = wOknie.filter(k => doses[k][0].status === "missed").length;

  const a = A.analyze(100);
  check(a.total === wOknie.length, `policzone wszystkie dni z okna (${a.total} z ${wOknie.length})`);
  check(a.taken === oczTaken,  `wziete zgadza sie (${a.taken} z ${oczTaken})`);
  check(a.missed === oczMissed, `pominiete zgadza sie (${a.missed} z ${oczMissed})`);
  check(Math.abs(a.doseSum - oczTaken) < 0.001, "suma tabletek zgadza sie z liczba dni");
  check(a.taken + a.missed === a.total, "kazdy policzony dzien jest albo wziety, albo nie");
  check(wziete + pominiete === 100, `dane testowe: ${wziete} + ${pominiete}`);

  const r = A.collectRows(100);
  check(r.length === 100, `raport ma 100 wierszy (${r.length})`);
  check(r.filter(x => x.status === "taken").length === oczTaken, "raport zgodny z analiza");
  check(r[r.length-1].key === A.todayKey(), "ostatni wiersz to dzisiaj");

  /* Wykres godzin: wszystkie o 20:00, wiec odchylenie ma byc zerowe. */
  A.__setState({ cfg:{ ...bazowyCfg, schedule:["20:00"] } });
  const a2 = A.analyze(100);
  check(a2.hist.reduce((s,x)=>s+x,0) === a2.taken, "histogram godzin liczy tyle samo dawek co analiza");
  check(a2.hist[20] === a2.taken, `wszystkie dawki w godzinie 20 (${a2.hist[20]} z ${a2.taken})`);
  check(a2.hist.filter((x,i)=> i !== 20 && x > 0).length === 0, "i w zadnej innej godzinie");
}

/* ═══════════ 4. LICZNIK TABLETEK POD OBCIAZENIEM ═══════════ */
head("Licznik tabletek przez sto rozliczen");
{
  const doses = {};
  for (let i = 100; i >= 1; i--)
    doses[dzien(i)] = { 0:{ status:"taken", dose:1, source:"device" } };

  A.__resetDb();
  D({ cfg:{ ...bazowyCfg, pillsLeft:200, pillsCountedUntil: dzien(101) }, doses });

  await A.settlePills();
  const poPierwszym = A.cfg.pillsLeft;
  check(poPierwszym === 100, `200 - 100 dni po 1 tabletce = ${poPierwszym}`);

  /* Powtorne przeliczenia nie moga odjac ani jednej tabletki wiecej. */
  for (let i = 0; i < 25; i++) await A.settlePills();
  check(A.cfg.pillsLeft === poPierwszym,
        `25 powtorzen nie zmienilo stanu (${A.cfg.pillsLeft})`);

  /* Nie schodzimy ponizej zera nawet przy absurdalnym zapasie. */
  A.__resetDb();
  D({ cfg:{ ...bazowyCfg, pillsLeft:3, pillsCountedUntil: dzien(101) }, doses });
  await A.settlePills();
  check(A.cfg.pillsLeft >= 0, `licznik nie zszedl ponizej zera (${A.cfg.pillsLeft})`);

  /* Polowki nie moga generowac bledu zaokraglen. */
  const pol = {};
  for (let i = 60; i >= 1; i--)
    pol[dzien(i)] = { 0:{ status:"taken", dose:0.5, source:"device" } };
  A.__resetDb();
  D({ cfg:{ ...bazowyCfg, pillsLeft:100, pillsCountedUntil: dzien(61) }, doses: pol });
  await A.settlePills();
  check(A.cfg.pillsLeft === 70, `60 polowek = 30 tabletek, zostalo ${A.cfg.pillsLeft}`);
  check(Number.isFinite(A.cfg.pillsLeft), "wynik jest liczba, nie NaN");
}

/* ═══════════ 5. ZDARZENIA W ZLEJ KOLEJNOSCI I DUPLIKATY ═══════════ */
head("Duplikaty i zla kolejnosc zdarzen");
{
  const k = dzien(3);
  const ts = Math.floor(new Date(k + "T20:00:00Z").getTime() / 1000) - 120 * 60;

  /* To samo otwarcie przyslane dwadziescia razy, w losowej kolejnosci. */
  const ev = [];
  for (let i = 0; i < 20; i++) ev.push({ ts: ts + (i % 3), type:"open", slot:0 });
  ev.sort(() => Math.random() - 0.5);

  A.__resetDb();
  D({ events: ev });
  await globalThis.reconcile();
  /* Zapis idzie transakcja na sciezke (D35), wiec `path` jest wlasnym
     polem wpisu - nie kluczem wewnatrz `val`, jak przy dawnym zbiorczym
     update().                                                          */
  const sciezki = new Set(A.__db.writes.map(x => x.path));
  check(sciezki.size === 1, `dwadziescia duplikatow -> jeden wpis (${sciezki.size})`);

  /* "missed" przyslane PO "open" nie moze skasowac wzietej dawki. */
  A.__resetDb();
  D({ events:[ { ts, type:"missed", slot:0 }, { ts: ts + 5, type:"open", slot:0 } ] });
  await globalThis.reconcile();
  const wpis = A.__db.data.users?.testuid?.doses?.[k]?.["0"];
  check(wpis?.status === "taken", `otwarcie wygrywa z pominieciem (${wpis?.status})`);

  /* Odwrotna kolejnosc w tablicy - wynik ma byc identyczny. */
  A.__resetDb();
  D({ events:[ { ts: ts + 5, type:"open", slot:0 }, { ts, type:"missed", slot:0 } ] });
  await globalThis.reconcile();
  const wpis2 = A.__db.data.users?.testuid?.doses?.[k]?.["0"];
  check(wpis2?.status === "taken", "kolejnosc w tablicy nie zmienia wyniku");
}

/* ═══════════ 6. CZTERYSTA ZDARZEN NARAZ ═══════════ */
head("Pelna paczka zdarzen z pudelka");
{
  const ev = [];
  for (let i = 0; i < 400; i++) {
    const k = dzien(i % 200);
    const ts = Math.floor(new Date(k + "T20:00:00Z").getTime() / 1000) - 120 * 60;
    ev.push({ ts, type: i % 5 === 0 ? "missed" : "open", slot:0 });
  }
  A.__resetDb();
  D({ events: ev });
  await przezyjeAsync("400 zdarzen przetworzonych bez wyjatku",
                      () => globalThis.reconcile());
  const dni = new Set(A.__db.writes.flatMap(x => Object.keys(x.val || {})));
  check(dni.size <= 200, `nie wiecej wpisow niz roznych dni (${dni.size})`);
  check(dni.size > 0, "cos jednak zapisano");
}

/* ═══════════ 7. RECZNA KOREKTA JEST SWIETA ═══════════ */
head("Reczna korekta nie do ruszenia");
{
  const k = dzien(2);
  const ts = Math.floor(new Date(k + "T20:00:00Z").getTime() / 1000) - 120 * 60;
  for (const typ of ["open", "missed"]) {
    A.__resetDb();
    D({ events: Array.from({length:50}, (_,i) => ({ ts: ts+i, type: typ, slot:0 })),
        doses:{ [k]: { 0:{ status:"taken", dose:0.5, source:"manual" } } } });
    await globalThis.reconcile();
    check(A.__db.writes.length === 0,
          `piecdziesiat zdarzen '${typ}' nie ruszylo recznego wpisu`);
  }
}

/* ═══════════ 8. INR W SKRAJNOSCIACH ═══════════ */
head("INR - wartosci skrajne i ocena");
{
  D({ cfg:{ ...bazowyCfg, inrMin:2, inrMax:3 } });
  /* inrState zwraca opis do wyswietlenia, nie sam kod - stad porownanie tekstu. */
  check(A.inrState(1.0).txt.includes("poniżej"), "1,0 ponizej zakresu");
  check(A.inrState(2.5).txt.includes("w zakresie"), "2,5 w zakresie");
  check(A.inrState(9.9).txt.includes("powyżej"), "9,9 powyzej zakresu");
  check(A.inrState(2).txt.includes("w zakresie"), "dokladnie dolna granica to jeszcze zakres");
  check(A.inrState(3).txt.includes("w zakresie"), "dokladnie gorna granica tez");
  check(A.inrState(1.0).col === "var(--warn)", "za nisko na pomaranczowo");
  check(A.inrState(9.9).col === "var(--bad)",  "za wysoko na czerwono");

  przezyje("wykres z jednym punktem", () => {
    D({ inr:{ [dzien(1)]: { value:2.5 } } }); A.renderInr();
  });
  przezyje("wykres z setka punktow", () => {
    const inr = {};
    for (let i = 100; i >= 1; i--) inr[dzien(i)] = { value: 1 + (i % 40) / 10 };
    D({ inr }); A.renderInr(); A.renderAnalysis();
  });
  przezyje("wykres gdy wszystkie wyniki identyczne", () => {
    const inr = {};
    for (let i = 10; i >= 1; i--) inr[dzien(i)] = { value: 2.5 };
    D({ inr }); A.renderInr();
  });
}

/* ═══════════ 9. PUSTKA ═══════════ */
head("Zupelnie pusta baza");
{
  D({});
  przezyje("wszystkie ekrany na pustych danych", () => {
    A.renderAll(); A.renderAnalysis(); A.renderDiag(); A.renderBoxLog();
    A.renderStatus(null);
  });
  /* Przy pudelku dzialajacym od dawna puste dni w przeszlosci to POMINIETE
     dawki - i tak ma byc. Zeby sprawdzic prawdziwa pustke, przesuwamy date
     uruchomienia na dzis: wtedy nie ma czego liczyc.                    */
  const a = A.analyze(30);
  check(a.total > 0, `dni sprzed dzis licza sie jako pominiete (${a.total})`);
  check(a.taken === 0, "ale zadna nie jest zmyslona jako wzieta");

  D({ cfg:{ ...bazowyCfg, trackingSince: A.todayKey() } });
  const a0 = A.analyze(30);
  check(a0.total === 0, `przed uruchomieniem pudelka zero dni (${a0.total})`);
  check(!Number.isNaN(a0.doseSum), "suma tabletek nie jest NaN-em");
  const r = A.collectRows(7);
  check(r.length === 7, "raport ma zadana liczbe wierszy mimo braku danych");
}

console.log("\n──────────────────────────────────────");
console.log(`  ZALICZONE: ${PASS}    BLEDY: ${FAIL}`);
console.log("──────────────────────────────────────");
process.exit(FAIL ? 1 : 0);
