

#include <Arduino.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <esp_camera.h>

#include "config.h"
#include "wifi_init.h"
#include "camera_pins.h"
#include "camera_init.h"    // ★ 要加
#include "cam_stream.h"     
#include "cam_control.h"
#include "servo_api.h"

WebServer server(80);
WebSocketsServer wsServer(82);

void setup() 
{
    Serial.begin(115200);
    Serial.println("\nBooting ESP32 CAM");

    initWiFi();  
    initCamera();         // ★ 必須呼叫，不然 camera driver 不會被初始化

    initStreamWS(wsServer);
    initCameraAPI(server);
    initServoAPI(server);

    server.begin();
    wsServer.begin();
}

void loop() 
{
    server.handleClient();
    wsServer.loop();
    sendCameraFrame(wsServer);  // 只有 initCamera() 成功後，這才能讀到 frame
}
