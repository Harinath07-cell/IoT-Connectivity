# IoT-Connectivity
# IoT & ESP32 Projects

A progressive collection of **ESP32-based IoT projects** developed as part of the ProtoSem learning journey. The repository contains five tasks, with each task implemented and maintained in a separate Git branch.

The projects progress from a simple **local ESP32 web server** to **cloud-connected IoT systems**, covering HTTP, MQTT, Adafruit IO, IFTTT, Firebase, sensor monitoring, remote control, automation, authentication, historical data logging, and dashboard development.

---

## 📌 Project Overview

This repository demonstrates the development of IoT applications using the **ESP32 microcontroller** and different communication and cloud technologies.

The tasks were designed progressively:

```text
ESP32 GPIO Control
       ↓
Local Web Server
       ↓
MQTT + Adafruit IO
       ↓
IFTTT Automation
       ↓
Firebase Cloud Integration
       ↓
Sensor Monitoring + Automation
       ↓
Historical Data + Web Dashboard
```

Each task builds on concepts learned in the previous task and introduces a new layer of IoT architecture.

---

# 🧩 Tasks

## Task 1 — ESP32 Web Server & HTML LED Control

### Objective

Develop a basic web server on the ESP32 that allows a user to control an LED through a web browser over a local Wi-Fi network.

### What was implemented

- ESP32 configured as a web server
- Wi-Fi connectivity using `WiFi.h`
- HTTP communication between browser and ESP32
- HTML-based control interface
- LED ON/OFF control using browser buttons
- GPIO 2 used for LED control
- HTTP routes for controlling the LED
- Serial Monitor used for monitoring Wi-Fi connection and device IP

### Architecture

```text
Web Browser
     │
     │ HTTP Request
     ↓
Wi-Fi Network
     │
     ↓
ESP32 Web Server
     │
     ↓
HTTP Route Handler
     │
     ↓
GPIO 2
     │
     ↓
LED
```

### Technologies

- ESP32
- Arduino IDE
- C/C++
- HTML
- Wi-Fi
- HTTP
- `WiFi.h`
- `WebServer.h`

### Key Learning

This task introduced the fundamentals of connecting a microcontroller to Wi-Fi and controlling physical hardware through a browser-based interface.

---

# Task 2 — Adafruit IO Dashboard & MQTT

### Objective

Extend the local ESP32 control system into a **cloud-connected IoT architecture** using Adafruit IO and MQTT.

### What was implemented

- ESP32 connected to the internet through Wi-Fi
- Adafruit IO configured as the cloud IoT platform
- MQTT communication between ESP32 and Adafruit IO
- Dashboard-based LED control
- MQTT publish/subscribe communication
- LED control through an Adafruit IO feed
- LED status published back to the cloud

### Architecture

```text
Adafruit IO Dashboard
          │
          │ MQTT Publish
          ↓
   Adafruit IO Broker
          │
          │ MQTT Subscribe
          ↓
         ESP32
          │
          ↓
       GPIO 2
          │
          ↓
         LED
```

Status information follows the reverse path:

```text
ESP32
  │
  │ MQTT Publish
  ↓
Adafruit IO
  │
  ↓
Dashboard
```

### Main MQTT Feeds

```text
led-control
led-status
```

### Technologies

- ESP32
- Arduino IDE
- C/C++
- Wi-Fi
- MQTT
- Adafruit IO
- `WiFi.h`
- `Adafruit_MQTT.h`
- `Adafruit_MQTT_Client.h`

### Key Learning

This task introduced **cloud-based IoT communication** and the MQTT publish/subscribe model.

Instead of communicating directly with a device on the same local network, the ESP32 communicates through a cloud broker.

---

# Task 3 — IFTTT + Adafruit IO IoT Automation

### Objective

Introduce **event-driven IoT automation** by connecting IFTTT with Adafruit IO and the ESP32.

### What was implemented

The system connects an external trigger to an IoT device through a cloud-based automation workflow.

```text
IFTTT Trigger
      │
      ↓
IFTTT Action
      │
      ↓
Adafruit IO Feed
      │
      │ MQTT
      ↓
ESP32
      │
      ↓
GPIO
      │
      ↓
LED / Output Device
```

### Automation Concept

The basic automation model is:

