#pragma once
#include <math.h>
#include "config.h"
#include "utils.h"
#include "cpg.h"


void servoTask(void *pv) {
  const float dt = 0.05f;

  for (;;) {
    if (!isPaused) {
      float t = millis() / 1000.0f;

      for (int j = 0; j < bodyNum; j++) {
        float outDeg = 0.0f;

        if (controlMode == 0) {
          outDeg = Ajoint * sinf(
            j / fmaxf(lambda * L, 1e-6f)
            + 2.0f * PI * frequency * t
          );
          angleDeg[j] = servoDefaultAngles[j] + outDeg;

        } else if (controlMode == 1) {
          float fb_phase = 0.0f, fb_amp = 0.0f;

          // 未來 feedback 用這裡
          updateCPG(t, dt, j, fb_phase, fb_amp);
          outDeg = getCPGOutput(j);
          angleDeg[j] = servoDefaultAngles[j] + outDeg;

        } else {
          angleDeg[j] = servoDefaultAngles[j];
        }
        // ★ 關鍵：在這裡定義「目標角」
        servoState[j].targetDeg = angleDeg[j];
        int pos = degreeToLX224(angleDeg[j]);
        moveLX224(j + 1, pos, 50);
        
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }

}

void readservoTask(void *pv)
{
  vTaskDelay(pdMS_TO_TICKS(10)); // ← 錯開半週期
  while (1)
  {
    for (int j = 0; j < bodyNum; j++)
    {
      int pos = readPositionLX224(j + 1);

      // === 若讀取失敗，直接略過 ===
      if (pos < 0) {
        continue;
      }

      // === 更新 position ===
      servoState[j].actualPos = pos;

      // === position → degree ===
      float deg = lx224ToDegree(pos);
      servoState[j].actualDeg = deg;

      // === 計算角度誤差 ===
      servoState[j].errorDeg =
        servoState[j].targetDeg - servoState[j].actualDeg;
    }
    accumulateServoError();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

