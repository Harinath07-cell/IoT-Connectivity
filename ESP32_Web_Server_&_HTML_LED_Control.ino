#include <WiFi.h>
#include <WebServer.h>

// Enter your local Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Initialize HTTP server on standard Web Port 80
WebServer server(80);

// Define LED GPIO Pin (GPIO 2 is built-in LED on ESP32 NodeMCU)
const int ledPin = 2;

// Handler for root URL ("/") -> Serves HTML Web Interface
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP32 Web Server LED Control</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #0f172a; color: #f8fafc; padding-top: 50px; }";
  html += "h1 { color: #38bdf8; margin-bottom: 30px; }";
  html += ".btn { display: inline-block; padding: 15px 32px; font-size: 18px; font-weight: bold; text-decoration: none; border-radius: 12px; margin: 10px; transition: 0.3s; }";
  html += ".btn-on { background-color: #00E5FF; color: #0f172a; }";
  html += ".btn-off { background-color: #ef4444; color: #ffffff; }";
  html += ".btn:hover { opacity: 0.85; transform: scale(1.05); }";
  html += "</style></head><body>";
  html += "<h1>ESP32 Web Server LED Control</h1>";
  html += "<p>Control hardware GPIO 2 directly over Wi-Fi</p>";
  html += "<p><a href='/led/on' className='btn btn-on'>TURN LED ON</a></p>";
  html += "<p><a href='/led/off' className='btn btn-off'>TURN LED OFF</a></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Handler for "/led/on" -> Toggles GPIO 2 HIGH
void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  Serial.println("Command Received: LED Turned ON");
  server.sendHeader("Location", "/");
  server.send(303); // Redirect back to root page
}

// Handler for "/led/off" -> Toggles GPIO 2 LOW
void handleLedOff() {
  digitalWrite(ledPin, LOW);
  Serial.println("Command Received: LED Turned OFF");
  server.sendHeader("Location", "/");
  server.send(303); // Redirect back to root page
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Initial State: OFF

  // Connect to Local Wi-Fi Network
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("
[SUCCESS] Wi-Fi Connected!");
  Serial.print("ESP32 Local IP Address: http://");
  Serial.println(WiFi.localIP());

  // Bind HTTP Route Handlers
  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);

  // Start HTTP Server
  server.begin();
  Serial.println("HTTP Web Server Started on Port 80.");
}

void loop() {
  // Continuously listen and process client HTTP requests
  server.handleClient();
}