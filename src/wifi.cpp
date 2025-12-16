#include <WiFi.h>
#include <Arduino.h>
#include "config.h"
void wifi_init() 
{
    
    WiFi.begin(WIFI_SSID);
    Serial.print("Verbinde mit WLAN");

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWLAN verbunden!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
}
