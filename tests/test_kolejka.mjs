/* =====================================================================
 *  KOLEJKA ZAPISOW W TELEFONIE  -  testy WYKONUJACE kod, nie czytajace go
 *
 *  Dotad tego obszaru pilnowalo trzynascie wyrazen regularnych po tresci
 *  index.html. Pierwsze z nich sprawdzalo doslownie tyle, ze funkcja
 *  zapiszPewnie() istnieje. Zaden z nich nie uruchomil jej ani razu.
 *
 *  A to jest najgrozniejszy tryb awarii tej aplikacji (D1, zasada 5):
 *  Firebase w trybie offline NIE odrzuca obietnicy - ona po prostu nigdy
 *  sie nie konczy. Ekran pokazuje ptaszka, dane nie docieraja.
 *
 *  Powod, dla ktorego nikt tego nie wykonal, byl techniczny: atrapa bazy
 *  nie umiala zawiesc. Teraz umie - __db.tryb = "blad" | "wisi".
 *
 *      node test_kolejka.mjs
 * ===================================================================== */
import * as A from "./app_module.mjs";

let PASS = 0, FAIL = 0;
const check = (c, m) => c ? PASS++ : (FAIL++, console.log("  FAIL  " + m));
const head  = t => console.log("\n=== " + t + " ===");

/* Limit czasu w zapiszPewnie() to osiem sekund. Czekanie na nie w kazdym
   tescie zamienialoby ten plik w minutowy przestoj, wiec skalujemy zegar.
   Skracamy TYLKO dlugie oczekiwania - krotkie zostaja bez zmian.      */
const prawdziwySetTimeout = globalThis.setTimeout;
globalThis.setTimeout = (fn, ms, ...r) =>
  prawdziwySetTimeout(fn, ms >= 1000 ? 20 : ms, ...r);

/* Cisza w konsoli - testy celowo wywoluja bledy zapisu. */
const bylError = console.error, bylWarn = console.warn;
console.error = () => {}; console.warn = () => {};
const przywrocKonsole = () => { console.error = bylError; console.warn = bylWarn; };

function czysto(tryb = "ok"){
  A.__resetDb();
  A.__db.tryb = tryb;
  A.magazyn.removeItem(A.OCZEK_KEY);
  A.__setState({ cfg: { schedule: ["20:00"], defaultDose: 5, tzOffsetMin: 120 } });
}

const SCIEZKA = "users/testuid/doses/2026-08-01/0";
const DAWKA   = { status: "taken", dose: 5, source: "manual", ts: 1750000000 };
const zapis   = () => ({ [SCIEZKA]: { ...DAWKA } });

/* ═══════════ 1. DROGA SZCZESLIWA ═══════════ */
head("Zapis, ktory przechodzi");
{
  czysto("ok");
  const wynik = await A.zapiszPewnie(zapis(), "Dawka");
  check(wynik === true, "zapiszPewnie zwraca sukces");
  check(A.oczekIle() === 0, `kolejka pusta po potwierdzeniu (jest ${A.oczekIle()})`);
  check(A.__db.data.users?.testuid?.doses?.["2026-08-01"]?.["0"]?.status === "taken",
        "dane naprawde sa w bazie");
}

/* ═══════════ 2. BAZA ODRZUCA ═══════════ */
head("Zapis odrzucony przez baze");
{
  czysto("blad");
  const wynik = await A.zapiszPewnie(zapis(), "Dawka");
  check(wynik === false, "zapiszPewnie NIE udaje sukcesu");
  check(A.oczekIle() === 1, `wpis zostaje w kolejce (jest ${A.oczekIle()})`);
  check(A.oczekWczytaj()[0].opis === "Dawka", "kolejka pamieta, czego dotyczyl zapis");
  check(A.__db.writes.length === 0, "nic nie trafilo do bazy");
}

