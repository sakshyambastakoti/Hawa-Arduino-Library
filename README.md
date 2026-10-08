# Hawa — Universal Embedded Agent & OTA Library for ESP32 and ESP8266

[![Arduino Library](https://img.shields.io/badge/Arduino%20Library-v1.0.0-00979D.svg)](https://www.arduino.cc/)
[![Architectures](https://img.shields.io/badge/Architectures-ESP32%20%7C%20ESP8266-E7352C.svg)](https://www.espressif.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Framework](https://img.shields.io/badge/Framework-Arduino%20%7C%20PlatformIO-0288D1.svg)](https://platformio.org/)

**Hawa** is an embedded C++ client library for Espressif microcontrollers (ESP32 and ESP8266). It bridges field-deployed hardware with the centralized **HAWA Cloud Gateway** via secure WebSockets (TLS) and high-throughput Over-The-Air (OTA) firmware transport.

The library eliminates hardcoded Wi-Fi credentials and firmware recompilations. Devices boot autonomously from parameters stored in Non-Volatile Storage (NVS / EEPROM), establish persistent cloud links, report live sensor telemetry, process remote dashboard commands, and update application binaries in real time.

---

## Architectural Overview

```
+-------------------------------------------------------------+
|                  HAWA ARDUINO CLIENT LIBRARY                |
|                                                             |
|   +-----------------------+     +-----------------------+   |
|   |   HawaConfig (NVS)    |     |   HawaOTA Engine      |   |
|   |   - Wi-Fi Credentials |     |   - HTTP/HTTPS Stream |   |
|   |   - Gateway URL       |     |   - MD5 Validation    |   |
|   |   - Device ID & Name  |     |   - Partition Rollback|   |
|   +-----------+-----------+     +-----------+-----------+   |
|               |                             |               |
|               +--------------+--------------+               |
|                              |                              |
|                              v                              |
|               +-----------------------------+               |
|               |  WebSocketsClient (WS/WSS)  |               |
|               |  - Heartbeat & Telemetry    |               |
|               |  - Live Serial Forwarding   |               |
|               |  - Command Dispatcher       |               |
|               +--------------+--------------+               |
+------------------------------|------------------------------+
                               |
                   WSS / HTTPS | Internet / LAN
                               v
+-------------------------------------------------------------+
|                      HAWA CLOUD GATEWAY                     |
|            (Render.com / Docker / Self-Hosted)              |
+-------------------------------------------------------------+
```

---

## Key Capabilities

* **Zero-Credential Sketches:** Avoid storing plaintext Wi-Fi networks and passwords in firmware source files. Microcontrollers are provisioned once via the browser-based Web Serial Flasher and persist configurations in NVS (ESP32) or EEPROM (ESP8266).
* **Dual-Protocol OTA Streaming:** Downloads updates over high-speed local HTTP when on the same subnet as the host gateway, or over TLS HTTPS (`WiFiClientSecure`) when communicating through cloud endpoints or Cloudflare tunnels.
* **Firmware Safety & Anti-Brick Protection:** Compatible with ESP-IDF dual-partition rollback tables. New firmware confirms successful network initialization via `esp_ota_mark_app_valid_cancel_rollback()`. If an update fails, the bootloader automatically reverts to the previous stable partition.
* **Real-Time Remote Serial Forwarding:** `Hawa.log(...)` transmits diagnostic strings to the local hardware UART while simultaneously streaming them to connected dashboard operators worldwide.
* **Bi-Directional Command Interface:** Dispatch remote actions directly from the dashboard to execute callback routines on the microcontroller (e.g., relay switching, diagnostics, parameter changes).
* **Lightweight Telemetry Channel:** Push numerical or textual telemetry metrics with minimal memory allocation overhead.

---

## Installation

### Method 1: Arduino Library Manager (Recommended)

1. In the Arduino IDE, navigate to **Sketch** -> **Include Library** -> **Manage Libraries...** (`Ctrl + Shift + I`).
2. Type **`Hawa`** into the search field.
3. Select version `1.0.0` or higher and click **Install**.
4. When prompted to install missing dependencies (`ArduinoJson`, `WebSockets`), select **Install All**.

### Method 2: PlatformIO

Add the library and required dependencies to your `platformio.ini` environment:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    https://github.com/sakshyambastakoti/Hawa-Arduino-Library.git
    bblanchon/ArduinoJson @ ^6.21.3
    links2004/WebSockets @ ^2.4.1
```

### Method 3: Manual Installation via Git

Clone the repository directly into your local Arduino libraries directory:

**Windows (PowerShell):**
```powershell
cd "$HOME\Documents\Arduino\libraries"
git clone https://github.com/sakshyambastakoti/Hawa-Arduino-Library.git Hawa
```

**macOS / Linux:**
```bash
cd ~/Documents/Arduino/libraries
git clone https://github.com/sakshyambastakoti/Hawa-Arduino-Library.git Hawa
```

---

## Dependencies

The following libraries are required for compilation:

| Dependency | Minimum Version | Repository |
| :--- | :---: | :--- |
| **ArduinoJson** | `>= 6.18.0` | [github.com/bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson) |
| **WebSockets** | `>= 2.3.6` | [github.com/Links2004/arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets) |

---

## Quickstart

### Standard Deployment (Zero Hardcoded Credentials)

This is the recommended workflow. The board connects using credentials stored in flash storage via the Web Serial Flasher:

```cpp
#include <Arduino.h>
#include <Hawa.h>

void setup() {
    Serial.begin(115200);

    // Initializes hardware, loads NVS network parameters, and establishes cloud link
    Hawa.begin();

    // Register remote dashboard command listener
    Hawa.onCommand("relay", [](const String& state) {
        digitalWrite(2, state == "ON" ? HIGH : LOW);
        Hawa.log("Relay toggled to state: " + state);
    });

    pinMode(2, OUTPUT);
}

void loop() {
    // Keeps WebSocket, heartbeats, and OTA listeners responsive
    Hawa.loop();

    // Transmit telemetry periodically
    static unsigned long lastMetrics = 0;
    if (millis() - lastMetrics > 5000) {
        lastMetrics = millis();
        Hawa.sendData("heap_free", ESP.getFreeHeap());
        Hawa.sendData("uptime_sec", millis() / 1000);
    }
}
```

### Manual Network Specification

If hardcoded credentials are intentionally required for static deployments:

```cpp
#include <Arduino.h>
#include <Hawa.h>

void setup() {
    Serial.begin(115200);

    // Explicit network, gateway endpoint, and device identifier
    Hawa.begin(
        "MY_WIFI_SSID", 
        "MY_WIFI_PASSWORD", 
        "https://hawa-platform.onrender.com",
        "Sensor-Node-01"
    );
}

void loop() {
    Hawa.loop();
}
```

---

## API Reference

### Initialization & Lifecycle

#### `void Hawa.begin()`
Initializes non-volatile memory, reads stored Wi-Fi configurations and the gateway URL, connects to the local wireless access point, and initializes the WebSocket client.

#### `void Hawa.begin(const char* ssid, const char* pass, const char* serverUrl, const char* deviceName = "Hawa-Device")`
Initializes the system with explicit network parameters, commits them to local flash memory, and initiates network connection routines.

#### `void Hawa.loop()`
Executes internal socket polling, processes incoming OTA frames, answers heartbeat ping frames, and processes incoming commands. Must be invoked frequently inside Arduino's `loop()`. Avoid lengthy blocking operations (e.g., `delay()`) within your primary application loop.

---

### Observability & Telemetry

#### `void Hawa.log(const String& message)`
Outputs the string parameter to the local hardware `Serial` interface and transmits it over the active WebSocket connection to the cloud dashboard terminal.

#### `void Hawa.sendData(const String& key, float value)`
Transmits numerical telemetry data associated with `key` to the dashboard metrics engine.

#### `void Hawa.sendData(const String& key, const String& value)`
Transmits arbitrary string status metrics associated with `key`.

---

### Command Dispatching

#### `void Hawa.onCommand(const String& command, HawaCommandCallback callback)`
Binds a callback handler to a named remote action triggered from the operator dashboard.

```cpp
Hawa.onCommand("reboot", [](const String& payload) {
    Hawa.log("Reboot command acknowledged.");
    delay(500);
    ESP.restart();
});
```

---

### Device Diagnostics & Getters

| Method | Return Type | Description |
| :--- | :---: | :--- |
| `Hawa.isConnected()` | `bool` | Returns `true` if an active WebSocket session is established with the gateway. |
| `Hawa.getDeviceId()` | `String` | Returns the canonical hardware identifier (e.g., `hawa-esp32-a1b2c3d4e5f6`). |
| `Hawa.getIp()` | `String` | Returns the local IPv4 address assigned by DHCP. |
| `Hawa.getDeviceName()` | `String` | Returns the configured friendly device name. |
| `Hawa.getServerUrl()` | `String` | Returns the active gateway URL. |

---

## AI Assistant Prompt Template

When instructing AI coding models (such as Claude, ChatGPT, or Gemini) to generate firmware utilizing the Hawa library, provide the following specification prompt:

```text
Write an ESP32 Arduino sketch using the Hawa IoT library.
Requirements:
1. Include <Hawa.h>.
2. Call Hawa.begin() inside setup() without hardcoded Wi-Fi credentials (credentials load dynamically from NVS).
3. Call Hawa.loop() continuously inside loop(). Do not include blocking delay() calls.
4. Use Hawa.log(...) instead of standalone Serial.println(...) for diagnostics.
5. Use Hawa.sendData("metric_name", value) for periodic sensor reporting.
6. Register remote hardware controls with Hawa.onCommand("cmd_name", callback).
```

---

## Technical Specifications

| Parameter | Specification |
| :--- | :--- |
| **Supported Hardware** | ESP32 (WROOM, WROVER, S2, S3, C3), ESP8266 (ESP-01, NodeMCU, D1 Mini) |
| **Transport Layer** | WebSockets (RFC 6455) over TLS (WSS) or Plain (WS) |
| **OTA Transport** | HTTP / HTTPS (Chunked Stream with MD5 verification) |
| **Flash Partition Requirement** | Standard Dual-Partition (OTA0 / OTA1) with Spiffs/LittleFS |
| **UART Baud Rate** | 115200 bps |
| **NVS / EEPROM Footprint** | ~512 bytes for network and configuration parameters |

---

## License

This software is released under the MIT License. See [LICENSE](LICENSE) for full details.

**Author:** Sakshyam Bastakoti  
**Repository:** [https://github.com/sakshyambastakoti/Hawa-Arduino-Library](https://github.com/sakshyambastakoti/Hawa-Arduino-Library)