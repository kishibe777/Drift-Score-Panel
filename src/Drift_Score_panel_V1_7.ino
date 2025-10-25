#include "config.h"
#include "sensor.h"
#include "led_control.h"
#include "audio.h"
#include "car_anim.h"
#include "effects.h"
#include "display.h"
#include "drift_car.h"

// Objets globaux
MD_Parola parola(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
Adafruit_NeoPixel led(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
HardwareSerial mySerial(1);
DFRobotDFPlayerMini dfplayer;
Preferences prefs;

// États partagés
float distance = 0, maxDistance = 80;
int score = 0, bestScore = 0;
bool displayLocked = false;
bool dfReady = false;

// Locaux .ino
bool carDetected = false;
unsigned long lastDetection = 0;
int lastPotValue = 0;
unsigned long lastPotChange = 0;
bool showPotDisplay = false;
bool lastButtonState = HIGH;
unsigned long lastButtonTime = 0;
// --- Mode mesure directe ---
bool measureMode = false;  // indique si on est en mode "M=xx"
unsigned long buttonPressStart = 0;
bool buttonHeld = false;

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BTN_VOL, INPUT_PULLUP);

  mx.begin();
  parola.begin();
  parola.setIntensity(8);
  parola.displayClear();
  led.begin();
  led.show();

  prefs.begin("driftpanel", false);
  initAudio();     // restaure volume
  dfReady = true;  // si DFPlayer absent, il suffit d’ignorer les plays

  loadDriftCar();
  mx.control(MD_MAX72XX::INTENSITY, 8);

  introAnimation();
  Serial.println("=== Drift Score Panel — Modular OK ===");
}

