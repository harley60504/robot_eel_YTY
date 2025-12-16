#pragma once
#include <Arduino.h>

// ========================================
// WiFi / AP 設定
// ========================================
#define AP_SSID   "Robot_AP"
#define AP_PASS   "12345678"
#define HOSTNAME  "hexapod"

// ========================================
// Robot Parameter（控制參數）
// ========================================
extern float frequency;
extern float Ajoint;
extern float lambda;
extern float L;
extern float feedbackGain;

extern int controlMode;
extern bool useFeedback;

// ========================================
// Servo Config
// ========================================
// 你目前機器人使用 6 顆 LX-224
static const int bodyNum = 6;
