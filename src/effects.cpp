#include "config.h"
#include "car_anim.h"
#include "display.h"

// Remplace une attente bloquante par une boucle qui laisse respirer car_anim
static inline void waitWithCarAnim(uint32_t ms)
{
  uint32_t tEnd = millis() + ms;
  while (millis() < tEnd) {
    updateCarAnimation();
    // petit yield pour l'ordonnanceur ESP32
    delay(1);
  }
}

void flameAnimation() {
  if (carAnimRunning) {
    carAnimRunning = false;
    mx.clear();
    mx.update();
  }
  displayLocked = true;

  const int rows = 8, cols = PANEL_COLS;
  const int cx = cols / 2, cy = rows / 2;

  // ⚡ flash initial
  mx.control(MD_MAX72XX::INTENSITY, 15);
  for (int y = 0; y < rows; y++)
    for (int x = 0; x < cols; x++)
      mx.setPoint(y, x, true);
  mx.update();
  waitWithCarAnim(80);  // delay(80);

  // 💥 onde de choc
  for (int r = 1; r < 16; r++) {
    mx.clear();
    for (int y = 0; y < rows; y++)
      for (int x = 0; x < cols; x++) {
        float dx = x - cx, dy = y - cy;
        float dist = sqrtf(dx * dx + dy * dy);
        if (fabsf(dist - r / 2.0f) < 0.6f && random(0, 100) < 90)
          mx.setPoint(y, x, true);
      }
    mx.update();
    waitWithCarAnim(40);  // delay(40);
  }

  // ✨ étincelles
  for (int t = 0; t < 25; t++) {
    mx.clear();
    for (int i = 0; i < 25; i++) {
      int x = random(0, cols), y = random(0, rows);
      mx.setPoint(y, x, true);
    }
    mx.update();
    waitWithCarAnim(40);  // delay(40);
  }

  // extinction progressive
  for (int i = 15; i >= 0; i--) {
    mx.control(MD_MAX72XX::INTENSITY, i);
    waitWithCarAnim(20);  // delay(20);
  }

  mx.clear();
  mx.update();
  mx.control(MD_MAX72XX::INTENSITY, 8);
  displayLocked = false;
}

void introAnimation() {
  // 🌟 fondu d’allumage
  for (int i = 0; i <= 8; i++) {
    mx.control(MD_MAX72XX::INTENSITY, i);
    waitWithCarAnim(60);  // delay(60);
  }

  // 🔊 son d’intro
  if (dfReady) dfplayer.play(4);

  // 🚗 voiture avant texte
  startCarAnimation();
  while (carAnimRunning) {
    updateCarAnimation();
    delay(1);  // cadence douce (évite de saturer le CPU)
  }

  // 📝 texte
  parola.displayClear();
  // vitesse 30 déjà rapide ; si tu veux garder setSpeed global, mets speed=0 ici
  parola.displayText("Drift Score Panel", PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  while (!parola.displayAnimate()) {
    parola.displayAnimate();
    // pendant l’animation Parola, on garde la voiture fluide si relancée ailleurs
    updateCarAnimation();
    delay(1);
  }

  // 🔢 compteur
  for (int n = 0; n <= 9999; n += 250) {
    displayScore(n);
    waitWithCarAnim(50);  // delay(50);
  }

  mx.clear();
  mx.update();
}
