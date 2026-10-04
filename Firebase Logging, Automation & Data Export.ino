#include <WiFi.h>
#include <DHT.h>

#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// =====================================================
// Wi-Fi Configuration
// =====================================================

#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// =====================================================
// Firebase Configuration
// =====================================================

#define API_KEY "YOUR_FIREBASE_API_KEY"

#define DATABASE_URL "https://YOUR-PROJECT-ID-default-rtdb.firebaseio.com/"

#define USER_EMAIL "YOUR_FIREBASE_EMAIL"
#define USER_PASSWORD "YOUR_FIREBASE_PASSWORD"

// =====================================================
// Pin Configuration
// =====================================================

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define LDR_PIN 34

#define RELAY_PIN 23

// =====================================================
// Objects
// =====================================================

DHT dht(DHT_PIN, DHT_TYPE);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// =====================================================
// Variables
// =====================================================

float temperature = 0;
float humidity = 0;

int lightLevel = 0;

// Manual bulb state
bool bulbState = false;

// AUTO or MANUAL
String mode = "MANUAL";

// Light threshold
int lightThreshold = 1500;

// Timing
unsigned long previousSensorMillis = 0;
unsigned long previousLogMillis = 0;
unsigned long previousControlMillis = 0;

const unsigned long SENSOR_INTERVAL = 5000;
const unsigned long LOG_INTERVAL = 30000;
const unsigned long CONTROL_INTERVAL = 2000;

// =====================================================
// Setup
// =====================================================

void setup()
{
  Serial.begin(115200);

  // DHT initialization
  dht.begin();

  // Relay initialization
  pinMode(RELAY_PIN, OUTPUT);

  // Bulb OFF initially
  digitalWrite(RELAY_PIN, LOW);

  // ===================================================
  // Connect to Wi-Fi
  // ===================================================

  Serial.println();
  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Wi-Fi Connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // ===================================================
  // Firebase Configuration
  // ===================================================

  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  Serial.println("Firebase initialized!");

  // ===================================================
  // Get initial control settings
  // ===================================================

  readControlSettings();
}

// =====================================================
// Main Loop
// =====================================================

void loop()
{
  if (!Firebase.ready())
  {
    return;
  }

  // ---------------------------------------------------
  // Read sensors
  // ---------------------------------------------------

  if (millis() - previousSensorMillis >= SENSOR_INTERVAL)
  {
    previousSensorMillis = millis();

    readSensors();

    uploadCurrentSensorData();
  }

  // ---------------------------------------------------
  // Read dashboard control settings
  // ---------------------------------------------------

  if (millis() - previousControlMillis >= CONTROL_INTERVAL)
  {
    previousControlMillis = millis();

    readControlSettings();

    if (mode == "AUTO")
    {
      automaticLightControl();
    }
    else
    {
      manualLightControl();
    }
  }

  // ---------------------------------------------------
  // Store historical data
  // ---------------------------------------------------

  if (millis() - previousLogMillis >= LOG_INTERVAL)
  {
    previousLogMillis = millis();

    createHistoryLog();
  }
}

// =====================================================
// Read DHT11 and LDR
// =====================================================

void readSensors()
{
  float newTemperature = dht.readTemperature();
  float newHumidity = dht.readHumidity();

  if (!isnan(newTemperature))
  {
    temperature = newTemperature;
  }

  if (!isnan(newHumidity))
  {
    humidity = newHumidity;
  }

  lightLevel = analogRead(LDR_PIN);

  Serial.println();
  Serial.println("================================");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity    : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Light Level : ");
  Serial.println(lightLevel);

  Serial.print("Mode        : ");
  Serial.println(mode);

  Serial.print("Threshold   : ");
  Serial.println(lightThreshold);

  Serial.print("Bulb        : ");

  if (bulbState)
  {
    Serial.println("ON");
  }
  else
  {
    Serial.println("OFF");
  }

  Serial.println("================================");
}

