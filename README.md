[README.md](https://github.com/user-attachments/files/23140461/README.md)
# 🏁 Tableau Score Drift

Un panneau de score interactif pour voiture de drift RC, basé sur ESP32-S3 SuperMini.

https://youtu.be/Xvv16W8SvOg?si=CKIVSCTOliZtr9Rx

## 🚗 Fonctionnalités
- Détection de distance via capteur ultrason HY-SRF05  
- Affichage sur matrice LED MAX7219 (4 modules)  
- Effets sonores via DFPlayer Mini + 8 ohms 3w speaker  
- Réglage de la distance max et du volume  
- Mode affichage de mesure en appui long

## ⚙️ Matériel
| Composant | Référence | Remarques |
|------------|------------|------------|
| Microcontrôleur | ESP32-S3 SuperMini | 240 MHz, Wi-Fi + USB natif |
| Capteur ultrason | HY-SRF05 | Mesure la distance de la voiture |
| Afficheur LED | MAX7219 x4 (8x32)| Affiche score et animations |
| Module audio | DFPlayer Mini / 8 ohms 3w speaker | Effets sonores |
| Bouton | Basic momentary switch | Contrôle du volume et mode mesure |
| Potentiomètre linéaire 10k | Rotary angle sensor V1.1 | Réglage de la distance de détection max (20 à 80 cm)
| Led RGB | DC5V-WS2812-3PIN | Effets lumineux à la détection |

## 🔌 Branchement
*comming soon*

## 🧰 Librairies Arduino
- 'MD_MAX72XX'
- 'MD_Parola'
- 'DFRobotDFPlayerMini'
- 'Adafruit_NeoPixel'

## ▶️ Installation
1. Cloner le dépôt  
   ```bash
   git clone https://github.com/kishibe777/Drift-Score-Panel.git
