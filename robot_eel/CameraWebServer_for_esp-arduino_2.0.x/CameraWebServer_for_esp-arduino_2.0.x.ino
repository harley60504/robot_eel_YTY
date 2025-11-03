#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

#define CAMERA_MODEL_XIAO_ESP32S3
#include "camera_pins.h"

// ===========================
// Wi-Fi 設定
// ===========================
const char* ssid = "YTY_2.4g";
const char* password = "weareytylab";

WebServer server(80);

// ===========================
// 主畫面 HTML
// ===========================
void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="UTF-8">
    <title>XIAO ESP32S3 高速相機</title>
    <style>
      body {
        background-color:#0a0a0a;
        color:#fff;
        text-align:center;
        font-family:"Segoe UI",sans-serif;
      }
      h1{color:#00e5ff;margin-top:10px;}
      #stream{
        width:800px;
        height:auto;
        margin-top:20px;
        border-radius:10px;
        box-shadow:0 0 25px rgba(0,255,255,0.4);
      }
    </style>
  </head>
  <body>
    <h1>⚡ XIAO ESP32S3 MJPEG 高畫質串流</h1>
    <div class="info">(320×240 @ ~25 FPS 優化模式)</div>
    <img id="stream" src="/stream"/>
  </body>
  </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

// ===========================
// MJPEG 串流
// ===========================
void handleStream() {
  WiFiClient client = server.client();
  String response =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  client.print(response);

  while (client.connected()) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) continue;

    client.printf("--frame\r\nContent-Type: image/jpeg\r\n\r\n");
    client.write(fb->buf, fb->len);
    client.printf("\r\n");
    esp_camera_fb_return(fb);
    delay(3);
  }
}

// ===========================
// 初始化相機
// ===========================
void setup() {
  Serial.begin(115200);
  Serial.println("\n🚀 啟動相機...");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
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

  config.xclk_freq_hz = 24000000;      
  config.pixel_format  = PIXFORMAT_JPEG;
  config.frame_size    = FRAMESIZE_QVGA;
  config.jpeg_quality  = 12;             
  config.fb_count      = 2;
  config.fb_location   = CAMERA_FB_IN_PSRAM;
  config.grab_mode     = CAMERA_GRAB_LATEST;

  if (!psramFound()) {
    Serial.println("⚠️ 未偵測 PSRAM，改用 DRAM");
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ 相機初始化失敗 0x%x\n", err);
    return;
  }
  Serial.println("✅ 相機初始化成功");

  sensor_t* s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  s->set_hmirror(s, 0);
  s->set_brightness(s, 1);
  s->set_contrast(s, 1);
  s->set_saturation(s, 1);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("📡 連線 Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("\n✅ Wi-Fi 已連線");
  Serial.print("📶 IP：");
  Serial.println(WiFi.localIP());

  // 路由
  server.on("/", handleRoot);
  server.on("/stream", handleStream);
  server.begin();

  Serial.println("🌐 伺服器啟動，開啟：");
  Serial.printf("➡️ http://%s/\n", WiFi.localIP().toString().c_str());
}

void loop() {
  server.handleClient();
}
