#pragma once
#include <math.h>
#include "config.h"
#include "utils.h"
#include "cpg.h"

// =====================================================
// Servo 更新函式（單次更新，不含 FreeRTOS）
// main.cpp 裡的 servoTask() 會呼叫它
// =====================================================
inline void updateAndMoveServos(float timeSec, float dt)
{
    if (!isPaused)
    {
        // ----- CPG 模式 -----
        if (controlMode == 1)
        {
            updateAllCPG(timeSec, dt);
        }

        // ----- 逐顆 servo -----
        for (int j = 0; j < bodyNum; j++)
        {
            float outDeg = 0.0f;

            if (controlMode == 0)   // Sin 模式
            {
                float phase = j / fmaxf(lambda * L, 1e-6f);
                outDeg = Ajoint * sinf(2 * M_PI * frequency * timeSec + phase);
            }
            else if (controlMode == 1)  // CPG 模式
            {
                outDeg = getCPGOutput(j);
            }
            else  // Offset 模式
            {
                outDeg = 0.0f;
            }

            angleDeg[j] = servoDefaultAngles[j] + outDeg;

            int pos = degreeToLX224(angleDeg[j]);
            moveLX224(j + 1, pos, 15);  // 建議 10–20 ms
        }
    }
}

// =====================================================
// FreeRTOS Servo Task：在 main.cpp 中建立
// =====================================================
void servoTask(void *param);
