#pragma once
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "esp_wifi.h"
#include "config.h"

namespace wifi {

Preferences prefs;

// ----------------------
// 工具：嘗試連線一次
// ----------------------
bool try_connect(const char* ssid, const char* pass, uint16_t dots = 40)
{
    WiFi.begin(ssid, pass);
    Serial.printf("try connect: %s", ssid);

    for (int i = 0; i < dots && WiFi.status() != WL_CONNECTED; i++) {
        delay(300);
        Serial.print(".");
    }
    Serial.println();

    return WiFi.status() == WL_CONNECTED;
}

// ----------------------
// 從 NVS 載入 WiFi 清單
// ----------------------
std::vector<std::pair<String,String>> load_list()
{
    prefs.begin("wifi", true);
    String raw = prefs.getString("list", "[]");
    prefs.end();

    std::vector<std::pair<String,String>> list;

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, raw);

    for (JsonObject o : doc.as<JsonArray>()) {
        list.push_back({ o["ssid"].as<String>(),
                         o["pass"].as<String>() });
    }
    return list;
}

// ----------------------
// 儲存 WiFi 清單
// ----------------------
void save_list(const std::vector<std::pair<String,String>>& list)
{
    DynamicJsonDocument doc(2048);
    JsonArray arr = doc.to<JsonArray>();

    for (auto& w : list) {
        JsonObject o = arr.createNestedObject();
        o["ssid"] = w.first;
        o["pass"] = w.second;
    }

    String out;
    serializeJson(arr, out);

    prefs.begin("wifi", false);
    prefs.putString("list", out);
    prefs.end();
}

// ----------------------
// WiFi 啟動流程
// ----------------------
void begin()
{
    Serial.println("\n== wifi begin ==");

    WiFi.mode(WIFI_AP_STA);
    WiFi.setSleep(false);
    esp_wifi_set_ps(WIFI_PS_NONE);

    WiFi.softAP(AP_SSID, AP_PASS);
    delay(200);

    Serial.printf("AP: ssid=%s pass=%s ip=%s\n",
                  AP_SSID, AP_PASS,
                  WiFi.softAPIP().toString().c_str());

    // mDNS
    MDNS.end();
    if (MDNS.begin(HOSTNAME)) {
        MDNS.addService("http", "tcp", 80);
        Serial.printf("mDNS: http://%s.local\n", HOSTNAME);
    }

    // STA init
    WiFi.disconnect(true, true);
    delay(200);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(HOSTNAME);

    auto saved = load_list();
    bool ok = false;

    if (saved.size() > 0) {
        Serial.println("saved wifi list found");

        for (auto& w : saved) {
            Serial.printf("try: %s\n", w.first.c_str());
            if (try_connect(w.first.c_str(), w.second.c_str())) {
                ok = true;
                break;
            }
        }
    }

    if (ok) {
        Serial.printf("connected: %s\n", WiFi.SSID().c_str());
        Serial.printf("STA: http://%s.local (%s)\n",
                      HOSTNAME,
                      WiFi.localIP().toString().c_str());
    }
    else {
        Serial.println("wifi failed → AP only mode");
        WiFi.disconnect(true, true);
        WiFi.mode(WIFI_AP);
        delay(200);
        Serial.printf("AP only: http://%s\n", WiFi.softAPIP().toString().c_str());
    }

    Serial.println("== wifi ok ==\n");
}

} // namespace wifi
