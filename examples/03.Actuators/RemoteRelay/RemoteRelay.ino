/*
  Hawa — Remote Relay & Actuator Control Example

  Demonstrates handling remote commands sent from the Hawa Web Dashboard.
*/

#include <Hawa.h>

#define RELAY_PIN 23

void setup() {
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

    // Register a remote command callback
    Hawa.onCommand("relay", [](const String& state) {
        if (state.equalsIgnoreCase("ON") || state == "1") {
            digitalWrite(RELAY_PIN, HIGH);
            Hawa.log("Remote Command: Relay turned ON");
        } else {
            digitalWrite(RELAY_PIN, LOW);
            Hawa.log("Remote Command: Relay turned OFF");
        }
    });

    Hawa.begin();
    Hawa.log("Smart Relay Node online and listening for dashboard commands.");
}

void loop() {
    Hawa.loop();
}
