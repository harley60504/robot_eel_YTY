#include "driver/uart.h"

#define HEADER 0x55
#define CMD_MOVE_TIME_WRITE 1
#define CMD_READ_POS 28

//--------------------------------------------------
// 計算 checksum
//--------------------------------------------------
uint8_t checksum(const uint8_t *buf, uint8_t len) {
  uint16_t sum = 0;
  for (int i = 2; i < len - 1; i++) sum += buf[i];
  return (uint8_t)(~sum);
}

//--------------------------------------------------
// 發封包
//--------------------------------------------------
void writePacket(uint8_t id, uint8_t cmd, const uint8_t *payload, uint8_t plen) {
  uint8_t len = 3 + plen;
  uint8_t buf[32];
  int idx = 0;

  buf[idx++] = HEADER;
  buf[idx++] = HEADER;
  buf[idx++] = id;
  buf[idx++] = len;
  buf[idx++] = cmd;

  for (int i = 0; i < plen; i++) buf[idx++] = payload[i];

  buf[idx] = checksum(buf, idx + 1);

  Serial1.write(buf, idx + 1);
}

//--------------------------------------------------
// LX224 MOVE
//--------------------------------------------------
void moveLX224(uint8_t id, int pos, uint16_t time_ms) {
  if (pos < 0) pos = 0;
  if (pos > 1000) pos = 1000;

  uint8_t p[4];
  p[0] = pos & 0xFF;
  p[1] = pos >> 8;
  p[2] = time_ms & 0xFF;
  p[3] = time_ms >> 8;

  writePacket(id, CMD_MOVE_TIME_WRITE, p, 4);
}

//--------------------------------------------------
// LX224 READ POSITION
//--------------------------------------------------
int readPositionLX224(uint8_t id) {
    // 清空舊資料避免混亂
    while (Serial1.available()) Serial1.read();

    // 發送讀取指令包
    uint8_t pkg[6];
    pkg[0] = 0x55;
    pkg[1] = 0x55;
    pkg[2] = id;
    pkg[3] = 3;
    pkg[4] = CMD_READ_POS;
    pkg[5] = checksum(pkg, 6);

    Serial1.write(pkg, 6);
    Serial1.flush();

    // 等伺服回應—延長到 3ms（最穩）
    delayMicroseconds(3000);

    // 等待至少 7 bytes（LX224 固定長度）
    unsigned long t0 = micros();
    while (Serial1.available() < 7) {
        if (micros() - t0 > 5000) return -1; // timeout 5ms
    }

    // 尋找封包起頭 0x55 0x55
    uint8_t h1 = 0, h2 = 0;
    while (Serial1.available()) {
        h1 = Serial1.read();
        if (h1 == 0x55) {
            if (Serial1.available()) {
                h2 = Serial1.read();
                if (h2 == 0x55) break; // 找到 header！
            }
        }
    }

    if (h1 != 0x55 || h2 != 0x55) return -1;

    // 讀剩下 5 bytes
    while (Serial1.available() < 5) {}

    uint8_t sid = Serial1.read();
    uint8_t len = Serial1.read();
    uint8_t cmd = Serial1.read();
    uint8_t posL = Serial1.read();
    uint8_t posH = Serial1.read();

    return posL | (posH << 8);
}


//--------------------------------------------------
// SETUP
//--------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(300);

  // XIAO ESP32S3 S 口 UART 腳位：
  // TX = GPIO 43 (D6)
  // RX = GPIO 44 (D7)
  Serial1.begin(115200, SERIAL_8N1, 44, 43);

  // ⭐⭐ 最關鍵：啟動半雙工模式，S 口才能接收 ⭐⭐
  uart_set_mode(UART_NUM_1, UART_MODE_RS485_HALF_DUPLEX);

  Serial.println("LX224 Bus Driver (S Port) Half-Duplex Ready");
}

//--------------------------------------------------
// LOOP
//--------------------------------------------------
void loop() {
  Serial.println("→ Move to 300");
  moveLX224(1, 300, 500);
  delay(700);

  int p1 = readPositionLX224(1);
  Serial.print("LX224 Position = ");
  Serial.println(p1);

  delay(700);

  Serial.println("→ Move to 700");
  moveLX224(1, 700, 500);
  delay(700);

  int p2 = readPositionLX224(1);
  Serial.print("LX224 Position = ");
  Serial.println(p2);

  delay(1000);
}
