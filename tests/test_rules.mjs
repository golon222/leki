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
import { wczytajReguly, sprawdzZapis, opiszBledy, nieobslugiwane,
         zostawiaMatches } from "./rules_engine.mjs";
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

/* ═══════════ 0. SKLADNIA, KTORA PRZYJMIE FIREBASE ═══════════

   TO SIE NAPRAWDE STALO, 2026-08-12. W regulach dla `otaCmd` stanelo
   `"$other": false` zamiast `"$other": { ".validate": false }`. Plik byl
   poprawnym JSON-em, silnik testowy przepuscil to bez slowa, wszystkie
   testy swiecily na zielono - a Firebase Console odmowil zapisu regul:
   "Line 82: Expected '{'". Kuba zobaczyl to jako pierwszy, przy wgrywaniu.

   Ta sama lekcja co przy atrapie NVS (D30): narzedzie lagodniejsze od
   oryginalu mierzy co innego, niz sie mysli. Silnik regul sprawdza, CO
   przechodzi przez reguly - nie sprawdzal, czy same reguly sa w ogole
   regulami.

   Zasada skladni Firebase jest prosta: kazdy wezel drzewa to OBIEKT.
   Wartosc skalarna wolno miec wylacznie kluczom z kropka na poczatku
   (.validate, .read, .write, .indexOn) - to sa dyrektywy, nie wezly.  */
head("Reguly sa skladniowo tym, co Firebase przyjmie");
{
  const surowe = JSON.parse(
    readFileSync(new URL("../database.rules.json", import.meta.url), "utf8")
      .replace(/^\s*\/\/.*$/gm, ""));
  const zle = [];
  (function chodz(wezel, sciezka){
    if (wezel === null || typeof wezel !== "object" || Array.isArray(wezel)) return;
    for (const [k, v] of Object.entries(wezel)) {
      const gdzie = sciezka ? `${sciezka}/${k}` : k;
      if (k.startsWith(".")) continue;            // dyrektywa - skalar jest w porzadku
      if (v === null || typeof v !== "object" || Array.isArray(v))
        zle.push(`${gdzie} = ${JSON.stringify(v)}`);
      else chodz(v, gdzie);
    }
  })(surowe, "");
  check(zle.length === 0,
        `wezly regul musza byc obiektami, a nie skalarami — Firebase odrzuci: ${zle.join(", ")}`);

  /* Osobno wildcardy, bo to na nich sie przewrocilismy i komunikat ma
     wskazywac winowajce po imieniu.                                   */
  const zleWild = zle.filter(s => s.includes("$"));
  check(zleWild.length === 0,
        `wildcard ($cos) musi dostac obiekt z regulami: ${zleWild.join(", ")}`);
}

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

/* GODZINA PRZYPOMNIENIA MUSI BYC GODZINA. Regula widziala dotad tylko
   "tablica ma dzieci", wiec "25:00" albo "aa:bb" przechodzilo. Pudelko
   liczy godzine przez toInt(): "25:00" to minuta 1500, a `matchSlot()`
   liczy wtedy roznice przez polnoc na MINUS i taka pozycja bije kazda
   poprawna godzine - przy "20:00|35:00" pol doby bylo "pora leku".
   Aplikacja czegos takiego nie zapisze, ale regula jest drzwiami, przez
   ktore wchodzi tez konsola Firebase i kazdy inny klient.            */
ok("harmonogram z jedna godzina",        "devices/pillbox1/config/schedule", ["08:00"]);
ok("harmonogram z dwiema",               "devices/pillbox1/config/schedule", ["08:00","20:00"]);
ok("polnoc i ostatnia minuta doby",      "devices/pillbox1/config/schedule", ["00:00","23:59"]);
odrzuc("godzina spoza doby",             "devices/pillbox1/config/schedule", ["25:00"]);
odrzuc("minuta spoza godziny",           "devices/pillbox1/config/schedule", ["08:99"]);
odrzuc("napis, ktory nie jest godzina",  "devices/pillbox1/config/schedule", ["aa:bb"]);
odrzuc("godzina bez zera wiodacego",     "devices/pillbox1/config/schedule", ["8:00"]);
odrzuc("pusta pozycja w harmonogramie",  "devices/pillbox1/config/schedule", ["20:00",""]);
odrzuc("liczba zamiast godziny",         "devices/pillbox1/config/schedule", [800]);
odrzuc("zapis poza drzewem regul",       "cokolwiek/tam", { a: 1 });
odrzuc("notatka dluzsza niz limit",
       "users/u1/doses/2026-08-01/0", { status: "taken", note: "x".repeat(301) });

