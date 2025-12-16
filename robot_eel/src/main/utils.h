#pragma once
#include "config.h"
#include <math.h>

// =====================================================
// 工具函式
// =====================================================
static inline float clampf(float x, float lo, float hi) {
  return x < lo ? lo : (x > hi ? hi : x);
}

static inline float linmap(float x, float in_min, float in_max, float out_min, float out_max) {
  if (fabs(in_max - in_min) < 1e-6f) return out_min;
  float r = (x - in_min) / (in_max - in_min);
  return out_min + r * (out_max - out_min);
}

inline int degreeToLX224(float deg) {
  deg = clampf(deg, 0.0f, 240.0f);
  return (int)(deg / 240.0f * 1000.0f);
}

// =====================================================
// LX224 基礎封包工具
// =====================================================
inline uint8_t checksum(const uint8_t *buf) {
  uint16_t sum = 0;
  uint8_t length = buf[3];
  for (int i = 2; i < length + 2; ++i) sum += buf[i];
  return (uint8_t)(~sum);
}

inline void writePacket(uint8_t id, uint8_t cmd, const uint8_t *payload, uint8_t plen) {
  uint8_t len = 3 + plen;
  uint8_t buf[32];
  int idx = 0;

  buf[idx++] = HEADER;
  buf[idx++] = HEADER;
  buf[idx++] = id;
  buf[idx++] = len;
  buf[idx++] = cmd;

  for (int i = 0; i < plen; ++i)
    buf[idx++] = payload[i];

  buf[idx] = checksum(buf);

  Serial1.write(buf, idx + 1);
}

// =====================================================
// LX224 移動指令
// =====================================================
inline void moveLX224(uint8_t id, int position, uint16_t time_ms) {
  position = position < 0 ? 0 : (position > 1000 ? 1000 : position);

  uint8_t p[4];
  p[0] = position & 0xFF;
  p[1] = (position >> 8) & 0xFF;
  p[2] = time_ms & 0xFF;
  p[3] = (time_ms >> 8) & 0xFF;

  writePacket(id, CMD_MOVE_TIME_WRITE, p, 4);
}

inline void setServoID(uint8_t targetId, uint8_t newId) {
  uint8_t p[1] = { newId };
  writePacket(targetId, CMD_ID_WRITE, p, 1);
  delay(50);
}


// =====================================================
// LX224 回傳讀取（最穩版本）
// - 封包對齊
// - 等待資料完全到齊
// - CSV 紀錄
// =====================================================
#define DEBUG_READ 1   // ← 打開 debug print

inline int readPositionLX224(uint8_t id) {
    while (Serial1.available()) Serial1.read();

    uint8_t pkg[6] = {0x55, 0x55, id, 3, CMD_READ_POS, 0};
    pkg[5] = checksum(pkg);

    Serial1.write(pkg, 6);

    unsigned long t0 = micros();

    // 讀取 header (0x55, 0x55)
    uint8_t header[2];
    while (micros() - t0 < 5000) {
        if (Serial1.available()) {
            header[0] = header[1];
            header[1] = Serial1.read();
            if (header[0] == 0x55 && header[1] == 0x55) break;
        }
    }
    if (!(header[0] == 0x55 && header[1] == 0x55)) return -1;

    // 讀取剩餘固定 5 bytes
    uint8_t resp[5];
    for (int i = 0; i < 5; i++) {
        while (!Serial1.available()) {
            if (micros() - t0 > 8000) return -1;
        }
        resp[i] = Serial1.read();
    }

    // resp[0] = ID
    // resp[1] = LEN
    // resp[2] = CMD
    // resp[3] = POS_L
    // resp[4] = POS_H

    if (resp[0] != id) return -1;
    if (resp[2] != CMD_READ_POS) return -1;

    int pos = resp[3] | (resp[4] << 8);

#if DEBUG_READ
    Serial.printf("[OK] ID=%d POS=%d\n", id, pos);
#endif
    return pos;
}
