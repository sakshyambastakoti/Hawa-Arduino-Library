# Hawa (हावा) — Arduino Library

[![Arduino Library](https://img.shields.io/badge/Arduino-Library-00979D.svg)](https://www.arduino.cc/)
[![Architectures](https://img.shields.io/badge/Architectures-ESP32%20%7C%20ESP8266-orange.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**Hawa (हावा)** is a lightweight, zero-friction IoT agent library for **ESP32** and **ESP8266**. It connects your microcontrollers to the **Hawa Web Command Center** across local networks and remote Cloudflare tunnels.

With Hawa, you can flash, provision, remotely monitor, and update your devices Over-The-Air (OTA) directly from a web browser without ever losing connectivity or hardcoding passwords into sketches.

---

## Key Features

* **Zero-Password Sketches**: Wi-Fi credentials and Cloudflare Tunnel URLs are provisioned via browser Web Serial and stored in **Non-Volatile Storage (NVS / EEPROM)**. Sketches never need hardcoded passwords!
* **Continuous Over-The-Air (OTA)**: Dual-mode updater supports direct local LAN HTTP streaming and remote Cloudflare HTTPS via `WiFiClientSecure`.
* **Smart Binary Support**: Automatically detects and extracts application images from merged `0x0000` binaries or regular sketch `.bin` files.
* **Live Remote Logs**: `Hawa.log("...")` prints to local Serial and streams in real time to the browser terminal.
* **Cloud Telemetry**: Send numeric and string metrics (`Hawa.sendData("temp", 24.5)`) to the dashboard.
* **Remote Commands**: Control GPIOs, relays, and hardware logic from the dashboard.
* **Anti-Brick Safety**: ESP32 partition rollback validation prevents bad OTA flashes from bricking your device.

---

## Quickstart

```cpp
#include <Hawa.h>

void setup() {
    Serial.begin(115200);

    // 1. Initialize Hawa: auto-loads Wi-Fi & Tunnel from flash memory
    Hawa.begin();

    // 2. Register dashboard remote commands
    Hawa.onCommand("relay", [](const String& state) {
        digitalWrite(2, state == "ON" ? HIGH : LOW);
        Hawa.log("Relay toggled to: " + state);
    });

    pinMode(2, OUTPUT);
}

void loop() {
    // 3. Keep OTA updater and cloud connection alive
    Hawa.loop();

    // 4. Send telemetry data anytime
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 5000) {
        lastSend = millis();
        Hawa.sendData("temperature", 24.5);
    }
}
```

---

## Installation

### Method 1: Git Clone into Arduino Libraries Folder
Clone this repository directly into your local Arduino libraries directory:

**Windows (PowerShell):**
```powershell
cd "$HOME\Documents\Arduino\libraries"
git clone https://github.com/sakshyambastakoti/Hawa-Arduino-Library.git Hawa
```

### Method 2: PlatformIO
Add to your `platformio.ini`:
```ini
lib_deps =
    https://github.com/sakshyambastakoti/Hawa-Arduino-Library.git
    bblanchon/ArduinoJson@^6.21.3
    links2004/WebSockets@^2.4.1
```

---

## Dependencies
Hawa requires these standard libraries:
* **[ArduinoJson](https://github.com/bblanchon/ArduinoJson)** (>= 6.18.0)
* **[WebSockets](https://github.com/Links2004/arduinoWebSockets)** (>= 2.3.6)

---

## AI Prompt Template (For ChatGPT, Claude, etc.)

When asking an AI coding assistant to write code for your ESP32/ESP8266, simply paste this snippet:

> *"Write an ESP32 Arduino sketch for [YOUR PROJECT DESCRIPTION].  
> Include `<Hawa.h>`, call `Hawa.begin()` in `setup()`, and `Hawa.loop()` in `loop()`.  
> Do NOT hardcode any Wi-Fi SSID, password, or server URL (Hawa loads them from NVS).  
> Use `Hawa.log(...)` for terminal logging and `Hawa.sendData(...)` for sensor telemetry."*

---

## API Reference

| Method | Description |
|---|---|
| `Hawa.begin()` | Starts the Hawa agent using credentials saved in flash (NVS). |
| `Hawa.begin(ssid, pass, url, name)` | Starts Hawa with explicit credentials and saves them to flash. |
| `Hawa.loop()` | Must be called inside `loop()` to process WebSockets and OTA updates. |
| `Hawa.log(message)` | Prints message to `Serial` and streams it to the remote Web Dashboard. |
| `Hawa.sendData(key, floatVal)` | Transmits numeric sensor telemetry to the dashboard. |
| `Hawa.sendData(key, stringVal)` | Transmits string status telemetry to the dashboard. |
| `Hawa.onCommand(name, callback)` | Registers a remote command handler called from the dashboard. |
| `Hawa.isConnected()` | Returns `true` if connected to the Hawa Cloud Hub. |
| `Hawa.getDeviceId()` | Returns the unique device identifier (e.g. `hawa-esp32-885721b64370`). |
| `Hawa.getIp()` | Returns the local IP address assigned by Wi-Fi router. |

---

## License
MIT License © 2026 Sakshyam Bastakoti
