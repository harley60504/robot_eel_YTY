#pragma once
#include <Arduino.h>
#include "esp_camera.h"
#include <ESPAsyncWebServer.h>

#define STREAM_BOUNDARY "frame"

class CameraStreamResponse : public AsyncAbstractResponse {
public:
    CameraStreamResponse() {
        _code = 200;
        _contentType = "multipart/x-mixed-replace; boundary=" STREAM_BOUNDARY;
        _sendContentLength = false;
    }

    bool _sourceValid() const override { return true; }

    size_t _fillBuffer(uint8_t *buf, size_t maxLen) override {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) return 0;

        String header = String("--" STREAM_BOUNDARY "\r\n") +
                        "Content-Type: image/jpeg\r\n" +
                        "Content-Length: " + String(fb->len) + "\r\n\r\n";

        size_t hlen = header.length();
        if (hlen + fb->len + 2 > maxLen) {
            esp_camera_fb_return(fb);
            return 0;
        }

        memcpy(buf, header.c_str(), hlen);
        memcpy(buf + hlen, fb->buf, fb->len);
        memcpy(buf + hlen + fb->len, "\r\n", 2);

        size_t total = hlen + fb->len + 2;
        esp_camera_fb_return(fb);
        return total;
    }
};
