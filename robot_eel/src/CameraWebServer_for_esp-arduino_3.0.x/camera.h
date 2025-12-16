#pragma once
#include <Arduino.h>
#include "esp_camera.h"
#include "camera_pins.h"
#include "camera_stream.h"
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

inline void camera_init() {
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;

    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QVGA;   // ✔ S3 穩定
    config.jpeg_quality = 12;
    config.fb_count = 2;

    if (esp_camera_init(&config) != ESP_OK) {
        Serial.println("Camera init failed");
        return;
    }

    Serial.println("Camera OK");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
        req->send(200, "text/html",
            "<h2>Xiao ESP32-S3 Camera</h2>"
            "<img src=\"/stream\" style=\"width:100%\">");
    });

    server.on("/stream", HTTP_GET, [](AsyncWebServerRequest *req){
        req->send(new CameraStreamResponse());
    });

    server.on("/snapshot", HTTP_GET, [](AsyncWebServerRequest *req){
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            req->send(500, "text/plain", "Camera error");
            return;
        }
        AsyncWebServerResponse *res =
            req->beginResponse_P(200, "image/jpeg", fb->buf, fb->len);
        req->send(res);
        esp_camera_fb_return(fb);
    });
}
