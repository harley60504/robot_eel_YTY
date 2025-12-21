#include "servo_api.h"
#include "config.h"
#include <ArduinoJson.h>

float target[SERVO_COUNT]={10,8,6,4};
float actual[SERVO_COUNT]={9,8,7,3};

void initServoAPI(WebServer &server){

  server.on("/servo_status", [&](){

    StaticJsonDocument<256> doc;
    JsonArray arr = doc.createNestedArray("servos");

    for(int i=0;i<SERVO_COUNT;i++){
      JsonObject o = arr.createNestedObject();
      o["id"]=i+1;
      o["target"]=target[i];
      o["actual"]=actual[i];
      o["error"]=target[i]-actual[i];
    }

    String json;
    serializeJson(doc,json);

    server.send(200,"application/json",json);
  });
}
