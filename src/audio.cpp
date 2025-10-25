#include "config.h"

static const int volumeSteps[6] = { 0, 6, 12, 18, 24, 30 };
int currentVolume = 3;

void initAudio() {
  // prefs déjà ouvert dans setup()
  currentVolume = prefs.getInt("volume", 3);
  currentVolume = constrain(currentVolume, 1, 6);

  mySerial.begin(9600, SERIAL_8N1, DF_TX, DF_RX);
  if (!dfplayer.begin(mySerial)) {
    Serial.println("❌ DFPlayer non détecté !");
    return;
  }
  dfplayer.volume(volumeSteps[currentVolume - 1]);
  Serial.printf("✅ DFPlayer prêt — Volume restauré : %d\n", volumeSteps[currentVolume - 1]);
}

void nextVolumeLevel() {
  currentVolume++;
  if (currentVolume > 6) currentVolume = 1;
  dfplayer.volume(volumeSteps[currentVolume - 1]);
  prefs.putInt("volume", currentVolume);
  Serial.printf("🔊 Volume réglé sur : %d\n", volumeSteps[currentVolume - 1]);
}
