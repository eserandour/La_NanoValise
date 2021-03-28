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
}

//////////////////////////////////////////////////////////////////////////////////////////
