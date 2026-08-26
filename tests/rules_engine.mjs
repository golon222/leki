/* =====================================================================
 *  MINI-SILNIK REGUL FIREBASE
 *
 *  Czyta PRAWDZIWY database.rules.json i mowi, czy dany zapis by przeszedl.
 *
 *  PO CO TO ISTNIEJE
 *  Regula "$other": { ".validate": false } odrzuca CALY wpis, gdy trafi
 *  w nim choc jedno nieznane pole. Ugryzlo to juz raz (pole openTs, D6)
 *  i objawia sie najgorzej jak mozna: zapis znika bez sladu, a po
 *  naprawie B3 zdarzenie odrzucone kodem 400 jest dodatkowo KASOWANE
 *  z kolejki pudelka. Do tej pory jedyna obrona byla notatka w CLAUDE.md
 *  i sprawdzenie, czy piec nazw pol wystepuje gdziekolwiek w pliku regul.
 *
 *  ZAKRES
 *  Obslugujemy dokladnie te konstrukcje, ktorych uzywa nasz plik regul:
 *  hasChildren, isString, isNumber, isBoolean, val(), .length, porownania
 *  oraz && / ||. Wyrazen z auth/data/root NIE liczymy - zamiast tego
 *  sprawdzamy prosciej, czy sciezka w ogole ma nad soba jakies .write
 *  inne niz false. Symulowanie logowania dawaloby falszywe alarmy, a
 *  nie tego tu pilnujemy.
 * ===================================================================== */
import { readFileSync } from "node:fs";

export function wczytajReguly(sciezka = new URL("../database.rules.json", import.meta.url)) {
  return JSON.parse(readFileSync(sciezka, "utf8")).rules;
}

/* Widok danych w takiej postaci, jakiej oczekuja wyrazenia z regul.

   Tablica jest w bazie zwyklym wezlem o kluczach "0", "1", ... - wiec
   hasChildren() musi na niej dzialac. cfg.schedule to wlasnie tablica
   i regula wymaga od niej hasChildren(); traktowanie tablicy jako
   "nie-obiektu" odrzucaloby KAZDY zapis harmonogramu.               */
function newDataZ(val) {
  const obiekt = val !== null && typeof val === "object";
  return {
    val: () => val,
    exists: () => val !== null && val !== undefined,
    isString:  () => typeof val === "string",
    isNumber:  () => typeof val === "number" && Number.isFinite(val),
    isBoolean: () => typeof val === "boolean",
    hasChildren: (lista) => {
      if (!obiekt) return false;
      if (!lista) return Object.keys(val).length > 0;
      return lista.every(k => val[k] !== undefined && val[k] !== null);
    },
    child: (k) => newDataZ(obiekt ? (val[k] ?? null) : null)
  };
}

/* `matches()` NIE ISTNIEJE W JS - i to nie byl szczegol.

   Firebase daje napisom metode `matches(/wzorzec/)`; JavaScript nie.
   Wywolanie leci wiec wyjatkiem, a `ocen()` lapie kazdy wyjatek i
   PRZEPUSZCZA zapis. Skutek: kazda regula z wyrazeniem regularnym byla
   dla testow niewidzialna - przechodzilo przez nia wszystko. Dotyczylo
   to `inrDue` (data pomiaru INR), a od 1.47.2 takze godzin w harmonogramie.

   Ta sama rodzina co atrapa Preferences i `structuredClone` w atrapie
   Firebase (D30, D39, D85): narzedzie lagodniejsze od oryginalu mierzy
   nie to, co trzeba - tyle ze tutaj nie mierzylo NIC.

   Podmieniamy wywolanie na funkcje pomocnicza zamiast opakowywac napis
   w obiekt String: reguly porownuja wartosci przez `===` z literalem
   (`newData.val() === 'usun'`), a obiekt String nigdy nie jest `===`
   napisowi. Lekarstwo bylo by wtedy gorsze od choroby.               */
