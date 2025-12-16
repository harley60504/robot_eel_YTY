#pragma once
#include <Arduino.h>
#include "ctrl_uart.h"
#include "config.h"
#include "utils.h"    // degreeToLX224()
#include "servo.h"    // readPositionLX224()
                     // angleDeg[]

//
// ===============================
// 全域變數（由 main 定義）
// ===============================
extern float frequency;
extern float Ajoint;
extern float lambda;
extern float L;
extern float feedbackGain;

// ===============================
// 原本的 sendStatus() (CMD = 0x30)
// 傳參數、時間等基本資訊
// ===============================
inline void sendStatus() {
    uint8_t p[32];
    int idx = 0;

    auto push_float = [&](float v) {
        uint8_t* b = (uint8_t*)&v;
        p[idx++] = b[0];
        p[idx++] = b[1];
        p[idx++] = b[2];
        p[idx++] = b[3];
    };

    auto push_u16 = [&](uint16_t v) {
        p[idx++] = v & 0xFF;
        p[idx++] = v >> 8;
    };

    // Robot parameters
    push_float(frequency);
    push_float(Ajoint);
    push_float(lambda);
    push_float(L);
    push_float(feedbackGain);

    // uptime (sec)
    uint16_t up = millis() / 1000;
    push_u16(up);

    ctrl_uart::sendPacket(0x30, p, idx);
}


// ===============================
// ★ 新增：sendServoStatus() (CMD = 0x40)
// 傳送每顆 LX-224：
// 目標角度(target)、回傳角度(real)、誤差(error)
// ===============================
inline void sendServoStatus() {
    uint8_t p[64];
    int idx = 0;

    for (int j = 0; j < bodyNum; j++) {

        // 目標位置（0~1000）
        int tgt = degreeToLX224(angleDeg[j]);

        // 讀取真實角度（LX224 回傳）
        int pos = readPositionLX224(j + 1);
        if (pos < 0) pos = 0;

        // 誤差
        int err = tgt - pos;

        // pack target
        p[idx++] = tgt & 0xFF;
        p[idx++] = tgt >> 8;

        // pack real pos
        p[idx++] = pos & 0xFF;
        p[idx++] = pos >> 8;

        // pack error
        p[idx++] = err & 0xFF;
        p[idx++] = err >> 8;
    }

    ctrl_uart::sendPacket(0x40, p, idx);
}

