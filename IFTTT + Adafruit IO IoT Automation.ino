#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// =====================================================
// 1. Wi-Fi Configuration
// =====================================================

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// =====================================================
// 2. Adafruit IO Configuration
// =====================================================

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883

#define AIO_USERNAME    "YOUR_ADAFRUIT_IO_USERNAME"
#define AIO_KEY         "YOUR_ADAFRUIT_IO_KEY"

// =====================================================
// 3. Hardware Configuration
// =====================================================

#define LED_PIN 2

// =====================================================
// 4. Wi-Fi and MQTT Client
// =====================================================

WiFiClient client;

Adafruit_MQTT_Client mqtt(
  &client,
  AIO_SERVER,
  AIO_SERVERPORT,
  AIO_USERNAME,
  AIO_KEY
);

// =====================================================
// 5. Adafruit IO Feed
// =====================================================

// IFTTT will send commands to this feed.
// Example values:
// ON
// OFF

Adafruit_MQTT_Subscribe automationFeed =
  Adafruit_MQTT_Subscribe(
    &mqtt,
    AIO_USERNAME "/feeds/automation-control"
  );

// Optional status feed
Adafruit_MQTT_Publish statusFeed =
  Adafruit_MQTT_Publish(
    &mqtt,
    AIO_USERNAME "/feeds/automation-status"
  );

// =====================================================
// 6. Connect to Wi-Fi
// =====================================================

void connectWiFi()
{
  Serial.println();
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("[SUCCESS] Wi-Fi Connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

// =====================================================
// 7. Connect to Adafruit IO MQTT
// =====================================================

void connectMQTT()
{
  int8_t ret;

  // If already connected, do nothing
  if (mqtt.connected())
  {
    return;
  }

  Serial.println();
  Serial.println("Connecting to Adafruit IO MQTT...");

  while ((ret = mqtt.connect()) != 0)
  {
    Serial.print("MQTT connection failed. Error: ");
    Serial.println(mqtt.connectErrorString(ret));

    mqtt.disconnect();

    delay(5000);

    Serial.println("Retrying MQTT connection...");
  }

  Serial.println("[SUCCESS] Connected to Adafruit IO MQTT!");

  // Subscribe to automation feed
  mqtt.subscribe(&automationFeed);

  Serial.println("Subscribed to:");
  Serial.println(
    String(AIO_USERNAME) + "/feeds/automation-control"
  );
}

// =====================================================
// 8. Process IFTTT Command
// =====================================================

void processCommand(String command)
{
  command.trim();

  Serial.print("Received command: ");
  Serial.println(command);

  // Convert command to uppercase
  command.toUpperCase();

  // ---------------------------------------------------
  // Turn LED ON
  // ---------------------------------------------------

  if (command == "ON" ||
      command == "1" ||
      command == "TRUE")
  {
    digitalWrite(LED_PIN, HIGH);

    Serial.println("Action: LED turned ON");

    // Send status back to Adafruit IO
    if (!statusFeed.publish("ON"))
    {
      Serial.println("Failed to publish ON status");
    }
    else
    {
      Serial.println("Status published: ON");
    }
  }

  // ---------------------------------------------------
  // Turn LED OFF
  // ---------------------------------------------------

  else if (command == "OFF" ||
           command == "0" ||
           command == "FALSE")
  {
    digitalWrite(LED_PIN, LOW);

    Serial.println("Action: LED turned OFF");

    // Send status back to Adafruit IO
    if (!statusFeed.publish("OFF"))
    {
      Serial.println("Failed to publish OFF status");
    }
    else
    {
      Serial.println("Status published: OFF");
    }
  }

  // ---------------------------------------------------
  // Unknown command
  // ---------------------------------------------------

  else
  {
    Serial.println("Unknown command received.");
    Serial.println("Expected: ON or OFF");
  }
}

// =====================================================
// 9. Setup
// =====================================================

void setup()
{
  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println("Task 3 - IFTTT IoT Automation");
  Serial.println("ESP32 + Adafruit IO + MQTT");
  Serial.println("====================================");

  // Configure LED
  pinMode(LED_PIN, OUTPUT);

  // Start with LED OFF
  digitalWrite(LED_PIN, LOW);

  // Connect Wi-Fi
  connectWiFi();

  // Connect MQTT
  connectMQTT();

  Serial.println();
  Serial.println("System Ready.");
  Serial.println("Waiting for IFTTT automation commands...");
}

// =====================================================
// 10. Main Loop
// =====================================================

void loop()
{
  // Reconnect Wi-Fi if disconnected
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wi-Fi disconnected.");
    connectWiFi();
  }

  // Reconnect MQTT if disconnected
  if (!mqtt.connected())
  {
    connectMQTT();
  }

  // Process incoming MQTT messages
  Adafruit_MQTT_Subscribe* subscription;

  while ((subscription = mqtt.readSubscription(1000)))
  {
    if (subscription == &automationFeed)
    {
      String command =
        String((char*)automationFeed.lastread);

      processCommand(command);
    }
  }

  // Keep MQTT connection alive
  mqtt.ping();

  delay(10);
}