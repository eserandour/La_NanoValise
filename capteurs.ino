//////////////////////////////////////////////////////////////////////////////////////////
/* 
   *** LES CAPTEURS
*/
//////////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <SparkFun_SCD30_Arduino_Library.h>  // https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
SCD30 airSensor;

// *** Variables
const byte NB_MESURES_MAX = 3;
int nbMesures;
int mesureBrute[NB_MESURES_MAX];
unsigned long numeroMesure = 0;

//////////////////////////////////////////////////////////////////////////////////////////

void initCapteurs()
{
  // Initialisation de l'entete du fichier
  enteteFichier = "MESURE" + SEPARATEUR 
                + "MILLISECONDES" + SEPARATEUR 
                + "PPMCO2" + SEPARATEUR 
                + "°C" + SEPARATEUR 
                + "%HR";
  Wire.begin();
  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////

void lectureCapteurs()
{
  nbMesures = 3;

  // The SCD30 has data ready every two seconds
  if (airSensor.dataAvailable()) {
    mesureBrute[0] = airSensor.getCO2();
    //mesureBrute[1] = airSensor.getTemperature();
    mesureBrute[2] = airSensor.getHumidity();
  }

  // LM35CZ
  // Vient remplacer la mesure initialement faite avec le SCD30 (trop élevée)
  mesureBrute[1] = thermometreRead(A3);  // Sur la broche analogique A3.
}

//////////////////////////////////////////////////////////////////////////////////////////

void afficherCapteurs()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CO2: ");
  lcd.print(mesureBrute[0]);  // CO2 en PPM
  lcd.print(" PPM");
  lcd.setCursor(0,1);
  lcd.print("AIR: ");
  lcd.print(mesureBrute[1]);  // Température en °C
  lcd.write(DEGRE);
  lcd.print("C ");
  lcd.print(mesureBrute[2]);  // % d'humidité relative
  lcd.print(" %HR");
  
  if (mesureBrute[0] < 800) {
    digitalWrite(LED_VERTE, HIGH);
    digitalWrite(LED_ORANGE, LOW);
    digitalWrite(LED_ROUGE, LOW);
  }
  else if (mesureBrute[0] >= 800 && mesureBrute[0] < 1000) {
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_ORANGE, HIGH);
    digitalWrite(LED_ROUGE, LOW);
  }
  else if (mesureBrute[0] >= 1000 && mesureBrute[0] < 1500) {
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_ORANGE, LOW);
    digitalWrite(LED_ROUGE, HIGH);
  }
    else if (mesureBrute[0] >= 1500) {
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_ORANGE, LOW);
    etatLedRouge = !etatLedRouge;
    digitalWrite(LED_ROUGE, etatLedRouge);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////


float thermometreRead(int broche)  // Capteur de température LM35CZ
{
  #define NBREADINGS 5
  long total = 0;
  for(int i=0; i<NBREADINGS; i++) {
    total = total + analogRead(broche);
    delay(20); // On laisse un peu de temps au capteur
  }
  float moyenne = float(total) / NBREADINGS; // On calcule la moyenne

  // Convertit moyenne en une tension en volts (un simple produit en croix). 
  float tension = 5.0 * moyenne / 1023.0;
  
  // L'amplification avec l'ampli op non inverseur est de Vs/Ve = (1 + R3 / R2)
  // R3 théorique = 39 kilohms
  // R2 théorique = 10 kilohms
  // Amplification théorique = (1 + (39 / 10)) = 4.9
  // Sur mon montage, quand en sortie de capteur on a 0,21 V,
  // au niveau de l'entrée analogique de la carte Arduino on a 1.04 V.
  // L'amplification mesurée est donc de 1.04 / 0,21 = 4.95.
  float tension_avant_amplification = tension / 4.9;
  
  // Le capteur de température LM35CZ fournit une 
  // réponse linéaire de 10 mV (0,01 V) par °C 
  // dans l'intervalle [+ 2 °C ; + 110 °C].  
  float temperature = tension_avant_amplification / 0.01;
  
  return temperature;
}

//////////////////////////////////////////////////////////////////////////////////////