```text
IF THIS
   ↓
Trigger occurs
   ↓
THEN THAT
   ↓
Update IoT action
```

The trigger is handled by IFTTT, while Adafruit IO acts as the communication layer between the automation service and ESP32.

### Technologies

- ESP32
- Arduino IDE
- IFTTT
- Adafruit IO
- MQTT
- Wi-Fi
- C/C++

### Key Learning

This task introduced the concept of **event-driven IoT automation**, where an external event can initiate an action on a physical device without directly interacting with the ESP32.

---

# Task 4 — Firebase IoT Monitoring Dashboard

### Objective

Move from simple device control toward a **cloud-based IoT monitoring system** using Firebase.

This task introduced sensor data collection, cloud storage, authentication, and a web dashboard.

### System Components

- ESP32
- DHT11 temperature and humidity sensor
- LDR light sensor
- Relay-controlled bulb/load
- Firebase Authentication
- Firebase Realtime Database
- Web dashboard

### System Architecture

```text
DHT11 ───────┐
             │
LDR ─────────┤
             ↓
           ESP32
             │
             │ Wi-Fi
             ↓
     Firebase Realtime DB
             │
             ↓
       Web Dashboard
             │
             │ Control Commands
             ↓
           ESP32
             │
             ↓
        Relay / Bulb
```

### DHT11

The DHT11 is used to collect:

- Temperature
- Humidity

### LDR

The LDR is used to measure the surrounding light level.

The analog value is read by the ESP32 and used as an input for monitoring and lighting control.

### Bulb Control

The ESP32 controls a relay through:

```text
GPIO 2 → Relay → Bulb / Load
```

### Firebase

Firebase provides the cloud layer for:

- Data storage
- Real-time updates
- User authentication
- Dashboard communication
- Remote control

### Authentication

The system demonstrates the difference between:

**Authentication**

> Verifying who the user is.

**Authorization**

> Determining what an authenticated user is allowed to access or control.

### Technologies

- ESP32
- DHT11
- LDR
- Relay
- Arduino IDE
- Firebase
- Firebase Realtime Database
- Firebase Authentication
- HTML
- CSS
- JavaScript
- Firebase Web SDK

### Key Learning

This task introduced the architecture of a cloud-connected IoT monitoring system where sensor information can be viewed remotely through a web interface.

---

# Task 5 — Firebase IoT Monitoring, Automation & Historical Logging

## Objective

Build a more complete IoT monitoring and control system by combining:

- Sensor monitoring
- Cloud storage
- Manual control
- Automatic control
- Threshold-based automation
- Historical data logging
- Web dashboard
- CSV data export

This task extends the Firebase concepts from Task 4 into a more structured IoT application.

---

## Sensors & Actuators

### DHT11

Measures:

```text
Temperature
Humidity
```

Connection:

```text
DHT11 DATA → ESP32 GPIO 4
```

### LDR

Measures:

```text
Light Intensity
```

Connection:

```text
LDR Analog Output → ESP32 GPIO 34
```

### Relay

Controls the lighting load:

```text
Relay IN → ESP32 GPIO 2
```

---

# ⚙️ Operating Modes

The system supports two operating modes.

## Manual Mode

The user controls the bulb from the web dashboard.

```text
Dashboard
    ↓
Firebase /iot/control
    ↓
ESP32
    ↓
Relay
    ↓
Bulb
```

The manual bulb state is stored under:

```text
iot/control/manualBulb
```

---

## Automatic Mode

The ESP32 uses the LDR reading and a configured threshold to determine the bulb state.

```text
LDR
 ↓
ESP32
 ↓
Read LDR Value
 ↓
Compare With Threshold
 ↓
Automatic Decision
 ↓
Relay
 ↓
Bulb
```

The threshold is stored under:

```text
iot/control/ldrThreshold
```

The operating mode is stored under:

```text
iot/control/mode
```

---

# ☁️ Firebase Database Structure

The Task 5 implementation uses a structured Firebase Realtime Database.

```text
iot
│
├── current
│   ├── temperature
│   ├── humidity
│   ├── ldr
│   ├── bulb
│   └── mode
│
├── control
│   ├── mode
│   ├── manualBulb
│   └── ldrThreshold
│
└── history
    ├── record
    │   ├── timestamp
    │   ├── temperature
    │   ├── humidity
    │   ├── ldr
    │   ├── bulb
    │   └── mode
    │
    └── ...
```

