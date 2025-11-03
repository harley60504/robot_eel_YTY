#include "usb/usb_host.h"
#include "esp_log.h"

static const char* TAG = "USB_HOST_TEST";

#define VBUS_EN_GPIO 7  // 依照實際接線修改

void enable_vbus(bool on) {
  pinMode(VBUS_EN_GPIO, OUTPUT);
  digitalWrite(VBUS_EN_GPIO, on ? HIGH : LOW);
}

void usb_event_callback(const usb_host_client_event_msg_t *event_msg, void *arg) {
  switch (event_msg->event) {
    case USB_HOST_CLIENT_EVENT_NEW_DEV:
      Serial.printf("🔌 USB device connected! Address: %d\n", event_msg->new_dev.address);
      break;
    case USB_HOST_CLIENT_EVENT_DEV_GONE:
      Serial.println("❌ USB device disconnected!");
      break;
    default:
      Serial.printf("⚠️ Unknown USB event: %d\n", event_msg->event);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nBooting...");

  enable_vbus(true);
  Serial.println("VBUS enabled.");

  usb_host_config_t host_config = {
    .skip_phy_setup = false,
    .intr_flags = ESP_INTR_FLAG_LEVEL1
  };
  esp_err_t err = usb_host_install(&host_config);
  if (err != ESP_OK) {
    Serial.printf("usb_host_install failed: %d\n", err);
    while (1) delay(1000);
  }
  Serial.println("USB Host installed.");

  // 註冊 USB 客戶端，讓我們能收到插拔事件通知
  usb_host_client_config_t client_config = {
    .is_synchronous = false,
    .max_num_event_msg = 5,
    .async = { .client_event_callback = usb_event_callback, .callback_arg = NULL }
  };

  usb_host_client_handle_t client_hdl;
  usb_host_client_register(&client_config, &client_hdl);
  Serial.println("🧠 USB Host client ready, waiting for devices...");
}

void loop() {
  // 持續處理 USB 事件
  usb_host_client_handle_t client_hdl = NULL;  // 在正式程式中可保存 client handle
  usb_host_client_handle_events(client_hdl, 1000);
}
