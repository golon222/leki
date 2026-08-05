/* Atrapa Firebase: zapisy trafiaja do zwyklego obiektu, zeby testy
   mogly sprawdzic, CO aplikacja naprawde zapisuje do bazy.            */
export const __db = { data: {}, writes: [] };
export function __resetDb(){ __db.data = {}; __db.writes = []; }

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
    forEach: cb => { if (cur && typeof cur === "object")
                       for (const [k, v] of Object.entries(cur)) cb({ key:k, val:()=>v }); }
  };
};
export const query = (r) => r;
export const orderByChild = () => ({});
export const limitToLast = () => ({});

export const set = async (r, val) => {
  __db.writes.push({ op:"set", path:r.path, val });
  setPath(__db.data, r.path, val);
};
export const remove = async (r) => {
  __db.writes.push({ op:"remove", path:r.path });
  setPath(__db.data, r.path, undefined);
};
export const update = async (r, val) => {
  __db.writes.push({ op:"update", path:r.path, val });
  for (const [k, v] of Object.entries(val)){
    setPath(__db.data, r.path ? `${r.path}/${k}` : k, v);
  }
};
