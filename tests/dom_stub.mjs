/* Bardzo prosty DOM: tyle, ile potrzeba, zeby funkcje renderujace
   z index.html wykonaly sie bez wyjatkow i zebysmy mogli sprawdzic
   wygenerowany HTML.                                                  */
class El {
  constructor(id){
    this.id = id; this._html = ""; this._text = ""; this.value = "";
    this.style = new Proxy({}, { get:(t,k)=>t[k]??"", set:(t,k,v)=>{t[k]=v; return true;} });
    this.className = ""; this.dataset = {}; this.onclick = null;
    this._classes = new Set();
  }
  /* Ustawienie innerHTML na liscie <option> musi zmienic value elementu -
     tak robi prawdziwa przegladarka. Bez tego kod, ktory buduje <select>
     i zaraz potem czyta z niego wartosc, w testach widzi pustke, a w
     telefonie dziala. Roznica objawialaby sie wylacznie u Kuby.       */
  /* textContent i innerHTML to W PRZEGLADARCE ta sama tresc widziana z dwoch
     stron: ustawienie jednego KASUJE drugie. Atrapa trzymala je jako dwa
     niezalezne pola, wiec test czytajacy innerHTML widzial napis sprzed
     kilku renderow - i dwa razy z rzedu pokazal wynik, ktorego w przegladarce
     by nie bylo. Ta sama lekcja co przy putString (D39): atrapa niewierna
     oryginalowi mierzy inna rzecz niz sie mysli.                        */
  set textContent(v){
    if (this._zepsuty) throw new Error(`test: element ${this.id} nie przyjmuje tresci`);
    this._text = String(v);
    /* Przypisanie do textContent wstawia TEKST, nie znaczniki - wiec odczyt
       przez innerHTML oddaje go zaescapowanego. To nie jest szczegol: na tym
       stoi test pilnujacy, ze tresc od uzytkownika nie moze stac sie kodem. */
    this._html = this._text.replace(/&/g, "&amp;").replace(/</g, "&lt;")
                           .replace(/>/g, "&gt;");
  }
  get textContent(){ return this._text; }

  set innerHTML(v){
    /* Element „zepsuty" - patrz __zepsujEl() na koncu pliku. */
    if (this._zepsuty) throw new Error(`test: element ${this.id} nie przyjmuje tresci`);
    this._html = String(v);
    /* Przyblizenie tekstu widocznego: bez tagow i bez encji HTML. */
    this._text = this._html.replace(/<[^>]*>/g, "")
                           .replace(/&nbsp;/g, " ").replace(/&amp;/g, "&")
                           .replace(/&lt;/g, "<").replace(/&gt;/g, ">")
                           .replace(/&quot;/g, '"').replace(/&#39;/g, "'");
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
    /* add() i remove() w przegladarce przyjmuja WIELE klas naraz. Atrapa
       brala tylko pierwsza, wiec kod czyszczacy cztery stany jednym
       wywolaniem wygladal w tescie na zepsuty, choc dziala. Ta sama lekcja
       co przy putString (D39): atrapa niewierna oryginalowi mierzy inna
       rzecz, niz sie mysli.                                            */
    return { add:(...xs)=>xs.forEach(x=>c.add(x)),
             remove:(...xs)=>xs.forEach(x=>c.delete(x)),
             toggle:(x,f)=>f?c.add(x):c.delete(x),
             contains:x=>c.has(x) };
  }
  click(){}
  /* Puste, ale MUSZA istniec: kod ustawiajacy kursor w polu wola je wprost,
     a brak metody w atrapie wywalilby test bledem, ktory nie ma nic
     wspolnego z tym, co ten test sprawdza. */
  focus(){}
  blur(){}
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
/* Wymuszenie AWARII JEDNEGO EKRANU. W przegladarce render pada zwykle na
   elemencie, ktorego nie ma w tej wersji HTML-a albo na polu, ktorego nie
   przyslalo starsze pudelko. Atrapa nie potrafi zwrocic null (kazde id
   dostaje element), wiec zepsucie robimy wprost: taki element odmawia
   przyjecia tresci. Osloniete rysowanie mozna wtedy sprawdzic na PRAWDZIWYM
   renderAll(), a nie na zastepniku.                                      */
globalThis.__zepsujEl = (id, wlacz = true) => { doc.getElementById(id)._zepsuty = wlacz; };
