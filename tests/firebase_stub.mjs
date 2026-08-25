/* Atrapa Firebase: zapisy trafiaja do zwyklego obiektu, zeby testy
   mogly sprawdzic, CO aplikacja naprawde zapisuje do bazy.

   Dwie rzeczy poza samym zapamietywaniem:

   1. KAZDY zapis jest przepuszczany przez PRAWDZIWY database.rules.json.
      Dzieki temu wszystkie testy aplikacji sprawdzaja przy okazji, czy
      to, co pisza, w ogole przeszloby przez reguly bazy. Pole dolozone
      do dawki albo do zdarzenia wywala test od razu, a nie dopiero
      w telefonie Kuby - gdzie objawia sie cisza.

   2. Da sie kazac jej ZAWIESC albo ZAWISNAC. Bez tego nie da sie
      przetestowac kolejki zapisow, a to wlasnie ona chroni przed
      najgorszym trybem awarii tej aplikacji: ekran pokazuje sukces,
      dane nie docieraja (D1).                                        */
import { wczytajReguly, sprawdzZapis, opiszBledy } from "./rules_engine.mjs";

const REGULY = wczytajReguly();

export const __db = {
  data: {},
  writes: [],
  /* "ok" - zapis przechodzi
     "blad" - baza odrzuca (jak przy zlych regulach)
     "wisi" - obietnica nigdy sie nie rozstrzyga (Firebase offline!) */
  tryb: "ok",
  /* Reguly sprawdzamy domyslnie. Testy, ktore CELOWO pisza smieci,
     moga to na chwile wylaczyc.                                     */
  sprawdzajReguly: true,
  odrzucone: []
};

export function __resetDb(){
  __db.data = {}; __db.writes = []; __db.tryb = "ok";
  __db.sprawdzajReguly = true; __db.odrzucone = [];
}

const setPath = (obj, path, val) => {
  const parts = path.split("/").filter(Boolean);
  let cur = obj;
  for (let i = 0; i < parts.length - 1; i++){
    if (typeof cur[parts[i]] !== "object" || cur[parts[i]] === null) cur[parts[i]] = {};
    cur = cur[parts[i]];
  }
  if (val === undefined) delete cur[parts[parts.length-1]];
  else cur[parts[parts.length-1]] = val;
};

/* Blad w ksztalcie tego, co naprawde rzuca Firebase przy odmowie regul.
   Aplikacja rozroznia "baza odmowila" od "bazy nie ma" po tym wlasnie
   ksztalcie, wiec atrapa musi go odwzorowac, a nie wymyslic wlasny. */
function bladOdmowy(szczegoly){
  const e = new Error("PERMISSION_DENIED: Permission denied — " + szczegoly);
  e.code = "PERMISSION_DENIED";
  return e;
}

/* Zachowanie bazy wedlug __db.tryb.
     "blad"   - baza nieosiagalna (siec, timeout) - NIE odmowa
     "odmowa" - baza osiagalna i odmawia (np. wygasla sesja)
     "wisi"   - Firebase offline: obietnica nigdy sie nie konczy (D1) */
function wedlugTrybu(){
  if (__db.tryb === "wisi")   return new Promise(() => {});
  if (__db.tryb === "odmowa") return Promise.reject(bladOdmowy("tryb testu"));
  if (__db.tryb === "blad")   return Promise.reject(new Error("baza nie odpowiada"));
  return Promise.resolve();
}

function waliduj(path, val){
  if (!__db.sprawdzajReguly) return;
  const w = sprawdzZapis(REGULY, path, val);
  if (!w.ok) {
    __db.odrzucone.push({ path, val, bledy: w.bledy });
    throw bladOdmowy("REGULY ODRZUCILY ZAPIS -> " + opiszBledy(w.bledy));
  }
}