/* Zmienna dawka. Zero jest DOZWOLONE (dzien bez leku, np. przed zabiegiem),
   ale tylko w rozpisaniu - dawka standardowa zero nie ma sensu i dalej jest
   odrzucana wyzej przez regule defaultDose.                              */
head("Rozpisanie tygodniowe i wyjatki na dzien");
odrzuc("schemat tygodniowy bez wszystkich dni",
       "devices/pillbox1/config/doseWeek", [1,1,1,1,1,1]);
odrzuc("dzien schematu jako napis",
       "devices/pillbox1/config/doseWeek", [1,1,"pol",1,1,1,1]);
odrzuc("dzien schematu ponad limit",
       "devices/pillbox1/config/doseWeek", [1,1,1,1,1,1,11]);
odrzuc("dzien schematu ujemny",
       "devices/pillbox1/config/doseWeek", [1,1,1,1,1,1,-1]);
odrzuc("wyjatek jako napis",  "devices/pillbox1/config/doseDays/2026-08-14", "0");
odrzuc("wyjatek ujemny",      "devices/pillbox1/config/doseDays/2026-08-14", -0.5);
odrzuc("wyjatek ponad limit", "devices/pillbox1/config/doseDays/2026-08-14", 11);

/* Skrzynka nadawcza na siec WiFi dodana z telefonu. Haslo lezy w bazie
   tylko do najblizszego polaczenia - pudelko kasuje je po zapisaniu. */
head("Siec WiFi dodawana z aplikacji");
odrzuc("siec bez nazwy",     "devices/pillbox1/config/wifiNowa", { pass: "tajne123" });
odrzuc("pusta nazwa sieci",  "devices/pillbox1/config/wifiNowa", { ssid: "", pass: "x" });
odrzuc("nazwa ponad 32 znaki",
       "devices/pillbox1/config/wifiNowa", { ssid: "s".repeat(33) });
odrzuc("haslo ponad limit WPA2",
       "devices/pillbox1/config/wifiNowa", { ssid: "dom", pass: "x".repeat(64) });
odrzuc("nazwa sieci jako liczba", "devices/pillbox1/config/wifiNowa", { ssid: 5 });

/* Polecenia na liscie sieci: usun / uzywaj tej. Bez hasla - aplikacja mowi
   CO zrobic, a nie podaje poswiadczen. */
odrzuc("polecenie bez akcji",  "devices/pillbox1/config/wifiCmd", { ssid: "dom" });
odrzuc("polecenie bez sieci",  "devices/pillbox1/config/wifiCmd", { akcja: "usun" });
odrzuc("akcja spoza slownika", "devices/pillbox1/config/wifiCmd",
       { akcja: "sformatuj", ssid: "dom" });
odrzuc("pusta nazwa w poleceniu", "devices/pillbox1/config/wifiCmd",
       { akcja: "usun", ssid: "" });

head("Silnik regul przepuszcza to, co poprawne");
ok("usuniecie sieci",  "devices/pillbox1/config/wifiCmd",
   { akcja: "usun", ssid: "hotspot Kuby", ts: 1750000000 });
ok("przelaczenie sieci", "devices/pillbox1/config/wifiCmd",
   { akcja: "priorytet", ssid: "dom", ts: 1750000000 });
ok("siec z aplikacji",       "devices/pillbox1/config/wifiNowa",
   { ssid: "hotspot Kuby", pass: "tajnehaslo" });
