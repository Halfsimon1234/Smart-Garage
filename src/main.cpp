#include <Arduino.h>
#include "wifi.h"
#include "camera.h"
#include "server.h"

void setup() {
    Serial.begin(115200);
    wifi_init();
    camera_init();
    server_start();
}

void loop() {
    // leer
}
