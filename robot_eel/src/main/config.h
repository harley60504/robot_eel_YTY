#pragma once
#include <Arduino.h>

#define bodyNum 6  // 六顆 LX224

struct HopfOscillator {
    float r;
    float theta;
    float alpha;
    float mu;
};

// Robot parameters
extern float servoDefaultAngles[bodyNum];
extern float angleDeg[bodyNum];
extern float Ajoint;
extern float frequency;
extern float lambda;
extern float L;
extern bool  isPaused;
extern int   controlMode;
extern bool  useFeedback;
extern float feedbackGain;

// CPG
extern HopfOscillator cpg[bodyNum];

// =====================================================
// LX224 Servo Protocol Definitions
// =====================================================

// 封包 Header
#define HEADER 0x55

// LX224 指令碼（依 LewanSoul / LX 系列通用協議）
#define CMD_MOVE_TIME_WRITE        0x01  // 設定位置 + 時間
#define CMD_MOVE_TIME_READ         0x02
#define CMD_MOVE_TIME_WAIT_WRITE   0x07
#define CMD_MOVE_TIME_WAIT_READ    0x08

#define CMD_ID_WRITE               0x13  // 設定 Servo ID
#define CMD_READ_POS               0x15  // 讀取目前位置