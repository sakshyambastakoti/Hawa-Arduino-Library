/*
  Hawa — Live Telemetry Sensor Example

  Demonstrates streaming sensor metrics (temperature, humidity, voltage) 
  directly to the Hawa Web Dashboard in real-time.
*/

#include <Hawa.h>

void setup() {
    Serial.begin(115200);

    // Initialize Hawa
    Hawa.begin();

    Hawa.log("Weather Telemetry Node Active");
}

void loop() {
    Hawa.loop();

    // Broadcast sensor telemetry every 3 seconds
    static unsigned long lastSensorRead = 0;
    if (millis() - lastSensorRead > 3000) {
        lastSensorRead = millis();

        // Simulated sensor values (replace with your DHT22 / BME280 code):
        float temperature = 24.5 + (random(-15, 15) / 10.0);
        float humidity = 58.0 + (random(-20, 20) / 10.0);

        // Send directly to Hawa Command Center Dashboard
        Hawa.sendData("temperature", temperature);
        Hawa.sendData("humidity", humidity);

        Hawa.log("Telemetry updated: " + String(temperature, 1) + " C | " + String(humidity, 1) + " %");
    }
}
