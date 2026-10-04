import { initializeApp } from "https://www.gstatic.com/firebasejs/12.18.0/firebase-app.js";
import {
  getAuth,
  onAuthStateChanged,
  signInWithEmailAndPassword,
  signOut
} from "https://www.gstatic.com/firebasejs/12.18.0/firebase-auth.js";

import {
  getDatabase,
  ref,
  onValue,
  set,
  query,
  limitToLast
} from "https://www.gstatic.com/firebasejs/12.18.0/firebase-database.js";


// =====================================================
// FIREBASE
// =====================================================

const firebaseConfig = {
  apiKey: "AIzaSyBdGaWWpdfB1HRfryl2WEiHD868ARz3V0s",
  authDomain: "home-monitoring-7bf0e.firebaseapp.com",
  databaseURL: "https://home-monitoring-7bf0e-default-rtdb.firebaseio.com",
  projectId: "home-monitoring-7bf0e",
  storageBucket: "home-monitoring-7bf0e.firebasestorage.app",
  messagingSenderId: "1000495478045",
  appId: "1:1000495478045:web:8ea2a6a416d93e5d409df4",
  measurementId: "G-0MKL73C70K"
};

const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const db = getDatabase(app);

console.log("Firebase initialized.");


// =====================================================
// ELEMENTS
// =====================================================

const loginView = document.getElementById("loginView");
const appView = document.getElementById("appView");

const loginForm = document.getElementById("loginForm");
const emailInput = document.getElementById("email");
const passwordInput = document.getElementById("password");
const loginError = document.getElementById("loginError");

const logoutBtn = document.getElementById("logoutBtn");
const userEmail = document.getElementById("userEmail");

const temperatureValue =
  document.getElementById("temperatureValue");

const humidityValue =
  document.getElementById("humidityValue");

const ldrValue =
  document.getElementById("ldrValue");

const bulbValue =
  document.getElementById("bulbValue");

const bulbSub =
  document.getElementById("bulbSub");

const modeMini =
  document.getElementById("modeMini");

const manualModeBtn =
  document.getElementById("manualModeBtn");

const autoModeBtn =
  document.getElementById("autoModeBtn");

const bulbOnBtn =
  document.getElementById("bulbOnBtn");

const bulbOffBtn =
  document.getElementById("bulbOffBtn");

const ldrThreshold =
  document.getElementById("ldrThreshold");

const saveThresholdBtn =
  document.getElementById("saveThresholdBtn");

const controlMessage =
  document.getElementById("controlMessage");

const connectionStatus =
  document.getElementById("connectionStatus");

const modeBadge =
  document.getElementById("modeBadge");

const statusBulb =
  document.getElementById("statusBulb");

const lastUpdated =
  document.getElementById("lastUpdated");

const historyBody =
  document.getElementById("historyBody");

const downloadCsvBtn =
  document.getElementById("downloadCsvBtn");


// =====================================================
// LOGIN
// =====================================================

loginForm.addEventListener("submit", async (event) => {

  event.preventDefault();

  const email = emailInput.value.trim();
  const password = passwordInput.value;

  loginError.textContent = "";
  loginError.classList.add("hidden");

  if (!email || !password) {
    showLoginError("Enter your email and password.");
    return;
  }

  const button =
    loginForm.querySelector("button[type='submit']");

  button.disabled = true;
  button.textContent = "Signing in...";

  console.log("Trying Firebase login:", email);

  try {

    const result =
      await signInWithEmailAndPassword(
        auth,
        email,
        password
      );

    console.log(
      "LOGIN SUCCESS:",
      result.user.email
    );

  } catch (error) {

    console.error(
      "FIREBASE LOGIN ERROR:",
      error
    );

    console.error(
      "ERROR CODE:",
      error.code
    );

    showLoginError(
      getFirebaseError(error)
    );

    button.disabled = false;
    button.textContent = "Sign in";
  }
});


// =====================================================
// FIREBASE AUTH STATE
// =====================================================

