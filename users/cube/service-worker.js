
//   In Chrome/Edge kun je de PWA requirements omzeilen:

// Ga naar chrome://flags
// Zoek "Insecure origins treated as secure"

const CACHE_NAME = "rubiks-cube-v1";
const urlsToCache = [
  "./",
  "./index.html",
  "./html/index.html",
  "./html/OefCorners.html", 
  "./html/OefEdge.html",
  "./html/template.html",
  "./icons/icon-192.png",
  "./icons/icon-512.png",
  "./css/style.css",
  "./css/template.css", 
  "./js/cube.js",
  "./js/index.js",
  "./js/oefBlind.js",
  "./js/template.js",
  "./manifest.json"
];

// Install event - cache resources
self.addEventListener("install", (event) => {
  console.log('🔧 Service Worker installing...');
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then((cache) => {
        console.log('📦 Caching app shell...');
        return cache.addAll(urlsToCache);
      })
      .then(() => {
        console.log('✅ App shell cached');
        return self.skipWaiting();
      })
      .catch((error) => {
        console.error('❌ Caching failed:', error);
      })
  );
});

// Activate event - clean up old caches
self.addEventListener("activate", (event) => {
  console.log('🚀 Service Worker activating...');
  event.waitUntil(
    caches.keys()
      .then((cacheNames) => {
        return Promise.all(
          cacheNames.map((cacheName) => {
            if (cacheName !== CACHE_NAME) {
              console.log('🗑️ Deleting old cache:', cacheName);
              return caches.delete(cacheName);
            }
          })
        );
      })
      .then(() => {
        console.log('✅ Service Worker activated');
        return self.clients.claim();
      })
  );
});

// Fetch event - serve cached content when offline
self.addEventListener("fetch", (event) => {
  // Skip chrome-extension requests die errors veroorzaken
  if (event.request.url.startsWith('chrome-extension://') || 
      event.request.url.includes('rokt.com') ||
      event.request.url.includes('apps.rokt.com')) {
    return;
  }

  event.respondWith(
    caches.match(event.request)
      .then((response) => {
        if (response) {
          console.log('📱 Serving from cache:', event.request.url);
          return response;
        }
        
        console.log('🌐 Fetching from network:', event.request.url);
        return fetch(event.request)
          .then((response) => {
            // Don't cache non-successful responses
            if (!response || response.status !== 200 || response.type !== 'basic') {
              return response;
            }

            // Skip caching chrome-extension requests
            if (event.request.url.startsWith('chrome-extension://')) {
              return response;
            }

            // Clone the response for caching
            const responseToCache = response.clone();
            caches.open(CACHE_NAME)
              .then((cache) => {
                cache.put(event.request, responseToCache);
              });

            return response;
          });
      })
      .catch((error) => {
        console.error('❌ Fetch failed:', error);
        if (event.request.destination === 'document') {
          return caches.match('./html/index.html');
        }
      })
  );
});