//////////////////////////////////////////////////////////////////////////////////////////
/*
   La NanoValise
   Copyright 2013-2021 - Eric Sérandour
   http://3615.entropie.org
*/
   const String VERSION = "2021.03.29"; // 23 h 37
/*   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
//////////////////////////////////////////////////////////////////////////////////////////
/*
   Compile sans problème avec Arduino 1.8.13.
   Pour téléverser le programme sur la carte Arduino avec le programmateur, enlever le
   module de carte SD afin d'éviter un conflit sur le bus SPI.
*/
//////////////////////////////////////////////////////////////////////////////////////////
/*
  L'électronique :
  
  * Carte Arduino Nano
  * Afficheur LCD alphanumérique 2 x 16 caractères DEM 16216 SGH + 1 potentiomètre de 1 kohms
  * Convertisseur de niveaux logiques bidirectionnel 5V / 3,3 V
  * Micro SD Card Adaptater Catalex
  * 2 boutons poussoir + 2 résistances de 10 kohms + 2 condensateurs de 10 nF
  * Capteur de CO2, température et humidité Sensirion SCD30

  Le circuit :
  
    Les entrées / sorties numériques :
  ======================================

  * Afficheur LCD sur D4, D5, D6, D7, D8, D9 de la carte Arduino
    Brochage de l'afficheur compatible HD44780 :
    1  : Vss : GND (fil noir)
    2  : Vdd : Power Supply +5V (fil rouge)
    3  : V0 : Contrast Adjust. Point milieu du potentiomètre 1 kohms (fil gris)
    4  : RS : Register Select Signal. => D9 de l'Arduino (fil blanc)
    5  : R/W : Data Read/Write. Relié à GND (fil noir)
    6  : E : Enable Signal. => D8 de l'Arduino (fil vert)
    7  : non connecté
    8  : non connecté
    9  : non connecté
    10 : non connecté
    11 : DB4 : Data Bus Line. => D7 de l'arduino (fil bleu)
    12 : DB5 : Data Bus Line. => D6 de l'arduino (fil marron)
    13 : DB6 : Data Bus Line. => D5 de l'arduino (fil violet)
    14 : DB7 : Data Bus Line. => D4 de l'arduino (fil jaune)
    15 : LED+ : Power supply for BKL(+). Relié à +5V (fil rouge) - Non connecté
    16 : LED- : Power supply for BKL(-). Relié à GND (fil noir) - Non connecté

  * Leds
    1 led verte en série avec une résistance de 270 ohms
    1 led orange en série avec une résistance de 270 ohms
    1 led rouge en série avec une résistance de 330 ohms

  * Clavier
  ** Bouton poussoir REC :  5V -> Bouton -> 10 kohms -> GND
                                         -> D2 de l'arduino (fil marron)
                            Condensateur de 10 nF aux bornes du bouton poussoir

  ** Bouton poussoir PLAY : 5V -> Bouton -> 10 kohms -> GND
                                         -> D3 de l'arduino (fil orange)
                            Condensateur de 10 nF aux bornes du bouton poussoir

  * La carte SD est reliée au bus SPI (bus série normalisé) de la manière suivante :
    CS   -> D10 (Voir la constante CHIP_SELECT)
    MOSI -> D11 (Master Out Slave In : Sortie de données séries)
    MISO -> D12 (Master In Slave Out : Entrée de données séries)
    SCK  -> D13 (Serial ClocK : Pour synchroniser les échanges de données)

  * Le capteur Sensirion SCD30 est relié de la manière suivante :
    Vin => 3,3 V de l'Arduino
    GND => GND
    SCL => LV4 (Convertisseur de niveaux logiques 3,3V / 5V) HV4 => A5 de l'Arduino (SCL) (fil vert)
    SDA => LV3 (Convertisseur de niveaux logiques 3,3V / 5V) HV3 => A4 de l'Arduino (SDA) (fil jaune)
*/
//////////////////////////////////////////////////////////////////////////////////////////

// *** Afficheur LCD
// On importe la bibliothèque
#include <LiquidCrystal.h>
// Initialisation de la bibliothèque avec les numéros de broches utilisées
LiquidCrystal lcd(9,8,7,6,5,4);   // Correspond sur l'afficheur à RS,Enable,DB4,DB5,DB6,DB7
const byte NB_LIGNES_LCD = 2;     // Nombre de lignes de l'écran
const byte NB_COLONNES_LCD = 16;  // Nombre de colonnes de l'écran
char ligne[NB_COLONNES_LCD]; // Pour le formatage des nombres avec la fonction dtostrf()
byte degre[8] = {   // Déclaration d’un tableau de 8 octets pour le caractère °.
  B00111,           // Définition de chaque octet au format binaire :
  B00101,           // 1 pour un pixel affiché – 0 pour un pixel éteint.
  B00111,           // Les 3 bits de poids forts sont ici inutiles.
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte caractereDegre = 0;

//////////////////////////////////////////////////////////////////////////////////////////

// *** Leds

const byte LED_VERTE = A0;
const byte LED_ORANGE = A1;
const byte LED_ROUGE = A2;

//////////////////////////////////////////////////////////////////////////////////////////

// *** Clavier

// *** Bouton poussoir REC
const byte BOUTON_REC = 2; // Broche 2
boolean memoireBoutonRec = HIGH;
byte valBoutonRec = 0;

// *** Bouton poussoir PLAY
const byte BOUTON_PLAY = 3; // Broche 3
boolean memoireBoutonPlay = HIGH;
byte valBoutonPlay = 0;

//////////////////////////////////////////////////////////////////////////////////////////

// *** Carte SD
// Note that even if it's not used as the SS pin, the hardware SS pin (10 on most Arduino
// boards) must be left as an output or the SD library functions will not work.
const byte CHIP_SELECT = 10;   // Chip Select de la carte SD.
// On importe la bibliothèque
#include <SPI.h>
#include <SD.h>
File dataFile;
String enteteFichier;
const String SEPARATEUR =";"; // Séparateur de données pour le tableur
                              // Ne pas choisir l'espace à cause de Date Heure (possible développement ultérieur)

//////////////////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  
  // Afficheur LCD
  lcd.begin(NB_COLONNES_LCD, NB_LIGNES_LCD);
  lcd.clear();
  lcd.print("V." + VERSION);
  lcd.createChar(caractereDegre, degre); // création du caractère personnalisé degré

  // Leds
  pinMode(LED_VERTE, OUTPUT);  // Led verte en sortie
  pinMode(LED_ORANGE, OUTPUT);  // Led orange en sortie
  pinMode(LED_ROUGE, OUTPUT);  // Led rouge en sortie

  // Clavier
  pinMode(BOUTON_REC, INPUT); // Bouton en entrée.
  pinMode(BOUTON_PLAY, INPUT); // Bouton en entrée.

  // Carte SD
  // Make sure that the default chip select pin is set to output, even if you don't use it.
  pinMode(CHIP_SELECT, OUTPUT); // Chip Select en sortie.
  // See if the card is present and can be initialized. 
  lcd.setCursor(0,1);
  if (!SD.begin(CHIP_SELECT)) {
    lcd.print("SD ERREUR");
  } else {
    lcd.print("SD OK");
  }
  delay(3000);
  lcd.clear();

  // Liaison série
  Serial.begin(9600);  // Setup serial. Transfert des données à 9600 bauds.
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  // Capteurs
  initCapteurs();
}

//////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
  afficherCapteurs();
  delay(250);
  
  if (getKeyRec() == 1) {
    enregistrerFichier();
  }
  else {
    if (getKeyPlay() == 1) {
      transfererFichier();
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