/* ═══════════ 3. FIREBASE OFFLINE - SEDNO D1 ═══════════ */
head("Firebase offline: obietnica, ktora nigdy sie nie konczy");
{
  czysto("wisi");
  /* Gdyby zabraklo Promise.race, to await zawisloby na zawsze i caly
     interfejs stanalby w miejscu. Test ma wlasny bezpiecznik, zeby
     regresja objawila sie jako BLAD, a nie jako wiszacy proces.      */
  const bezpiecznik = new Promise(r => prawdziwySetTimeout(() => r("ZAWISLO"), 3000));
  const wynik = await Promise.race([A.zapiszPewnie(zapis(), "Dawka"), bezpiecznik]);
  check(wynik === false, `offline konczy sie porazka, nie zawieszeniem (${wynik})`);
  check(A.oczekIle() === 1, "wpis czeka w kolejce na powrot sieci");
}

/* ═══════════ 4. KOLEJNOSC: NAJPIERW KOLEJKA, POTEM WYSYLKA ═══════════ */
head("Wpis trafia do kolejki PRZED wyslaniem");
{
  czysto("wisi");
  const w = A.zapiszPewnie(zapis(), "Dawka");         // celowo bez await
  check(A.oczekIle() === 1,
        "w trakcie wysylki wpis juz jest w kolejce - inaczej zamkniecie " +
        "aplikacji w zlym momencie zgubiloby dawke bez sladu");
  await w;
}

/* ═══════════ 5. DOSLANIE ZALEGLOSCI ═══════════ */
head("Powrot sieci dosyla zaleglosci");
{
  czysto("blad");
  await A.zapiszPewnie(zapis(), "Dawka");
  check(A.oczekIle() === 1, "wpis czeka");
  A.__db.tryb = "ok";
  const ile = await A.oczekWyslij();
  check(ile === 1, `doslano ${ile} zapis`);
  check(A.oczekIle() === 0, "kolejka pusta po doslaniu");
  check(A.__db.data.users.testuid.doses["2026-08-01"]["0"].status === "taken",
        "dane doszly do bazy");
}

head("Doslanie przerywa na pierwszym niepowodzeniu");
{
  czysto("blad");
  await A.zapiszPewnie({ [SCIEZKA]: { ...DAWKA } }, "Dawka 1");
  await A.zapiszPewnie({ ["users/testuid/doses/2026-08-02/0"]: { ...DAWKA } }, "Dawka 2");
  check(A.oczekIle() === 2, "dwa wpisy czekaja");
  const ile = await A.oczekWyslij();
  check(ile === 0, `baza dalej nie odpowiada -> doslano ${ile}`);
  check(A.oczekIle() === 2, "oba wpisy zostaja - nic nie zginelo");
}

head("Czesciowe doslanie: co przeszlo, znika; reszta czeka");
{
  czysto("blad");
  await A.zapiszPewnie({ [SCIEZKA]: { ...DAWKA } }, "Dawka 1");
  await A.zapiszPewnie({ ["users/testuid/doses/2026-08-02/0"]: { ...DAWKA } }, "Dawka 2");
  /* Baza przyjmuje pierwszy zapis i zrywa sie dopiero przy drugim -
     przelaczamy tryb w chwili, gdy pierwszy zapis zostal odnotowany. */
  A.__db.tryb = "ok";
  let n = 0;
  const dopiszOryginalnie = A.__db.writes.push.bind(A.__db.writes);
  A.__db.writes.push = (...a) => {
    const r = dopiszOryginalnie(...a);
    if (++n === 1) A.__db.tryb = "blad";
    return r;
  };
  const ile = await A.oczekWyslij();
  A.__db.writes.push = dopiszOryginalnie;

  check(ile === 1, `doslano dokladnie jeden zapis (${ile})`);
  check(A.oczekIle() === 1, `drugi czeka dalej (w kolejce ${A.oczekIle()})`);
  check(A.oczekWczytaj()[0].opis === "Dawka 2", "w kolejce zostal TEN, ktory nie przeszedl");
}

