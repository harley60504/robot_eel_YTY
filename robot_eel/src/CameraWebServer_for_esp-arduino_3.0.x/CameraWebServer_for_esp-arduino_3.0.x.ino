#include <Arduino.h>
#include "wifi_mgr.h"
#include "camera.h"
#include "web_ui.h"
#include "camera_uart.h"

// ★★★ 真正定義（只能這裡有）★★★
AsyncWebServer server(80);
void setup() {
    Serial.begin(115200);
    delay(300);

    // WiFi (AP + STA)
    wifi::begin();

    // Camera 初始化
    camera_init();

    // UART → Controller
    camera_uart::begin();   // TX=D9, RX=D10

    // 啟動 Web UI + Camera Stream
    setupWebUI();

    Serial.println("📷 Camera Board Ready.");
}

void loop() {
    camera_uart::update();   // 必須：解析 Controller 回傳的 servo 狀態
}