onAuthStateChanged(auth, (user) => {

  if (user) {

    console.log(
      "Authenticated:",
      user.email
    );

    loginView.classList.add("hidden");
    appView.classList.remove("hidden");

    if (userEmail) {
      userEmail.textContent = user.email;
    }

    startFirebaseListeners();

  } else {

    console.log("Not authenticated.");

    loginView.classList.remove("hidden");
    appView.classList.add("hidden");
  }
});


// =====================================================
// LOGOUT
// =====================================================

logoutBtn.addEventListener("click", async () => {

  try {

    await signOut(auth);

    console.log("Signed out.");

  } catch (error) {

    console.error(
      "Logout error:",
      error
    );
  }
});


// =====================================================
// FIREBASE LISTENERS
// =====================================================

function startFirebaseListeners() {

  // -----------------------------
  // Firebase connection
  // -----------------------------

  onValue(
    ref(db, ".info/connected"),
    (snapshot) => {

      if (snapshot.val() === true) {

        connectionStatus.className =
          "status-pill status-live";

        connectionStatus.innerHTML =
          '<span class="status-dot"></span> Live';

      } else {

        connectionStatus.className =
          "status-pill status-error";

        connectionStatus.innerHTML =
          '<span class="status-dot"></span> Offline';
      }
    }
  );


  // -----------------------------
  // Current sensor values
  // -----------------------------

  onValue(
    ref(db, "current"),
    (snapshot) => {

      const data =
        snapshot.val() || {};

      const temperature =
        Number(data.temperature);

      const humidity =
        Number(data.humidity);

      const ldr =
        Number(data.ldr);

      if (
        Number.isFinite(temperature)
      ) {
        temperatureValue.textContent =
          temperature.toFixed(1) + " °C";
      }

      if (
        Number.isFinite(humidity)
      ) {
        humidityValue.textContent =
          humidity.toFixed(0) + " %";
      }

      if (
        Number.isFinite(ldr)
      ) {
        ldrValue.textContent =
          Math.round(ldr);
      }


      const bulbOn =
        data.bulbState === true ||
        data.bulbState === 1 ||
        data.bulbState === "ON";


      bulbValue.textContent =
        bulbOn ? "ON" : "OFF";

      bulbValue.classList.toggle(
        "state-on",
        bulbOn
      );

      bulbValue.classList.toggle(
        "state-off",
        !bulbOn
      );


      statusBulb.textContent =
        bulbOn ? "ON" : "OFF";


      if (data.timestamp) {

        lastUpdated.textContent =
          new Date(
            Number(data.timestamp)
          ).toLocaleString();
      }


      updateMode(
        data.mode || "manual"
      );
    }
  );


  // -----------------------------
  // Control settings
  // -----------------------------

  onValue(
    ref(db, "control"),
    (snapshot) => {

      const control =
        snapshot.val() || {};

      updateMode(
        control.mode || "manual"
      );

      if (
        control.ldrThreshold !== undefined
      ) {

        ldrThreshold.value =
          control.ldrThreshold;
      }
    }
  );


  // -----------------------------
  // History
  // -----------------------------

  onValue(
    query(
      ref(db, "history"),
      limitToLast(100)
    ),
    (snapshot) => {

      const data =
        snapshot.val() || {};

      renderHistory(data);
      renderCharts(data);
    }
  );
}


// =====================================================
// MODE
// =====================================================

function updateMode(mode) {

  const automatic =
    String(mode).toLowerCase() ===
    "automatic";

  manualModeBtn.classList.toggle(
    "active",
    !automatic
  );

  autoModeBtn.classList.toggle(
    "active",
    automatic
  );

  const text =
    automatic
      ? "AUTOMATIC"
      : "MANUAL";

  modeMini.textContent = text;
  modeBadge.textContent = text;

  bulbSub.textContent =
    automatic
      ? "Automatic LDR control"
      : "Manual control";
}


// =====================================================
// MANUAL MODE
// =====================================================

manualModeBtn.addEventListener(
  "click",
  async () => {

    try {

      await set(
        ref(db, "control/mode"),
        "manual"
      );

      showMessage(
        "Manual mode enabled."
      );

    } catch (error) {

      console.error(error);

      showMessage(
        "Failed to change mode.",
        true
      );
    }
  }
);