head("Udany zapis kasuje TYLKO swoj wpis");
{
  /* Kolejka rzadko ma jeden wpis. Gdyby potwierdzenie czyscilo ja cala,
     jeden udany zapis kasowalby cudze zaleglosci - czyli dokladnie te
     dawki, ktore czekaja, bo wczesniej nie doszly.                    */
  czysto("blad");
  await A.zapiszPewnie({ ["users/testuid/doses/2026-08-05/0"]: { ...DAWKA } }, "Zalegla");
  check(A.oczekIle() === 1, "zaleglosc czeka");

  A.__db.tryb = "ok";
  await A.zapiszPewnie({ ["users/testuid/doses/2026-08-06/0"]: { ...DAWKA } }, "Nowa");
  check(A.oczekIle() === 1, `zaleglosc przezyla cudzy udany zapis (w kolejce ${A.oczekIle()})`);
  check(A.oczekWczytaj()[0].opis === "Zalegla", "i to nadal TA sama zaleglosc");
}

head("Rownolegle zapisy nie kasuja sie nawzajem");
{
  czysto("wisi");
  const wiszacy = A.zapiszPewnie({ ["users/testuid/doses/2026-08-07/0"]: { ...DAWKA } }, "Wiszaca");
  A.__db.tryb = "ok";
  await A.zapiszPewnie({ ["users/testuid/doses/2026-08-08/0"]: { ...DAWKA } }, "Szybka");
  check(A.oczekIle() === 1,
        `zapis w locie nie zostal skasowany przez szybszego sasiada (w kolejce ${A.oczekIle()})`);
  check(A.oczekWczytaj()[0].opis === "Wiszaca", "w kolejce zostal ten, ktory jeszcze wisi");
  await wiszacy;
}

/* ═══════════ 6. PONOWIENIE JEST BEZPIECZNE ═══════════ */
head("Ponowienie zapisuje stan, nie zmiane");
{
  czysto("blad");
  await A.zapiszPewnie({ [`devices/pillbox1/config/pillsBase`]: 30 }, "Tabletki");
  A.__db.tryb = "ok";
  await A.oczekWyslij();
  const po1 = A.__db.data.devices.pillbox1.config.pillsBase;
  /* Te same dane wyslane drugi raz nie moga niczego podwoic - dlatego
     w kolejce siedzi WARTOSC DOCELOWA, a nie roznica.                */
  A.oczekZapisz([{ id: "x", ts: Date.now(), opis: "Tabletki",
                   updates: { [`devices/pillbox1/config/pillsBase`]: 30 } }]);
  await A.oczekWyslij();
  check(po1 === 30 && A.__db.data.devices.pillbox1.config.pillsBase === 30,
        `dwukrotne doslanie daje ten sam stan (${po1} -> ${A.__db.data.devices.pillbox1.config.pillsBase})`);
}

/* ═══════════ 7. KOLEJKA PRZEZYWA ZAMKNIECIE APLIKACJI ═══════════ */
head("Kolejka przezywa zamkniecie aplikacji");
{
  czysto("blad");
  await A.zapiszPewnie(zapis(), "Dawka");
  /* Nowy odczyt z magazynu = to samo, co po ponownym otwarciu strony. */
  const zMagazynu = JSON.parse(A.magazyn.getItem(A.OCZEK_KEY));
  check(Array.isArray(zMagazynu) && zMagazynu.length === 1,
        "wpis lezy w pamieci trwalej przegladarki, nie tylko w RAM");
  check(zMagazynu[0].updates[SCIEZKA].status === "taken",
        "razem z pelna trescia zapisu");
}