ok("siec otwarta, bez hasla", "devices/pillbox1/config/wifiNowa", { ssid: "hotel-wifi" });
ok("rozpisanie tygodniowe",  "devices/pillbox1/config/doseWeek", [1,1,1,0.5,1,1,1.5]);
ok("dzien bez leku w schemacie", "devices/pillbox1/config/doseWeek", [0,1,1,1,1,1,1]);
ok("wyjatek na konkretny dzien", "devices/pillbox1/config/doseDays/2026-08-14", 0.5);
ok("odstawienie przed zabiegiem", "devices/pillbox1/config/doseDays/2026-08-14", 0);
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
  ssid: "dom", nets: "dom|hotspot Kuby", netMsg: "przyjeta, kasowanie hasla HTTP 200", fw: "1.22.0",
  boots: 12, queued: 0, dropped: 0, nvsFail: 0, nvsFailKey: "",
  nvsFree: 512,
  charging: false, chargeSince: 0, chargeFromPct: -1, lastCharge: 0,
  prevCharge: 0, boxOpen: false, openSince: 0,
  /* Drgniecia styku kontaktronu (D95) - pomiar, po ktorym poznac, czy po
     odfiltrowaniu odbic zostalo jeszcze cos sprzetowego.               */
  reedBounce: 0,
  /* Aktualizacja przez WiFi (D59). otaBad to suma MD5 wersji, ktora sie
     nie uruchomila - 32 znaki albo pusto, nigdy nic pomiedzy.         */
  otaMsg: "za malo baterii - postaw na ladowarke", otaWersja: "1.38.0",
  otaHaslo: true, otaFail: 0, otaBad: "",
  otaMd5: "0123456789abcdef0123456789abcdef", otaProsba: true,
  /* Bot Telegram (D67). Samego TOKENU tu nie ma i nie moze byc - status
     jest jedynym wezlem, ktory pudelko nadpisuje przy kazdym wybudzeniu,
     a token jest sekretem tej samej klasy co haslo do WiFi.           */
  tg: true, tgMsg: "wyslane: 1"
};
check(POLA_STATUSU.every(p => p in PRZYKLAD_STATUSU),
      `pola statusu nieopisane w tescie: ${POLA_STATUSU.filter(p => !(p in PRZYKLAD_STATUSU))}`);
ok("pelny status z pushStatus()", "devices/pillbox1/status", PRZYKLAD_STATUSU);

/* Lista sieci widzianych przez pudelko (D65). Zapis odrzucony przez reguly
   wyglada DOKLADNIE tak samo jak "nic sie nie stalo" - a tego objawu
   szukalismy w tym projekcie przez kilka dni. Stad te testy: sprawdzaja,
   ze prawdziwy ksztalt wysylany przez skanujSieci() przejdzie, i ze
   smiec nie przejdzie.                                                */
head("Lista sieci z pudelka przechodzi przez reguly");
ok("lista z kilkoma sieciami", "devices/pillbox1/scan",
   { ts: 1750000000, nets: { 0: { s: "dom", r: -55 }, 1: { s: "sasiad", r: -82 } } });
ok("pusta lista - pudelko nie widzi nic", "devices/pillbox1/scan", { ts: 1750000000 });
/* DOKLADNIE ten ksztalt wysyla skanujSieci(): ArduinoJson serializuje `nets`
   jako TABLICE, nie obiekt z kluczami. Firebase zapisuje ja jako obiekt
   z indeksami, ale reguly musza przepuscic jedno i drugie - inaczej test
   przechodzilby na ksztalcie, ktorego pudelko nigdy nie wysyla.        */
ok("tablica, tak jak serializuje ja ArduinoJson", "devices/pillbox1/scan",
   { ts: 1750000000, nets: [ { s: "dom", r: -55 }, { s: "sasiad", r: -82 } ] });
/* Hotspot iPhone'a z apostrofem w nazwie. W aplikacji to pulapka na onclick,
   tutaj sprawdzamy tylko, ze reguly go nie odrzuca.                    */
ok("nazwa z apostrofem", "devices/pillbox1/scan",
   { ts: 1750000000, nets: { 0: { s: "Kuba's iPhone", r: -60 } } });