---

# 📊 Current Data

The `current` node contains the latest system state.

```text
iot/current
```

It stores:

- Temperature
- Humidity
- LDR value
- Bulb state
- Current operating mode

This information is displayed on the web dashboard.

---

# 📝 Historical Data

The ESP32 periodically creates a historical record containing:

```text
Timestamp
Temperature
Humidity
LDR
Bulb State
Operating Mode
```

The records are stored under:

```text
iot/history
```

This allows previous sensor readings and system states to be analyzed later.

---

# 📁 CSV Export

The dashboard can retrieve historical records from Firebase and generate a CSV file.

This allows the collected IoT data to be used for:

- Data analysis
- Record keeping
- Spreadsheet analysis
- Further processing

---

# 🔄 Complete Task 5 Data Flow

```text
             ┌──────────────┐
             │    DHT11     │
             │ Temp/Humidity│
             └──────┬───────┘
                    │
                    │
             ┌──────▼───────┐
             │     LDR      │
             │ Light Level  │
             └──────┬───────┘
                    │
                    ↓
             ┌──────────────┐
             │    ESP32     │
             │              │
             │ Sensor Read  │
             │ Processing   │
             │ Automation   │
             └──────┬───────┘
                    │
              Wi-Fi │
                    ↓
        ┌─────────────────────┐
        │ Firebase Realtime DB│
        │                     │
        │ Current             │
        │ Control             │
        │ History             │
        └───────┬─────────────┘
                │
                ↓
        ┌─────────────────────┐
        │   Web Dashboard     │
        │                     │
        │ Temperature         │
        │ Humidity            │
        │ LDR                 │
        │ Bulb State          │
        │ Mode                │
        │ History             │
        └─────────┬───────────┘
                  │
             Control Data
                  │
                  ↓
               ESP32
                  │
                  ↓
               Relay
                  │
                  ↓
                Bulb
```

---

# 🧰 Hardware Used

The projects use different hardware depending on the task.

| Component | Purpose |
|---|---|
| ESP32 | Main IoT microcontroller |
| DHT11 | Temperature and humidity sensing |
| LDR | Light intensity sensing |
| Relay Module | Electrical load control |
| LED | Output indicator |
| Breadboard | Circuit prototyping |
| Jumper Wires | Connections |
| USB Cable | Programming and power |
| Resistors | Circuit/sensor interfacing |
| Bulb / Load | Demonstration output |

---

# 💻 Software & Technologies

### Programming

- C/C++
- HTML
- CSS
- JavaScript

### Development

- Arduino IDE
- ESP32 Board Package
- Serial Monitor

### Communication

- Wi-Fi
- HTTP
- MQTT

### Cloud Platforms

- Adafruit IO
- Firebase Realtime Database
- Firebase Authentication
- IFTTT

### Libraries

```text
WiFi.h
WebServer.h
Adafruit_MQTT.h
Adafruit_MQTT_Client.h
Firebase_ESP_Client.h
DHT.h
time.h
```

---

# 🌐 Communication Technologies

The repository demonstrates multiple communication models.

| Technology | Used For |
|---|---|
| HTTP | Browser → ESP32 communication |
| Wi-Fi | Network connectivity |
| MQTT | ESP32 ↔ Adafruit IO communication |
| IFTTT | Event-based automation |
| Firebase | Cloud data storage and control |
| Web APIs / Firebase SDK | Dashboard ↔ Cloud communication |

---

# 🌳 Repository Structure

Each task is maintained separately so that the implementation and learning progression can be viewed independently.

```text
IoT-ESP32-Projects/
│
├── Task 1
│   └── ESP32 Web Server
│
├── Task 2
│   └── Adafruit IO + MQTT
│
├── Task 3
│   └── IFTTT + Adafruit IO
│
├── Task 4
│   └── Firebase IoT Dashboard
│
└── Task 5
    └── Firebase Monitoring + Automation
```

The Git repository uses **separate branches for the individual tasks**, allowing each implementation to be maintained and reviewed independently.

---

# 🚀 Getting Started

## 1. Clone the Repository

```bash
git clone <repository-url>
cd <repository-name>
```

## 2. Select a Task Branch

```bash
git branch
```

