/* Bardzo prosty DOM: tyle, ile potrzeba, zeby funkcje renderujace
   z index.html wykonaly sie bez wyjatkow i zebysmy mogli sprawdzic
   wygenerowany HTML.                                                  */
class El {
  constructor(id){
    this.id = id; this._html = ""; this.textContent = ""; this.value = "";
    this.style = new Proxy({}, { get:(t,k)=>t[k]??"", set:(t,k,v)=>{t[k]=v; return true;} });
    this.className = ""; this.dataset = {}; this.onclick = null;
    this._classes = new Set();
  }
  /* Ustawienie innerHTML na liscie <option> musi zmienic value elementu -
     tak robi prawdziwa przegladarka. Bez tego kod, ktory buduje <select>
     i zaraz potem czyta z niego wartosc, w testach widzi pustke, a w
     telefonie dziala. Roznica objawialaby sie wylacznie u Kuby.       */
  set innerHTML(v){
    this._html = String(v);
    const opcje = [...this._html.matchAll(/<option value="([^"]*)"([^>]*)>/g)];
    if (opcje.length) {
      const zazn = opcje.find(o => /\bselected\b/.test(o[2]));
      this.value = (zazn || opcje[0])[1];
      this._opcje = opcje.map(o => o[1]);
    }
  }
  get innerHTML(){ return this._html; }
  get options(){ return (this._opcje || []).map(v => ({ value: v })); }
  querySelectorAll(){ return []; }
  get classList(){
    const c = this._classes;
    return { add:x=>c.add(x), remove:x=>c.delete(x), toggle:(x,f)=>f?c.add(x):c.delete(x),
             contains:x=>c.has(x) };
  }
  click(){}
}

const els = new Map();
const doc = {
  getElementById(id){
    if (!els.has(id)) els.set(id, new El(id));
    return els.get(id);
  },
  createElement(){ return new El("tmp"); }
};

globalThis.document = doc;
globalThis.window   = globalThis;
globalThis.alert    = () => {};
globalThis.addEventListener = () => {};
globalThis.prompt   = () => null;
/* navigator w Node ma tylko getter - podmieniamy przez defineProperty */
const fakeReg = {
  update: async () => {},
  installing: null,
  addEventListener: () => {}
};
Object.defineProperty(globalThis, "navigator", {
  value: {
    serviceWorker: {
      register: async () => fakeReg,
      addEventListener: () => {},
      controller: null,
      getRegistration: async () => fakeReg
    },
    storage: { persist: async () => true }
  },
  writable: true, configurable: true
});
globalThis.location = { reload: () => {} };
globalThis.setInterval = () => 0;
globalThis.__els = els;
globalThis.__resetEls = () => els.clear();