odrzuc("lista bez znacznika czasu", "devices/pillbox1/scan",
       { nets: { 0: { s: "dom", r: -55 } } });
odrzuc("siec bez sily sygnalu", "devices/pillbox1/scan",
       { ts: 1750000000, nets: { 0: { s: "dom" } } });
odrzuc("nazwa dluzsza niz 32 znaki", "devices/pillbox1/scan",
       { ts: 1750000000, nets: { 0: { s: "s".repeat(33), r: -55 } } });
/* Dodatkowe pole odrzuca CALY wpis, a z nim liste, po ktora ktos siegnal -
   ta sama rodzina co D6/D13/D15. Dlatego firmware odsiewa u siebie. */
odrzuc("nieznane pole przy sieci", "devices/pillbox1/scan",
       { ts: 1750000000, nets: { 0: { s: "dom", r: -55, kanal: 6 } } });
odrzuc("sila sygnalu jako napis", "devices/pillbox1/scan",
       { ts: 1750000000, nets: { 0: { s: "dom", r: "-55" } } });

/* Zlecenie skanu z aplikacji. */
ok("prosba o skan", "devices/pillbox1/config/wifiScan", { ts: 1750000000 });
odrzuc("prosba bez znacznika czasu", "devices/pillbox1/config/wifiScan", {});
odrzuc("prosba z doklejonym polem", "devices/pillbox1/config/wifiScan",
       { ts: 1750000000, kto: "kuba" });

/* --- Bot Telegram (D67) ------------------------------------------------
   Token jedzie przez baze tak samo jak haslo do WiFi i tak samo znika po
   odebraniu. Reguly sa tu OSTATNIA linia obrony przed wpisem, ktorego
   pudelko nie zrozumie - a odrzucony wpis wyglada dokladnie tak samo jak
   "nic sie nie stalo", wiec sprawdzamy obie strony.                     */
head("Bot Telegram przechodzi przez reguly");
const TOKEN_OK = "1234567890:AAHdqTcvCH1vGWJxfSeofSAs0K5PALDsaw";
ok("bot z aplikacji", "devices/pillbox1/config/tgNowy",
   { token: TOKEN_OK, chat: "123456789", ts: 1750000000 });
ok("czat grupowy (id ujemne)", "devices/pillbox1/config/tgNowy",
   { token: TOKEN_OK, chat: "-1001234567890" });
odrzuc("bot bez czatu",  "devices/pillbox1/config/tgNowy", { token: TOKEN_OK });
odrzuc("bot bez tokenu", "devices/pillbox1/config/tgNowy", { chat: "123456789" });
odrzuc("token za krotki", "devices/pillbox1/config/tgNowy",
       { token: "123:abc", chat: "123456789" });
odrzuc("token za dlugi", "devices/pillbox1/config/tgNowy",
       { token: "1".repeat(65), chat: "123456789" });
odrzuc("pusty czat", "devices/pillbox1/config/tgNowy",
       { token: TOKEN_OK, chat: "" });
odrzuc("czat jako liczba, nie napis", "devices/pillbox1/config/tgNowy",
       { token: TOKEN_OK, chat: 123456789 });
/* Doklejone pole odrzuca CALY wpis, wiec token nie dojechalby wcale -
   ta sama rodzina bledow co D6/D13/D15.                               */
odrzuc("bot z doklejonym polem", "devices/pillbox1/config/tgNowy",
       { token: TOKEN_OK, chat: "123456789", kto: "kuba" });

ok("odlaczenie bota", "devices/pillbox1/config/tgCmd",
   { akcja: "usun", ts: 1750000000 });
ok("wiadomosc probna", "devices/pillbox1/config/tgCmd",
   { akcja: "test", ts: 1750000000 });
odrzuc("polecenie bota bez akcji", "devices/pillbox1/config/tgCmd", { ts: 1750000000 });
odrzuc("akcja bota spoza slownika", "devices/pillbox1/config/tgCmd",
       { akcja: "wyslij", ts: 1750000000 });
