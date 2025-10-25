#pragma once
#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <DFRobotDFPlayerMini.h>
#include <Adafruit_NeoPixel.h>
#include <HardwareSerial.h>
#include <Preferences.h>

// === Brochage / panneau ===
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define DIN_PIN 13
#define CLK_PIN 14
#define CS_PIN 15
#define MAX_DEVICES 4
#define PANEL_COLS (MAX_DEVICES * 8)  // 32 colonnes visibles

// === Capteur / entrées ===
#define TRIG_PIN 4
#define ECHO_PIN 5
#define POT_PIN 2
#define BTN_VOL 10

// === Audio DFPlayer ===
#define DF_RX 17
#define DF_TX 18

// === LED WS2812 ===
#define LED_PIN 12
#define LED_COUNT 1

// === Objets globaux (instanciés dans .ino) ===
extern MD_Parola parola;
extern MD_MAX72XX mx;
extern Adafruit_NeoPixel led;
extern HardwareSerial mySerial;
extern DFRobotDFPlayerMini dfplayer;
extern Preferences prefs;

// === États partagés (définis dans .ino) ===
extern float distance, maxDistance;
extern int score, bestScore;
extern bool displayLocked;
extern bool dfReady;
