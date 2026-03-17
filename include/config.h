#pragma once

// WLAN
#define WIFI_SSID       "EuerWLAN"      // z.B. "Zuhause_5G"
#define WIFI_PASSWORD   "EuerPasswort"  // euer WLAN Passwort

// Server — IP des Laptops (im Terminal: ipconfig auf Windows, ifconfig auf Mac)
#define SERVER_URL      "http://10.51.50.139:5000/check"  // eure Laptop-IP von vorhin!

// Hardware
#define RELAY_PIN       12

// Kamera-Modi
#define MOTION_FRAME_SIZE    FRAMESIZE_QVGA
#define OCR_FRAME_SIZE       FRAMESIZE_VGA