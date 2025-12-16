#include <WebServer.h>
#include "esp_camera.h"

WebServer server(80);

void handle_capture() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        server.send(500, "text/plain", "Kamera Fehler");
        return;
    }

    server.send(200, "image/jpeg", fb->buf, fb->len);
    esp_camera_fb_return(fb);
}

void server_start() {
    server.on("/capture", HTTP_GET, handle_capture);
    server.begin();
}
