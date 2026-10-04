#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// =====================================================
// Wi-Fi Configuration
// =====================================================

#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// =====================================================
// Adafruit IO Configuration
// =====================================================

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883

#define AIO_USERNAME    "YOUR_ADAFRUIT_IO_USERNAME"
#define AIO_KEY         "YOUR_ADAFRUIT_IO_KEY"

// =====================================================
// ESP32 Hardware
// =====================================================

#define LED_PIN 2

// =====================================================
// Wi-Fi Client
// =====================================================

WiFiClient client;

// =====================================================
// MQTT Client
// =====================================================

Adafruit_MQTT_Client mqtt(
  &client,
  AIO_SERVER,
  AIO_SERVERPORT,
  AIO_USERNAME,
  AIO_KEY
);

// =====================================================
// MQTT Feed
// Feed name: led-control
// =====================================================

Adafruit_MQTT_Subscribe ledFeed =
  Adafruit_MQTT_Subscribe(
    &mqtt,
    AIO_USERNAME "/feeds/led-control"
  );

// Feed for publishing current LED status
Adafruit_MQTT_Publish ledStatus =
  Adafruit_MQTT_Publish(
    &mqtt,
    AIO_USERNAME "/feeds/led-status"
  );

// =====================================================
// Connect to Wi-Fi
// =====================================================

void connectWiFi() {

  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

// =====================================================
// Connect to MQTT
// =====================================================

void MQTT_connect() {

  int8_t ret;

  // Already connected?
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to Adafruit IO MQTT...");

  while ((ret = mqtt.connect()) != 0) {

    Serial.println(mqtt.connectErrorString(ret));

    Serial.println("Retrying MQTT connection in 5 seconds...");

    mqtt.disconnect();

    delay(5000);
  }

  Serial.println("MQTT Connected!");
}

// =====================================================
// Setup
// =====================================================

void setup() {

  Serial.begin(115200);

  // Configure built-in LED
  pinMode(LED_PIN, OUTPUT);

  // Start with LED OFF
  digitalWrite(LED_PIN, LOW);

  // Connect to Wi-Fi
  connectWiFi();

  // Subscribe to LED control feed
  mqtt.subscribe(&ledFeed);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32 Adafruit IO MQTT LED Control");
  Serial.println("=================================");
}

// =====================================================
// Main Loop
// =====================================================

void loop() {

  // Make sure MQTT is connected
  MQTT_connect();

  // Check for incoming MQTT messages
  Adafruit_MQTT_Subscribe *subscription;

  while ((subscription = mqtt.readSubscription(5000))) {

    // Check if message came from LED feed
    if (subscription == &ledFeed) {

      String command = (char *)ledFeed.lastread;

      Serial.print("Received command: ");
      Serial.println(command);

      // ==========================================
      // LED ON
      // ==========================================

      if (command == "ON" || command == "on" || command == "1") {

        digitalWrite(LED_PIN, HIGH);

        Serial.println("Built-in LED: ON");

        // Publish status
        if (!ledStatus.publish("ON")) {
          Serial.println("Failed to publish LED status");
        }
        else {
          Serial.println("LED status published: ON");
        }
      }

      // ==========================================
      // LED OFF
      // ==========================================

      else if (command == "OFF" || command == "off" || command == "0") {

        digitalWrite(LED_PIN, LOW);

        Serial.println("Built-in LED: OFF");

        // Publish status
        if (!ledStatus.publish("OFF")) {
          Serial.println("Failed to publish LED status");
        }
        else {
          Serial.println("LED status published: OFF");
        }
      }

      else {

        Serial.println("Unknown command received.");
      }
    }
  }
}