void loop() {
  // === Gestion bouton volume / appui court & long ===
  bool btnState = digitalRead(BTN_VOL);

  if (btnState == LOW && !buttonHeld) {
    if (buttonPressStart == 0) {
      // première détection de l'appui
      buttonPressStart = millis();
    } else {
      unsigned long heldTime = millis() - buttonPressStart;

      // --- Appui long : bascule mode mesure ---
      if (heldTime > 2000) {
        buttonHeld = true;
        measureMode = !measureMode;
        mx.clear();

        if (measureMode) {
          Serial.println("🔍 Mode mesure directe activé");
          displayMeasure(0);
        } else {
          Serial.println("↩️ Sortie du mode mesure");
          displayScore(0);
        }
      }
    }
  } else if (btnState == HIGH && buttonPressStart != 0) {
    // Relâchement du bouton → déterminer si court
    unsigned long pressDuration = millis() - buttonPressStart;

    if (pressDuration < 500 && !buttonHeld && !measureMode) {
      // --- Appui court valide → changement de volume ---
      nextVolumeLevel();

      led.setPixelColor(0, 150, 150, 150);
      led.show();
      displayLocked = true;
      displayVolumeBar(currentVolume);

      unsigned long tEnd = millis() + 800;
      while (millis() < tEnd) {
        updateCarAnimation();
        delay(5);
      }

      displayLocked = false;
      led.setPixelColor(0, 0, 0, 0);
      led.show();
      if (!carAnimRunning) displayScore(bestScore);
    }

    // reset du suivi d’appui
    buttonPressStart = 0;
    buttonHeld = false;
  }


  // Potentiomètre → distance max
  int potValue = analogRead(POT_PIN);
  maxDistance = map(potValue, 0, 4095, 20, 80);
  if (abs(potValue - lastPotValue) > 120) {
    lastPotValue = potValue;
    lastPotChange = millis();
    showPotDisplay = true;
    displayLocked = true;
    displayMaxDistance(maxDistance);
    Serial.printf("🔧 maxDistance: %.1f cm\n", maxDistance);
  }
  if (showPotDisplay && millis() - lastPotChange > 1500) {
    showPotDisplay = false;
    displayLocked = false;
  }

  // --- Mode mesure directe (affiche M=xx en continu) ---
  if (measureMode) {
    float dist = getDistanceCM();

    if (dist > 0 && dist < maxDistance) {
      // ✅ Distance dans la plage → afficher la valeur en cm
      displayMeasure(dist);
    } else {
      // ⚠️ Distance hors plage → afficher "M=--"
      mx.clear();
      char buf[6];
      snprintf(buf, sizeof(buf), "M=--");
      const int offsetModule = 1, decalagePixel = -3;
      for (int i = 0; i < 4; i++) {
        mx.setChar(((3 - i) + offsetModule) * 8 + decalagePixel, buf[i]);
      }
      mx.update();
    }

    delay(100);  // rafraîchissement fluide
    return;      // 🔹 stoppe ici → ignore le reste du loop
  }

  if (!displayLocked) {
    // Mesure + LED
    distance = getDistanceCM();
    updateLedByDistance(distance, maxDistance);

    // Score (9999 à 3 cm)
    const float MIN_DIST_CM = 3.0f;
    if (distance > 0) {
      float denom = max(1.0f, (maxDistance - MIN_DIST_CM));
      float s = (maxDistance - distance) * 9999.0f / denom;
      score = (int)constrain(round(s), 0, 9999);
    } else score = 0;
    if (score > bestScore) bestScore = score;

    // Détection
    if (distance > 0 && distance < maxDistance) {
      if (!carDetected && !carAnimRunning) startCarAnimation();
      carDetected = true;
      lastDetection = millis();
    }
    // === Fin de passage ===
    else if (carDetected && millis() - lastDetection > 600) {
      Serial.printf("🏁 Fin du passage - meilleur score : %d\n", bestScore);

      // --- Son selon le score ---
      int soundIndex = 0;
      if (bestScore >= 9000) soundIndex = 1;
      else if (bestScore >= 5000) soundIndex = 2;
      else if (bestScore > 0) soundIndex = 3;
      if (dfReady && soundIndex > 0) dfplayer.play(soundIndex);

      // --- Animation spéciale si top score ---
      if (bestScore >= 9000) flameAnimation();

      // --- Effet blink + fade final ---
      int blinkCount = (bestScore >= 9000) ? 5 : 3;
      int fadeDelay = (bestScore >= 9000) ? 10 : 20;
      int offDelay = (bestScore >= 9000) ? 100 : 150;

      unsigned long fadeStart = millis();
      for (int i = 0; i < blinkCount; i++) {
        // montée lumineuse progressive
        for (int intensity = 0; intensity <= 15; intensity++) {
          mx.control(MD_MAX72XX::INTENSITY, intensity);
          if (!carAnimRunning) displayScore(bestScore);
          updateCarAnimation();
          delay(fadeDelay);
        }

        delay(100);  // pic de luminosité

        // descente plus rapide
        for (int intensity = 15; intensity >= 0; intensity--) {
          mx.control(MD_MAX72XX::INTENSITY, intensity);
          updateCarAnimation();
          delay(fadeDelay / 2);
        }

        mx.clear();
        mx.update();
        delay(offDelay);
      }

      // --- Réaffiche le score final ---
      mx.control(MD_MAX72XX::INTENSITY, 10);
      if (!carAnimRunning) displayScore(bestScore);

      // --- Maintien du score affiché un court moment ---
      unsigned long scoreHoldEnd = millis() + 3000;
      while (millis() < scoreHoldEnd) {
        updateCarAnimation();
        delay(5);
      }

      // --- Reset passage ---
      bestScore = 0;
      carDetected = false;
      mx.control(MD_MAX72XX::INTENSITY, 8);
      led.setPixelColor(0, 0, 0, 0);
      led.show();
      if (!carAnimRunning) displayScore(0);
    }

    // === Aucun passage détecté ===
    else if (!carDetected) {
      if (!carAnimRunning) displayScore(0);
    }
    updateCarAnimation();
  }
}