Switch to the required task:

```bash
git checkout <task-branch>
```

---

## 3. Open the Arduino Project

Open the corresponding `.ino` file using:

**Arduino IDE**

Select:

```text
Board:
ESP32 Dev Module
```

Select the appropriate COM port and upload the program.

---

# 🔐 Configuration

Different tasks require different credentials.

### Task 1

Configure:

```cpp
WIFI_SSID
WIFI_PASSWORD
```

### Task 2 / Task 3

Configure:

```cpp
WIFI_SSID
WIFI_PASSWORD
ADAFRUIT_IO_USERNAME
ADAFRUIT_IO_KEY
```

### Task 4 / Task 5

Configure:

```text
Wi-Fi credentials
Firebase project
Firebase credentials
Database URL
Authentication configuration
```

Credentials should **not be committed to the repository**.

Use placeholders or environment/configuration files where appropriate.

---

# ⚠️ Hardware Safety

Some tasks use relay modules and may demonstrate control of electrical loads.

**Do not directly work with mains AC wiring unless you are trained and properly supervised.**

For development and testing, a low-voltage load or properly isolated demonstration setup is recommended.

The ESP32 should not be connected directly to a mains supply or mains load without an appropriate relay, isolation, protection, and safe wiring setup.

---

# 🧪 Testing

The projects were tested progressively using:

- Arduino Serial Monitor
- ESP32 hardware
- Browser interfaces
- Adafruit IO dashboards
- MQTT communication
- IFTTT automation
- Firebase Console
- Firebase Realtime Database
- Web dashboards
- Physical LED/bulb outputs

Testing focused on verifying both **software communication** and **physical hardware response**.

---

# 🧠 Learning Progression

The main purpose of this repository is to demonstrate the progression from basic embedded programming to complete IoT systems.

```text
Task 1
Local Device Control
        ↓
Task 2
Cloud Communication
        ↓
Task 3
Event-Based Automation
        ↓
Task 4
Cloud Monitoring
        ↓
Task 5
Monitoring + Automation
+ Historical Data
```

Through these tasks, the project explores how an IoT system can evolve from controlling a single GPIO pin to collecting sensor data, storing it in the cloud, providing remote control, automating physical devices, and maintaining historical records.

---

# 📚 Key Learning Outcomes

Through this project series, I gained practical experience in:

- ESP32 programming
- GPIO control
- Embedded web servers
- HTTP communication
- Wi-Fi networking
- MQTT publish/subscribe architecture
- Cloud IoT platforms
- Adafruit IO
- IFTTT automation
- Firebase Realtime Database
- Firebase Authentication
- Sensor integration
- Relay control
- Manual and automatic control systems
- Threshold-based automation
- Real-time cloud dashboards
- Historical data logging
- CSV data export
- Debugging IoT hardware and software
- Designing end-to-end IoT architectures

---

# 🔮 Future Improvements

The current implementations can be extended with:

- Better sensor calibration
- More accurate environmental sensors
- PIR-based occupancy detection
- Adaptive lighting algorithms
- Real-time charts and analytics
- Mobile application integration
- Push notifications
- Improved authentication and authorization
- Role-based access control
- Automated anomaly detection
- Energy consumption monitoring
- More advanced IoT analytics
- Secure MQTT/Firebase communication
- Improved database management for long-term data

---

# 👨‍💻 Project Context

This repository was developed as part of my **ProtoSem / Forge learning journey**, where the tasks progressively introduced embedded systems, electronics, IoT communication, cloud platforms, automation, and software-hardware integration.

Rather than treating each task as an isolated exercise, the project demonstrates the progression of an IoT solution from:

**Device → Network → Cloud → Automation → Monitoring → Data**

---

# 📌 Conclusion

This repository represents a progressive implementation of IoT concepts using the ESP32.

Starting with a simple browser-controlled LED, the project gradually introduces MQTT, cloud IoT platforms, IFTTT automation, Firebase, sensor monitoring, remote control, threshold-based automation, and historical data management.

The overall goal is to understand how **hardware, software, communication protocols, cloud services, and user interfaces work together to build practical IoT systems.**

---

## 🔗 Repository

The complete source code for all five tasks is available in this repository, with each task maintained in its respective Git branch.

**Explore the branches to view the individual implementations and development progression.**