// =====================================================
// AUTOMATIC MODE
// =====================================================

autoModeBtn.addEventListener(
  "click",
  async () => {

    try {

      await set(
        ref(db, "control/mode"),
        "automatic"
      );

      showMessage(
        "Automatic mode enabled."
      );

    } catch (error) {

      console.error(error);

      showMessage(
        "Failed to change mode.",
        true
      );
    }
  }
);


// =====================================================
// BULB ON
// =====================================================

bulbOnBtn.addEventListener(
  "click",
  async () => {

    try {

      await set(
        ref(db, "control/mode"),
        "manual"
      );

      await set(
        ref(db, "control/manualBulb"),
        true
      );

      showMessage(
        "Bulb ON command sent."
      );

    } catch (error) {

      console.error(error);

      showMessage(
        "Failed to turn bulb ON.",
        true
      );
    }
  }
);


// =====================================================
// BULB OFF
// =====================================================

bulbOffBtn.addEventListener(
  "click",
  async () => {

    try {

      await set(
        ref(db, "control/mode"),
        "manual"
      );

      await set(
        ref(db, "control/manualBulb"),
        false
      );

      showMessage(
        "Bulb OFF command sent."
      );

    } catch (error) {

      console.error(error);

      showMessage(
        "Failed to turn bulb OFF.",
        true
      );
    }
  }
);


// =====================================================
// LDR THRESHOLD
// =====================================================

saveThresholdBtn.addEventListener(
  "click",
  async () => {

    const value =
      Number(ldrThreshold.value);

    if (
      !Number.isFinite(value) ||
      value < 0 ||
      value > 4095
    ) {

      showMessage(
        "Enter a value between 0 and 4095.",
        true
      );

      return;
    }

    try {

      await set(
        ref(db, "control/ldrThreshold"),
        value
      );

      showMessage(
        "LDR threshold saved."
      );

    } catch (error) {

      console.error(error);

      showMessage(
        "Failed to save threshold.",
        true
      );
    }
  }
);


// =====================================================
// HISTORY TABLE
// =====================================================

function renderHistory(data) {

  historyBody.innerHTML = "";

  const rows =
    Object.values(data)
      .filter(
        item =>
          item &&
          typeof item === "object"
      )
      .sort(
        (a, b) =>
          Number(a.timestamp || 0) -
          Number(b.timestamp || 0)
      )
      .reverse();


  if (rows.length === 0) {

    historyBody.innerHTML = `
      <tr>
        <td colspan="6" class="empty">
          No readings yet.
        </td>
      </tr>
    `;

    return;
  }


  rows.forEach((row) => {

    const tr =
      document.createElement("tr");

    const bulb =
      row.bulbState === true ||
      row.bulbState === 1 ||
      row.bulbState === "ON";


    tr.innerHTML = `
      <td>
        ${formatTimestamp(row.timestamp)}
      </td>

      <td>
        ${Number(row.temperature || 0).toFixed(1)} °C
      </td>

      <td>
        ${Number(row.humidity || 0).toFixed(0)} %
      </td>

      <td>
        ${Math.round(Number(row.ldr || 0))}
      </td>

      <td class="${bulb ? "state-on" : "state-off"}">
        ${bulb ? "ON" : "OFF"}
      </td>

      <td>
        ${String(row.mode || "manual").toUpperCase()}
      </td>
    `;

    historyBody.appendChild(tr);
  });
}


// =====================================================
// CHARTS
// =====================================================

function renderCharts(data) {

  if (
    typeof Chart === "undefined"
  ) {
    return;
  }

  const rows =
    Object.values(data)
      .filter(
        item =>
          item &&
          typeof item === "object"
      )
      .sort(
        (a, b) =>
          Number(a.timestamp || 0) -
          Number(b.timestamp || 0)
      );


  const labels =
    rows.map(
      row =>
        new Date(
          Number(row.timestamp)
        ).toLocaleTimeString(
          [],
          {
            hour: "2-digit",
            minute: "2-digit"
          }
        )
    );


  createChart(
    "temperatureChart",
    labels,
    rows.map(
      row =>
        Number(row.temperature || 0)
    ),
    "Temperature"
  );


  createChart(
    "humidityChart",
    labels,
    rows.map(
      row =>
        Number(row.humidity || 0)
    ),
    "Humidity"
  );


  createChart(
    "ldrChart",
    labels,
    rows.map(
      row =>
        Number(row.ldr || 0)
    ),
    "LDR"
  );
}


