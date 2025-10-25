#include "config.h"
#include "drift_car.h"

#define CAR_IMG_WIDTH 21
#define CAR_IMG_HEIGHT 8

uint8_t driftCarSprite[CAR_IMG_WIDTH];
bool carAnimRunning = false;
static int carAnimPos = 0;

static const int SMOKE_MAX = 6;
static int smokePos[SMOKE_MAX];
static uint8_t smokeLife[SMOKE_MAX];
static unsigned long lastCarAnimUpdate = 0;

void loadDriftCar() {
  for (int x = 0; x < CAR_IMG_WIDTH; x++) {
    uint8_t colByte = 0;
    for (int y = 0; y < CAR_IMG_HEIGHT; y++) {
      int idx = y * CAR_IMG_WIDTH + x;
      if (header_data[idx] == 1) colByte |= (1 << y);
    }
    driftCarSprite[x] = colByte;
  }
}

void startCarAnimation() {
  carAnimRunning = true;
  carAnimPos = -CAR_IMG_WIDTH;
  for (int i = 0; i < SMOKE_MAX; i++) { smokePos[i] = -100; smokeLife[i] = 0; }
}

void updateCarAnimation() {
  if (!carAnimRunning) return;

  unsigned long now = millis();
  if (now - lastCarAnimUpdate < 8) return;   // cadence ~80 FPS
  lastCarAnimUpdate = now;

  mx.clear();

  // Fumée
  if (random(0, 100) < 35) {
    for (int i = 0; i < SMOKE_MAX; i++)
      if (smokeLife[i] == 0) { smokePos[i] = carAnimPos - 2; smokeLife[i] = 8 + random(0, 4); break; }
  }
  for (int i = 0; i < SMOKE_MAX; i++) if (smokeLife[i]) {
    int fx = smokePos[i];
    for (int p = 0; p < 3; p++) {
      int cx = fx - p + random(-1, 2);
      int cy = 5 + random(-2, 3);
      if (cx >= 0 && cx < PANEL_COLS && cy >= 0 && cy < 8) mx.setPoint(cy, cx, true);
    }
    smokeLife[i]--; smokePos[i]--;
  }

  // Voiture (miroir corrigé)
  for (int col = 0; col < CAR_IMG_WIDTH; col++) {
    int c = carAnimPos + col;
    if (c >= 0 && c < PANEL_COLS) mx.setColumn(c, driftCarSprite[CAR_IMG_WIDTH - 1 - col]);
  }

  mx.update();
  carAnimPos += 1;
  if (carAnimPos > PANEL_COLS + CAR_IMG_WIDTH) { carAnimRunning = false; mx.clear(); mx.update(); }
}
