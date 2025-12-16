#pragma once
#include <Arduino.h>

namespace camera_uart {

HardwareSerial UART(1);

// Camera TX → Controller RX = D9
// Camera RX ← Controller TX = D10
static const int TX_PIN = 9;
static const int RX_PIN = 10;

// ===== 解析後存放區（提供給 WebUI 用）=====
float param_frequency = 0;
float param_amplitude = 0;
float param_lambda = 0;
float param_L = 0;
float param_gain = 0;
uint16_t uptime_sec = 0;

struct ServoData {
    int target;
    int real;
    int error;
};
ServoData servo[6];

// ===== 封包緩衝 =====
uint8_t buf[128];
int idx = 0;
bool inPacket = false;
uint8_t expectedLen = 0;

void begin() {
    UART.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial.println("[CAM UART] Ready");
}

// --------------------------------------------------
// 傳送封包：AA | CMD | LEN | DATA... | CHECKSUM
// --------------------------------------------------
void sendPacket(uint8_t cmd, const uint8_t* data, uint8_t len) {
    uint8_t out[64];

    out[0] = 0xAA;
    out[1] = cmd;
    out[2] = len;

    for (int i = 0; i < len; i++)
        out[3 + i] = data[i];

    uint8_t sum = 0;
    for (int i = 1; i < 3 + len; i++)
        sum += out[i];

    out[3 + len] = ~sum;

    UART.write(out, 4 + len);
}

// --------------------------------------------------
// checksum
// --------------------------------------------------
bool verifyChecksum(uint8_t* p, int totalLen) {
    uint8_t sum = 0;
    for (int i = 1; i < totalLen - 1; i++)
        sum += p[i];
    return ((uint8_t)~sum) == p[totalLen - 1];
}

// --------------------------------------------------
// 處理控制板回傳資料
// --------------------------------------------------
void handlePacket(uint8_t cmd, uint8_t* payload, uint8_t len) {

    if (cmd == 0x30 && len == 22) {
        // 5 floats + uptime(2 bytes)
        memcpy(&param_frequency, &payload[0], 4);
        memcpy(&param_amplitude, &payload[4], 4);
        memcpy(&param_lambda,   &payload[8], 4);
        memcpy(&param_L,        &payload[12],4);
        memcpy(&param_gain,     &payload[16],4);

        uptime_sec = payload[20] | (payload[21] << 8);

        Serial.printf("[UART] Params F=%.2f A=%.1f λ=%.2f L=%.2f G=%.2f U=%d\n",
            param_frequency, param_amplitude, param_lambda, param_L, param_gain, uptime_sec);
    }

    else if (cmd == 0x40 && len == 36) {   // ★★★ 正確長度 6 * 6 bytes
        for (int i = 0; i < 6; i++) {
            servo[i].target = payload[i*6+0] | (payload[i*6+1] << 8);
            servo[i].real   = payload[i*6+2] | (payload[i*6+3] << 8);
            servo[i].error  = payload[i*6+4] | (payload[i*6+5] << 8);
        }
        Serial.println("[UART] Servo status updated.");
    }
}

// --------------------------------------------------
// UART RX（一定要在 loop() 呼叫）
// --------------------------------------------------
void update() {

    while (UART.available()) {
        uint8_t b = UART.read();

        if (!inPacket) {
            if (b == 0xAA) {
                inPacket = true;
                idx = 0;
                buf[idx++] = b;
            }
            continue;
        }

        buf[idx++] = b;

        if (idx == 3) expectedLen = buf[2];

        if (idx == 3 + expectedLen + 1) {

            if (!verifyChecksum(buf, idx)) {
                Serial.println("[UART] checksum error");
                inPacket = false;
                continue;
            }

            uint8_t cmd = buf[1];
            uint8_t* payload = &buf[3];

            handlePacket(cmd, payload, expectedLen);

            inPacket = false;
        }
    }
}

} // namespace camera_uart