/* ═══════════ 8. ODPORNOSC MAGAZYNU ═══════════ */
head("Uszkodzony magazyn nie wywala aplikacji");
{
  czysto("ok");
  A.magazyn.setItem(A.OCZEK_KEY, "{to nie jest JSON");
  check(A.oczekIle() === 0, "uszkodzona tresc czytana jako pusta kolejka");
  A.magazyn.setItem(A.OCZEK_KEY, '{"a":1}');
  check(A.oczekIle() === 0, "tresc innego ksztaltu tez nie wywala");
  A.magazyn.setItem(A.OCZEK_KEY, '[null, 5, {"brak":"updates"}, {"updates":{"a":1}}]');
  check(A.oczekIle() === 1, `smieci odfiltrowane, zostaje jeden sensowny wpis (${A.oczekIle()})`);
}

head("Kolejka ma sufit - nie rosnie w nieskonczonosc");
{
  czysto("ok");
  const duzo = [];
  for (let i = 0; i < 260; i++)
    duzo.push({ id: "i" + i, ts: i, opis: "x", updates: { [`users/testuid/inr/2026-01-01`]: { value: 2 } } });
  A.oczekZapisz(duzo);
  check(A.oczekIle() === 200, `sufit 200 wpisow (jest ${A.oczekIle()})`);
  check(A.oczekWczytaj()[0].id === "i60",
        "przy przepelnieniu wypadaja NAJSTARSZE, nie najnowsze");
}

/* ═══════════ 9. DROGI ZAPISU UZYTKOWNIKA ═══════════ */
head("Ustawienia zapisywane polami, nie calym obiektem");
{
  czysto("ok");
  await A.zapiszCfg({ pillsBase: 30, pillsBaseFrom: "2026-08-01" }, "Ustawienia");
  const w = A.__db.writes.at(-1);
  const klucze = Object.keys(w.val);
  check(klucze.length === 2 && klucze.every(k => k.includes("/config/")),
        `kazde pole osobna sciezka (${klucze.join(", ")})`);
  check(!klucze.some(k => k.endsWith("/config")),
        "nikt nie nadpisuje calego config - ponowienie skasowaloby cudza zmiane");
}

/* ═══════════ 10. ZNANE OGRANICZENIE: WPIS TRWALE ODRZUCONY ═══════════
   Zapis, ktorego reguly nie przyjma NIGDY, stoi na czele kolejki i
   blokuje wszystko za soba - dokladnie tak, jak blad B3 po stronie
   pudelka, tylko ze tu nie ma odpowiednika queueDrop(). Test PRZYPINA
   dzisiejsze zachowanie, zeby ewentualna naprawa byla swiadoma.     */
head("Trwale odrzucony wpis blokuje kolejke (znane, B6 w DECYZJE)");
{
  czysto("ok");
  A.__db.sprawdzajReguly = true;
  /* Wpis z polem spoza regul - baza odrzuci go i dzis, i za tydzien. */
  A.oczekZapisz([
    { id: "zly", ts: 1, opis: "Zly", updates: { [SCIEZKA]: { ...DAWKA, lewePole: 1 } } },
    { id: "dobry", ts: 2, opis: "Dobry", updates: { ["users/testuid/doses/2026-08-03/0"]: { ...DAWKA } } }
  ]);
  const ile = await A.oczekWyslij();
  check(ile === 0, `nic nie przeszlo (${ile})`);
  check(A.oczekIle() === 2,
        "poprawny zapis stoi za nieprzyjmowalnym i nie ma jak sie przecisnac");
  check(A.__db.data.users?.testuid?.doses?.["2026-08-03"] === undefined,
        "dobra dawka NIE trafila do bazy - to jest cena tego ograniczenia");
}

przywrocKonsole();
globalThis.setTimeout = prawdziwySetTimeout;

console.log("\n──────────────────────────────────────");
console.log(`  ZALICZONE: ${PASS}    BLEDY: ${FAIL}`);
console.log("──────────────────────────────────────");
process.exit(FAIL ? 1 : 0);
