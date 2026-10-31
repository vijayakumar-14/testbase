#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <math.h>
#include "DHTesp.h"

const int DHT_PIN = 15;
const unsigned long SEND_INTERVAL_MS = 10000;

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const char* FIREBASE_URL =
  "https://test-base-2fb81-default-rtdb.firebaseio.com";

DHTesp dhtSensor;
unsigned long lastSend = 0;
bool firstReading = true;

void connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
}

void sendTemperature(float celsius) {
  WiFiClientSecure tlsClient;

  // Demo setting: skips certificate checking. Do not use this for a real device.
  tlsClient.setInsecure();

  HTTPClient http;
  String url = String(FIREBASE_URL) + "/sensors/temperature.json";

  if (!http.begin(tlsClient, url)) {
    Serial.println("Could not start HTTPS request");
    return;
  }

  http.addHeader("Content-Type", "application/json");

  String payload = "{\"value\":";
  payload += String(celsius, 1);
  payload += ",\"unit\":\"C\",\"device\":\"esp32-wokwi\",";
  payload += "\"updatedAt\":{\".sv\":\"timestamp\"}}";

  int responseCode = http.PUT(payload);

  Serial.print("Firebase response: ");
  Serial.println(responseCode);

  if (responseCode > 0) {
    Serial.println(http.getString());
  } else {
    Serial.println(http.errorToString(responseCode));
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  connectToWiFi();
}

void loop() {
  if (firstReading || millis() - lastSend >= SEND_INTERVAL_MS) {
    firstReading = false;
    lastSend = millis();

    if (WiFi.status() != WL_CONNECTED) {
      connectToWiFi();
    }

    TempAndHumidity reading = dhtSensor.getTempAndHumidity();

    if (isnan(reading.temperature)) {
      Serial.println("DHT22 read failed");
    } else {
      Serial.print("Temperature: ");
      Serial.print(reading.temperature, 1);
      Serial.println(" C");
      sendTemperature(reading.temperature);
    }
  }

  delay(100);
}
