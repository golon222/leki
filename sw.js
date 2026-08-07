/* Minimalny service worker - wymagany, aby iOS traktowal strone jak PWA.
   Cache'ujemy tylko wlasna powloke; dane Firebase zawsze z sieci.        */
/* WAŻNE: ta wartość musi zgadzać się z APP_VERSION w index.html.
   Jej zmiana to sygnał dla iPhone'a, że jest nowa wersja aplikacji —
   stary cache zostaje wtedy skasowany, a strona sama się przeładuje. */
const CACHE = "pillbox-2026-08-07.4";
const SHELL = ["./", "./index.html",
  "./tabletka.gif", "./manifest.json"];

self.addEventListener("install", e => {
  e.waitUntil(caches.open(CACHE).then(c => c.addAll(SHELL)).then(() => self.skipWaiting()));
});

self.addEventListener("activate", e => {
  e.waitUntil(
    caches.keys()
      .then(ks => Promise.all(ks.filter(k => k !== CACHE).map(k => caches.delete(k))))
      .then(() => self.clients.claim())
  );
});

/* Czy tę odpowiedź wolno w ogóle zapamiętać?

   Historia z życia: do repozytorium trafił kiedyś przez pomyłkę kod
   firmware zamiast index.html. Service worker grzecznie go zapamiętał,
   a gdy publikacja strony potem padła, w kółko podawał z pamięci kod
   w C++ zamiast aplikacji. Wyglądało to jak zepsuta aplikacja, choć plik
   na serwerze był już od dawna poprawny.

   Stąd dwa warunki: zapamiętujemy tylko odpowiedzi udane (nie 404, nie
   strony błędu), a stronę tylko wtedy, gdy serwer twierdzi, że to HTML. */
function worthCaching(req, res) {
  if (!res || !res.ok || res.type === "opaque") return false;
  const type = res.headers.get("content-type") || "";
  const isPage = req.mode === "navigate" || new URL(req.url).pathname.endsWith(".html");
  if (isPage && !type.includes("text/html")) return false;
  return true;
}

self.addEventListener("fetch", e => {
  const url = new URL(e.request.url);
  if (e.request.method !== "GET") return;
  if (url.origin !== self.location.origin) return;      // Firebase / CDN - bez cache

  e.respondWith(
    fetch(e.request)
      .then(r => {
        if (worthCaching(e.request, r)) {
          const copy = r.clone();
          caches.open(CACHE).then(c => c.put(e.request, copy));
        }
        return r;
      })
      .catch(() => caches.match(e.request).then(r => r || caches.match("./index.html")))
  );
});

/* Awaryjne wyczyszczenie pamięci podręcznej z poziomu aplikacji.
   Używane przez przycisk „Wyczyść pamięć i przeładuj" w Ustawieniach —
   ratunek, gdy w cache utknie coś, czego nie da się wyprzeć odświeżaniem. */
self.addEventListener("message", e => {
  if (e.data === "wyczysc-cache") {
    e.waitUntil(
      caches.keys()
        .then(ks => Promise.all(ks.map(k => caches.delete(k))))
        .then(() => self.registration.unregister())
        .then(() => self.clients.matchAll())
        .then(cs => cs.forEach(c => c.navigate(c.url)))
    );
  }
});