export const initializeApp = () => ({});
export const getAuth = () => ({ currentUser: { uid:"testuid", email:"test@example.com" } });
export const getDatabase = () => ({});
export const signInWithEmailAndPassword = async () => ({});
export const onAuthStateChanged = () => {};
export const signOut = async () => {};
export const setPersistence = async () => {};
export const indexedDBLocalPersistence = { type: "indexedDB" };
export const browserLocalPersistence  = { type: "localStorage" };
export const ref = (_db, path = "") => ({ path });
export const onValue = () => {};
export const goOnline = () => {};
export const get = async (r) => {
  const parts = (r.path || "").split("/").filter(Boolean);
  let cur = __db.data;
  for (const p of parts) cur = (cur && typeof cur === "object") ? cur[p] : undefined;
  return {
    val: () => cur ?? null,
    /* forEach PRZERYWA sie, gdy callback zwroci true - tak dziala prawdziwy
       DataSnapshot.forEach w Firebase. To nie jest szczegol:

           s.forEach(c => events.push(...))

       arrow bez klamer zwraca wynik push(), czyli DLUGOSC tablicy - zawsze
       liczbe >= 1, czyli zawsze true. Petla konczy sie po PIERWSZYM dziecku
       i aplikacja widzi jedno zdarzenie zamiast czterdziestu dwoch (B25).

       Atrapa ignorowala dotad wynik callbacku, wiec ten blad byl dla testow
       niewidzialny - tak samo jak Preferences.begin() dla firmware.       */
    forEach: cb => { if (cur && typeof cur === "object")
                       for (const [k, v] of Object.entries(cur))
                         /* Prawdziwy SDK robi `!!action(...)` - przerywa na KAZDEJ
                            prawdziwej wartosci, nie tylko na dokladnym true.
                            Wlasnie dlatego push() (zwraca dlugosc >= 1) urywa
                            petle po pierwszym dziecku.                      */
                         if (cb({ key:k, val:()=>v })) return true;
                     return false; }
  };
};
export const query = (r) => r;
export const orderByChild = () => ({});
export const limitToLast = () => ({});

/* Firebase SERIALIZUJE wartosc w chwili zapisu. Atrapa trzymala referencje,
   wiec pozniejsza zmiana obiektu w aplikacji po cichu zmieniala to, co
   "lezy w bazie" - a to znaczy, ze test mogl sprawdzac zupelnie co innego,
   niz mysli. Zlapane na kopii zapasowej: `zbierzKopie()` oddaje zywy obiekt
   `doses`, wiec wyczyszczenie kalendarza w tescie oproznialo TAKZE kopie
   zapisana wczesniej w bazie. Ta sama rodzina co Preferences.begin() i
   forEach w D39: atrapa niewierna oryginalowi mierzy nie to, co trzeba. */
const migawka = v => (v === undefined || v === null || typeof v !== "object")
  ? v : structuredClone(v);

export const set = async (r, val) => {
  waliduj(r.path, val);
  await wedlugTrybu();
  __db.writes.push({ op:"set", path:r.path, val });
  setPath(__db.data, r.path, migawka(val));
};
export const remove = async (r) => {
  await wedlugTrybu();
  __db.writes.push({ op:"remove", path:r.path });
  setPath(__db.data, r.path, undefined);
};
export const update = async (r, val) => {
  /* Aktualizacja wielosciezkowa jest w Firebase ATOMOWA: jedno pole
     odrzucone przez reguly kasuje calosc. Dlatego walidujemy wszystko
     PRZED zapisaniem czegokolwiek.                                    */
  for (const [k, v] of Object.entries(val))
    waliduj(r.path ? `${r.path}/${k}` : k, v);
  await wedlugTrybu();
  __db.writes.push({ op:"update", path:r.path, val });
  for (const [k, v] of Object.entries(val)){
    setPath(__db.data, r.path ? `${r.path}/${k}` : k, migawka(v));
  }
};

/* runTransaction() - kluczowe dla D35 (koniec wyscigu w doReconcile).

   Prawdziwy Firebase wola funkcje aktualizujaca z BIEZACA wartoscia z
   SERWERA - nie z lokalnej podrecznej kopii klienta. To wlasnie ta
   roznica zamyka cala rodzine bledow B27/B28: zaden lokalny stan (stary,
   nieaktualny, sprzed odswiezenia) nie moze juz wplynac na wynik.

   Zwracajac `undefined` z funkcji aktualizujacej PORZUCAMY transakcje -
   nic sie nie zapisuje, `committed` wraca `false`. Inaczej niz set/update,
   __db.tryb sprawdzamy DOPIERO PO wywolaniu funkcji aktualizujacej -
   dokladnie jak prawdziwy SDK: brak sieci nie przeszkadza ODCZYTAC
   biezacej wartosci z podrecznej kopii, przeszkadza dopiero PRZY ZAPISIE. */
export const runTransaction = async (r, aktualizuj) => {
  const obecna = (() => {
    const parts = (r.path || "").split("/").filter(Boolean);
    let cur = __db.data;
    for (const p of parts) cur = (cur && typeof cur === "object") ? cur[p] : undefined;
    return cur ?? null;
  })();
  const nowa = aktualizuj(obecna);
  if (nowa === undefined) return { committed: false, snapshot: { val: () => obecna } };
  waliduj(r.path, nowa);
  await wedlugTrybu();
  __db.writes.push({ op:"transaction", path:r.path, val:nowa });
  setPath(__db.data, r.path, migawka(nowa));
  return { committed: true, snapshot: { val: () => nowa } };
};
