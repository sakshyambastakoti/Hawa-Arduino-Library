/*
  Hawa — Quickstart Example
  
  Demonstrates minimal integration of the Hawa IoT agent.
  Wi-Fi credentials and Cloudflare tunnel URLs are automatically loaded 
  from flash memory (NVS). No hardcoded passwords needed!
*/

#include <Hawa.h>

#define LED_PIN 2

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    // Initialize Hawa agent (auto-connects to saved Wi-Fi & Hawa Cloud Hub)
    Hawa.begin();

    Hawa.log("Hawa Quickstart Node Initialized!");
}

void loop() {
    // Keep Hawa OTA updates and Cloudflare tunnel alive
    Hawa.loop();

    // Your custom sketch code:
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 1000) {
        lastBlink = millis();
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        Hawa.log("Heartbeat blink: state " + String(digitalRead(LED_PIN)));
    }
}
