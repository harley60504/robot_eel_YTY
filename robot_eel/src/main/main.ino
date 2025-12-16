#include <Arduino.h>
#include "config.h"
#include "utils.h"
#include "cpg.h"
#include "servo.h"
#include "ctrl_uart.h"
#include "status.h"

// -----------------------------
// Robot parameters（在 main 定義全域變數）
// -----------------------------
float servoDefaultAngles[bodyNum] = {120,120,120,120,120,120};
float angleDeg[bodyNum];

float Ajoint       = 20.0f;
float frequency    = 0.7f;
float lambda       = 0.7f;
float L            = 0.85f;

bool  isPaused     = false;
int   controlMode  = 2;     // 0=Sin, 1=CPG, 2=Offset
bool  useFeedback  = false;
float feedbackGain = 1.0f;

// CPG 陣列
HopfOscillator cpg[bodyNum];


// =======================================================
//  Servo Task
//  - 控制伺服器，永不被 WiFi / UART 卡住（固定 Core 1）
// =======================================================
void servoTask(void *param)
{
    TickType_t delayTick = 20 / portTICK_PERIOD_MS;  // 50Hz
    uint32_t lastMicros = micros();
    float t = 0;

    while (true)
    {
        if (!isPaused)
        {
            uint32_t now = micros();
            float dt = (now - lastMicros) * 1e-6f;
            lastMicros = now;
            t += dt;

            // ========== CPG 模式 ==========
            if (controlMode == 1)
                updateAllCPG(t, dt);

            // ======== 更新所有伺服角度 ========
            for (int j = 0; j < bodyNum; j++)
            {
                float outDeg = 0.0f;

                if (controlMode == 0) {                 // Sin 模式
                    float phase = j / fmaxf(lambda * L, 1e-6f);
                    outDeg = Ajoint * sinf(2 * PI * frequency * t + phase);
                }
                else if (controlMode == 1) {            // CPG 模式
                    outDeg = getCPGOutput(j);
                }
                else {                                  // OFFSET 模式
                    outDeg = 0;
                }

                angleDeg[j] = servoDefaultAngles[j] + outDeg;

                // 寫入 LX-224
                int pos = degreeToLX224(angleDeg[j]);
                moveLX224(j + 1, pos, 15);
            }
        }

        vTaskDelay(delayTick);
    }
}


// =======================================================
// UART Rx Task（解析 Camera 端指令）
// =======================================================
void uartRxTask(void *param)
{
    uint8_t cmd = 0;
    uint8_t *payload = nullptr;
    uint8_t len = 0;

    while (true)
    {
        if (ctrl_uart::readPacket(cmd, payload, len))
        {
            switch (cmd)
            {
                case 0x10:    // setMode
                    if (len == 1) {
                        controlMode = payload[0];
                        if (controlMode == 1) initCPG();
                        Serial.printf("[UART] Mode = %d\n", controlMode);
                    }
                    break;

                case 0x11:    // setAmplitude
                    if (len == 4) memcpy(&Ajoint, payload, 4);
                    Serial.printf("[UART] Amplitude = %.2f\n", Ajoint);
                    break;

                case 0x12:    // setFrequency
                    if (len == 4) memcpy(&frequency, payload, 4);
                    Serial.printf("[UART] Frequency = %.2f\n", frequency);
                    break;

                case 0x13:    // setLambda
                    if (len == 4) memcpy(&lambda, payload, 4);
                    Serial.printf("[UART] Lambda = %.2f\n", lambda);
                    break;

                case 0x14:    // setL
                    if (len == 4) memcpy(&L, payload, 4);
                    Serial.printf("[UART] L = %.2f\n", L);
                    break;

                case 0x15:    // setFeedbackGain
                    if (len == 4) memcpy(&feedbackGain, payload, 4);
                    Serial.printf("[UART] fbGain = %.2f\n", feedbackGain);
                    break;

                case 0x16:    // toggleFeedback
                    useFeedback = !useFeedback;
                    Serial.printf("[UART] Feedback = %d\n", useFeedback);
                    break;

                case 0x17:    // togglePause
                    isPaused = !isPaused;
                    Serial.printf("[UART] Pause = %d\n", isPaused);
                    break;

                default:
                    Serial.printf("[UART] Unknown cmd %02X\n", cmd);
                    break;
            }
        }

        vTaskDelay(2);
    }
}



// =======================================================
// Status Task（每秒回傳兩種資料）
//   CMD=0x30 → 基礎參數
//   CMD=0x40 → Servo 真實角度/誤差
// =======================================================
void statusTask(void *param)
{
    while (true) {
        sendStatus();
        sendServoStatus();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}



// =======================================================
// Setup
// =======================================================
void setup()
{
    Serial.begin(115200);
    delay(300);

    // Servo UART（RS485）
    Serial1.begin(115200, SERIAL_8N1, 43, 44);
    // uart_set_mode(UART_NUM_1, UART_MODE_RS485_HALF_DUPLEX);

    // Camera UART
    ctrl_uart::begin();

    // 初始化 CPG
    initCPG();

    // ---- Servo Task → Core 1 ----
    xTaskCreatePinnedToCore(
        servoTask, "ServoTask", 4096, NULL, 2, NULL, 1);

    // ---- UART Rx Task → Core 0 ----
    xTaskCreatePinnedToCore(
        uartRxTask, "UartRx", 4096, NULL, 1, NULL, 0);

    // ---- Status Task → Core 0 ----
    xTaskCreatePinnedToCore(
        statusTask, "StatusTask", 4096, NULL, 1, NULL, 0);

    Serial.println("A-Board Controller Ready.");
}


// =======================================================
// Loop（空著）
// =======================================================
void loop() {}
