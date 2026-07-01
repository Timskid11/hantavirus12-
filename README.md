# Hantavirus Risk Assessment — Web App

A C++ backend (REST API) paired with an HTML/CSS/JS frontend that estimates
Hantavirus (HPS) risk from reported symptoms and rodent-exposure history.

## Architecture

```
hantavirus-app/
├── backend/                 C++ REST API
│   ├── Symptom.h             Symptom value type (id, label, phase, weight)
│   ├── Patient.h             Patient's reported symptoms + exposure flags
│   ├── DiagnosisResult.h     Result type returned by the engine
│   ├── DiagnosticEngine.h/.cpp   Symptom catalog + scoring rules (the "brain")
│   ├── main.cpp               HTTP server, wires the engine to two routes
│   └── include/
│       ├── httplib.h          cpp-httplib (header-only HTTP server)
│       └── json.hpp           nlohmann/json (header-only JSON)
└── frontend/                 Static site, calls the API over fetch()
    ├── index.html
    ├── style.css
    └── script.js
```

The backend is plain OOP C++ (no framework beyond the two header-only
libraries above), so it's straightforward to explain line-by-line for a
course assignment. The frontend is intentionally plain HTML/CSS/JS with
no build step — open it and it talks to the API.

## How the scoring works

`DiagnosticEngine` holds a catalog of 11 symptoms split into two phases:

- **Early / prodromal** (fatigue, fever, muscle aches, headache, dizziness,
  chills, abdominal problems) — lower weight, non-specific.
- **Late / cardiopulmonary** (coughing, shortness of breath, chest
  tightness, rapid heartbeat) — higher weight, clinically urgent.

Exposure history (rodent contact, rural/enclosed spaces, disturbing dust
in an enclosed space) adds to the score. Scores map to LOW / MODERATE /
HIGH / CRITICAL, with one override: any late-stage symptom **plus**
confirmed exposure is always escalated to CRITICAL, regardless of raw
score, since HPS can progress to respiratory failure quickly.

## Build & run the backend

Requires a C++17 compiler (g++ or clang++) and pthreads (standard on
Linux/macOS).

```bash
cd backend
g++ -std=c++17 -O2 -I include -pthread main.cpp DiagnosticEngine.cpp -o hantavirus_server
./hantavirus_server
```

You should see:
```
Hantavirus diagnostic API listening on http://localhost:8080
```

### API routes

- `GET /api/symptoms` — returns the symptom catalog as JSON.
- `POST /api/diagnose` — body:
  ```json
  {
    "symptomIds": ["fever", "coughing"],
    "rodentExposure": true,
    "ruralOrEnclosedSpaceExposure": false,
    "recentCleaningOfEnclosedSpace": true
  }
  ```
  returns score, risk level, flagged symptoms, and a recommendation.
- `GET /api/health` — simple liveness check.

## Run the frontend

The frontend is static — no build step. From the `frontend/` folder:

```bash
python3 -m http.server 8081
```

Then open `http://localhost:8081` in a browser, **with the backend
already running on port 8080** (the frontend calls it directly via
`fetch`). CORS is already enabled on the backend for this.

## Deploying the backend on Render

Render doesn't have a native C++ buildpack, so the backend deploys as a
**Docker web service** — a `Dockerfile` is already included.

1. Push this project to a GitHub repo.
2. On Render: **New → Web Service** → connect the repo.
3. Set **Root Directory** to `backend`.
4. Render will auto-detect the `Dockerfile` (Environment: Docker). Leave
   the build/start commands blank — the `CMD` in the Dockerfile handles it.
5. No need to set a `PORT` env var yourself — Render injects one
   automatically at runtime, and `main.cpp` already reads it
   (`std::getenv("PORT")`, falling back to 8080 locally).
6. Deploy. Once live, your API is at `https://<your-service-name>.onrender.com`.
7. Test it: `curl https://<your-service-name>.onrender.com/api/health`

Note: Render's free tier spins the service down after inactivity, so the
first request after idling can take 30–60 seconds to wake up.

## Deploying the frontend on Render

The frontend is static, so use a **Static Site** instead of a web service:

1. On Render: **New → Static Site** → connect the same repo.
2. Set **Root Directory** to `frontend`, leave build command empty,
   publish directory as `.`.
3. Before deploying, edit `frontend/script.js` and change `API_BASE` to
   your backend's Render URL from the step above (not `localhost:8080`).
4. Deploy. Your app is now live end-to-end at the static site's URL.

## Notes for the assignment writeup

- OOP is used throughout: `Symptom`, `Patient`, `DiagnosisResult`, and
  `DiagnosticEngine` are separate classes with single responsibilities.
- The backend is a genuine client/server split — the frontend is a
  separate static site that only talks to the API, never touches C++
  directly.
- This is a student diagnostic aid, not a medical device — the
  frontend footer says so explicitly.
