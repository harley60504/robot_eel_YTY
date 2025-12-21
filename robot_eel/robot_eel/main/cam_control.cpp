#include "cam_control.h"
#include "esp_camera.h"

void initCameraAPI(WebServer &server){

  server.on("/set", [&](){

    sensor_t *s = esp_camera_sensor_get();

    if(server.hasArg("brightness")){
      s->set_brightness(s, server.arg("brightness").toInt());
    }
    if(server.hasArg("contrast")){
      s->set_contrast(s, server.arg("contrast").toInt());
    }
    if(server.hasArg("quality")){
      s->set_quality(s, server.arg("quality").toInt());
    }

    server.send(200,"text/plain","OK");
  });
}
