#include "../include/gate.h"
#include "../include/config.h"
#include <Arduino.h>

void gate_init() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);  // Relais aus beim Start
    Serial.println("Relais bereit.");
}

void gate_open() {
    Serial.println("Tor öffnet!");
    digitalWrite(RELAY_PIN, HIGH);  // Relais an
    delay(1000);                     // 1 Sekunde Impuls
    digitalWrite(RELAY_PIN, LOW);   // Relais aus
    Serial.println("Impuls gesendet.");
}