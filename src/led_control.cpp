#include "config.h"

void updateLedByDistance(float currentDistance, float maxDist) {
  if (currentDistance <= 0 || currentDistance > maxDist) {
    led.setPixelColor(0, 0, 0, 0);
    led.show();
    return;
  }
  const float MIN_DIST_CM = 3.0f;  // rouge à ~3 cm
  float denom = max(1.0f, (maxDist - MIN_DIST_CM));
  float ratio = (maxDist - currentDistance) / denom;  // 0..1
  ratio = constrain(ratio, 0.0f, 1.0f);

  uint8_t r, g;
  if (ratio < 0.5f) { r = (uint8_t)(ratio * 2.0f * 255.0f); g = 255; }
  else              { r = 255; g = (uint8_t)((1.0f - (ratio - 0.5f)*2.0f) * 255.0f); }

  led.setPixelColor(0, r, g, 0);
  led.show();
}
