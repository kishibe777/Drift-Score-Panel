#include "display.h"

void displayScore(int val) {
  mx.clear();
  char buf[5];
  snprintf(buf, sizeof(buf), "%04d", val);
  const int offsetModule = 1;
  const int decalagePixel = -3;
  for (int i = 0; i < 4; i++)
    mx.setChar(((3 - i) + offsetModule) * 8 + decalagePixel, buf[i]);
  mx.update();
}

void displayMaxDistance(float d) {
  mx.clear();
  char buf[6];
  snprintf(buf, sizeof(buf), "D=%02d", (int)d);
  const int offsetModule = 1, decalagePixel = -3;
  for (int i = 0; i < 4; i++)
    mx.setChar(((3 - i) + offsetModule) * 8 + decalagePixel, buf[i]);
  mx.update();
}

void displayVolumeLevel(int level) {
  mx.clear();

  // Décalage d'affichage : volume 1 → 0, volume 6 → 5
  int displayLevel = max(0, level - 1);

  char buf[6];
  snprintf(buf, sizeof(buf), "VO=%d", displayLevel);

  const int offsetModule = 1, decalagePixel = -3;
  for (int i = 0; i < 4 && buf[i]; i++)
    mx.setChar(((3 - i) + offsetModule) * 8 + decalagePixel, buf[i]);

  mx.update();
}

void displayVolumeBar(int level) {
  // Volume interne 1..6 → nombre de blocs remplis 0..5
  int filled = constrain(level - 1, 0, 5);

  mx.clear();

  const int totalBlocks = 5;       // 5 blocs fixes
  const int colsPerBlock = 6;      // 5 colonnes + 1 espace
  const uint8_t FULL_COL = 0xFF;   // bloc plein
  const uint8_t EMPTY_COL = 0x18;  // deux lignes centrales (bloc vide visuel)

  // 🔁 Dessin inversé (de droite à gauche)
  for (int i = 0; i < totalBlocks; i++) {
    int base = (totalBlocks - 1 - i) * colsPerBlock;  // bloc 0 = le plus à droite
    bool isFilled = (i < filled);                     // active les blocs de droite vers gauche

    for (int c = 0; c < 5; c++) {
      int col = base + c + 1;
      if (col >= 0 && col < PANEL_COLS) {
        mx.setColumn(col, isFilled ? FULL_COL : EMPTY_COL);
      }
    }
  }

  mx.update();
}

void displayMeasure(float cm) {
  mx.clear();
  char buf[6];
  snprintf(buf, sizeof(buf), "M=%02d", (int)cm);
  const int offsetModule = 1, decalagePixel = -3;
  for (int i = 0; i < 4; i++) {
    mx.setChar(((3 - i) + offsetModule) * 8 + decalagePixel, buf[i]);
  }
  mx.update();
}

