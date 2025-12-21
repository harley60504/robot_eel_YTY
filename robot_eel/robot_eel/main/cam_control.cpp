#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include <esp_camera.h>

extern WebSocketsServer wsServer;   // 使用 main.ino 宣告的實例

void initControlWS(WebSocketsServer &ws)
{
    ws.onEvent([](uint8_t num, WStype_t type, uint8_t *payload, size_t length){

        if(type != WStype_TEXT) return;

        StaticJsonDocument<256> doc;

        if(deserializeJson(doc, payload)){
            wsServer.sendTXT(num, "{\"error\":\"bad json\"}");
            return;
        }

        sensor_t *s = esp_camera_sensor_get();

        const char* cmd = doc["cmd"];

        if(strcmp(cmd, "set_mode") == 0){
            int m = doc["value"] | 0;
            // TODO: 實際控制
            wsServer.sendTXT(num, "{\"ok\":true}");
            return;
        }

        if(strcmp(cmd, "servo_angle") == 0){
            int degree = doc["value"] | 0;
            // TODO: 實際 Servo UART
            wsServer.sendTXT(num, "{\"ok\":true}");
            return;
        }

        if(strcmp(cmd, "camera_param") == 0){

            if(doc.containsKey("brightness"))
                s->set_brightness(s, doc["brightness"]);

            if(doc.containsKey("contrast"))
                s->set_contrast(s, doc["contrast"]);

            if(doc.containsKey("quality"))
                s->set_quality(s, doc["quality"]);

            if(doc.containsKey("framesize"))
                s->set_framesize(s, (framesize_t)doc["framesize"]);

            wsServer.sendTXT(num, "{\"ok\":true}");
            return;
        }

        wsServer.sendTXT(num, "{\"error\":\"unknown cmd\"}");
    });
}