// =====================================================
// Upload Current Sensor Data
// =====================================================

void uploadCurrentSensorData()
{
  Firebase.RTDB.setFloat(
    &fbdo,
    "/iot/sensors/temperature",
    temperature
  );

  Firebase.RTDB.setFloat(
    &fbdo,
    "/iot/sensors/humidity",
    humidity
  );

  Firebase.RTDB.setInt(
    &fbdo,
    "/iot/sensors/lightLevel",
    lightLevel
  );
}

// =====================================================
// Read Control Settings From Firebase
// =====================================================

void readControlSettings()
{
  // ---------------------------------------------------
  // Read Mode
  // ---------------------------------------------------

  if (Firebase.RTDB.getString(
        &fbdo,
        "/iot/control/mode"))
  {
    mode = fbdo.stringData();

    mode.toUpperCase();
  }

  // ---------------------------------------------------
  // Read Light Threshold
  // ---------------------------------------------------

  if (Firebase.RTDB.getInt(
        &fbdo,
        "/iot/control/threshold"))
  {
    lightThreshold = fbdo.intData();
  }

  // ---------------------------------------------------
  // Read Manual Bulb State
  // ---------------------------------------------------

  if (Firebase.RTDB.getBool(
        &fbdo,
        "/iot/control/bulb"))
  {
    bulbState = fbdo.boolData();
  }
}

// =====================================================
// Manual Bulb Control
// =====================================================

void manualLightControl()
{
  digitalWrite(
    RELAY_PIN,
    bulbState ? HIGH : LOW
  );
}

// =====================================================
// Automatic Light Control
// =====================================================

void automaticLightControl()
{
  /*
     If the light level is below the threshold,
     the environment is considered dark.

     Therefore:
     
     DARK  -> BULB ON
     LIGHT -> BULB OFF
  */

  if (lightLevel < lightThreshold)
  {
    bulbState = true;
  }
  else
  {
    bulbState = false;
  }

  digitalWrite(
    RELAY_PIN,
    bulbState ? HIGH : LOW
  );

  // Update the automatically calculated state
  // in Firebase so the dashboard can display it.

  Firebase.RTDB.setBool(
    &fbdo,
    "/iot/control/bulb",
    bulbState
  );
}

// =====================================================
// Create Historical Log
// =====================================================

void createHistoryLog()
{
  // Create a unique key using Firebase push()
  String path = "/iot/history";

  if (Firebase.RTDB.pushInt(
        &fbdo,
        path,
        millis()))
  {
    String logPath = fbdo.dataPath();

    // -------------------------------------------------
    // Timestamp
    // -------------------------------------------------

    Firebase.RTDB.setString(
      &fbdo,
      logPath + "/timestamp",
      String(millis())
    );

    // -------------------------------------------------
    // Temperature
    // -------------------------------------------------

    Firebase.RTDB.setFloat(
      &fbdo,
      logPath + "/temperature",
      temperature
    );

    // -------------------------------------------------
    // Humidity
    // -------------------------------------------------

    Firebase.RTDB.setFloat(
      &fbdo,
      logPath + "/humidity",
      humidity
    );

    // -------------------------------------------------
    // Light Level
    // -------------------------------------------------

    Firebase.RTDB.setInt(
      &fbdo,
      logPath + "/lightLevel",
      lightLevel
    );

    // -------------------------------------------------
    // Bulb State
    // -------------------------------------------------

    Firebase.RTDB.setBool(
      &fbdo,
      logPath + "/bulbStatus",
      bulbState
    );

    // -------------------------------------------------
    // Mode
    // -------------------------------------------------

    Firebase.RTDB.setString(
      &fbdo,
      logPath + "/mode",
      mode
    );

    Serial.println("Historical data saved to Firebase.");
  }
  else
  {
    Serial.print("History log failed: ");
    Serial.println(fbdo.errorReason());
  }
}