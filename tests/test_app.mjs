/* =====================================================================
 *  Testy aplikacji. Uruchamiaja PRAWDZIWY kod z index.html
 *  na atrapach Firebase i DOM.
 *      node build_app_module.mjs && node test_app.mjs
 * ===================================================================== */
import * as A from "./app_module.mjs";
import { readFileSync } from "node:fs";

let PASS = 0, FAIL = 0;
const check = (cond, msg) => cond ? PASS++ : (FAIL++, console.log("  FAIL  " + msg));
const head  = t => console.log("\n=== " + t + " ===");

/* trackingSince ustawiamy daleko w przeszlosci, zeby testy sprzed tej funkcji
   dalej sprawdzaly to, co mialy sprawdzac. Granice testujemy osobno. */
/* cfg z testu DOKLADA sie do domyslnego, a nie zastepuje go w calosci -
   inaczej pola pominiete w tescie zostawaly z testu poprzedniego i wynik
   zalezal od kolejnosci uruchomienia.                                    */
const DEF_CFG = { schedule:["08:00"], tz:"Europe/Warsaw",
  tzOffsetMin:120, trackingSince:"2000-01-01", defaultDose:1,
  drugName:"Warfin", drugStrength:5, inrMin:2.0, inrMax:3.0, pillsLeft:undefined,
  pillsCountedUntil:undefined, pillsBase:undefined, pillsBaseFrom:undefined };
const D = (o={}) => A.__setState({ doses:{}, inr:{}, events:[], ...o,
                                   cfg:{ ...DEF_CFG, ...(o.cfg||{}) } });

const today = A.todayKey();
/* Przesuniecie liczone od DOBY LEKOWEJ, nie od zegara - inaczej testy
   uruchomione miedzy polnoca a granica doby przesuwalyby sie o jeden dzien. */
const shift = n => { const d = A.medDate(new Date()); d.setDate(d.getDate()+n); return A.dateKey(d); };

/* ═══════════ 1. TABLETKA ═══════════ */
head("Rysowanie tabletki");
D();
const full = A.tabletSVG(1, 60, 0), half = A.tabletSVG(0.5, 60, 0), none = A.tabletSVG(0, 60, 0);
check(full.includes("#f0a6c0"), "Warfin 5 mg rysowany na rozowo");
check(half.includes('width="30"'), "polowka wypelniona w 50% szerokosci");
check(full.includes('width="60"'), "cala tabletka wypelniona w 100%");
check(none.includes('width="0"'), "zero = pusta tabletka");
check(none.includes("stroke-dasharray=\"3 3\""), "pusta ma przerywany obrys");
check(full.includes("stroke-dasharray=\"0\""), "pelna ma ciagly obrys");
A.__setState({ cfg:{ drugStrength:3 } });
check(A.tabletSVG(1,60,0).includes("#8ab4f8"), "3 mg rysowany na niebiesko");
A.__setState({ cfg:{ drugStrength:5 } });

head("Dawka jako zestaw tabletek");
check((A.doseGraphic(1,50).match(/<svg/g)||[]).length===1, "1 tabl. = jedna grafika");
check((A.doseGraphic(1.5,50).match(/<svg/g)||[]).length===2, "1,5 tabl. = dwie grafiki");
check((A.doseGraphic(2,50).match(/<svg/g)||[]).length===2, "2 tabl. = dwie grafiki");
check(A.doseGraphic(0,50).includes("brak"), "0 tabl. pokazuje 'brak'");
check(A.doseGraphic(7,50).includes("× 7"), "duza dawka skracana do 'x N'");
check(A.doseGraphic(1.5,50).includes('width="25"'), "druga tabletka wypelniona w polowie");

/* ═══════════ 2. STATUS DNIA ═══════════ */
head("Kolorowanie dni w kalendarzu");
D({ doses:{
  [shift(-3)]: { 0:{ status:"taken", dose:1, source:"device" } },
  [shift(-2)]: { 0:{ status:"taken", dose:0.5, source:"manual" } },
  [shift(-1)]: { 0:{ status:"missed", dose:0, source:"device" } },
}});
check(A.dayStatus(shift(-3))==="taken",   "pelna dawka -> zielony");
check(A.dayStatus(shift(-2))==="partial", "pol tabletki -> pomaranczowy");
check(A.dayStatus(shift(-1))==="missed",  "pominieta -> czerwony");
check(A.dayStatus(shift(-9))==="missed",  "przeszlosc bez wpisu -> czerwony");
check(A.dayStatus(shift(+3))==="future",  "przyszlosc -> neutralny");
check(A.dayStatus(today)==="future",      "dzis bez wpisu nie jest czerwony");
check(A.dayDose(shift(-2)).sum===0.5,     "suma dawki dnia = 0,5");
check(A.dayDose(shift(-1)).sum===0,       "pominiety dzien: 0 tabletek");
check(A.dayDose(shift(-9)).any===false,   "dzien bez wpisu: any=false");

head("Dawka standardowa 0,5 tabletki");
A.__setState({ cfg:{ defaultDose:0.5 } });
check(A.dayStatus(shift(-2))==="taken", "przy standardzie 0,5 pol tabletki to pelna dawka");
check(A.dayStatus(shift(-3))==="taken", "1 tabl. przy standardzie 0,5 nadal zielone");
A.__setState({ cfg:{ defaultDose:1 } });

/* ═══════════ 3. STREFA CZASOWA ═══════════ */
head("Przeliczanie czasu urzadzenia");
D();
const t0800 = Math.floor(Date.UTC(2026,6,31,6,0,0)/1000);   /* 08:00 czasu PL */
check(A.devKey(t0800)==="2026-07-31", "08:00 lokalnie -> " + A.devKey(t0800));
check(A.devHM(t0800)==="08:00", "godzina lokalna -> " + A.devHM(t0800));

const t2330 = Math.floor(Date.UTC(2026,6,31,21,30,0)/1000); /* 23:30 PL */
check(A.devKey(t2330)==="2026-07-31", "23:30 lokalnie to wciaz 31 lipca");
const t0030 = Math.floor(Date.UTC(2026,6,31,22,30,0)/1000); /* 00:30 PL 1 sierpnia */
check(A.devKey(t0030)==="2026-07-31",
      "00:30 nalezy jeszcze do doby z 31 lipca -> " + A.devKey(t0030));
check(A.devHM(t0030)==="00:30", "ale godzina otwarcia zostaje prawdziwa: " + A.devHM(t0030));
check(A.slotMin("08:30")===510, "08:30 = 510 min");

/* ═══════════ 4. REKONCYLIACJA ═══════════ */
head("Zdarzenia z pudelka -> wpisy w kalendarzu");
A.__resetDb();
D({ events:[ { ts:t0800, type:"open", slot:0, battery:80 } ] });
await globalThis.reconcile();
let w = A.__db.data.users.testuid.doses["2026-07-31"]["0"];
check(w.status==="taken" && w.dose===1 && w.source==="device", "otwarcie -> wzięte, 1 tabl., zrodlo=pudelko");

A.__resetDb();
D({ events:[ { ts:t0800, type:"missed", slot:0 } ] });
await globalThis.reconcile();
w = A.__db.data.users.testuid.doses["2026-07-31"]["0"];
check(w.status==="missed" && w.dose===0, "zdarzenie 'missed' -> nie wziete, 0 tabletek");

head("Recznej korekty nie wolno nadpisac");
A.__resetDb();
D({ events:[ { ts:t0800, type:"open", slot:0 } ],
    doses:{ "2026-07-31": { 0:{ status:"taken", dose:0.5, source:"manual" } } } });
await globalThis.reconcile();
check(A.__db.writes.length===0, "wpis reczny zostaje nietkniety (zapisow: " + A.__db.writes.length + ")");

A.__resetDb();
D({ events:[ { ts:t0800, type:"missed", slot:0 } ],
    doses:{ "2026-07-31": { 0:{ status:"taken", dose:1, source:"device" } } } });
await globalThis.reconcile();
check(A.__db.writes.length===0, "'missed' nie kasuje juz zapisanego przyjecia");

head("Odpornosc na duplikaty i brak numeru dawki");
A.__resetDb();
D({ events:[ { ts:t0800, type:"open", slot:0 }, { ts:t0800+120, type:"open", slot:0 } ] });
await globalThis.reconcile();
const paths = new Set(A.__db.writes.flatMap(x => Object.keys(x.val||{})));
check(paths.size===1, "dwa otwarcia tego samego dnia -> jeden wpis (" + paths.size + ")");

A.__resetDb();
D({ events:[ { ts:t0800, type:"open", slot:-1 } ] });
await globalThis.reconcile();
check(!!A.__db.data.users?.testuid?.doses?.["2026-07-31"], "brak numeru dawki -> przy jednej dawce dziennie trafia do slotu 0");

A.__resetDb();
D({ events:[ { ts:0, type:"open", slot:0 } ] });
await globalThis.reconcile();
check(A.__db.writes.length===0, "zdarzenie bez czasu jest pomijane");

A.__resetDb();
D({ cfg:{ schedule:["08:00","20:00"] }, events:[ { ts:t0800, type:"open", slot:-1 } ] });
await globalThis.reconcile();
check(!!A.__db.data.users?.testuid?.doses?.["2026-07-31"]?.["0"], "dwie dawki: 08:00 dopasowane po godzinie do slotu 0");

/* ═══════════ 5. LICZNIK TABLETEK ═══════════ */
head("Odejmowanie tabletek z opakowania");
A.__resetDb();
D({ cfg:{ pillsLeft:100, pillsCountedUntil:shift(-4) },
    doses:{ [shift(-3)]:{0:{status:"taken",dose:1}},
            [shift(-2)]:{0:{status:"taken",dose:0.5}},
            [shift(-1)]:{0:{status:"missed",dose:0}},
            [today]:    {0:{status:"taken",dose:1}} } });
await A.settlePills();
check(A.cfg.pillsLeft===98.5, "100 - 1 - 0,5 = " + A.cfg.pillsLeft + " (dzis jeszcze nie odjete)");
check(A.cfg.pillsCountedUntil===shift(-1), "znacznik przesuniety na " + A.cfg.pillsCountedUntil);

const beforeSecond = A.cfg.pillsLeft;
await A.settlePills();
check(A.cfg.pillsLeft===beforeSecond, "powtorne przeliczenie nie odejmuje drugi raz");

A.__resetDb();
D({ cfg:{ pillsLeft:10, pillsCountedUntil:shift(-1) }, doses:{ [today]:{0:{status:"taken",dose:1}} } });
await A.settlePills();
check(A.cfg.pillsLeft===10, "dzisiejsza dawka nie jest jeszcze rozliczana");

A.__resetDb();
D({ cfg:{ pillsLeft:0.5, pillsCountedUntil:shift(-3) },
    doses:{ [shift(-2)]:{0:{status:"taken",dose:1}} } });
await A.settlePills();
check(A.cfg.pillsLeft===0, "licznik nie schodzi ponizej zera (" + A.cfg.pillsLeft + ")");

/* B4: dzien doslany WSTECZ, starszy od juz rozliczonych. Przy liczeniu
   roznicowym przepadal na zawsze i licznik pokazywal wiecej niz w pudelku. */
A.__resetDb();
D({ cfg:{ pillsLeft:100, pillsCountedUntil:shift(-4) },
    doses:{ [shift(-1)]:{0:{status:"taken",dose:1}} } });
await A.settlePills();
check(A.cfg.pillsLeft===99, "recznie dopisany wczorajszy dzien odjety (" + A.cfg.pillsLeft + ")");
check(A.cfg.pillsBase===100 && A.cfg.pillsBaseFrom===shift(-3),
      "migracja: baza 100 od " + A.cfg.pillsBaseFrom);
/* Teraz pudelko wraca z wyjazdu i dosyla dzien STARSZY niz wczoraj. */
A.__setState({ doses:{ [shift(-3)]:{0:{status:"taken",dose:1}},
                       [shift(-1)]:{0:{status:"taken",dose:1}} } });
await A.settlePills();
check(A.cfg.pillsLeft===98, "dzien doslany wstecz tez sie odejmuje (" + A.cfg.pillsLeft + ")");

/* Recznie policzone tabletki = stan TERAZ. Dni zamkniete wczesniej sa juz
   w tej liczbie, wiec pudelko dosylajac je pozniej nie moze odjac drugi raz. */
A.__resetDb();
D();
await globalThis.addPills(50);
check(A.cfg.pillsBaseFrom===today, "reczny wpis ustawia baze na dzis");
A.__setState({ doses:{ [shift(-2)]:{0:{status:"taken",dose:1}},
                       [shift(-1)]:{0:{status:"taken",dose:1}} } });
await A.settlePills();
check(A.cfg.pillsLeft===50, "dawki sprzed reczonego przeliczenia nie odejmuja sie (" + A.cfg.pillsLeft + ")");

/* ═══════════ 6. INR ═══════════ */
head("Ocena wyniku INR");
D({ cfg:{ inrMin:2.0, inrMax:3.0 } });
check(A.inrState(2.4).txt==="w zakresie",     "2,4 w zakresie 2,0-3,0");
check(A.inrState(2.0).txt==="w zakresie",     "2,0 na granicy = w zakresie");
check(A.inrState(3.0).txt==="w zakresie",     "3,0 na granicy = w zakresie");
check(A.inrState(1.7).txt==="poniżej zakresu","1,7 ponizej");
check(A.inrState(3.6).txt==="powyżej zakresu","3,6 powyzej");
A.__setState({ cfg:{ inrMin:2.5, inrMax:3.5 } });
check(A.inrState(2.4).txt==="poniżej zakresu","po zmianie zakresu 2,4 jest juz za nisko");
A.__setState({ cfg:{ inrMin:2.0, inrMax:3.0 } });

head("Wykres INR");
D({ inr:{ "2026-07-01":{value:2.1,ts:1}, "2026-07-15":{value:3.4,ts:2}, "2026-07-29":{value:2.4,ts:3} } });
const chart = A.inrChart(Object.keys(A.inr).sort());
check(chart.includes("<polyline"), "linia przebiegu narysowana");
check((chart.match(/<circle/g)||[]).length===3, "trzy punkty pomiarowe");
check(chart.includes("rgba(52,211,153,.14)"), "pasmo terapeutyczne obecne");
check(A.inrChart([]).includes("po pierwszym pomiarze"), "brak danych -> komunikat zamiast pustego wykresu");
check(A.inrChart(["2026-07-01"]).includes("<circle"), "pojedynczy pomiar tez sie rysuje");

/* ═══════════ 7. KALENDARZ + RENDER ═══════════ */
head("Renderowanie bez wyjatkow");
/* Kalendarz pokazuje BIEZACY miesiac, wiec do testu siatki uzywamy dni,
   ktore na pewno w nim wypadaja (1-28), a nie przesuniec wzgledem dzis. */
const inMonth = d => { const n = new Date();
  return A.dateKey(new Date(n.getFullYear(), n.getMonth(), d)); };

D({ doses:{ [inMonth(3)]:{0:{status:"taken",dose:1,source:"device",ts:t0800}} },
    inr:{ [inMonth(5)]:{value:2.4,ts:t0800} } });
