#pragma once
#include <math.h>
#include "config.h"
#include "utils.h"
#include "cpg.h"

inline void servoTask(void *pv) {
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

        // ★ 關鍵修改：用 util.h 高層 API
        setServoDegree(j, angleDeg[j], 50);
      }
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

