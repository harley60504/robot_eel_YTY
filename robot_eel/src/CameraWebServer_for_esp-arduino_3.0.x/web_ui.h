#pragma once
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "index_html.h"
#include "camera_uart.h"
#include "camera_stream.h"

extern AsyncWebServer server;

inline void setupWebUI() {

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
        req->send_P(200, "text/html", INDEX_HTML);
    });

    // MJPEG Stream
    server.on("/cam", HTTP_GET, [](AsyncWebServerRequest *req){
        req->send(new CameraStreamResponse());
    });

    // Status JSON
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req){
        StaticJsonDocument<2048> doc;

        doc["frequency"] = camera_uart::param_frequency;
        doc["amplitude"] = camera_uart::param_amplitude;
        doc["lambda"]    = camera_uart::param_lambda;
        doc["L"]         = camera_uart::param_L;
        doc["fbGain"]    = camera_uart::param_gain;

        JsonArray arr = doc.createNestedArray("servo");
        for (int i = 0; i < 6; i++) {
            JsonObject o = arr.createNestedObject();
            o["id"]     = i + 1;
            o["target"] = camera_uart::servo[i].target;
            o["read"]   = camera_uart::servo[i].real;
            o["error"]  = camera_uart::servo[i].error;
        }

        String json;
        serializeJson(doc, json);
        req->send(200, "application/json", json);
    });

    server.begin();
    Serial.println("Web UI Ready");
}
