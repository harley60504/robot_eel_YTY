#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include <Adafruit_ADS1X15.h>
#include <PL_ADXL355.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// LX-224
#define SERVO_TX_PIN 43
#define CMD_MOVE_TIME_WRITE 0x01
#define HEADER 0x55
#define BROADCAST_ID 0xFE
#define CMD_ID_WRITE 0x13
#define bodyNum 6


struct HopfOscillator {
  float r;
  float theta;
  float alpha;
  float mu;
};

extern WebServer server;

// WiFi
extern const char* AP_SSID;
extern const char* AP_PASS;
extern const char* HOSTNAME;


// Servo / params
extern float servoDefaultAngles[bodyNum];
extern float angleDeg[bodyNum];
extern float Ajoint;
extern float frequency;
extern float lambda;
extern float L;
extern float adsMinValidVoltage;
extern bool  isPaused;
extern int   controlMode;
extern bool  useFeedback;
extern float feedbackGain;

// CPG
extern HopfOscillator cpg[bodyNum];


// forward decl
void logADSDataEveryMinute();
