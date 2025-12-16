#pragma once
#include <Arduino.h>

namespace ctrl_uart {

// ================= UART 設定 =================
HardwareSerial UART(1);

static const int RX_PIN = 9;
static const int TX_PIN = 10;

// ================= RX Buffer =================
static uint8_t buf[32];
static int idx = 0;
static bool inPacket = false;

// ================= 初始化 =================
inline void begin() {
    UART.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial.println("CTRL UART Ready");
}

// ================= checksum =================
// 格式: [0]=0xAA [1]=CMD [2]=LEN [...] [LAST]=~sum
inline bool verifyChecksum(uint8_t* p, int totalLen) {
    uint8_t sum = 0;
    for (int i = 1; i < totalLen - 1; i++) {
        sum += p[i];
    }
    return (uint8_t)(~sum) == p[totalLen - 1];
}

// ================= TX =================
inline void sendPacket(uint8_t cmd, const uint8_t* payload, uint8_t len) {
    uint8_t pkt[32];
    int p = 0;

    pkt[p++] = 0xAA;   // Header
    pkt[p++] = cmd;    // CMD
    pkt[p++] = len;    // LEN

    for (int i = 0; i < len; i++) {
        pkt[p++] = payload[i];
    }

    uint8_t sum = 0;
    for (int i = 1; i < p; i++) {
        sum += pkt[i];
    }
    pkt[p++] = ~sum;

    UART.write(pkt, p);
}

// ================= RX =================
inline bool readPacket(uint8_t& cmd, uint8_t*& payload, uint8_t& len) {

    while (UART.available()) {
        uint8_t b = UART.read();

        // 等 header
        if (!inPacket) {
            if (b == 0xAA) {
                inPacket = true;
                idx = 0;
                buf[idx++] = b;
            }
            continue;
        }

        buf[idx++] = b;

        // 讀到 LEN
        if (idx == 3) {
            len = buf[2];
        }

        // 完整封包
        if (idx == 3 + len + 1) {

            if (!verifyChecksum(buf, idx)) {
                Serial.println("CTRL UART checksum error");
                inPacket = false;
                return false;
            }

            cmd = buf[1];
            payload = &buf[3];
            inPacket = false;
            return true;
        }
    }
    return false;
}

} // namespace ctrl_uart
