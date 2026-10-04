#include <WiFi.h>
#include <DHT.h>
#include <Firebase_ESP_Client.h>

// Firebase helper libraries
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ===============================
// Wi-Fi Configuration
// ===============================

#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ===============================
// Firebase Configuration
// ===============================

#define API_KEY "YOUR_FIREBASE_API_KEY"

#define DATABASE_URL "https://YOUR-PROJECT-ID-default-rtdb.firebaseio.com/"

// Firebase Authentication
#define USER_EMAIL "YOUR_FIREBASE_USER_EMAIL"
#define USER_PASSWORD "YOUR_FIREBASE_USER_PASSWORD"

// ===============================
// Pin Configuration
// ===============================

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define LDR_PIN 34

#define RELAY_PIN 23

// ===============================
// Objects
// ===============================

DHT dht(DHT_PIN, DHT_TYPE);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ===============================
// Variables
// ===============================

float temperature;
float humidity;
int ldrValue;

bool bulbState = false;

unsigned long previousMillis = 0;

const unsigned long interval = 5000;

// ===============================
// Setup
// ===============================

void setup()
{
  Serial.begin(115200);

  // Initialize DHT
  dht.begin();

  // Initialize relay
  pinMode(RELAY_PIN, OUTPUT);

  // Keep bulb OFF initially
  digitalWrite(RELAY_PIN, LOW);

  // ===============================
  // Connect to Wi-Fi
  // ===============================

  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // ===============================
  // Firebase Configuration
  // ===============================

  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  Serial.println("Firebase initialized!");
}

// ===============================
// Main Loop
// ===============================

void loop()
{
  // Read bulb control from Firebase
  readBulbControl();

  // Send sensor data every 5 seconds
  if (millis() - previousMillis >= interval)
  {
    previousMillis = millis();

    readSensors();

    sendSensorData();
  }
}

// ===============================
// Read DHT11 and LDR
// ===============================

void readSensors()
{
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  ldrValue = analogRead(LDR_PIN);

  // Check DHT11 readings
  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read from DHT11!");

    return;
  }

  Serial.println("-----------------------------");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  Serial.print("Bulb: ");

  if (bulbState)
  {
    Serial.println("ON");
  }
  else
  {
    Serial.println("OFF");
  }
}

// ===============================
// Send Sensor Data to Firebase
// ===============================

void sendSensorData()
{
  if (!Firebase.ready())
  {
    Serial.println("Firebase not ready");

    return;
  }

  // Temperature
  if (Firebase.RTDB.setFloat(
        &fbdo,
        "/iot/sensors/temperature",
        temperature))
  {
    Serial.println("Temperature uploaded");
  }
  else
  {
    Serial.println(fbdo.errorReason());
  }

  // Humidity
  if (Firebase.RTDB.setFloat(
        &fbdo,
        "/iot/sensors/humidity",
        humidity))
  {
    Serial.println("Humidity uploaded");
  }
  else
  {
    Serial.println(fbdo.errorReason());
  }

  // LDR
  if (Firebase.RTDB.setInt(
        &fbdo,
        "/iot/sensors/ldr",
        ldrValue))
  {
    Serial.println("LDR value uploaded");
  }
  else
  {
    Serial.println(fbdo.errorReason());
  }
}

// ===============================
// Read Bulb Control from Firebase
// ===============================

void readBulbControl()
{
  if (!Firebase.ready())
  {
    return;
  }

  if (Firebase.RTDB.getBool(
        &fbdo,
        "/iot/control/bulb"))
  {
    bool newBulbState = fbdo.boolData();

    if (newBulbState != bulbState)
    {
      bulbState = newBulbState;

      controlBulb();

      Serial.print("Bulb state changed to: ");

      if (bulbState)
      {
        Serial.println("ON");
      }
      else
      {
        Serial.println("OFF");
      }
    }
  }
  else
  {
    Serial.println("Failed to read bulb state");
    Serial.println(fbdo.errorReason());
  }
}

// ===============================
// Control Relay / Bulb
// ===============================

void controlBulb()
{
  if (bulbState)
  {
    digitalWrite(RELAY_PIN, HIGH);
  }
  else
  {
    digitalWrite(RELAY_PIN, LOW);
  }
}