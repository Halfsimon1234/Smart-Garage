#include <Arduino.h>
#include <WiFi.h>
#include "../include/wifi.h"
#include "../include/config.h"

void wifi_connect() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    Serial.print("Verbinde mit WLAN");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WLAN verbunden!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
}