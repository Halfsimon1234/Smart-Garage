#pragma once
#include "esp_camera.h"

// Initialisiert die Kamera-Hardware
void camera_init();

// Macht ein Foto, schickt es an den Server
// Gibt true zurück wenn Zugang erlaubt, false wenn nicht
bool camera_capture_and_check();

// Gibt einen einzelnen Frame zurück (für Bewegungserkennung)
camera_fb_t* camera_get_frame();    