/* Token w POLECENIU bylby wyciekiem bez zadnego zysku: polecenia nie
   znikaja po odebraniu tak jak `tgNowy`, tylko po wykonaniu.          */
odrzuc("token doklejony do polecenia", "devices/pillbox1/config/tgCmd",
       { akcja: "test", token: TOKEN_OK });

/* Historia nieudanych zapisow NVS (D47) - lepiona recznie w nvsFailLogJson(),
   tak jak lidLogJson() ponizej, wiec te same zasady: pola bierzemy z opisu,
   nie z parsowania funkcji, bo wynik to zserializowany String, nie doc[]. */
head("Historia nieudanych zapisow NVS przechodzi przez reguly");
ok("paczka z jednym wpisem", "devices/pillbox1/nvsfaillog/p1",
   { wpisy: { 0: { ts: 1750000000, klucz: "q37" } } });
ok("paczka z kilkoma wpisami", "devices/pillbox1/nvsfaillog/p1",
   { wpisy: { 0: { ts: 1750000000, klucz: "q37" }, 1: { ts: 1750000100, klucz: "tok" } } });
ok("ts=0, gdy zegar byl nieznany", "devices/pillbox1/nvsfaillog/p1",
   { wpisy: { 0: { ts: 0, klucz: "dw" } } });
odrzuc("paczka bez wpisy", "devices/pillbox1/nvsfaillog/p1", { cos: 1 });
odrzuc("wpis bez klucza", "devices/pillbox1/nvsfaillog/p1",
       { wpisy: { 0: { ts: 1750000000 } } });
odrzuc("wpis bez ts", "devices/pillbox1/nvsfaillog/p1",
       { wpisy: { 0: { klucz: "q37" } } });
odrzuc("ts ujemny", "devices/pillbox1/nvsfaillog/p1",
       { wpisy: { 0: { ts: -1, klucz: "q37" } } });
odrzuc("nieznane pole w paczce ($other:false)", "devices/pillbox1/nvsfaillog/p1",
       { wpisy: { 0: { ts: 1, klucz: "q37" } }, cokolwiek: 1 });
odrzuc("nieznane pole we wpisie ($other:false)", "devices/pillbox1/nvsfaillog/p1",
       { wpisy: { 0: { ts: 1, klucz: "q37", dodatkowe: 1 } } });

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

  /* `matches()` NIE ISTNIEJE W JS - wywolanie leci wyjatkiem, a `ocen()`
     lapie kazdy wyjatek i PRZEPUSZCZA zapis. Przez to kazda regula
     z wyrazeniem regularnym byla dla testow niewidzialna: przechodzilo
     przez nia wszystko. Silnik podmienia je teraz na wlasna funkcje;
     ta kontrola pilnuje, ze zadna nie zostala niepodmieniona.        */
  const zRegexem = walidacje.filter(w => typeof w === "string" && w.includes(".matches("));
  check(zRegexem.length >= 3,
        `w regulach sa wyrazenia regularne (${zRegexem.length}) - jest czego pilnowac`);
  const nieprzelozone = zRegexem.filter(zostawiaMatches);
  check(nieprzelozone.length === 0,
        `silnik umie policzyc kazde matches() w regulach; nieprzelozone: ` +
        JSON.stringify(nieprzelozone));
}

/* I dowod, ze silnik naprawde LICZY te wyrazenia, a nie tylko przepuszcza. */
head("Wyrazenia regularne w regulach dzialaja");
ok("tag z listy",                    "users/u1/tags/2026-08-01", { abx: true });
odrzuc("tag spoza listy",            "users/u1/tags/2026-08-01", { cokolwiek: true });
odrzuc("termin INR w zlym formacie", "devices/pillbox1/config/inrDue", "26-08-2026");
ok("termin INR poprawny",            "devices/pillbox1/config/inrDue", "2026-08-26");

console.log("\n──────────────────────────────────────");
console.log(`  ZALICZONE: ${PASS}    BLEDY: ${FAIL}`);
console.log("──────────────────────────────────────");
process.exit(FAIL ? 1 : 0);