const charts = {};


function createChart(
  id,
  labels,
  values,
  label
) {

  const canvas =
    document.getElementById(id);

  if (!canvas) return;


  if (charts[id]) {
    charts[id].destroy();
  }


  charts[id] =
    new Chart(
      canvas,
      {
        type: "line",

        data: {
          labels,

          datasets: [
            {
              label,
              data: values,

              borderColor: "#111827",

              backgroundColor:
                "rgba(17,24,39,0.05)",

              borderWidth: 2,

              tension: 0.3,

              pointRadius: 0,

              fill: true
            }
          ]
        },

        options: {

          responsive: true,

          maintainAspectRatio: false,

          plugins: {
            legend: {
              display: false
            }
          },

          scales: {
            y: {
              beginAtZero: true
            }
          }
        }
      }
    );
}


// =====================================================
// CSV
// =====================================================

downloadCsvBtn.addEventListener(
  "click",
  () => {

    const rows =
      Object.values(
        window.lastHistory || {}
      );

    if (rows.length === 0) {

      showMessage(
        "No historical data available.",
        true
      );

      return;
    }


    let csv =
      "Timestamp,Temperature,Humidity,LDR,Bulb State,Operating Mode\n";


    rows.forEach((row) => {

      const timestamp =
        formatTimestamp(
          row.timestamp
        );

      const bulb =
        row.bulbState
          ? "ON"
          : "OFF";

      csv +=
        `"${timestamp}",` +
        `"${row.temperature ?? ""}",` +
        `"${row.humidity ?? ""}",` +
        `"${row.ldr ?? ""}",` +
        `"${bulb}",` +
        `"${row.mode ?? ""}"\n`;
    });


    const blob =
      new Blob(
        [csv],
        {
          type:
            "text/csv;charset=utf-8;"
        }
      );


    const url =
      URL.createObjectURL(blob);

    const a =
      document.createElement("a");

    a.href = url;

    a.download =
      "home-monitoring-history.csv";

    document.body.appendChild(a);

    a.click();

    a.remove();

    URL.revokeObjectURL(url);
  }
);


// =====================================================
// KEEP HISTORY FOR CSV
// =====================================================

onValue(
  ref(db, "history"),
  (snapshot) => {

    window.lastHistory =
      snapshot.val() || {};
  }
);


// =====================================================
// HELPERS
// =====================================================

function showLoginError(message) {

  loginError.textContent = message;
  loginError.classList.remove(
    "hidden"
  );
}


function showMessage(
  message,
  error = false
) {

  controlMessage.textContent =
    message;

  controlMessage.style.color =
    error
      ? "#b91c1c"
      : "#15803d";

  setTimeout(() => {

    controlMessage.textContent = "";

  }, 3000);
}


function getFirebaseError(error) {

  switch (error.code) {

    case "auth/invalid-credential":
      return "Invalid email or password.";

    case "auth/invalid-login-credentials":
      return "Invalid email or password.";

    case "auth/user-not-found":
      return "This email is not registered.";

    case "auth/wrong-password":
      return "Incorrect password.";

    case "auth/invalid-email":
      return "Invalid email address.";

    case "auth/operation-not-allowed":
      return "Email/password sign-in is not enabled in Firebase.";

    case "auth/unauthorized-domain":
      return "This website is not authorized in Firebase Authentication.";

    case "auth/network-request-failed":
      return "Network error. Check your internet connection.";

    case "auth/too-many-requests":
      return "Too many attempts. Try again later.";

    default:
      return `Firebase error: ${error.code || error.message}`;
  }
}


function formatTimestamp(timestamp) {

  const value =
    Number(timestamp);

  if (
    !Number.isFinite(value) ||
    value <= 0
  ) {
    return "--";
  }

  return new Date(value)
    .toLocaleString();
}