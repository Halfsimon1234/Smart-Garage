#include "../include/camera.h"
#include "../include/config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Pin-Definitionen für AI-Thinker ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void camera_init() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = Y2_GPIO_NUM;
    config.pin_d1       = Y3_GPIO_NUM;
    config.pin_d2       = Y4_GPIO_NUM;
    config.pin_d3       = Y5_GPIO_NUM;
    config.pin_d4       = Y6_GPIO_NUM;
    config.pin_d5       = Y7_GPIO_NUM;
    config.pin_d6       = Y8_GPIO_NUM;
    config.pin_d7       = Y9_GPIO_NUM;
    config.pin_xclk     = XCLK_GPIO_NUM;
    config.pin_pclk     = PCLK_GPIO_NUM;
    config.pin_vsync    = VSYNC_GPIO_NUM;
    config.pin_href     = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn     = PWDN_GPIO_NUM;
    config.pin_reset    = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_GRAYSCALE;  // Graustufen für Bewegungserkennung
    config.frame_size   = MOTION_FRAME_SIZE;
    config.jpeg_quality = 20;
    config.fb_count     = 1;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Kamera Fehler: 0x%x\n", err);
    } else {
        Serial.println("Kamera bereit!");
    }
}

camera_fb_t* camera_get_frame() {
    return esp_camera_fb_get();
}

bool camera_capture_and_check() {
    // Auf JPEG + hohe Auflösung umschalten für OCR
    sensor_t* s = esp_camera_sensor_get();
    s->set_framesize(s, OCR_FRAME_SIZE);
    s->set_pixformat(s, PIXFORMAT_JPEG);
    delay(100);

    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Foto fehlgeschlagen");
        s->set_framesize(s, MOTION_FRAME_SIZE);
        s->set_pixformat(s, PIXFORMAT_GRAYSCALE);
        return false;
    }

    // Bild per HTTP an Server schicken
    HTTPClient http;
    http.begin(SERVER_URL);
    http.setTimeout(10000);

    String boundary = "----ESP32Boundary";
    http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

    String body_start = "--" + boundary + "\r\n"
                        "Content-Disposition: form-data; name=\"image\"; "
                        "filename=\"plate.jpg\"\r\n"
                        "Content-Type: image/jpeg\r\n\r\n";
    String body_end = "\r\n--" + boundary + "--\r\n";

    size_t total = body_start.length() + fb->len + body_end.length();
    uint8_t* payload = (uint8_t*)malloc(total);

    if (!payload) {
        Serial.println("Kein Speicher!");
        esp_camera_fb_return(fb);
        return false;
    }

    memcpy(payload, body_start.c_str(), body_start.length());
    memcpy(payload + body_start.length(), fb->buf, fb->len);
    memcpy(payload + body_start.length() + fb->len, body_end.c_str(), body_end.length());

    int code = http.POST(payload, total);
    free(payload);
    esp_camera_fb_return(fb);

    bool open = false;
    if (code == 200) {
        String response = http.getString();
        Serial.println("Antwort: " + response);
        open = response.indexOf("\"open\":true") >= 0;
    } else {
        Serial.printf("HTTP Fehler: %d\n", code);
    }

    http.end();

    // Zurück auf Bewegungsmodus
    s->set_framesize(s, MOTION_FRAME_SIZE);
    s->set_pixformat(s, PIXFORMAT_GRAYSCALE);
    delay(100);

    return open;
}