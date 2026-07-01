// Point this at your backend's URL.
// Local dev: http://localhost:8080
// Deployed on Render: something like https://hantavirus-api.onrender.com
const API_BASE = "http://localhost:8080";

const earlyContainer = document.getElementById("early-symptoms");
const lateContainer = document.getElementById("late-symptoms");
const form = document.getElementById("diagnostic-form");
const formError = document.getElementById("form-error");
const caseNumberEl = document.getElementById("case-number");

const resultEmpty = document.getElementById("result-empty");
const resultContent = document.getElementById("result-content");
const stampEl = document.getElementById("stamp");
const stampTextEl = document.getElementById("stamp-text");
const scoreEl = document.getElementById("score-value");
const lateEl = document.getElementById("late-value");
const exposureEl = document.getElementById("exposure-value");
const flaggedListEl = document.getElementById("flagged-list");
const recommendationEl = document.getElementById("recommendation-text");

function setCaseNumber() {
  const now = new Date();
  const stamp = now.toISOString().slice(0, 10).replace(/-/g, "") +
    "-" + String(now.getHours()).padStart(2, "0") + String(now.getMinutes()).padStart(2, "0");
  caseNumberEl.textContent = "NO. " + stamp;
}

function renderChecklist(container, symptoms) {
  container.innerHTML = "";
  symptoms.forEach((s) => {
    const label = document.createElement("label");
    label.className = "check";
    label.innerHTML = `
      <input type="checkbox" value="${s.id}">
      <span>${s.label}</span>
    `;
    container.appendChild(label);
  });
}

async function loadSymptoms() {
  formError.textContent = "";
  try {
    const res = await fetch(`${API_BASE}/api/symptoms`);
    if (!res.ok) throw new Error(`Server responded ${res.status}`);
    const symptoms = await res.json();
    renderChecklist(earlyContainer, symptoms.filter((s) => s.phase === "early"));
    renderChecklist(lateContainer, symptoms.filter((s) => s.phase === "late"));
  } catch (err) {
    formError.textContent =
      "Can't reach the diagnostic server. Start the backend on port 8080, then reload.";
  }
}

function collectSelectedIds(container) {
  return Array.from(container.querySelectorAll('input[type="checkbox"]:checked')).map(
    (el) => el.value
  );
}

form.addEventListener("submit", async (e) => {
  e.preventDefault();
  formError.textContent = "";

  const symptomIds = [
    ...collectSelectedIds(earlyContainer),
    ...collectSelectedIds(lateContainer),
  ];

  const payload = {
    symptomIds,
    rodentExposure: document.getElementById("rodentExposure").checked,
    ruralOrEnclosedSpaceExposure: document.getElementById("ruralOrEnclosedSpaceExposure").checked,
    recentCleaningOfEnclosedSpace: document.getElementById("recentCleaningOfEnclosedSpace").checked,
  };

  try {
    const res = await fetch(`${API_BASE}/api/diagnose`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload),
    });
    if (!res.ok) throw new Error(`Server responded ${res.status}`);
    const result = await res.json();
    renderResult(result);
  } catch (err) {
    formError.textContent =
      "Can't reach the diagnostic server. Start the backend on port 8080 and try again.";
  }
});

function renderResult(result) {
  resultEmpty.hidden = true;
  resultContent.hidden = false;

  stampTextEl.textContent = result.level;
  stampEl.setAttribute("data-level", result.level);

  scoreEl.textContent = result.score;
  lateEl.textContent = result.lateStagePresent ? "YES" : "NO";
  exposureEl.textContent = result.exposureConfirmed ? "YES" : "NO";

  flaggedListEl.innerHTML = "";
  if (result.flaggedSymptoms.length === 0) {
    const li = document.createElement("li");
    li.textContent = "None reported.";
    flaggedListEl.appendChild(li);
  } else {
    result.flaggedSymptoms.forEach((label) => {
      const li = document.createElement("li");
      li.textContent = label;
      flaggedListEl.appendChild(li);
    });
  }

  recommendationEl.textContent = result.recommendation;
}

setCaseNumber();
loadSymptoms();
