//////////////////////////////////////////////////////////////////////////////////////////
/* 
   *** LES CAPTEURS
*/
//////////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <SparkFun_SCD30_Arduino_Library.h>
SCD30 airSensor;

// *** Variables
const byte NB_MESURES_MAX = 3;
int nbMesures;
int mesureBrute[NB_MESURES_MAX];
unsigned long numeroMesure = 0;
boolean etatLedRouge = LOW;

//////////////////////////////////////////////////////////////////////////////////////////

void initCapteurs()
{
  // Initialisation de l'entete du fichier
  enteteFichier = "MESURE" + SEPARATEUR 
                + "MILLISECONDES" + SEPARATEUR 
                + "PPMCO2" + SEPARATEUR 
                + "°C" + SEPARATEUR 
                + "%HUM";
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
    mesureBrute[1] = airSensor.getTemperature();
    mesureBrute[2] = airSensor.getHumidity();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////

void afficherCapteurs()
{
  lectureCapteurs();
  lcd.setCursor(0,0);
  lcd.print("CO2: ");
  lcd.print(mesureBrute[0]);
  lcd.print(" PPM");
  lcd.setCursor(0,1);
  lcd.print("AIR: ");
  lcd.print(mesureBrute[1]);
  lcd.write(caractereDegre);
  lcd.print("C");
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
