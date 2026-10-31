import { initializeApp } from "https://www.gstatic.com/firebasejs/13.1.0/firebase-app.js";
import {
  getDatabase,
  onValue,
  ref
} from "https://www.gstatic.com/firebasejs/13.1.0/firebase-database.js";

const firebaseConfig = {
  apiKey: "AIzaSyD3Q_CMuMgIZKVn-09KMzkWqnjQUCP6nlA",
  authDomain: "test-base-2fb81.firebaseapp.com",
  databaseURL: "https://test-base-2fb81-default-rtdb.firebaseio.com",
  projectId: "test-base-2fb81",
  storageBucket: "test-base-2fb81.firebasestorage.app",
  messagingSenderId: "762253667867",
  appId: "1:762253667867:web:f0f98e60e83b30555d12be",
  measurementId: "G-9WTY2SLE6H"
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);
const temperatureRef = ref(database, "sensors/temperature");

const temperatureElement = document.querySelector("#temperature");
const statusElement = document.querySelector("#status");
const updatedElement = document.querySelector("#updated");
const deviceElement = document.querySelector("#device");

onValue(
  temperatureRef,
  (snapshot) => {
    if (!snapshot.exists()) {
      statusElement.textContent = "Connected; waiting for ESP32 data";
      temperatureElement.textContent = "--.-";
      return;
    }

    const data = snapshot.val();
    const value = Number(data.value);

    if (!Number.isFinite(value)) {
      statusElement.textContent = "Firebase data has an unexpected format";
      return;
    }

    temperatureElement.textContent = value.toFixed(1);
    deviceElement.textContent = data.device || "ESP32";
    updatedElement.textContent = data.updatedAt
      ? `Last update: ${new Date(Number(data.updatedAt)).toLocaleTimeString()}`
      : "Reading received";
    statusElement.textContent = "Live";
  },
  (error) => {
    console.error(error);
    statusElement.textContent = "Firebase error — check your database rules";
  }
);
