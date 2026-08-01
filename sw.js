/* Minimalny service worker - wymagany, aby iOS traktowal strone jak PWA.
   Cache'ujemy tylko wlasna powloke; dane Firebase zawsze z sieci.        */
/* WAŻNE: ta wartość musi zgadzać się z APP_VERSION w index.html.
   Jej zmiana to sygnał dla iPhone'a, że jest nowa wersja aplikacji —
   stary cache zostaje wtedy skasowany, a strona sama się przeładuje. */
const CACHE = "pillbox-2026-08-01.4";
const SHELL = ["./", "./index.html", "./manifest.json"];

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

self.addEventListener("fetch", e => {
  const url = new URL(e.request.url);
  if (e.request.method !== "GET") return;
  if (url.origin !== self.location.origin) return;      // Firebase / CDN - bez cache

  e.respondWith(
    fetch(e.request)
      .then(r => {
        const copy = r.clone();
        caches.open(CACHE).then(c => c.put(e.request, copy));
        return r;
      })
      .catch(() => caches.match(e.request).then(r => r || caches.match("./index.html")))
  );
});