let threw = null;
try { A.renderCalendar(); A.renderToday(); A.renderInr(); A.renderPills(); }
catch(e){ threw = e; }
check(!threw, "wszystkie ekrany renderuja sie bez bledu: " + (threw?.message||"ok"));
const grid = document.getElementById("calGrid").innerHTML;
check(grid.includes('class="day'), "siatka kalendarza wygenerowana");
check((grid.match(/data-k="/g)||[]).length >= 28, "miesiac ma komplet dni ("
      + (grid.match(/data-k="/g)||[]).length + ")");
check(grid.includes('class="inr"'), "dzien z pomiarem INR ma znacznik");
check(grid.includes(`data-k="${inMonth(15)}"`), "15. dzien miesiaca obecny w siatce");
check(grid.includes(`data-k="${today}"`), "dzisiejszy dzien obecny w siatce");
check(grid.includes("td\"") || grid.includes("today"), "dzis wyrozniony obwodka");
const adh = document.getElementById("adherence").textContent;
check(adh === "–" || adh.endsWith("%"), "skutecznosc: " + adh);

head("Puste dane nie wywalaja aplikacji");
D();
threw = null;
try { A.renderCalendar(); A.renderToday(); A.renderInr(); A.renderPills(); } catch(e){ threw = e; }
check(!threw, "brak jakichkolwiek danych: " + (threw?.message||"ok"));
check(document.getElementById("pillsBig").textContent==="–", "licznik pokazuje kreske zamiast NaN");

/* ═══════════ 8. RAPORT ═══════════ */
head("Raport dla lekarza");
D({ doses:{ [shift(-2)]:{0:{status:"taken",dose:1,source:"device",ts:t0800}},
            [shift(-1)]:{0:{status:"missed",dose:0,source:"device",ts:t0800}} },
    inr:{ [shift(-1)]:{value:2.4,ts:t0800} } });
const rows = A.collectRows(5);
check(rows.length===5, "5 dni = " + rows.length + " wierszy");
check(rows[rows.length-1].key===today, "ostatni wiersz to dzisiaj");
check(rows.find(r=>r.key===shift(-2)).status==="taken", "dzien przyjecia oznaczony");
check(rows.find(r=>r.key===shift(-1)).status==="missed", "dzien pominiety oznaczony");
check(rows.find(r=>r.key===shift(-1)).inr==="2,4", "INR trafil do raportu");
check(rows.find(r=>r.key===today).status==="", "dzis nie jest jeszcze oceniany");
check(rows.find(r=>r.key===shift(-4)).status==="missed", "starszy dzien bez wpisu = pominiety");
check(rows.find(r=>r.key===shift(-2)).src==="pudełko", "zrodlo wpisu w raporcie");

head("Formatowanie liczb i bezpieczenstwo HTML");
check(A.nf(0.5)==="0,5", "przecinek dziesietny: " + A.nf(0.5));
check(A.nf(1)==="1", "liczba calkowita bez przecinka");
check(A.nf(98.5)==="98,5", "98,5");
check(A.esc('<img src=x onerror="a">')==="&lt;img src=x onerror=&quot;a&quot;&gt;", "znaczniki HTML sa neutralizowane");
check(A.relTime(30)==="przed chwilą" && A.relTime(7200)==="2 godz. temu" && A.relTime(86400*2)==="2 dni temu",
      "opisy czasu: " + A.relTime(30) + " / " + A.relTime(7200) + " / " + A.relTime(86400*2));

/* ═══════════ 9. GODZINA PRZYJECIA ═══════════ */
head("Zapis rzeczywistej godziny otwarcia");
A.__resetDb();
D({ events:[ { ts:t0800, type:"open", slot:0 } ] });
await globalThis.reconcile();
let rec = A.__db.data.users.testuid.doses["2026-07-31"]["0"];
check(rec.openTs===t0800, "godzina otwarcia zapisana osobno jako openTs");
check(A.openTimeOf(rec)===t0800, "openTimeOf odczytuje godzine z pudelka");
check(A.openTimeOf({ source:"manual", ts:12345 })===null,
      "wpis czysto reczny nie udaje zmierzonej godziny");
check(A.openTimeOf({ source:"device", ts:t0800 })===t0800,
      "starszy wpis bez openTs nadal dziala (zgodnosc wstecz)");
check(A.openTimeOf(null)===null, "brak wpisu nie wywala funkcji");
check(A.openMinutes({ openTs:t0800 })===480, "08:00 = 480 minut");

head("Formatowanie godzin i odchylen");
check(A.hm(480)==="08:00", "480 min -> " + A.hm(480));
check(A.hm(495)==="08:15", "495 min -> " + A.hm(495));
check(A.hm(null)==="—", "brak danych -> kreska");
check(A.dev2(25)==="+25 min", "+25 min -> " + A.dev2(25));
check(A.dev2(-90)==="−1 h 30 min", "-90 min -> " + A.dev2(-90));

/* ═══════════ 10. ANALIZA ═══════════ */
head("Statystyki por przyjmowania");
const at = (dayOffset, hour, min) => {
  const d = new Date(); d.setDate(d.getDate()-dayOffset);
  return Math.floor(Date.UTC(d.getFullYear(), d.getMonth(), d.getDate(), hour-2, min)/1000);
};
const dose = (o,h,m) => ({ 0:{ status:"taken", dose:1, source:"device",
                              ts:at(o,h,m), openTs:at(o,h,m) } });
D({ cfg:{ schedule:["08:00"], defaultDose:1 }, doses:{
  [shift(-1)]: dose(1, 8, 10),
  [shift(-2)]: dose(2, 8, 0),
  [shift(-3)]: dose(3, 7, 50),
  [shift(-4)]: dose(4,10, 30),
  [shift(-5)]: { 0:{ status:"missed", dose:0, source:"device", ts:at(5,8,0) } },
}});
let an = A.analyze(6);
check(an.n===4, "cztery dni z zarejestrowana godzina (" + an.n + ")");
check(an.taken===4 && an.missed===1, "4 wziete, 1 pominieta (" + an.taken + "/" + an.missed + ")");
check(an.median===485, "mediana 08:05 = " + A.hm(an.median));
check(an.earliest===470 && an.latest===630, "zakres " + A.hm(an.earliest) + "–" + A.hm(an.latest));
check(an.plan===480, "godzina planowana 08:00");
check(an.within30===3, "trzy dawki w ±30 min od planu (" + an.within30 + ")");
check(an.late60===1, "jedna dawka ponad godzine pozniej (" + an.late60 + ")");
check(an.early60===0, "zadna nie byla ponad godzine wczesniej");
check(Math.round(an.meanDev)===38, "srednie odchylenie +38 min (" + Math.round(an.meanDev) + ")");
check(an.hist[8]===2 && an.hist[7]===1 && an.hist[10]===1,
      "histogram: 7h=" + an.hist[7] + " 8h=" + an.hist[8] + " 10h=" + an.hist[10]);
check(an.sd > 0, "rozrzut policzony: ±" + Math.round(an.sd) + " min");

head("Reczne korekty nie zaklamuja godzin");
D({ cfg:{ schedule:["08:00"] }, doses:{
  [shift(-1)]: { 0:{ status:"taken", dose:1, source:"manual",
                     ts:Math.floor(new Date(shift(-1)+"T12:00:00").getTime()/1000) } } }});
an = A.analyze(3);
check(an.taken===1, "reczny wpis liczy sie do regularnosci");
check(an.n===0, "ale NIE wchodzi do statystyki godzin (" + an.n + ")");
check(an.median===null, "brak zmyslonej mediany");

head("Podzial na dni tygodnia");
D({ cfg:{ schedule:["08:00"] }, doses:{
  [shift(-1)]: dose(1,8,0), [shift(-2)]: dose(2,8,0),
  [shift(-7)]: { 0:{ status:"missed", dose:0, source:"device", ts:at(7,8,0) } } }});
an = A.analyze(8);
const dowSum = an.byDow.reduce((a,d)=>a+d.total,0);
check(dowSum===7, "kazdy zamkniety dzien trafil do dnia tygodnia (" + dowSum + ")");
const d1 = (new Date(shift(-1)+"T12:00:00").getDay()+6)%7;
check(an.byDow[d1].taken===1, "wczorajszy dzien policzony we wlasciwym dniu tygodnia");

head("Analiza pustych danych");
D();
an = A.analyze(30);
check(an.n===0 && an.median===null && an.sd===null, "brak danych nie wywala statystyk");
check(an.hist.every(v=>v===0), "histogram wyzerowany");
check(!isNaN(an.taken) && !isNaN(an.missed), "liczniki nie sa NaN");

/* ═══════════ ODSTEP MIEDZY POMIARAMI INR ═══════════
   Aplikacja NIE decyduje, jak czesto mierzyc - odstep podaje uzytkownik.
   Tu sprawdzamy sama arytmetyke dat i to, ze nowy pomiar przesuwa termin. */
head("Odstep miedzy pomiarami INR");

D({ cfg:{}, inr:{} });
check(A.inrOdstep() === 21, "domyslnie 3 tygodnie (" + A.inrOdstep() + ")");
check(A.inrTerminKey() === null, "bez pomiarow nie ma terminu");
check(A.inrDoTerminu() === null, "i nie ma czego odliczac");

D({ cfg:{ inrEveryDays:0 }, inr:{ [shift(-5)]:{ value:2.4, ts:at(5,12,0) } } });
check(A.inrOdstep() === 0, "zero wylacza przypominanie");
check(A.inrTerminKey() === null, "wylaczone = brak terminu mimo pomiaru");

/* Pomiar 10 dni temu, odstep 21 -> termin za 11 dni. */
D({ cfg:{ inrEveryDays:21 }, inr:{ [shift(-10)]:{ value:2.4, ts:at(10,12,0) } } });
check(A.inrDoTerminu() === 11, "10 dni po pomiarze zostaje 11 dni (" + A.inrDoTerminu() + ")");

/* Dokladnie w dniu terminu: zero, nie liczba ujemna. */
D({ cfg:{ inrEveryDays:21 }, inr:{ [shift(-21)]:{ value:2.4, ts:at(21,12,0) } } });
check(A.inrDoTerminu() === 0, "w dniu terminu dokladnie 0 (" + A.inrDoTerminu() + ")");

/* Po terminie liczba UJEMNA - na tym opiera sie ostrzezenie w kafelku. */
D({ cfg:{ inrEveryDays:21 }, inr:{ [shift(-30)]:{ value:2.4, ts:at(30,12,0) } } });
check(A.inrDoTerminu() === -9, "9 dni po terminie to -9 (" + A.inrDoTerminu() + ")");

/* NAJWAZNIEJSZE: nowy pomiar przesuwa termin sam z siebie. Gdyby liczyl sie
   od pierwszego zamiast od ostatniego, termin zostawalby w przeszlosci na
   zawsze i ostrzezenie nigdy by nie zgaslo.                              */
D({ cfg:{ inrEveryDays:21 }, inr:{
  [shift(-30)]:{ value:2.4, ts:at(30,12,0) },
  [shift(-2)] :{ value:2.6, ts:at(2,12,0)  } } });
check(A.inrDoTerminu() === 19,
      "liczy od OSTATNIEGO pomiaru, nie pierwszego (" + A.inrDoTerminu() + ")");

/* Wpis bez liczbowego wyniku nie moze udawac pomiaru i przesuwac terminu. */
D({ cfg:{ inrEveryDays:21 }, inr:{
  [shift(-30)]:{ value:2.4, ts:at(30,12,0) },
  [shift(-1)] :{ ts:at(1,12,0) } } });
check(A.inrDoTerminu() === -9,
      "wpis bez pola value nie przesuwa terminu (" + A.inrDoTerminu() + ")");

check(A.dniTxt(1) === "1 dzień", "odmiana: 1 dzien");
check(A.dniTxt(5) === "5 dni",   "odmiana: 5 dni");

/* TU BYL BLAD: pierwsza wersja oznaczala termin sama przerywana ramka wokol
   kratki. Technicznie dzialalo, praktycznie Kuba tego NIE ZAUWAZYL - szukal
   ikonki, bo tak oznaczone sa dni z pomiarem. Test pilnuje teraz IKONKI,
   a nie samej klasy CSS, bo to ikonka jest tym, co widac.

   TU BYL DRUGI BLAD, tym razem w samym tescie: termin = dzis + 21 dni.
   Gdy "dzis" wypada w ostatniej dekadzie miesiaca (w sierpniu - od 11.),
   termin przeskakuje do nastepnego miesiaca, a kalendarz domyslnie
   pokazuje TEN "dzis" - wiec dnia terminu nie ma na siatce w ogole i test
   pada, choc kod jest poprawny. Zalapalo to dopiero losowe TZ w 2b/10,
   ale bez niego padloby samo, gdy tylko "dzis" przesunieloby sie za 10.
   Kazda z dwoch sprawdzanych kratek jest wiec renderowana we WLASCIWYM
   dla siebie miesiacu, a nie w tym, w ktorym akurat jest "teraz".        */
D({ cfg:{ inrEveryDays:21 }, inr:{ [today]:{ value:2.5, ts:at(0,12,0) } } });
const termKeyTest = A.inrTerminKey();
const [termRok, termMies] = termKeyTest.split("-").map(Number);

A.__setView(termRok, termMies - 1);
A.renderCalendar();
{
  const kal = document.getElementById("calGrid").innerHTML;
  check(kal.includes("inrdue-i"), "dzien terminu ma widoczna IKONKE, nie samo obramowanie");
  check((kal.match(/inrdue-i/g)||[]).length === 1, "dokladnie jeden dzien terminu");
}

const [dzisRok, dzisMies] = today.split("-").map(Number);
A.__setView(dzisRok, dzisMies - 1);
A.renderCalendar();
{
  const kal = document.getElementById("calGrid").innerHTML;
  const kratki = kal.split('<div class="day');
  const dzisiaj = kratki.find(k => k.includes('data-k="'+today+'"')) || "";
  check(/class="inr"/.test(dzisiaj) && !/inrdue-i/.test(dzisiaj),
        "dzien Z POMIAREM ma swoj znacznik, nie ikonke terminu");
}

/* Wylaczone przypominanie nie moze zostawiac znacznika na kalendarzu. */
D({ cfg:{ inrEveryDays:0 }, inr:{ [today]:{ value:2.5, ts:at(0,12,0) } } });
A.renderCalendar();
check(!document.getElementById("calGrid").innerHTML.includes("inrdue"),
      "przy wylaczonym przypominaniu kalendarz jest czysty");

head("INR a regularnosc z poprzedzajacego tygodnia");
D({ cfg:{ schedule:["08:00"] }, doses:{
  [shift(-1)]: dose(1,8,0), [shift(-2)]: dose(2,8,30), [shift(-3)]: dose(3,7,30),
  [shift(-4)]: { 0:{ status:"missed", dose:0, source:"device", ts:at(4,8,0) } },
  [shift(-5)]: dose(5,8,0), [shift(-6)]: dose(6,8,0), [shift(-7)]: dose(7,8,0) },
  inr:{ [today]:{ value:2.4, ts:at(0,12,0) } } });
const ctx = A.inrContext(today, 7);
check(ctx.total===7, "okno obejmuje 7 dni PRZED pomiarem (" + ctx.total + ")");
check(ctx.taken===6, "6 z 7 dni z przyjeta dawka (" + ctx.taken + ")");
check(ctx.adherence===86, "regularnosc 86% (" + ctx.adherence + "%)");
check(ctx.avgTime===480, "srednia godzina 08:00 (" + A.hm(ctx.avgTime) + ")");
check(ctx.doseSum===6, "6 tabletek w tygodniu (" + ctx.doseSum + ")");

const ctxEmpty = A.inrContext("2020-01-15", 7);
check(ctxEmpty.taken===0 && ctxEmpty.avgTime===null, "okres bez danych nie psuje wyliczen");

head("Ekran analizy renderuje sie");
D({ cfg:{ schedule:["08:00"] }, doses:{ [shift(-1)]: dose(1,8,10), [shift(-2)]: dose(2,8,40) },
    inr:{ [shift(-1)]:{ value:2.4, ts:at(1,12,0) } } });
document.getElementById("anRange").value = "90";
threw = null;
try { A.renderAnalysis(); } catch(e){ threw = e; }
check(!threw, "renderAnalysis bez wyjatku: " + (threw?.message||"ok"));
check(document.getElementById("anMedian").textContent.includes(":"), "typowa godzina wyswietlona: "
      + document.getElementById("anMedian").textContent);
check(document.getElementById("anHours").innerHTML.includes("<rect"), "histogram godzin narysowany");
check(document.getElementById("anDow").innerHTML.includes("<rect"), "wykres dni tygodnia narysowany");
check(document.getElementById("anInr").innerHTML.includes("regularności"), "zestawienie INR obecne");
check(document.getElementById("anPunct").textContent.endsWith("%"), "punktualnosc: "
      + document.getElementById("anPunct").textContent);

D();
threw = null;
try { A.renderAnalysis(); } catch(e){ threw = e; }
check(!threw, "ekran analizy przy zerze danych: " + (threw?.message||"ok"));
check(document.getElementById("anHours").innerHTML.includes("kilku dniach"),
      "zamiast pustego wykresu pokazuje sie komunikat");

head("Raport zawiera godziny z pudelka");
D({ cfg:{ schedule:["08:00"] }, doses:{ [shift(-1)]: dose(1,8,10) } });
const rr = A.collectRows(3).find(r=>r.key===shift(-1));
check(rr.time==="08:10", "godzina w raporcie: " + rr.time);
D({ doses:{ [shift(-1)]:{0:{status:"taken",dose:1,source:"manual",ts:at(1,12,0)}} } });
check(A.collectRows(3).find(r=>r.key===shift(-1)).time==="",
      "wpis reczny bez zmierzonej godziny zostawia kolumne pusta");

/* ═══════════ 11. TRWALE LOGOWANIE ═══════════ */
head("Sesja i ekran powitalny");
const html = (await import("node:fs")).readFileSync(
  new URL("../index.html", import.meta.url), "utf8");
check(html.includes("indexedDBLocalPersistence"), "sesja zapisywana w IndexedDB");
check(html.includes("browserLocalPersistence"), "localStorage jako zapas");
check(html.includes("navigator.storage?.persist"), "prosba o nieusuwanie danych przez iOS");
check(html.includes('id="splash"'), "ekran powitalny obecny");
check(/<div id="login" class="hide"/.test(html), "ekran logowania startuje ukryty");
check(html.includes("await authReady"), "logowanie czeka na ustawienie trwalej sesji");
check(html.includes("Wylogować się?"), "wylogowanie wymaga potwierdzenia");

head("Karta pudelka");
const ids = new Set([...html.matchAll(/id="([\w-]+)"/g)].map(m => m[1]));
for (const el of ["batBig","batVolt","battFill","batBar","batEta","lastSync","lastSyncRel",
                  "queueInfo","devInfo","batChip"])
  check(ids.has(el), `element ${el} istnieje w HTML`);

D();
threw = null;
try { A.renderStatus(null); } catch(e){ threw = e; }
check(!threw, "brak danych z pudelka nie wywala karty: " + (threw?.message||"ok"));
check(document.getElementById("batVolt").textContent.includes("nie połączyło"),
      "czytelny komunikat zamiast pustego pola");
check(document.getElementById("lastSync").textContent === "nigdy", "ostatnia synchronizacja: nigdy");

threw = null;
try { A.renderStatus({ battery:63, volt:3.91, lastSeen:Math.floor(Date.now()/1000)-600,
                       fw:"1.0.0", ssid:"Dom", rssi:-58, boots:12, queued:0 }); }
catch(e){ threw = e; }
check(!threw, "karta z danymi renderuje sie: " + (threw?.message||"ok"));
check(document.getElementById("batBig").textContent === "63%", "procent baterii: "
      + document.getElementById("batBig").textContent);
check(document.getElementById("batEta").textContent.includes("dni"), "szacowany czas do ladowania");
check(document.getElementById("lastSyncRel").textContent.includes("min"), "opis wzgledny czasu");

head("Uklad: brak przepelnien");
const css = html.match(/<style>([\s\S]*?)<\/style>/)[1];
check(css.includes("repeat(6,minmax(0,1fr))"), "szesc przyciskow licznika w rownych kolumnach");
check(!/\.pillpad button\{[^}]*flex:1/.test(css), "przyciski nie rozpychaja sie trescia");
check(css.includes("calc(10px + env(safe-area-inset-top))"), "naglowek rezerwuje pasek statusu iOS");
/* Ten test wymagal `padding:0 0 env(safe-area-inset-bottom)` na body -
   i mial to za "brak dublowania". Ale pasek nawigacji jest position:fixed,
   wiec liczy sie wzgledem OKNA: wypelnienie body nie dawalo mu niczego,
   a dokladalo wysokosci pod trescia. Rezerwowac musi ten, kto naprawde
   stoi nad wcieciem - czyli sam pasek.                                  */
check(/nav\{[^}]*padding-bottom:env\(safe-area-inset-bottom\)/.test(css),
      "pasek nawigacji sam rezerwuje miejsce nad wcieciem ekranu");
check(!/body\{[^}]*env\(safe-area-inset-bottom\)/.test(css),
      "a body juz tego nie dubluje");
check((css.match(/min-width:0/g)||[]).length >= 4, "zabezpieczenia flexboxa obecne");
check(css.includes("overflow-wrap:anywhere"), "dlugi tekst sie lamie");

head("Uklad nie moze uciekac w bok");
check(/html,body\{[^}]*overflow-x:hidden/.test(css), "dokument nie przewija sie poziomo");
check(/html,body\{[^}]*max-width:100%/.test(css), "dokument nie jest szerszy niz ekran");
check(css.includes("svg{max-width:100%}"), "wykresy nie rozpychaja strony");
check(/main\{overflow-x:hidden\}/.test(css), "tresc glowna przycieta do szerokosci");

head("Odswiezanie w czasie rzeczywistym");
check(html.includes("goOnline(db)"), "polaczenie wznawiane po powrocie do aplikacji");
check(html.includes('addEventListener("visibilitychange", wakeUp)'),
      "reakcja na powrot z tla");
check(html.includes('window.addEventListener("focus", wakeUp)'), "reakcja na focus");
check(html.includes('window.addEventListener("pageshow", wakeUp)'), "reakcja na pageshow");
check(html.includes("setInterval(wakeUp"), "okresowa siatka bezpieczenstwa");
check(html.includes('id="refreshBtn"') && html.includes('onclick="odswiez()"'),
      "reczny przycisk odswiezania w naglowku");
check(/window\.odswiez = \(\) => wakeUp\(true\)/.test(html),
      "odswiezanie dostepne z przycisku, w trybie recznym");

head("Bledy zapisu sa widoczne");
check(html.includes("Nie udało się zapisać do bazy"), "nieudany zapis pokazuje komunikat");
check(/console\.error\("zapis kalendarza:"/.test(html), "blad trafia takze do konsoli");

head("Stare reguly bazy nie moga blokowac kalendarza");
check(html.includes("const { openTs, ...rest } = v"),
      "przy odrzuceniu zapisu druga proba idzie bez pol dodanych pozniej");
check(html.includes("staleRules = true"), "aplikacja zapamietuje, ze reguly sa stare");
check(/Regu\u0142y bezpiecze\u0144stwa s\u0105 przestarza\u0142e/.test(html),
      "i mowi o tym wprost w diagnostyce");
check(html.includes("database.rules.json"), "podaje nazwe pliku do wgrania");

head("Test polaczenia krok po kroku");
check(html.includes("window.testPolaczenia"), "funkcja testu istnieje");
check(html.includes('onclick="testPolaczenia()"'), "i przycisk, ktory ja uruchamia");
for (const krok of ["Zalogowany", "właścicielem pudełka",
                    "Zdarzeń z pudełka", "Ostatnie otwarcie",
                    "ZAPIS ODRZUCONY", "Odczytane z powrotem"])
  check(html.includes(krok), "test sprawdza krok: " + krok);
check(html.includes("poprawiony ręcznie"),
      "wykrywa takze reczna korekte blokujaca nadpisanie");

head("Pola formularzy nie rozpychaja kart");
check(css.includes(".pair{display:grid"), "pary pol na siatce, nie na flexie");
check(css.includes("minmax(0,1.25fr) minmax(0,1fr)"), "kolumny nie daja sie rozepchnac");
check(/input\[type=date\]/.test(css), "pole daty ma wlasne reguly dla iOS");
check(css.includes("min-width:0;outline:none") || /input,select\{[^}]*min-width:0/.test(css),
      "kazde pole moze sie zwezic ponizej swojej tresci");
/* Bylo tu twarde "dokladnie dwie pary". Kazde nowe pole w ustawieniach lamalo
   ten test bez zadnego zwiazku z tym, czego mial pilnowac. Sprawdzamy wiec
   INTENCJE: par jest co najmniej tyle co bylo, a zadna nie ma kolumn, ktore
   dalyby sie rozepchnac trescia.                                          */
{
  const pary = html.match(/class="pair"[^>]*/g) || [];
  check(pary.length >= 2, "pary pol sa uzyte (" + pary.length + ")");
  check(pary.every(p => !/grid-template-columns/.test(p) || /minmax\(0,/.test(p)),
        "kazda para ma kolumny minmax(0,...) - nie da sie ich rozepchnac");
}

head("Diagnostyka na osobnym ekranie");

/* Cztery karty diagnostyczne przenioslo sie z Ustawien na wlasny ekran.
   Test pilnuje PODZIALU, bo bez niego pierwsza nowa karta wroci tam, gdzie
   bylo najblizej - czyli do Ustawien, ktore wlasnie odchudzilismy.       */
{
  const wytnij = (id) => {
    const od = html.indexOf(`<section id="${id}"`);
    return od < 0 ? "" : html.slice(od, html.indexOf("</section>", od));
  };
  const ust  = wytnij("tab-set");
  const diag = wytnij("tab-diag");

  check(diag.length > 0, "istnieje osobny ekran Diagnostyki");
  /* Szukamy NAGLOWKA karty, nie samej frazy - karta wejsciowa w Ustawieniach
     wymienia te nazwy w opisie ("Co przyslalo pudelko, autotest, ...") i to
     jest w porzadku. Chodzi o to, gdzie stoi KARTA.                       */
  const naglowek = (sekcja, karta) => sekcja.includes(`<h3>${karta}</h3>`);
  for (const karta of ["Co przysłało pudełko", "Autotest pudełka",
                       "Historia pudełka", "Test wieczka"]){
    check(naglowek(diag, karta) && !naglowek(ust, karta),
          `„${karta}" jest w Diagnostyce, nie w Ustawieniach`);
  }
  /* Ustawienia maja zostac z rzeczami, ktore COS USTAWIAJA. */
  for (const karta of ["Lek i dawkowanie", "Zakres terapeutyczny INR",
                       "Jak często mierzysz INR"]){
    check(naglowek(ust, karta), `„${karta}" zostaje w Ustawieniach`);
  }
  /* NAJWAZNIEJSZE: ostrzezenia nie moga sie schowac razem z kartami.
     Oznaczaja utrate danych, wiec musza byc widoczne bez wchodzenia glebiej. */
  check(ust.includes('id="setWarn"'),
        "miejsce na ostrzezenia zostalo w Ustawieniach");
  check(!diag.includes('id="setWarn"'),
        "ostrzezenia NIE przeniosly sie do Diagnostyki");
  check(/showTab\('diag'\)/.test(ust), "z Ustawien da sie wejsc w Diagnostyke");
  check(/showTab\('set'\)/.test(diag), "z Diagnostyki da sie wrocic");
}

/* Ostrzezenia pokazuja sie tylko wtedy, gdy jest o czym ostrzegac. */
A.renderOstrzezenia();
{
  const w = document.getElementById("setWarn");
  check(!!w, "element ostrzezen istnieje w DOM");
  check(!w.innerHTML.trim(), "przy czystym stanie ostrzezenia sa puste");
}

/* Straty zgloszone przez pudelko (B3/B5). Bez tego komunikatu odrzucony
   albo niezapisany wpis znika bez sladu - kalendarz wyglada normalnie. */
check(A.ostrzStraty()==="", "gdy pudelko nic nie zglasza, nie strasza nikogo");
A.renderStatus({ battery:80, dropped:2, nvsFail:0 });
{
  const w = document.getElementById("setWarn").innerHTML;
  check(/utrat[eę] danych/.test(w), "odrzucone zdarzenia widoczne w Ustawieniach");
  check(/<b>2<\/b>/.test(w), "liczba odrzuconych podana wprost");
}
A.renderStatus({ battery:80, dropped:0, nvsFail:1 });
check(/pami[eę]ci pude[lł]ka si[eę] nie uda[lł]/.test(
        document.getElementById("setWarn").innerHTML),
      "nieudany zapis do pamieci tez widoczny");
A.renderStatus({ battery:80 });
check(!document.getElementById("setWarn").innerHTML.trim(),
      "status bez licznikow strat nie zostawia ostrzezenia");

head("Legenda kalendarza");
/* Ikonka bez podpisu to zagadka. Legenda pod kalendarzem musi ja tlumaczyc,
   i to tym samym ksztaltem, ktorego uzywa INR_DUE_MARK - inaczej te dwa
   miejsca rozjada sie po pierwszej zmianie wygladu.                      */
check(/czas na pomiar INR/.test(html), "legenda kalendarza tlumaczy nowa ikonke");
check(/legend[\s\S]{0,900}circle cx="6" cy="6" r="4\.4"/.test(html),
      "legenda uzywa TEJ SAMEJ ikonki co znacznik w kalendarzu");
/* Wyjasnienie stoi przy USTAWIENIU odstepu, nie pod kalendarzem - tam
   zabieralo miejsce przy kazdym spojrzeniu, a potrzebne jest raz.        */
check(!/Pomarańczowy znak pokazuje dzień/.test(html),
      "pod kalendarzem NIE ma juz akapitu wyjasniajacego");
check(/Termin liczy się od ostatniego wyniku[\s\S]{0,80}pomarańczowy znak/.test(html),
      "wyjasnienie jest przy ustawieniu odstepu");

check(!/id="inrDate"[\s\S]{0,200}flex:1/.test(html), "stary uklad flex przy dacie usuniety");

head("Wersja aplikacji");
const vApp = html.match(/APP_VERSION = "([^"]+)"/)[1];
const swjs = (await import("node:fs")).readFileSync(
  new URL("../sw.js", import.meta.url), "utf8");
const vSw = swjs.match(/CACHE = "pillbox-([^"]+)"/)[1];
check(vApp === vSw, `numer wersji zgodny w index.html i sw.js (${vApp} / ${vSw})`);
check(html.includes('id="appVersion"'), "wersja widoczna w Ustawieniach");

head("Korekty licznika tabletek");
for (const v of ["-10","-1","-0.5","1","10","100"])
  check(html.includes(`addPills(${v})`), `przycisk ${v} obecny`);
check(!html.includes("addPills(-100)"), "usunieto -100 (za grube na korekte)");

A.__resetDb();
D({ cfg:{ pillsLeft:100, pillsCountedUntil:shift(-1) } });
await globalThis.addPills(-0.5);
check(A.cfg.pillsLeft===99.5, "100 − ½ = " + A.cfg.pillsLeft);
await globalThis.addPills(-0.5);
check(A.cfg.pillsLeft===99, "99,5 − ½ = " + A.cfg.pillsLeft);
await globalThis.addPills(100);
check(A.cfg.pillsLeft===199, "nowe opakowanie: " + A.cfg.pillsLeft);
A.__setState({ cfg:{ pillsLeft:0.5 } });
await globalThis.addPills(-1);
check(A.cfg.pillsLeft===0, "nie schodzi ponizej zera: " + A.cfg.pillsLeft);
A.__setState({ cfg:{ pillsLeft:99.5 } });
check(A.nf(99.5)==="99,5", "wyswietlanie polowki: " + A.nf(99.5));

/* ═══════════ 14. DOBA LEKOWA KONCZY SIE NAD RANEM ═══════════ */
head("Granica doby w aplikacji");

/* Ta sama liczba co DAY_START_HOUR w config.h - to najwazniejszy warunek
   calej funkcji. Rozjazd o jedna godzine oznacza, ze pudelko i telefon
   przypisza te sama tabletke do dwoch roznych dni.                      */
const cfgH = readFileSync(new URL("../firmware/PillBox/config.h", import.meta.url), "utf8");
const mDay = cfgH.match(/#define\s+DAY_START_HOUR\s+(\d+)/);
check(!!mDay, "config.h definiuje DAY_START_HOUR");
check(mDay && +mDay[1] === A.DAY_START_HOUR,
      `aplikacja (${A.DAY_START_HOUR}) i firmware (${mDay?.[1]}) licza dobe tak samo`);

/* Przesuniecie daty. */
const medAt = (h,mi=0) => { const d = new Date(2026,7,2,h,mi,0); return A.dateKey(A.medDate(d)); };
check(medAt(20)==="2026-08-02", "20:00 -> " + medAt(20));
check(medAt(23,59)==="2026-08-02", "23:59 -> " + medAt(23,59));
check(medAt(0,10)==="2026-08-01", "00:10 nalezy do 1 sierpnia -> " + medAt(0,10));
check(medAt(2)==="2026-08-01", "02:00 (dawka przed snem) -> " + medAt(2));
check(medAt(2,59)==="2026-08-01", "02:59 to ostatnia minuta starej doby -> " + medAt(2,59));
check(medAt(3)==="2026-08-02", "03:00 zaczyna nowa dobe -> " + medAt(3));
check(medAt(8)==="2026-08-02", "rano to juz nowy dzien -> " + medAt(8));

/* devKey (zdarzenia z pudelka) i medDate (zegar telefonu) musza sie zgadzac,
   inaczej kalendarz pokazalby co innego niz pudelko zapisalo.           */
const nightTs = Math.floor(Date.UTC(2026,7,2,0,0,0)/1000);   /* 02:00 czasu PL */
check(A.devKey(nightTs)==="2026-08-01",
      "zdarzenie o 02:00 trafia do 1 sierpnia -> " + A.devKey(nightTs));
check(A.devHM(nightTs)==="02:00", "a godzina w szczegolach zostaje prawdziwa");

/* Rekoncyliacja: nocne otwarcie nie moze zalozyc wpisu na nowy dzien. */
A.__resetDb();
D({ events:[ { ts:nightTs, type:"open", slot:0 } ] });
await globalThis.reconcile();
check(!!A.__db.data.users?.testuid?.doses?.["2026-08-01"],
      "nocna tabletka zapisana na 1 sierpnia");
check(!A.__db.data.users?.testuid?.doses?.["2026-08-02"],
      "i NIE na 2 sierpnia (to byloby zafalszowanie adherencji)");

/* yesterdayKey liczy sie od doby lekowej, nie od zegara. */
check(A.yesterdayKey() !== A.todayKey(), "wczoraj to nie dzis");
const dt = (new Date(A.todayKey()+"T12:00:00") - new Date(A.yesterdayKey()+"T12:00:00"))/86400000;
check(Math.round(dt) === 1, "wczoraj jest dokladnie dobe przed dzis (" + dt + ")");

head("Baner: pudelko zostalo otwarte");
const banner = document.getElementById("openWarn");
A.renderOpenWarn(null);
check(banner.classList.contains("hide"), "brak danych -> baner ukryty");
A.renderOpenWarn({ battery:80, boxOpen:false });
check(banner.classList.contains("hide"), "zamkniete -> baner ukryty");

const openedAt = Math.floor(Date.now()/1000) - 20*60;
A.renderOpenWarn({ battery:80, boxOpen:true, openSince:openedAt });
check(!banner.classList.contains("hide"), "otwarte -> baner widoczny");
const txt = document.getElementById("openWarnWhen").textContent;
check(/20 min/.test(txt), "podaje, od jak dawna: " + txt);
check(/od \d\d:\d\d/.test(txt), "i o ktorej zostalo otwarte: " + txt);

A.renderOpenWarn({ battery:80, boxOpen:true,
                   openSince: Math.floor(Date.now()/1000) - 150*60 });
check(/2 h 30 min/.test(document.getElementById("openWarnWhen").textContent),
      "dluzsze otwarcie liczone w godzinach: " + document.getElementById("openWarnWhen").textContent);

A.renderOpenWarn({ battery:80, boxOpen:true, openSince:1 });
check(document.getElementById("openWarnWhen").textContent.length > 0,
      "pudelko bez zegara i tak dostaje sensowny tekst");
A.renderOpenWarn({ battery:80, boxOpen:false });
check(banner.classList.contains("hide"), "po zamknieciu baner znika");

head("Dni sprzed uruchomienia pudelka");
D({ cfg:{ trackingSince: shift(-5) } });
check(A.dayStatus(shift(-9))==="nodata", "dzien przed startem -> brak danych, nie czerwony");
check(A.dayStatus(shift(-5))==="missed", "dzien startu juz sie liczy");
check(A.dayStatus(shift(-3))==="missed", "pozniejszy dzien bez wpisu nadal pominiety");

/* Reczny wpis wygrywa nawet przed data startu - skoro cos tam wpisales,
   to znaczy, ze wiesz lepiej niz domyslna regula. */
D({ cfg:{ trackingSince: shift(-5) },
    doses:{ [shift(-9)]: { 0:{ status:"taken", dose:1, source:"manual" } } } });
check(A.dayStatus(shift(-9))==="taken", "reczny wpis sprzed startu jest respektowany");

/* Adherencja nie moze karac za czasy, ktorych nikt nie mierzyl. */
D({ cfg:{ trackingSince: shift(-2) },
    doses:{ [shift(-1)]: { 0:{ status:"taken", dose:1, source:"device" } } } });
const anaT = A.analyze(30);
check(anaT.total === 2, "liczone tylko dni od startu (" + anaT.total + ")");
check(anaT.taken === 1, "w tym jedna dawka wzieta");

const rowsT = A.collectRows(10);
const nod = rowsT.filter(r => r.status === "nodata");
check(nod.length > 0, "raport oznacza dni sprzed startu jako brak danych");
check(nod.every(r => r.dose === ""), "i nie wpisuje im zera tabletek");

head("Strefy czasowe");
check(A.TZ_LIST.length >= 20, "lista ma " + A.TZ_LIST.length + " stref");
check(A.TZ_LIST.some(z => z[0] === "Europe/Warsaw"), "jest Polska");
check(A.TZ_DEFAULT === "Europe/Warsaw", "i jest domyslna");
check(A.TZ_LIST.every(z => Array.isArray(z) && z.length === 2 && z[0] && z[1]),
      "kazda pozycja ma identyfikator i nazwe");
check(new Set(A.TZ_LIST.map(z => z[0])).size === A.TZ_LIST.length, "bez powtorek");

/* Przesuniecia liczone z prawdziwego kalendarza - sprawdzamy na konkretnych
   datach, bo to jedyny sposob, zeby zlapac blad w czasie letnim. */
const lato = new Date(Date.UTC(2026, 6, 1, 12));
const zima = new Date(Date.UTC(2026, 0, 15, 12));
check(A.tzOffsetFor("Europe/Warsaw", lato) === 120, "Polska latem: +120 min");
check(A.tzOffsetFor("Europe/Warsaw", zima) === 60,  "Polska zima: +60 min");
check(A.tzOffsetFor("UTC", lato) === 0, "UTC zawsze 0");
check(A.tzOffsetFor("America/New_York", zima) === -300, "Nowy Jork zima: -300 min");
check(A.tzOffsetFor("Asia/Tokyo", lato) === 540, "Tokio bez czasu letniego: +540 min");
{ const err = console.error; console.error = () => {};   /* oczekiwany blad - nie zasmiecamy wyniku */
  check(A.tzOffsetFor("nie-ma-takiej", lato) === null, "nieznana strefa nie wywala aplikacji");
  console.error = err; }

check(A.tzOffsetTxt(120) === "UTC+02:00", "opis dodatniego: " + A.tzOffsetTxt(120));
check(A.tzOffsetTxt(-300) === "UTC−05:00", "opis ujemnego: " + A.tzOffsetTxt(-300));
check(A.tzOffsetTxt(0) === "UTC+00:00", "zero tez czytelne");
check(A.tzLabel("Europe/Warsaw").includes("Polska"), "nazwa po polsku");

head("Blokada obracania ekranu");
check(html.includes('id="rotateLock"'), "zaslona na poziom istnieje");
check(/orientation:landscape/.test(css), "wlacza sie tylko w poziomie");
check(/max-height:520px/.test(css), "i tylko na telefonach, nie na tablecie");
check(/#rotateLock\{display:none\}/.test(css), "w pionie ukryta");
check(/main,nav,header\{display:none/.test(css), "reszta aplikacji chowana, zeby sie nie rozjezdzala");
const mf = JSON.parse(readFileSync(new URL("../manifest.json", import.meta.url), "utf8"));
check(mf.orientation === "portrait", "manifest tez prosi o pion (dziala poza iOS)");

head("Diagnostyka: co przyslalo pudelko");
const diag = document.getElementById("diagList");

D({ events: [] });
A.renderDiag();
check(/nie przysłało jeszcze/.test(diag.innerHTML), "brak zdarzen -> czytelny komunikat");
check(/reguły\s+bezpieczeństwa/.test(diag.innerHTML),
      "podpowiada, gdzie szukac, gdy pudelko wysyla, a aplikacja nie widzi");

/* Zdarzenie jest, wpisu w kalendarzu nie ma - to musi rzucac sie w oczy. */
D({ events: [ { ts:t0800, type:"open", slot:0 } ], doses:{} });
A.renderDiag();
check(/BRAK w kalendarzu/.test(diag.innerHTML),
      "otwarcie bez wpisu oznaczone jako brakujace");
check(/2026-07-31/.test(diag.innerHTML), "pokazana doba lekowa zdarzenia");
check(/08:00/.test(diag.innerHTML), "pokazana prawdziwa godzina otwarcia");

/* Ten sam przypadek, ale wpis juz jest.

   Zdarzenia Z POKRYCIEM sa domyslnie zwiniete - lista powstala do pytania
   "dlaczego tej dawki nie ma w kalendarzu", a czterdziesci zielonych
   ptaszkow na nie nie odpowiada. Rozwijamy ja, zeby sprawdzic sam wiersz. */
D({ events: [ { ts:t0800, type:"open", slot:0 } ],
    doses:{ "2026-07-31": { 0:{ status:"taken", dose:1, source:"device" } } } });
A.renderDiag();
check(/Wszystkie/.test(diag.innerHTML) && !/jest w kalendarzu/.test(diag.innerHTML),
      "gdy wszystko sie zgadza, lista jest zwinieta do jednego zdania");
window.diagPrzelacz();
check(/jest w kalendarzu/.test(diag.innerHTML), "zdarzenie rozliczone oznaczone na zielono");
window.diagPrzelacz();
check(!/BRAK w kalendarzu/.test(diag.innerHTML), "i bez ostrzezenia");

/* Zdarzenia niedotyczace dawki nie moga straszyc czerwonym "BRAK" ani
   liczyc sie jako "bez pokrycia" - uruchomienie nie jest dawka.        */
D({ events: [ { ts:t0800, type:"boot" } ], doses:{} });
A.renderDiag();
check(/Wszystkie/.test(diag.innerHTML) && !/BRAK w kalendarzu/.test(diag.innerHTML),
      "samo uruchomienie nie jest zglaszane jako brak pokrycia");
window.diagPrzelacz();
check(/nie dotyczy dawki/.test(diag.innerHTML), "uruchomienie opisane jako nieistotne");
check(!/BRAK w kalendarzu/.test(diag.innerHTML), "i nie liczone jako zgubiona dawka");
window.diagPrzelacz();

/* Sedno porzadkow: gdy cos NIE ma pokrycia, widac to bez klikania. */
D({ events: [ { ts:t0800, type:"open", slot:0 },
               { ts:t0800 - 86400, type:"open", slot:0 } ],
    doses:{ "2026-07-31": { 0:{ status:"taken", dose:1, source:"device" } } } });
A.renderDiag();
check(/nie ma\s*\n?\s*pokrycia|nie ma pokrycia/.test(diag.innerHTML.replace(/\s+/g," ")),
      "niepokryte zdarzenie jest widoczne od razu, bez rozwijania");
check((diag.innerHTML.match(/doba lekowa/g) || []).length === 1,
      "i tylko ono - pokryte nie zasmiecaja listy");

/* Nocne otwarcie ma trafic do doby poprzedniej takze w diagnostyce. */
D({ events: [ { ts:nightTs, type:"open", slot:0 } ], doses:{} });
A.renderDiag();
check(/2026-08-01/.test(diag.innerHTML), "otwarcie o 02:00 opisane jako doba z 1 sierpnia");
check(/02:00/.test(diag.innerHTML), "z zachowaniem prawdziwej godziny");

head("Bledy odczytu z bazy sa widoczne");
check(html.includes("const onErr = where =>"), "kazdy nasluch ma obsluge bledu");
check((html.match(/onErr\("/g) || []).length >= 5,
      "wszystkie piec nasluchow: " + (html.match(/onErr\("/g) || []).length);
check(html.includes('id="diagErr"'), "miejsce na komunikat o odmowie dostepu");
check(html.includes("Baza odmówiła dostępu"), "komunikat mowi wprost, co sie stalo");

head("Ratunek, gdy w pamieci utknie zla wersja");
check(html.includes('onclick="wyczyscCache()"'), "przycisk czyszczenia pamieci w Ustawieniach");
check(html.includes("window.wyczyscCache"), "i jego definicja");
check(/getRegistrations\(\)[\s\S]{0,200}unregister/.test(html),
      "wyrejestrowuje service workera, nie tylko kasuje cache");
check(/caches\.keys\(\)[\s\S]{0,200}caches\.delete/.test(html), "kasuje wszystkie pamieci");
check(/location\.replace/.test(html), "i przeladowuje z pominieciem pamieci posredniej");

const swSrc = readFileSync(new URL("../sw.js", import.meta.url), "utf8");
check(swSrc.includes("worthCaching"), "service worker filtruje, co wolno zapamietac");
check(/res\.ok/.test(swSrc), "nie zapamietuje odpowiedzi bledu (404, strona awarii)");
check(/text\/html/.test(swSrc), "strony zapamietuje tylko gdy serwer mowi, ze to HTML");
check(swSrc.includes("wyczysc-cache"), "worker przyjmuje polecenie wyczyszczenia");

/* ── B25: aplikacja widziala JEDNO zdarzenie zamiast czterdziestu dwoch ──
   Kuba przyslal zrzut Diagnostyki, na ktorym stalo obok siebie:
       "Pokazano 1 z 1 zdarzen"      (tablica events w aplikacji)
       "Zdarzen z pudelka: 42"       (sprawdzenie polaczenia)
   To samo zapytanie, ta sama chwila, ten sam uzytkownik.

   Roznily sie JEDNYM znakiem - klamra:
       s.forEach(c => evs.push(...));      <- dziala, zwraca undefined
       s.forEach(c => events.push(...))    <- zwraca push() = dlugosc >= 1

   DataSnapshot.forEach PRZERYWA enumeracje, gdy callback zwroci true.
   push() zwraca liczbe, liczba >= 1 jest prawdziwa - wiec petla konczyla
   sie po pierwszym dziecku. ZAWSZE.

   Dlatego dni 6-9 sierpnia nie dawaly sie odtworzyc: uzupelnianie
   kalendarza dostawalo jedno zdarzenie i nie mialo z czego ich zrobic.  */
head("Zdarzenia z pudelka czytane w CALOSCI, nie pierwsze z brzegu");
{
  /* Zachowanie, nie zapis: atrapa Firebase przerywa forEach na true,
     tak jak prawdziwy SDK. Ten sam ksztalt bledu z klamrami i bez.    */
  const { get, __db } = await import("./firebase_stub.mjs");
  __db.data = { probka: { a:{ts:1}, b:{ts:2}, c:{ts:3}, d:{ts:4} } };
  const snap = await get({ path: "probka" });

  const zKlamrami = [];
  snap.forEach(c => { zKlamrami.push(c.key); });
  check(zKlamrami.length === 4, `z klamrami czyta wszystkie 4 (${zKlamrami.length})`);

  const bezKlamer = [];
  snap.forEach(c => bezKlamer.push(c.key));
  check(bezKlamer.length === 1,
        `bez klamer atrapa URYWA po pierwszym (${bezKlamer.length}) - tak jak Firebase`);
}

/* I kontrola samego zrodla: zaden odczyt zdarzen nie moze uzywac formy
   bez klamer. Regula jest prosta - callback forEach nie zwraca wartosci. */
{
  /* `\s*(?!\{)` NIE dziala: \s* moze sie cofnac do zera znakow i lookahead
     przechodzi na spacji. Wymagamy wprost, zeby pierwszy niebialy znak
     ciala nie byl klamra.                                              */
  const zle = [...html.matchAll(/\.forEach\(\s*\w+\s*=>[ \t]*([^\s{][^\n;]*)/g)]
    .filter(m => /\.push\(/.test(m[1]));
  check(zle.length === 0,
        `zaden forEach na danych z bazy nie zwraca wyniku push() (${zle.length}: ` +
        `${zle.map(m => m[0].slice(0, 45)).join(" | ")})`);
}

/* Pasek nawigacji "rozjezdzal sie przy scrollowaniu" na iPhonie (B26). */
head("Pasek nawigacji trzyma sie dolu ekranu");
{
  const css = html.slice(0, html.indexOf("</style>"));
  const bez = (css.match(/(?<!-)\bbackdrop-filter:/g) || []).length;
  const zPrefiksem = (css.match(/-webkit-backdrop-filter:/g) || []).length;
  check(zPrefiksem >= bez && zPrefiksem > 0,
        `kazde rozmycie tla ma prefiks -webkit- dla iOS (${zPrefiksem}/${bez})`);
  check(/nav\{[^}]*transform:translateZ\(0\)/.test(css),
        "pasek ma wlasna warstwe kompozycji - inaczej zostaje w tyle za przewijaniem");
  check(!/body\{[^}]*padding:0 0 env\(safe-area-inset-bottom\)/.test(css),
        "body nie rezerwuje juz safe-area drugi raz po pasku");
  check(/main\{[^}]*padding:[^;}]*env\(safe-area-inset-bottom\)/.test(css),
        "za to tresc rezerwuje miejsce na pasek RAZEM z safe-area");
}

/* Historia pudelka: 32 wiersze rutyny spychaly wszystko inne poza ekran. */
head("Historia pudelka zwinieta do ostatnich pieciu");
{
  const wpisy = [];
  for (let i = 0; i < 12; i++) wpisy.push(`${1786300000 + i*3600}|reed|wyslane|80|0`);
  A.__setState({ boxLog: wpisy });
  A.renderBoxLog();
  const el = document.getElementById("boxLog");
  const ile = h => (h.match(/w kolejce|· bateria/g) || []).length;
  check(ile(el.innerHTML) === 5, `domyslnie piec wpisow (${ile(el.innerHTML)})`);
  check(/Pokaż całą historię \(12\)/.test(el.innerHTML), "i przycisk z prawdziwa liczba");
  window.logPrzelacz();
  check(ile(el.innerHTML) === 12, `po rozwinieciu wszystkie (${ile(el.innerHTML)})`);
  window.logPrzelacz();

  /* Przy krotkiej historii przycisk bylby tylko halasem. */
  A.__setState({ boxLog: [`1786300000|reed|wyslane|80|0`] });
  A.renderBoxLog();
  check(!/Pokaż całą historię/.test(document.getElementById("boxLog").innerHTML),
        "przy jednym wpisie nie ma czego rozwijac");
  A.__setState({ boxLog: [] });
}

/* ── B27: odmowa bazy opisana jako "czeka na polaczenie" ──
   Kuba: "wpisuje recznie tego 8, a po wyjsciu z aplikacji mi sie to cofa
   do tego ze nie wzialem".

   Firebase pokazuje WLASNY zapis lokalnie od razu, wiec zmiana byla widoczna
   na ekranie. Dopiero serwer ja odrzucal, SDK cofal wartosc - i dzien wracal
   do "nie wziete". Aplikacja mowila przy tym "czeka na polaczenie, nie
   zginie", czyli doklandnie odwrotnie niz bylo: odmowa NIE minie sama.

   To samo rozroznienie robi juz oczekWyslij() (D16) i trwaleOdrzucony()
   w pudelku (D13) - tu go brakowalo.                                    */
head("Odmowa bazy nie udaje braku sieci");
{
  const box = document.getElementById("toasts");

  A.__db.tryb = "odmowa";
  await A.zapiszPewnie({ [`users/testuid/doses/2026-08-08/0`]:
    { status:"taken", dose:1, source:"manual", ts:1786237200 } }, "Zmiana dawki");
  check(/ODRZUCIŁA/.test(box.innerHTML),
        `odmowa nazwana po imieniu: ${box.textContent.slice(0,60)}`);
  check(!/nie zginie/.test(box.innerHTML),
        "i NIE obiecujemy, ze zapis sam dojdzie");

  A.__db.tryb = "blad";
  await A.zapiszPewnie({ [`users/testuid/doses/2026-08-08/0`]:
    { status:"taken", dose:1, source:"manual", ts:1786237200 } }, "Zmiana dawki");
  check(/nie zginie/.test(box.innerHTML),
        `brak sieci nadal opisany spokojnie: ${box.textContent.slice(0,60)}`);
  check(!/ODRZUCIŁA/.test(box.innerHTML), "bo tu zapis naprawde poczeka");

  A.__db.tryb = "ok";
  A.oczekZapisz([]);
}

head("Automatyczne uzupelnianie kalendarza");
/* Sedno: to ma dzialac bez klikania. Nasluch zdarzen i nasluch kalendarza
   MUSZA same wolac uzupelnianie, a jeden nieudany odczyt nie moze go zabic. */
check(/onValue\(query\(ref\(db, `devices\/\$\{DEVICE_ID\}\/events`[\s\S]{0,1400}doReconcile\(true\)/.test(html),
      "nasluch zdarzen sam uzupelnia kalendarz");
check(/users\/\$\{uid\}\/doses`\), s => \{[\s\S]{0,400}doReconcile\(true\)/.test(html),
      "nasluch kalendarza tez - na wypadek innej kolejnosci nadejscia danych");
check(html.includes("Promise.allSettled"),
      "odswiezanie znosi pojedynczy nieudany odczyt");
check(!/await Promise\.all\(\[/.test(html),
      "nigdzie nie zostalo Promise.all, ktore przerywa wszystko");
/* TEN TEST UTRWALAL BLAD. Sprawdzal, ze doReconcile stoi na KONCU
   odswiezania - czyli za szescioma wywolaniami rysujacymi. Wyjatek
   w ktorymkolwiek z nich (brakujacy element, pole, ktorego starszy
   firmware nie przysyla) leci do catch i uzupelnianie sie nie wykonuje.
   Kuba zglosil dokladnie to: "dzisiejszy dzien zsynchronizowal sie
   dopiero po nacisnieciu przelicz, a to sie samo powinno robic".

   Wlasciwy niezmiennik jest odwrotny: dawka w kalendarzu przed
   kosmetyka. Rysowanie moze paść - uzupelnianie nie.                  */
check(/await doReconcile\(true\);\s*(?:\/\*[\s\S]*?\*\/\s*)?renderStatus\(/.test(html),
      "uzupelnianie PRZED rysowaniem w odswiezaniu - wyjatek w renderze go nie zabija");
{
  /* Kolejnosc liczona wprost W CIELE wakeUp(), zeby test nie zalezal od
     jednego wzorca ani nie trafil w renderBoxLog z innego nasluchu.    */
  const od = html.indexOf("Promise.allSettled");
  const cialo = html.slice(od, od + 3000);
  const i = cialo.indexOf("await doReconcile(true);");
  const r = cialo.indexOf("renderBoxLog(); renderTesty();");
  check(i > 0 && r > i,
        `w odswiezaniu doReconcile (+${i}) wyprzedza rysowanie (+${r})`);
}
/* --- Siatka bezpieczenstwa musi patrzec TAM, GDZIE PISZE uzupelnianie ---
   brakujePokrycia() czytalo doses[dzien][ev.slot], a doReconcile() od
   naprawy B9 pisze ZAWSZE do slotu 0. Przy dwoch przypomnieniach
   zdarzenie ze `slot: 1` nie mialo wiec pokrycia NIGDY - siatka wolala
   uzupelnianie co minute bez konca i nigdy nie meldowala "pokryte".
   Dwie funkcje opisujace ten sam fakt musza liczyc go tak samo.       */
{
  const ts = Math.floor(Date.now()/1000);
  /* Klucz dnia liczymy DOKLADNIE tak, jak robi to brakujePokrycia() -
     przez devKey(ts), a nie todayKey(). Doba lekowa zaczyna sie o 3:00,
     wiec o 01:xx w niektorych strefach te dwa daja rozne dni i test
     padalby losowo w przebiegu 2b/10 (ta sama pulapka co N3).        */
  const dzis = A.devKey(ts);

  A.__setState({ events: [{ id:"e1", ts, type:"open", slot: 1 }], doses: {} });
  check(A.brakujePokrycia() === true, "brak wpisu w kalendarzu = brakuje pokrycia");

  /* Pudelko przyslalo slot 1, ale dawka siedzi - jak zawsze - w slocie 0. */
  A.__setState({ events: [{ id:"e1", ts, type:"open", slot: 1 }],
                 doses: { [dzis]: { 0: { status:"taken", source:"device" } } } });
  check(A.brakujePokrycia() === false,
        "dawka w slocie 0 pokrywa zdarzenie z dowolnym numerem przypomnienia");

  A.__setState({ events: [{ id:"e1", ts, type:"open", slot: 0 }],
                 doses: { [dzis]: { 0: { status:"missed", source:"device" } } } });
  check(A.brakujePokrycia() === true,
        "wpis 'pominiete' przy zdarzeniu otwarcia to nadal brak pokrycia");

  A.__setState({ events: [{ id:"e1", ts, type:"open", slot: 0 }],
                 doses: { [dzis]: { 0: { status:"missed", source:"manual" } } } });
  check(A.brakujePokrycia() === false, "reczna korekta zamyka sprawe");
  A.__setState({ events: [], doses: {} });
}

check(html.includes("lastRec = { at: Date.now()"), "kazde uzupelnienie zostawia slad");
check(html.includes("Ostatnie automatyczne uzupełnienie"), "i jest on widoczny w diagnostyce");
check(html.includes("nic nie wymagało zmiany"),
      "rozroznia 'nie zadzialalo' od 'nie bylo czego dopisac'");

head("Wyniki autotestu w aplikacji");
const tl = document.getElementById("testList");

D({ boxLog: [] });
A.renderTesty();
check(/nie był jeszcze uruchamiany/.test(tl.innerHTML), "brak testu -> czytelny tekst");

/* Pelny przebieg zakonczony sukcesem. */
const bazaT = t0800;
D({ boxLog: [
  `${bazaT}|KONTAKTRON (otwarto pudelko)|autotest|85|0`,
  `${bazaT}|KONTAKTRON (otwarto pudelko)|test:START|85|0`,
  `${bazaT+2}|KONTAKTRON (otwarto pudelko)|test:bateria OK|85|0`,
  `${bazaT+4}|KONTAKTRON (otwarto pudelko)|test:kontaktron OK|85|0`,
  `${bazaT+6}|KONTAKTRON (otwarto pudelko)|test:przycisk OK|85|0`,
  `${bazaT+8}|KONTAKTRON (otwarto pudelko)|test:pamiec OK|85|0`,
  `${bazaT+10}|KONTAKTRON (otwarto pudelko)|test:wifi OK|85|0`,
  `${bazaT+12}|KONTAKTRON (otwarto pudelko)|test:zegar OK|85|0`,
  `${bazaT+14}|KONTAKTRON (otwarto pudelko)|test:baza OK|85|0`,
  `${bazaT+16}|KONTAKTRON (otwarto pudelko)|test:KONIEC 0 bledow|85|0`
]});
A.renderTesty();
check(/Wszystko sprawne/.test(tl.innerHTML), "komplet bez bledow opisany wprost");
check((tl.innerHTML.match(/✓/g) || []).length === 7, "siedem etapow na zielono");
check(!/✗/.test(tl.innerHTML), "i ani jednego krzyzyka");
check(/kontaktron \(magnes\)/.test(tl.innerHTML), "nazwy etapow po polsku");
check(/08:00/.test(tl.innerHTML), "z godzina przebiegu");

/* Przebieg z bledem. */
D({ boxLog: [
  `${bazaT}|timer|test:START|85|0`,
  `${bazaT+2}|timer|test:bateria OK|85|0`,
  `${bazaT+4}|timer|test:kontaktron BLAD|85|0`,
  `${bazaT+6}|timer|test:KONIEC 1 bledow|85|0`
]});
A.renderTesty();
check(/1 błąd/.test(tl.innerHTML), "liczba bledow w naglowku");
check((tl.innerHTML.match(/✗/g) || []).length === 1, "nieudany etap oznaczony");
check((tl.innerHTML.match(/✓/g) || []).length === 1, "udany nadal na zielono");

/* Dwa przebiegi pod rzad - pokazujemy tylko OSTATNI. */
D({ boxLog: [
  `${bazaT}|timer|test:START|85|0`,
  `${bazaT+2}|timer|test:bateria BLAD|85|0`,
  `${bazaT+4}|timer|test:KONIEC 1 bledow|85|0`,
  `${bazaT+100}|timer|test:START|85|0`,
  `${bazaT+102}|timer|test:bateria OK|85|0`,
  `${bazaT+104}|timer|test:KONIEC 0 bledow|85|0`
]});
A.renderTesty();
check(/Wszystko sprawne/.test(tl.innerHTML), "widac wynik ostatniego przebiegu");
check((tl.innerHTML.match(/✗/g) || []).length === 0, "stary, nieudany przebieg nie miesza");

/* Przebieg przerwany - bez linii KONIEC. */
D({ boxLog: [ `${bazaT}|timer|test:START|85|0`,
              `${bazaT+2}|timer|test:bateria OK|85|0` ] });
A.renderTesty();
check(/przerwany/.test(tl.innerHTML), "przerwany przebieg opisany uczciwie");

/* Wpisy testowe nie moga zasmiecac zwyklej historii. */
D({ boxLog: [ `${bazaT}|timer|test:bateria OK|85|0`,
              `${bazaT+5}|timer|brak wifi|85|2` ] });
A.renderBoxLog();
const bl2 = document.getElementById("boxLog").innerHTML;
check(/brak wifi/.test(bl2), "zwykle wpisy zostaja w historii");
check(!/test:/.test(bl2), "a wyniki testu maja wlasna karte");

head("Instrukcja autotestu jest przy wynikach");
/* Przy sklejonym pudelku jedyna informacja w trakcie testu to pikniecia -
   wiec legenda MUSI byc tam, gdzie uzytkownik i tak patrzy.            */
/* Sygnaly sa teraz dwa: start i koniec czesci wymagajacej czlowieka.
   Legenda ma opisywac dokladnie to, co pudelko robi - ani wiecej.   */
for (const n of ["1×","2×"])
  check(html.includes(">" + n + "<"), "legenda wymienia sygnal " + n);
check(/rusz wieczkiem/.test(html), "jedyny etap z Twoim udzialem jest opisany");
check(/możesz odłożyć/.test(html), "i wiadomo, kiedy mozna odlozyc pudelko");
check(/Potrzebny jesteś tylko na początku/.test(html), "zasada wyjasniona");
check(/koniec testu/.test(html), "sygnatura konca opisana");
check(/nie pojawi się poniżej/.test(html),
      "brak sieci wprost tlumaczy, czemu wyniku nie widac");

head("Historia pudelka (czarna skrzynka)");
const bl = document.getElementById("boxLog");

D({ boxLog: [] });
A.renderBoxLog();
check(/nie przysłało jeszcze historii/.test(bl.innerHTML), "brak historii -> czytelny tekst");

/* Format linii ustala firmware: czas|powod|co sie stalo|bateria|kolejka */
D({ boxLog: [
  `${t0800}|KONTAKTRON (otwarto pudelko)|wyslane|82|0`,
  `${t0800 + 3600}|timer|brak wifi|81|3`,
  `${t0800 + 7200}|timer|meldunek OK|81|0`
]});
A.renderBoxLog();
check(/otwarcie pude\u0142ka/.test(bl.innerHTML), "powod tlumaczony na polski");
check(/08:00/.test(bl.innerHTML), "pokazana godzina wybudzenia");
check(/bateria 82%/.test(bl.innerHTML), "pokazany stan baterii");
check(/3 w kolejce/.test(bl.innerHTML), "widac zalegle zdarzenia");
check(bl.innerHTML.indexOf("meldunek OK") < bl.innerHTML.indexOf("brak wifi"),
      "najnowsze wpisy na gorze");

/* Nieudane wybudzenia musza sie odrozniac kolorem - o to w tym chodzi. */
const czerwone = (bl.innerHTML.match(/var\(--bad\)/g) || []).length;
const zielone  = (bl.innerHTML.match(/var\(--ok\)/g)  || []).length;
check(czerwone === 1, "jeden wpis oznaczony jako nieudany (" + czerwone + ")");
check(zielone === 2, "dwa jako udane (" + zielone + ")");

/* Pudelko bez zegara zapisuje czas 0 - nie moze to dac "1970". */
D({ boxLog: [`0|reset/wlaczenie|start|100|0`] });
A.renderBoxLog();
check(/czas nieznany/.test(bl.innerHTML), "brak czasu opisany wprost");
check(!/1970/.test(bl.innerHTML), "i bez daty z 1970 roku");

check(html.includes('id="boxLog"'), "karta historii w Ustawieniach");
check(html.includes(`devices/${"${DEVICE_ID}"}/log`), "aplikacja czyta historie z bazy");

head("Tabletka jako bryla");
D();
{
  const b = A.tablet3D(1, 56, true);
  const paskow = (b.match(/<i style/g) || []).length;
  check(paskow === A.TAB3D_SEGMENTOW,
        `wieniec ma ${A.TAB3D_SEGMENTOW} paskow (jest ${paskow})`);
  check((b.match(/<svg/g) || []).length === 2, "awers i rewers, nie jedna scianka");
  check(/rotateY\(0.00deg\)/.test(b) && /rotateY\(350.00deg\)/.test(b),
        "paski rozlozone na pelnym obrocie");
  /* Tabletka ma wygladac na TABLETKE, a nie na monete - grubosc rzedu
     jednej osmej srednicy daje ten efekt.                             */
  const gr = +(/--gr:(\d+)px/.exec(b) || [0,0])[1];
  check(gr >= 6 && gr <= 9, `polowa grubosci to ${gr}px przy srednicy 56`);
  check(b.includes("translateZ(28.00px)"), "paski nadal na promieniu");
  check(b.includes("kreci"), "wzieta dawka sie obraca");
  check(!A.tablet3D(0, 56, false).includes("kreci"), "niewzieta stoi w miejscu");
  check(b.includes('onclick="pchnijTabletke(this)"'), "reaguje na dotkniecie");

  /* Bez cieniowania wieniec wyglada jak plaska obrecz i bryla sie rozsypuje. */
  const kolory = new Set((b.match(/--kol:rgb\([^)]*\)/g) || []));
  check(kolory.size > 10, `paski roznie cieniowane (${kolory.size} odcieni)`);
}

check(A.cieniuj("#d9799c", 1) === "rgb(217,121,156)", "mnoznik 1 nie zmienia koloru");
check(A.cieniuj("#d9799c", 0.5) === "rgb(109,61,78)", "polowa przyciemnia");
check(A.cieniuj("#ffffff", 2) === "rgb(255,255,255)", "nie przekracza 255");
check(A.cieniuj("nie-kolor", 0.5) === "nie-kolor", "smieciowy kolor nie wywala funkcji");

check(css.includes("transform-style:preserve-3d"), "bryla faktycznie w 3D");
check(css.includes("perspective:520px"), "z perspektywa, nie w rzucie plaskim");
check(/backface-visibility:hidden/.test(css), "tylnia scianka nie przeswituje przez przednia");
check(/@keyframes obrotTabletki/.test(css), "obrot wokol wlasnej osi");
check(/rotateX\(-16deg\)/.test(css), "krazek przechylony, zeby bylo widac grubosc");
check(/prefers-reduced-motion[\s\S]{0,220}animation:none/.test(css),
      "przy systemowym ograniczeniu animacji tabletka stoi");

head("Stale polaczenie z baza");
/* Ekran pokazujacy stan sprzed godziny wyglada dokladnie tak samo jak
   aktualny. Dlatego stan polaczenia musi byc WIDOCZNY, a zerwanie -
   prostowane samo, bez czekania na to, az ktos otworzy aplikacje.    */
check(html.includes('id="netDot"'), "kropka stanu polaczenia w naglowku");
check(/\.info\/connected/.test(html), "aplikacja slucha stanu polaczenia");
check(/kropka\.classList\.toggle\("zywe", teraz\)/.test(html),
      "kropka odzwierciedla prawde, a nie zaloznie");
check(/bylPolaczony === false && teraz[\s\S]{0,400}wakeUp\(\)/.test(html),
      "po powrocie polaczenia aplikacja sama dociaga zaleglosci");

/* ── Kolejka zapisow po stronie telefonu ──
   Pudelko ma kolejke w pamieci nieulotnej. Telefon dlugo nie mial nic:
   zapis bez sieci znikal po cichu, bo Firebase w trybie offline nie
   odrzuca obietnicy, tylko trzyma ja w nieskonczonosc.               */
/* ── Podglad ladowania ──
   Na kablu prad jest za darmo, wiec pudelko melduje sie co minute.
   Aplikacja ma to POKAZAC, zeby nie trzeba bylo nic odswiezac.     */
head("Podglad ladowania na zywo");
A.renderStatus({ battery: 62, volt: 3.95, lastSeen: Math.floor(Date.now()/1000), charging: true });
check(/Ładowanie/.test(document.getElementById("batVolt").textContent),
      "przy ladowaniu karta pisze wprost, ze trwa ladowanie");
/* Napiecie przy podlaczonym kablu to napiecie LADOWARKI. Pokazanie go
   jako stanu ogniwa byloby klamstwem - i to takim, ktore znika dopiero
   po odlaczeniu, wiec nie do wychwycenia na oko.                     */
check(!/3\.95 V/.test(document.getElementById("batVolt").textContent),
      "nie pokazujemy napiecia ladowarki jako napiecia ogniwa");
check(/nie da się zmierzyć/.test(document.getElementById("batEta").textContent),
      "uzytkownik wie, dlaczego procent stoi w miejscu");
check(/ładuje/.test(document.getElementById("batChip").textContent),
      "znacznik w naglowku tez to pokazuje");
A.renderStatus({ battery: 62, volt: 3.95, lastSeen: Math.floor(Date.now()/1000) });
check(/3\.95 V · ogniwo 470 mAh/.test(document.getElementById("batVolt").textContent),
      "bez ladowania karta wraca do prawdziwego napiecia");
check(!/ładuje/.test(document.getElementById("batChip").textContent),
      "i znacznik gasnie");
A.renderStatus({ battery: 62, volt: 3.95, charging: "tak" });
check(!/ładowanie w toku/.test(document.getElementById("batVolt").textContent),
      "smiec w polu charging nie zapala wskaznika");
check(/@keyframes ladowanie/.test(html) && /\.laduje\{animation/.test(html),
      "pasek pulsuje podczas ladowania");

/* ── Szacunek czasu ladowania ──
   Liczba ma byc ostrozna i monotoniczna. Zaniżony szacunek jest gorszy
   niz zaden: co pol godziny "jeszcze 10 minut" wyglada jak awaria.  */
/* ── Ile realnie starcza jedno ladowanie ──
   Deklarowane "trzy miesiace" to wyliczenie z poboru pradu. Ta linijka
   pokazuje, ile wychodzi naprawde - wiec musi byc odporna na smieci. */
head("Dni od ostatniego ladowania");
{
  const teraz = Math.floor(Date.now()/1000), doba = 86400;
  check(/nie zanotowano/.test(A.opisLadowan({})), "bez danych mowimy wprost, ze ich nie ma");
  check(/12 dni/.test(A.opisLadowan({ lastCharge: teraz - 12*doba })),
        "liczy dni od ostatniego ladowania");
  check(/1 dzień/.test(A.opisLadowan({ lastCharge: teraz - doba })),
        "jeden dzien odmieniony poprawnie");
  check(/starczyło na 74 dni/.test(
          A.opisLadowan({ lastCharge: teraz - 5*doba, prevCharge: teraz - 79*doba })),
        "pokazuje, ile starczylo poprzednim razem");
  check(A.opisLadowan({ lastCharge: teraz + 99*doba }) === "",
        "data z przyszlosci nie jest pokazywana");
  check(!/starczyło/.test(A.opisLadowan({ lastCharge: teraz - doba, prevCharge: teraz })),
        "poprzednie ladowanie pozniejsze niz ostatnie to smiec - pomijamy");
  check(!/NaN|undefined/.test(A.opisLadowan({ lastCharge: "wczoraj", prevCharge: {} })),
        "tekst zamiast daty nie produkuje NaN");
}

head("Odliczanie do pelna");
check(A.minutyDoPelna(100) === 0, "pelne ogniwo nie potrzebuje czasu");
check(A.minutyDoPelna(0) > A.minutyDoPelna(50), "z zera trwa dluzej niz z polowy");
check(A.minutyDoPelna(50) > A.minutyDoPelna(90), "z polowy dluzej niz z 90%");
let malejaco = true;
for (let p = 0; p < 100; p++) if (A.minutyDoPelna(p) < A.minutyDoPelna(p+1)) malejaco = false;
check(malejaco, "szacunek maleje monotonicznie z kazdym procentem");
check(A.minutyDoPelna(0) > 90 && A.minutyDoPelna(0) < 240,
      `pelne ladowanie w rozsadnym zakresie (${A.minutyDoPelna(0)} min)`);
/* Ostatnie procenty musza kosztowac WIECEJ niz srodkowe - inaczej wzor
   jest liniowy i konczy sie tym, ze "5 minut" trwa pol godziny.     */
check(A.minutyDoPelna(90) - A.minutyDoPelna(95) > A.minutyDoPelna(50) - A.minutyDoPelna(55),
      "faza koncowa liczona jako wolniejsza, bo taka jest");
check(A.minutyDoPelna(-50) === A.minutyDoPelna(0), "bzdurny procent nie wywraca wzoru");
check(A.minutyDoPelna(999) === 0, "ani z drugiej strony");

const teraz = Math.floor(Date.now()/1000);
check(A.opisLadowania({}) === null, "bez danych nie zgadujemy");
check(A.opisLadowania({ chargeSince: teraz, chargeFromPct: -1 }) === null,
      "nieznany punkt startowy tez nie");
check(/do pełna/.test(A.opisLadowania({ chargeSince: teraz, chargeFromPct: 60 })),
      "przy komplecie danych podajemy szacunek");
check(/Powinno być naładowane/.test(
        A.opisLadowania({ chargeSince: teraz - 99999, chargeFromPct: 60 })),
      "po przekroczeniu szacunku nie liczymy w minus");
A.renderStatus({ battery: 62, volt: 4.19, charging: true, chargeSince: teraz, chargeFromPct: 62 });
check(/do pełna/.test(document.getElementById("batEta").textContent),
      "odliczanie widac na karcie baterii");

/* ── Tabletka na ekranie glownym ── */
/* ── Siatka bezpieczenstwa uzupelniania ──
   Dawka, ktora nie trafila do kalendarza, wyglada jak zapomniana
   tabletka. To najgrozniejszy rodzaj cichej awarii w tym projekcie. */
head("Zdarzenie bez pokrycia w kalendarzu");
{
  const dzis = A.todayKey();
  const ts = Math.floor(Date.now()/1000) - 3600;
  D({ events:[{ ts, type:"open", slot:0 }], doses:{} });
  check(A.brakujePokrycia() === true, "otwarcie bez wpisu w kalendarzu jest wykrywane");

  D({ events:[{ ts, type:"open", slot:0 }],
      doses:{ [A.devKey(ts)]: { 0:{ status:"taken", dose:1, source:"device", ts } } } });
  check(A.brakujePokrycia() === false, "gdy wpis juz jest, nic nie zglaszamy");

  D({ events:[{ ts, type:"open", slot:0 }],
      doses:{ [A.devKey(ts)]: { 0:{ status:"missed", dose:0, source:"manual", ts } } } });
  check(A.brakujePokrycia() === false, "reczna korekta nie jest traktowana jak brak");

  D({ events:[{ ts, type:"missed", slot:0 }], doses:{} });
  check(A.brakujePokrycia() === false, "pominiecia nie wymagaja pokrycia");

  D({ events:[], doses:{} });
  check(A.brakujePokrycia() === false, "brak zdarzen to nie jest brak pokrycia");
  D({ events:[{ type:"open" }, { ts:"x", type:"open" }], doses:{} });
  let ocalalo = true;
  try { A.brakujePokrycia(); } catch { ocalalo = false; }
  check(ocalalo, "smieci w zdarzeniach nie wywalaja sprawdzenia");
}
check(/setInterval\([\s\S]{0,200}brakujePokrycia\(\)[\s\S]{0,80}60000\)/.test(html),
      "sprawdzenie chodzi cyklicznie, a nie tylko przy zdarzeniach");
check(/doReconcile\(true\);\s*renderSettings\(\); renderAll\(\);/.test(html),
      "zmiana ustawien tez uruchamia uzupelnianie - i to PRZED rysowaniem");

head("Instrukcja autotestu zgodna z firmware");
check(!/naciśnij raz/.test(html), "instrukcja nie kaze juz sprawdzac przycisku");
check(/rusz wieczkiem/.test(html), "kontaktron nadal wymaga reakcji");
check(/dowodem, że buzzer działa/.test(html),
      "pierwsze pikniecie opisane jako dowod dzialania buzzera");
check(!/7×/.test(html), "nie obiecujemy siedmiu piknięc, skoro sa dwa");

head("Tabletka na ekranie glownym");
check(/tabletka\.gif/.test(html), "uzywamy przygotowanego pliku, nie bryly CSS");
check(/width="58" height="58"/.test(html),
      "rozmiar taki sam jak poprzedniej tabletki");
check(/\.tabgif:not\(\.zazyta\)/.test(html),
      "przed wzieciem dawki tabletka wyglada inaczej niz po");

/* ── Cicha porazka zapisu ──
   Automat dziala w tle. Gdy zapis nie przechodzi, kalendarz zostaje pusty
   i wyglada to jak zepsuta synchronizacja - bez slowa wyjasnienia.    */
head("Nieudane uzupelnianie musi byc widoczne");
check(/zapis kalendarza[\s\S]{0,400}toast\(/.test(html),
      "blad zapisu pokazywany takze w trybie automatycznym");

/* ── Walidacja tego, co uzytkownik moze wpisac ──
   Kazdy z tych przypadkow konczyl sie CICHA szkoda: pusta godzina
   rozjezdzala numery przypomnien miedzy apka a pudelkiem, data z
   przyszlosci wylaczala przypomnienie o INR, pusty prompt zerowal
   licznik tabletek, a za duza dawka powodowala odrzucenie CALEGO
   zapisu ustawien z komunikatem "czeka na polaczenie".            */
head("Aplikacja nie wypuszcza danych, ktorych baza albo pudelko nie przyjmie");
check(/filter\(h => \/\^\(\[01\]\\d\|2\[0-3\]\):\[0-5\]\\d\$\/\.test\(h\)\)/.test(html),
      "godziny przypomnien filtrowane do formatu HH:MM");
check(/Podaj przynajmniej jedna godzine przypomnienia|Podaj przynajmniej jedną godzinę przypomnienia/.test(html),
      "brak poprawnej godziny konczy sie komunikatem, a nie pustym harmonogramem");
check(/Math\.min\(10, Math\.max\(0\.5, \+document\.getElementById\("defDose"\)/.test(html),
      "dawka przycinana do zakresu, ktory dopuszczaja reguly bazy (0,5-10)");
check(/max="10"/.test(html), "pole dawki ma gorny limit takze w HTML");
check(/date > dzisiajKey\(\)/.test(html), "data pomiaru INR nie moze byc z przyszlosci");
check(/pole\.max = dzisiajKey\(\)/.test(html), "kalendarz sam nie pozwala wybrac jutra");
check(/String\(v\)\.trim\(\) !== ""/.test(html),
      "pusty prompt nie zeruje licznika tabletek");
check(/id="inrVal"[^>]*max="15"/.test(html),
      "gorna granica INR w polu zgadza sie z regulami bazy (15)");

head("Zakres terapeutyczny INR wybiera sie z LIST, nie wpisuje");
{
  /* Wpisywanie liczby na telefonie to klawiatura, przecinek kontra kropka
     i literowka "25" zamiast "2,5". Lista tego nie dopuszcza.          */
  const el = id => document.getElementById(id);
  A.__setState({ cfg: { schedule:["20:00"], defaultDose:1, tzOffsetMin:120,
                        inrMin:2, inrMax:3, inrEveryDays:21 } });
  A.renderSettings();

  check(/<select id="inrMin"/.test(html), "pole Od to lista, nie input");
  check(/<select id="inrMax"/.test(html), "pole Do to lista, nie input");
  check(el("inrMin").value === "2", `lista Od pokazuje zapisana wartosc (${el("inrMin").value})`);
  check(el("inrMax").value === "3", `lista Do pokazuje zapisana wartosc (${el("inrMax").value})`);

  const kroki = A.inrKrokiZakresu();
  check(kroki[0] === A.INR_ZAKRES_MIN, `lista zaczyna sie od ${A.INR_ZAKRES_MIN}`);
  check(kroki[kroki.length-1] === A.INR_ZAKRES_MAX, `i konczy na ${A.INR_ZAKRES_MAX}`);
  check(kroki.includes(2.5), "2,5 jest na liscie - polowki maja byc mozliwe");
  check(kroki.every(v => Math.abs(v*10 - Math.round(v*10)) < 1e-9),
        "zadna wartosc nie ma ogona zmiennoprzecinkowego (0.1+0.2 != 0.3)");
  check(!kroki.some(v => v < 1 || v > 5), "nic poza 1-5 nie da sie wybrac");

  /* Sedno: odwrocony zakres ma byc NIE DO WYKLIKANIA, a nie lapany
     komunikatem po fakcie. Pusty przedzial oznaczalby, ze kazdy pomiar
     jest poza celem, a pasek celu na wykresie mialby ujemna wysokosc. */
  for (const od of ["1", "2.5", "4.9"]) {
    el("inrMin").value = od;
    globalThis.inrZakresZmieniony();
    const mozliwe = el("inrMax").options.map(o => +o.value);
    check(mozliwe.every(v => v > +od),
          `przy od=${od} lista Do ma wylacznie wartosci wieksze (najmniejsza ${Math.min(...mozliwe)})`);
    check(+el("inrMax").value > +od,
          `przy od=${od} wybrane Do (${el("inrMax").value}) jest wieksze od Od`);
  }

  /* Podniesienie Od ponad aktualne Do musi przesunac Do, a nie zostawic
     zakresu w stanie niemozliwym. */
  el("inrMin").value = "2"; globalThis.inrZakresZmieniony();
  el("inrMax").value = "2.4";
  el("inrMin").value = "3"; globalThis.inrZakresZmieniony();
  check(+el("inrMax").value > 3, `Do podnioslo sie razem z Od (${el("inrMax").value})`);

  const zapisz = async (od, doo) => {
    A.__resetDb();
    A.__setState({ cfg: { schedule:["20:00"], defaultDose:1, tzOffsetMin:120,
                          inrMin:2, inrMax:3 } });
    el("inrMin").value = od; el("inrMax").value = doo;
    await globalThis.saveInrRange();
    const w = A.__db.writes.at(-1)?.val;
    return w ? [w["devices/pillbox01/config/inrMin"], w["devices/pillbox01/config/inrMax"]] : null;
  };
  check(JSON.stringify(await zapisz("2","3")) === "[2,3]", "zakres 2-3 zapisany");
  check(JSON.stringify(await zapisz("2.5","3.5")) === "[2.5,3.5]", "zakres z polowkami zapisany");
  /* Listy tego nie dopuszczaja, ale zapis idzie do bazy medycznej -
     ostatnie sprawdzenie w kodzie ma zostac.                          */
  check(await zapisz("3","2") === null, "odwrocony zakres odrzucony takze w kodzie zapisu");
}

head("Odstep pomiarow INR wybiera sie z listy");
{
  const el = id => document.getElementById(id);
  check(/<select id="inrEvery"/.test(html), "odstep to lista, nie input");

  const dni = A.INR_ODSTEPY.map(o => o[0]);
  check(dni[0] === 0, "pierwsza pozycja wylacza przypominanie");
  check(A.INR_ODSTEPY[0][1] === "wyłączone", "i mowi to wprost, a nie zerem");
  check(dni.includes(21), "domyslne 21 dni jest na liscie");
  check(dni.every(d => d >= 0 && d <= 365), "wszystkie odstepy w rozsadnym zakresie");
  check(new Set(dni).size === dni.length, "bez duplikatow");
  check(dni.every((d, i) => i === 0 || d > dni[i-1]), "posortowane rosnaco");
  check(A.INR_ODSTEPY.every(([, opis]) => opis && opis.length < 20),
        "kazda pozycja ma krotki, ludzki podpis");

  A.__setState({ cfg: { schedule:["20:00"], defaultDose:1, tzOffsetMin:120,
                        inrMin:2, inrMax:3, inrEveryDays:14 } });
  A.renderSettings();
  check(el("inrEvery").value === "14", `lista pokazuje zapisany odstep (${el("inrEvery").value})`);
  check(/Co 14 dni/.test(el("inrEveryHint").textContent),
        `podpowiedz mowi, ile to dni ("${el("inrEveryHint").textContent}")`);

  /* Wartosc spoza listy - np. ustawiona starsza wersja aplikacji - ma
     zostac dopisana, a nie po cichu zmieniona na inna.               */
  A.__setState({ cfg: { inrEveryDays: 17 } });
  A.renderSettings();
  check(el("inrEvery").value === "17",
        `nietypowy odstep z bazy zostaje zachowany (${el("inrEvery").value})`);

  A.__setState({ cfg: { inrEveryDays: 0 } });
  A.renderSettings();
  check(/wyłączone/i.test(el("inrEveryHint").textContent),
        `zero opisane slowem, nie liczba ("${el("inrEveryHint").textContent}")`);
}

/* ── Milczace pudelko ──
   Powstalo z konkretnego zdarzenia: Kuba pojechal na wyjazd, pudelko
   stracilo WiFi w czwartek rano i przez 2,5 dnia nie odezwalo sie ani
   razu. Aplikacja pokazywala przez ten czas kalendarz jak zwykle -
   spokojny i kompletny - bez slowa, ze dane sa sprzed trzech dni.  */
head("Aplikacja mowi, gdy pudelko przestalo sie odzywac");
{
  const godz = h => Math.floor(Date.now()/1000 - h*3600);
  const przy = (h, extra = {}) => {
    A.renderStatus({ lastSeen: godz(h), ...extra });
    return A.ostrzMilczy();
  };

  check(A.MILCZY_PROG_H >= 24 && A.MILCZY_PROG_H <= 48,
        `prog miedzy doba a dwiema (${A.MILCZY_PROG_H} h) - pudelko melduje sie ` +
        `samo co 12 h, wiec krotsza cisza jest normalna`);

  /* Cisza, gdy jest cisza. Ostrzezenie, ktore pali sie zawsze, przestaje
     cokolwiek znaczyc - a to jest jedno z dwoch, ktore NIE sa schowane
     w Diagnostyce (D11).                                              */
  A.renderStatus(null);
  check(A.ostrzMilczy() === "", "brak statusu w ogole nie straszy przy pierwszym uruchomieniu");
  check(przy(0.5) === "", "swiezy kontakt - nic nie pokazujemy");
  check(przy(12)  === "", "12 godzin ciszy to normalny cykl snu");
  check(przy(A.MILCZY_PROG_H - 1) === "", "tuz pod progiem jeszcze cisza");
  A.renderStatus({ lastSeen: 0 });
  check(A.ostrzMilczy() === "", "lastSeen = 0 traktujemy jak brak danych, nie jak rok 1970");

  /* I ostrzezenie, gdy jest o czym mowic. */
  const dzien = przy(A.MILCZY_PROG_H + 1);
  check(dzien !== "", "powyzej progu ostrzezenie sie pojawia");
  check(/milczy od/.test(dzien), "nazywa rzecz po imieniu");
  check(/nie trafi/.test(dzien),
        "i mowi, CO to znaczy: dawki nie sa w kalendarzu");
  check(/hotspot/.test(dzien), "podaje wyjscie, a nie tylko diagnoze");

  /* Scenariusz Kuby: 2,5 dnia bez sieci, trzy dawki czekaja w pudelku. */
  const wyjazd = przy(60, { queued: 3 });
  check(/2 dni/.test(wyjazd), `liczy doby w dol, nie zaokragla w gore (${wyjazd.match(/milczy od [^<]*/)})`);
  check(/3<\/b> zdarzenia/.test(wyjazd),
        "mowi, ile dawek czeka w pudelku - i odmienia to po polsku");

  /* Jedna liczba w calym komunikacie. Wczesniej naglowek liczyl doby
     w dol, a zdanie nizej bralo relTime(), ktore zaokragla - wychodzily
     dwie rozne liczby o tym samym w jednym akapicie.                  */
  const ileRazy = (wyjazd.match(/2 dni/g) || []).length;
  check(ileRazy === 2 && !/3 dni/.test(wyjazd),
        `ten sam okres opisany ta sama liczba (${ileRazy}x "2 dni", brak "3 dni")`);

  /* Dopelniacz: "od doby", nie "od 1 dzien". */
  check(/od doby/.test(dzien) && !/od 1 dzie/.test(dzien),
        `poprawna odmiana przy jednej dobie (${dzien.match(/milczy od [^<]*/)})`);

  /* Liczba mnoga po polsku: 1 zdarzenie, 2-4 zdarzenia, 5+ zdarzen. */
  for (const [n, forma] of [[1,"zdarzenie"], [3,"zdarzenia"], [5,"zdarzeń"], [12,"zdarzeń"]])
    check(new RegExp(`${n}<\\/b> ${forma}`).test(przy(30, { queued: n })),
          `${n} -> "${forma}"`);

  /* Ostrzezenie ma byc na wierzchu, nie w Diagnostyce (D11). */
  A.renderStatus({ lastSeen: godz(60) });
  A.renderOstrzezenia();
  check(/milczy od/.test(document.getElementById("setWarn").innerHTML),
        "trafia do bloku ostrzezen w Ustawieniach, a nie do Diagnostyki");
}

/* ── Zaleglosci, ktore nie schodza z pudelka (B20) ──
   Kuba wrocil z wyjazdu, pudelko zlapalo WiFi w domu, zameldowalo sie -
   i jedyne, co sie zmienilo, to napis "10 zdarzen czeka w pamieci
   pudelka". Dawki z czterech dni nie dojechaly ani wtedy, ani przy
   kolejnych polaczeniach, a aplikacja pisala o tym tym samym spokojnym
   zdaniem co o chwilowym braku zasiegu.

   Dowod, ze to awaria: pudelko wysyla status DOPIERO po probie
   oproznienia kolejki (flushQueue(); pushStatus();). Swiezy status
   z niezerowym `queued` jest wiec zapisem z chwili, w ktorej pudelko
   MIALO polaczenie i mimo to nic nie wyslalo.                         */
head("Aplikacja odroznia brak zasiegu od zatkanej kolejki");
{
  const godz = h => Math.floor(Date.now()/1000 - h*3600);
  const przy = (h, extra = {}) => {
    A.renderStatus({ lastSeen: godz(h), ...extra });
    return A.ostrzZatkana();
  };

  check(A.ZATKANA_SWIEZOSC_H > 4,
        `prog swiezosci (${A.ZATKANA_SWIEZOSC_H} h) wiekszy niz maksymalny ` +
        `backoff pudelka (4 h) - inaczej krzyczelibysmy na pudelko, ktore ` +
        `po prostu jeszcze nie sprobowalo`);
  check(A.ZATKANA_SWIEZOSC_H < A.MILCZY_PROG_H,
        "i mniejszy niz prog milczenia - te dwa ostrzezenia nie moga sie nakladac");

  A.renderStatus(null);
  check(A.ostrzZatkana() === "", "brak statusu nie straszy");
  check(przy(1) === "", "swiezy kontakt z pusta kolejka - cisza");
  check(przy(1, { queued: 0 }) === "", "queued = 0 to nie jest problem");
  check(przy(48, { queued: 10 }) === "",
        "stary kontakt zostawiamy ostrzMilczy() - jedno zdarzenie, jeden komunikat");
  A.renderStatus({ lastSeen: 0, queued: 10 });
  check(A.ostrzZatkana() === "", "lastSeen = 0 to brak danych, nie rok 1970");

  /* Scenariusz Kuby, co do liczby. */
  const zatkana = przy(5, { queued: 10 });
  check(zatkana !== "", "swiezy kontakt + niepusta kolejka = ostrzezenie");
  check(/10<\/b> zdarzeń/.test(zatkana), "podaje liczbe i odmienia ja po polsku");
  check(/nie jest brak zasięgu/.test(zatkana),
        "mowi WPROST, ze to nie jest brak sieci - inaczej powtarzalibysmy klamstwo");
  check(/ręcznie/.test(zatkana), "podaje wyjscie: wpisz dawki recznie");
  check(/firmware/.test(zatkana), "i druga polowe wyjscia: wgraj nowszy firmware");

  /* Oba ostrzezenia naraz byloby sprzeczne: "milczy od 2 dni" i
     "laczylo sie 5 godzin temu" nie moga byc prawda jednoczesnie.   */
  A.renderStatus({ lastSeen: godz(5), queued: 10 });
  check(A.ostrzMilczy() === "" && A.ostrzZatkana() !== "",
        "swiezy kontakt: mowi zatkana, milczy nie");
  A.renderStatus({ lastSeen: godz(60), queued: 10 });
  check(A.ostrzMilczy() !== "" && A.ostrzZatkana() === "",
        "dawny kontakt: mowi milczy, zatkana nie");

  /* Na wierzchu, nie w Diagnostyce - to samo kryterium co D11. */
  A.renderStatus({ lastSeen: godz(3), queued: 7 });
  A.renderOstrzezenia();
  check(/nie schodzą z pudełka/.test(document.getElementById("setWarn").innerHTML),
        "trafia do bloku ostrzezen w Ustawieniach");
}

/* ── Wolne miejsce w pamieci trwalej ──
   Cala pamiec pudelka to jedna partycja 20 kB na kolejke, dziennik
   wieczka, czarna skrzynke i token. Gdy sie zapelni, kazdy z tych
   mechanizmow zaczyna gubic wpisy po cichu. nvsFail mowi, ze to juz
   sie dzieje; nvsFree mowi, ze zaraz zacznie.                        */
head("Pudelko melduje, ile zostalo mu pamieci");
{
  check(A.NVS_MALO > 0 && A.NVS_MALO < 200, `prog "malo" jest sensowny (${A.NVS_MALO})`);
  check(A.nvsMalo({ nvsFree: 5 }) === true,  "5 wolnych wpisow to malo");
  check(A.nvsMalo({ nvsFree: 500 }) === false, "500 to duzo");
  /* Starszy firmware nie przysyla tego pola w ogole. Brak pomiaru NIE
     jest pomiarem rownym zeru - inaczej kazde stare pudelko krzyczaloby
     o pelnej pamieci, ktorej nikt nie zmierzyl.                        */
  check(A.nvsMalo({}) === false, "brak pola = brak pomiaru, nie alarm");
  check(A.nvsMalo({ nvsFree: -1 }) === false, "-1 znaczy 'nie dalo sie odczytac'");

  A.renderStatus({ battery: 80, nvsFree: 3 });
  const s = A.ostrzStraty();
  check(/prawie pełna/.test(s), "prawie pelna pamiec trafia do ostrzezen o utracie danych");
  check(/3 wolnych/.test(s), "z konkretna liczba, nie samym przymiotnikiem");
  A.renderStatus({ battery: 80, nvsFree: 600 });
  check(A.ostrzStraty() === "", "duzo miejsca - cisza");

  /* Diagnostyka pokazuje liczbe zawsze, gdy pudelko ja przysyla. */
  A.renderStatus({ battery: 80, lastSeen: Math.floor(Date.now()/1000), nvsFree: 512 });
  A.renderDiag();
  const info = document.getElementById("devInfo").innerHTML;
  check(/wolnej pamięci: 512/.test(info), "liczba widoczna w Diagnostyce");
  A.renderStatus({ battery: 80, lastSeen: Math.floor(Date.now()/1000) });
  A.renderDiag();
  check(!/wolnej pamięci/.test(document.getElementById("devInfo").innerHTML),
        "starszy firmware bez tego pola nie dostaje pustego wiersza");
}

head("Kolejka zapisow w telefonie");
check(/async function zapiszPewnie\(/.test(html), "istnieje zapis z gwarancja");
check(!/await set\(ref\(db/.test(html) && !/await update\(ref\(db, `devices/.test(html),
      "zaden zapis uzytkownika nie omija kolejki");
check(/Promise\.race\(\[[\s\S]{0,200}setTimeout/.test(html),
      "zapis ma wlasny limit czasu - inaczej offline zawiesilby interfejs");
check(/oczekZapisz\(\[\.\.\.oczekWczytaj\(\), \{ id/.test(html),
      "wpis trafia do kolejki PRZED wyslaniem, nie po");
check(/oczekZapisz\(oczekWczytaj\(\)\.filter\(o => o\.id !== id\)\)/.test(html),
      "z kolejki znika dopiero po potwierdzeniu z bazy");
check(/bylPolaczony === false && teraz[\s\S]{0,200}oczekWyslij\(\)/.test(html),
      "powrot sieci wyzwala doslanie zaleglosci");
check(/oczekWyslij[\s\S]{0,900}break;/.test(html),
      "przy dalszym braku bazy przerywamy, zamiast probowac w kolko");
check(/devices\/\$\{DEVICE_ID\}\/config\/\$\{k\}/.test(html),
      "ustawienia zapisywane polami - ponowienie nie skasuje cudzej zmiany");
check(/localStorage\.setItem\("pillbox-test"/.test(html),
      "brak localStorage nie wywala aplikacji");
check(/oczekIle\(\)/.test(html) && /zapisów czeka/.test(html),
      "zaleglosci sa widoczne w diagnostyce, a nie tylko w konsoli");
check(/bylPolaczony === true && !teraz[\s\S]{0,200}goOnline\(db\)/.test(html),
      "po zerwaniu prosi o natychmiastowy powrot");
check(/devices\/\$\{DEVICE_ID\}\/status`\)\);[\s\S]{0,120}renderStatus/.test(html),
      "lekki dozor samego stanu pudelka");
check(/\}, 30 \* 1000\);/.test(html), "co pol minuty, gdy aplikacja jest widoczna");
check(/@keyframes tetno/.test(css), "zywe polaczenie widac po pulsowaniu");

head("Widac, ze aplikacja pracuje");
/* Sedno: klikniecie, po ktorym nic sie nie dzieje, jest odbierane jako
   awaria - nawet jesli w srodku wszystko idzie dobrze.                 */
check(html.includes('id="topbar"'), "pasek postepu istnieje");
check(html.includes('id="toasts"'), "miejsce na komunikaty istnieje");
check(html.includes('id="refreshIco"'), "ikona odswiezania da sie animowac");
check(/ico\.classList\.toggle\("spin"/.test(html), "ikona kreci sie podczas pracy");
check(/pasek\.classList\.toggle\("on"/.test(html), "pasek postepu wlacza sie podczas pracy");
check(/if \(recznie && zajete > 0\) return;/.test(html),
      "dwuklik nie uruchamia dwoch odswiezen naraz");

{
  const box = document.getElementById("toasts");
  A.toast("Wszystko aktualne", "ok");
  check(box.innerHTML.includes("Wszystko aktualne"), "komunikat sie pokazuje");
  check(box.innerHTML.includes("toast ok"), "z wlasciwym rodzajem");
  A.toast("Cos poszlo nie tak", "err");
  check(box.innerHTML.includes("Cos poszlo nie tak"), "nowy zastepuje poprzedni");
  check(!box.innerHTML.includes("Wszystko aktualne"), "i stary znika");
  check((box.innerHTML.match(/class="toast/g) || []).length === 1,
        "zawsze tylko jeden komunikat naraz");
  A.toast('<img src=x onerror="zle()">', "info");
  check(!box.innerHTML.includes("<img src=x"), "tresc komunikatu tez jest odkazana");
}

check(html.includes("Nowe zdarzenia z pudełka"), "mowi konkretnie, co przyszlo");
check(html.includes("Wszystko aktualne"), "i gdy nic sie nie zmienilo");
check(html.includes("Odświeżono częściowo"), "oraz gdy czesc odczytow padla");

head("Gest pociagniecia w dol");
check(/addEventListener\("touchstart"/.test(html), "reaguje na dotkniecie");
check(/addEventListener\("touchmove"/.test(html),  "i na przeciagniecie");
check(/addEventListener\("touchend"/.test(html),   "i na puszczenie");
check(/window\.scrollY <= 0/.test(html), "tylko przy samej gorze strony");
check(html.includes("Puść, aby odświeżyć"), "podpowiada, kiedy pusci");
check(/passive: true/.test(html), "nasluchy nie blokuja przewijania");

head("Seria dni bez pominiecia");
D({ doses:{ [shift(-1)]:{0:{status:"taken",dose:1}},
            [shift(-2)]:{0:{status:"taken",dose:1}},
            [shift(-3)]:{0:{status:"taken",dose:1}},
            [shift(-4)]:{0:{status:"missed",dose:0}} } });
check(A.seriaDni() === 3, "trzy dni z rzedu (" + A.seriaDni() + ")");

D({ doses:{ [shift(-1)]:{0:{status:"missed",dose:0}} } });
check(A.seriaDni() === 0, "pominiecie wczoraj zeruje serie");

/* Dzis celowo nie liczy sie do serii - dzien jeszcze trwa. */
D({ doses:{ [today]:{0:{status:"taken",dose:1}},
            [shift(-1)]:{0:{status:"taken",dose:1}} } });
check(A.seriaDni() === 1, "dzisiejsza dawka nie zawyza serii (" + A.seriaDni() + ")");

D({ cfg:{ trackingSince: shift(-2) },
    doses:{ [shift(-1)]:{0:{status:"taken",dose:1}},
            [shift(-2)]:{0:{status:"taken",dose:1}} } });
check(A.seriaDni() === 2, "seria zatrzymuje sie na dacie uruchomienia pudelka");

head("Odliczanie do nastepnej dawki");
D({ cfg:{ schedule:["20:00"] } });
const nast = A.doNastepnej();
check(nast !== null, "jest nastepna dawka");
check(nast.h === "20:00", "o wlasciwej godzinie");
check(nast.za > 0 && nast.za <= 1440, `za ${nast.za} min - w granicach doby`);

D({ cfg:{ schedule:[] } });
check(A.doNastepnej() === null, "pusty harmonogram nie wywala odliczania");
D({ cfg:{ schedule:["zle","20:00"] } });
check(A.doNastepnej()?.h === "20:00", "smieciowa godzina jest pomijana");

check(A.opisCzasu(0) === "za chwilę", "zero minut opisane po ludzku");
check(A.opisCzasu(45) === "za 45 min", "minuty");
check(A.opisCzasu(60) === "za 1 h", "rowna godzina bez zerowych minut");
check(A.opisCzasu(135) === "za 2 h 15 min", "godziny i minuty");
check(html.includes("setInterval(() => { if (!document.hidden) renderToday(); }"),
      "odliczanie faktycznie odlicza, a nie stoi");

head("Pierscien skutecznosci");
check(html.includes('id="adhArc"'), "pierscien istnieje");
check(html.includes('id="adhSub"'), "z podpisem ile z ilu dni");
check(/strokeDashoffset/.test(html), "wypelnienie liczone z procentu");
check(/proc >= 90 \? "var\(--ok\)"/.test(html), "kolor zalezy od wyniku");
check(/st !== "future" && st !== "nodata"/.test(html),
      "dni sprzed uruchomienia nie zanizaja skutecznosci miesiaca");

head("Komunikaty zamiast blokujacych okienek");
check(!/alert\(n \?/.test(html), "potwierdzenie uzupelnienia nie blokuje ekranu");
check(html.includes('toast("Zapisano · pudełko pobierze przy najbliższym połączeniu"'),
      "zapis ustawien tez");
check(/alert\("Nie udało się zapisać do bazy/.test(html),
      "ale prawdziwy blad nadal zatrzymuje");

head("Zgodnosc wersji aplikacji");
check(/const APP_VERSION = "([\d.\-]+)"/.test(html), "index.html deklaruje wersje");
const av = html.match(/const APP_VERSION = "([\d.\-]+)"/)?.[1];
const sw = readFileSync(new URL("../sw.js", import.meta.url), "utf8");
const cv = sw.match(/const CACHE = "pillbox-([\d.\-]+)"/)?.[1];
check(av === cv, `wersja w index.html (${av}) zgadza sie z sw.js (${cv})`);

console.log("\n──────────────────────────────────────");
console.log(`  ZALICZONE: ${PASS}    BLEDY: ${FAIL}`);
console.log("──────────────────────────────────────");
process.exit(FAIL ? 1 : 0);
