#pragma once
#include "config.h"
#include <Arduino.h>
#include <math.h>

// =====================================================
// Servo 狀態
// =====================================================
struct ServoState {
  float targetDeg = 0.0f;
  float actualDeg = 0.0f;
  float errorDeg  = 0.0f;
  int   targetPos = 0;
  int   actualPos = 0;
  unsigned long t_ms = 0;
};

static ServoState servoState[bodyNum];

// =====================================================
// 基本工具
// =====================================================
static inline float clampf(float x, float lo, float hi) {
  return x < lo ? lo : (x > hi ? hi : x);
}

// =====================================================
// 角度 <-> LX224
// =====================================================
static inline int degreeToLX224(float deg) {
  deg = clampf(deg, 0.0f, 240.0f);
  return (int)(deg / 240.0f * 1000.0f);
}

static inline float lx224ToDegree(int pos) {
  pos = pos < 0 ? 0 : (pos > 1000 ? 1000 : pos);
  return pos * (240.0f / 1000.0f);
}

// =====================================================
// LX-224 封包工具
// =====================================================
static inline uint8_t checksum(const uint8_t *buf) {
  uint16_t sum = 0;
  uint8_t len = buf[3];
  for (int i = 2; i < len + 2; ++i) sum += buf[i];
  return (uint8_t)(~sum);
}

static inline void writePacket(uint8_t id,
                               uint8_t cmd,
                               const uint8_t *payload,
                               uint8_t plen) {
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
// LX-224 MOVE
// =====================================================
static inline void moveLX224(uint8_t id,
                             int position,
                             uint16_t time_ms) {
  position = position < 0 ? 0 : (position > 1000 ? 1000 : position);

  uint8_t p[4];
  p[0] = position & 0xFF;
  p[1] = position >> 8;
  p[2] = time_ms & 0xFF;
  p[3] = time_ms >> 8;

  writePacket(id, CMD_MOVE_TIME_WRITE, p, 4);
}

// =====================================================
// LX-224 READ_POS（原樣保留）
// =====================================================
inline int readPositionLX224(uint8_t id) {
  while (Serial1.available()) Serial1.read();

  uint8_t pkg[6] = {0x55, 0x55, id, 3, CMD_READ_POS, 0};
  pkg[5] = checksum(pkg);
  Serial1.write(pkg, 6);

  unsigned long t0 = micros();
  uint8_t h[2] = {0, 0};

  while (micros() - t0 < 5000) {
    if (Serial1.available()) {
      h[0] = h[1];
      h[1] = Serial1.read();
      if (h[0] == 0x55 && h[1] == 0x55) break;
    }
  }
  if (!(h[0] == 0x55 && h[1] == 0x55)) return -1;

  uint8_t r[5];
  for (int i = 0; i < 5; i++) {
    while (!Serial1.available()) {
      if (micros() - t0 > 8000) return -1;
    }
    r[i] = Serial1.read();
  }

  if (r[0] != id) return -1;
  if (r[2] != CMD_READ_POS) return -1;

  return r[3] | (r[4] << 8);
}

// =====================================================
// 高層 API：送 MOVE（不讀）
// =====================================================
static inline void setServoDegree(uint8_t index,
                                  float deg,
                                  uint16_t time_ms = 50) {
  if (index >= bodyNum) return;

  ServoState &s = servoState[index];
  int pos = degreeToLX224(deg);

  s.targetDeg = deg;
  s.targetPos = pos;

  moveLX224(index + 1, pos, time_ms);
}

// =====================================================
// ★ 低頻、安全的 Feedback（你要的那種）
// =====================================================
static inline void updateServoFeedbackSlow() {
  static unsigned long lastReadMs = 0;

  // ★ 關鍵：慢速（500 ms，≈ 2 Hz）
  if (millis() - lastReadMs < 500) return;
  lastReadMs = millis();

  for (int i = 0; i < bodyNum; i++) {
    int pos = readPositionLX224(i + 1);
    if (pos < 0) continue;

    servoState[i].actualPos = pos;
    servoState[i].actualDeg = lx224ToDegree(pos);
    servoState[i].errorDeg  = servoState[i].actualDeg
                              - servoState[i].targetDeg;
    servoState[i].t_ms = millis();

    Serial.printf(
      "[FB] id=%d pos=%d deg=%.1f err=%.1f\n",
      i + 1,
      pos,
      servoState[i].actualDeg,
      servoState[i].errorDeg
    );
  }
}
void feedbackTask(void *pv) {
  for (;;) {
    updateServoFeedbackSlow();   // ★ 就放這裡
    vTaskDelay(50 / portTICK_PERIOD_MS); // task 自己跑很快也沒關係
  }
}