const WYWOLANIE_MATCHES =
  /((?:newData(?:\.child\((?:'[^']*'|"[^"]*")\))?\.val\(\))|__klucz)\.matches\(\s*(\/(?:\\.|[^\/])+\/[a-z]*)\s*\)/g;

/* `$tag`, `$i`, `$date` - nazwa klucza, pod ktory wlasnie piszemy. Firebase
   udostepnia ja w wyrazeniu pod nazwa wildcardu; my podstawiamy pod nia
   jedna zmienna. Wystarcza, bo `.validate` w tym pliku odwoluje sie zawsze
   do wildcardu WLASNEGO poziomu (wyrazenia siegajace wyzej i tak zawieraja
   auth/root, wiec sa oznaczone jako nieobslugiwane).                    */
const WILDCARD = /\$[A-Za-z_]\w*/g;

function __matches(v, re) { return typeof v === "string" && re.test(v); }

/* Czy po podstawieniach zostalo cos, czego nie umiemy policzyc. Regula
   niepoliczalna PRZEPUSZCZA zapis (patrz `ocen`), wiec cicha luka w tym
   miejscu znaczy "test nie sprawdza tej reguly w ogole" - a dokladnie tak
   przez caly czas dzialalo `matches()`. Lepiej wiedziec.               */
export function zostawiaMatches(wyr) {
  if (typeof wyr !== "string" || !wyr.includes(".matches(")) return false;
  return wyr.replace(WILDCARD, "__klucz").replace(WYWOLANIE_MATCHES, "OK")
            .includes(".matches(");
}

const pamiec = new Map();
function skompiluj(wyr) {
  if (!pamiec.has(wyr)) {
    /* Wyrazenia z naszego pliku sa podzbiorem skladni JS, wiec zamiast
       pisac wlasny parser podstawiamy newData i liczymy wprost.       */
    const js = wyr.replace(WILDCARD, "__klucz")
                  .replace(WYWOLANIE_MATCHES, "__matches($1, $2)");
    pamiec.set(wyr, new Function("newData", "__matches", "__klucz",
                                 `"use strict"; return (${js});`));
  }
  return pamiec.get(wyr);
}

/* Czego ten silnik NIE umie policzyc. Wyrazenia z auth/data/root/now
   wymagalyby symulowania logowania i stanu calej bazy - swiadomie tego
   nie robimy (patrz naglowek).                                        */
const NIEOBSLUGIWANE = /\b(auth|data|root|now)\b/;
export const nieobslugiwane = wyr => typeof wyr === "string" && NIEOBSLUGIWANE.test(wyr);

function ocen(wyr, val, klucz = "") {
  if (wyr === true || wyr === false) return wyr;
  if (typeof wyr !== "string") return true;
  /* PRZEPUSZCZAMY, czego nie umiemy policzyc - zamiast zglaszac blad.

     Wczesniej bylo tu `catch { return false; }`, czyli kazde wyrazenie
     odwolujace sie do auth/data/root konczylo sie "reguly odrzucily
     zapis". Gdyby ktos dopisal takie .validate, testy zaczelyby krzyczec
     na CALKIEM POPRAWNY plik regul, a komunikat wskazywalby na dane
     zamiast na silnik. Zamiast tego przepuszczamy, a test_rules.mjs
     osobno pilnuje, czy w pliku nie pojawilo sie cos, czego nie
     obsluguje - i mowi to wprost.                                    */
  if (nieobslugiwane(wyr)) return true;
  try { return !!skompiluj(wyr)(newDataZ(val), __matches, klucz); }
  catch { return true; }
}

/* Ktory wezel regul odpowiada danemu kluczowi.
   Firebase: dokladne dopasowanie wygrywa, inaczej jedyny $wildcard
   na tym poziomie (nazwa $other nie ma zadnego specjalnego znaczenia). */
function wezelDla(reguly, klucz) {
  if (!reguly || typeof reguly !== "object") return null;
  if (Object.prototype.hasOwnProperty.call(reguly, klucz)) return reguly[klucz];
  const dziki = Object.keys(reguly).find(k => k.startsWith("$"));
  return dziki ? reguly[dziki] : null;
}

function rozbij(sciezka) {
  return String(sciezka).split("/").filter(Boolean);
}

/* Czy nad ta sciezka wisi jakiekolwiek prawo zapisu.
   Reguly Firebase kaskaduja w dol: wystarczy JEDEN wezel po drodze. */
function wolnoPisac(reguly, czesci) {
  let wezel = reguly, mozna = false;
  if (wezel[".write"] !== undefined && wezel[".write"] !== false) mozna = true;
  for (const c of czesci) {
    wezel = wezelDla(wezel, c);
    if (!wezel) break;
    if (wezel[".write"] !== undefined && wezel[".write"] !== false) mozna = true;
  }
  return mozna;
}

/* Rekurencyjne sprawdzenie .validate na wezle i wszystkich dzieciach. */
function sprawdzWezel(wezel, val, sciezka, bledy, klucz = "") {
  if (val === null || val === undefined) return;      // kasowanie nie jest walidowane

  if (wezel && wezel[".validate"] !== undefined) {
    if (!ocen(wezel[".validate"], val, klucz)) {
      bledy.push({
        sciezka,
        powod: wezel[".validate"] === false
          ? "pole nieznane regulom ($other: false) - baza odrzuci CALY wpis"
          : `.validate nie przeszlo: ${wezel[".validate"]}`,
        wartosc: val
      });
      return;                       // dalej w glab nie ma sensu
    }
  }

  if (val !== null && typeof val === "object") {
    for (const [k, v] of Object.entries(val)) {
      sprawdzWezel(wezelDla(wezel, k), v, `${sciezka}/${k}`, bledy, k);
    }
  }
}

/* Glowne wejscie: czy zapis wartosci pod sciezka przeszedlby przez reguly.
   Zwraca { ok, bledy: [{sciezka, powod, wartosc}] }.                  */
export function sprawdzZapis(reguly, sciezka, wartosc) {
  const czesci = rozbij(sciezka);
  const bledy = [];

  if (wartosc !== null && wartosc !== undefined && !wolnoPisac(reguly, czesci))
    bledy.push({ sciezka, powod: "zadna regula nie daje prawa zapisu pod ta sciezke", wartosc });

  /* Zejdz regulami do miejsca zapisu. Po drodze rowniez trzeba sprawdzic
     .validate rodzicow - Firebase liczy je dla kazdego wezla, ktorego
     dane sie zmieniaja. Rodzicow bez pelnych danych nie da sie tu
     wiarygodnie ocenic (np. hasChildren na czesciowym stanie), wiec
     zaczynamy walidacje dokladnie od wezla, w ktory piszemy.          */
  let wezel = reguly;
  for (const c of czesci) {
    wezel = wezelDla(wezel, c);
    if (!wezel) break;
  }

  sprawdzWezel(wezel, wartosc, "/" + czesci.join("/"), bledy,
               czesci[czesci.length - 1] || "");
  return { ok: bledy.length === 0, bledy };
}

/* Wygodny opis bledu do komunikatu wyjatku. */
export function opiszBledy(bledy) {
  return bledy.map(b => `${b.sciezka}: ${b.powod}`).join("; ");
}
