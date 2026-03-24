#include <Arduino.h>
#include "../include/config.h"
#include "../include/dominik.h"
#include "../include/camera.h"
#include "../include/gate.h"

// Bewegungserkennung Einstellungen
#define MOTION_THRESHOLD    30      // Wie stark muss sich ein Pixel ändern? (0-255)
#define MOTION_MIN_PIXELS   500     // Wie viele Pixel müssen sich ändern?
#define COOLDOWN_MS         8000    // 8 Sekunden warten nach einer Erkennung

uint8_t* prev_frame = nullptr;
unsigned long last_trigger_ms = 0;

bool detect_motion(camera_fb_t* current) {
    // Beim ersten Aufruf: nur speichern, noch kein Vergleich möglich
    if (prev_frame == nullptr) {
        prev_frame = (uint8_t*)malloc(current->len);
        if (prev_frame) memcpy(prev_frame, current->buf, current->len);
        return false;
    }

    // Pixel für Pixel vergleichen
    int changed_pixels = 0;
    for (size_t i = 0; i < current->len; i++) {
        int diff = abs((int)current->buf[i] - (int)prev_frame[i]);
        if (diff > MOTION_THRESHOLD) 
        {
            changed_pixels++;
        }
    }

    // Aktuellen Frame speichern für nächsten Vergleich
    memcpy(prev_frame, current->buf, current->len);

    Serial.printf("Veränderte Pixel: %d\n", changed_pixels);
    return changed_pixels > MOTION_MIN_PIXELS;
}

void setup() {
    Serial.begin(115200);
    wifi_connect();
    camera_init();
    gate_init();
    Serial.println("System bereit — warte auf Bewegung...");
}

void loop() 
{
    bool in_cooldown = (millis() - last_trigger_ms) < COOLDOWN_MS;

    // Frame holen für Bewegungserkennung
    camera_fb_t* fb = camera_get_frame();
    if (!fb) 
    {
        Serial.println("Kamera Fehler");
        delay(500);
        return;
    }

    if (!in_cooldown && detect_motion(fb)) {
        Serial.println("Bewegung erkannt! Kennzeichen wird geprüft...");
        esp_camera_fb_return(fb);

        // Kurz warten damit Auto gut positioniert ist
        delay(300);

        bool access = camera_capture_and_check();
        if (access) {
            gate_open();
        } else {
            Serial.println("Zugang verweigert.");
        }

        last_trigger_ms = millis();

    } else {
        esp_camera_fb_return(fb);
    }

    delay(200);

}