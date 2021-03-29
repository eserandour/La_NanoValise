//////////////////////////////////////////////////////////////////////////////////////////
/*
   *** ENREGISTRER FICHIER
*/
//////////////////////////////////////////////////////////////////////////////////////////

// *** Horloge

const unsigned long CADENCE[8] = {
100,         // 100 MS
1000,        // 1 S
5000,        // 5 S
15000,       // 15 S
60000,       // 1 MIN
300000,      // 5 MIN
900000,      // 15 MIN
3600000      // 1 H
};
unsigned long cadenceDefaut = CADENCE[2]; // Valeur par défaut.
unsigned long deltaMesures = cadenceDefaut; // Intervalle entre 2 mesures (en ms).
unsigned long time = 0;
unsigned long timeOffset = 0;
boolean recording = false;

//////////////////////////////////////////////////////////////////////////////////////////

void enregistrerFichier()
{
  do {
    if (!recording) {
      nouveauFichier();
      recording = true;
      numeroMesure = 0;
    }
    else {
        time = millis();
        if (numeroMesure == 0) {
          timeOffset = time;
        }
        unsigned long duree = time - timeOffset;
        lcd.clear();
        afficherCapteurs();
        lcd.setCursor(0,1);
        lcd.print("DUREE: ");
        String dureeFormatee = "";
        int nbHeures = long(numeroMesure*deltaMesures/3600000);
        int nbMinutes = long(numeroMesure*deltaMesures/60000-nbHeures*60);
        int nbSecondes = long(numeroMesure*deltaMesures/1000-nbHeures*3600-nbMinutes*60);
        if (nbHeures < 10) {
          dureeFormatee = "0";
        }
        dureeFormatee += String(nbHeures, DEC);
        if (deltaMesures < 3600000) {
          dureeFormatee += ":";  
          if (nbMinutes < 10) {
            dureeFormatee += "0";
          }
          dureeFormatee += String(nbMinutes, DEC);
        }
        if (deltaMesures < 60000) {
          dureeFormatee += ":";      
          if (nbSecondes < 10) {
            dureeFormatee += "0";
          }      
          dureeFormatee += String(nbSecondes, DEC);
        }
        lcd.print(dureeFormatee);
        
        // Vers la carte SD
        // Open the file. Note that only one file can be open at a time,
        // so you have to close this one before opening another.
        File dataFile = SD.open("data.txt", FILE_WRITE);
        // If the file is available, write to it.
        if (dataFile) {
          dataFile.print(numeroMesure);
          dataFile.print(SEPARATEUR);
          dataFile.print(duree);
          for (int i=0; i<(nbMesures); i++) {
            dataFile.print(SEPARATEUR);
            dataFile.print(mesureBrute[i]);
          }      
          dataFile.println("");
          dataFile.close(); 
        }
        // If the file isn't open, pop up an error.
        else {
          Serial.println("Error opening datalog.txt");  
         
        }
        
        // Vers le port série
        Serial.print(numeroMesure);
        Serial.print(SEPARATEUR);
        Serial.print(duree);
        for (int i=0; i<(nbMesures); i++) {
          Serial.print(SEPARATEUR);
          Serial.print(mesureBrute[i]);
        }
        Serial.println("");
        
        // Mécanisme de régulation.
        // On regarde où on en est au niveau temps parce qu'une boucle dure ici environ 33 ms
        time = millis();
        duree = time - timeOffset;
        long correction = duree-(numeroMesure*deltaMesures);
        long tempsPause = deltaMesures-correction; // Pour éviter de rares bugs lors de
        if (tempsPause < 0) {                      // la régulation : il peut arriver que
          tempsPause = 0;                          // correction dépasse 100 ms.
        }
        unsigned long tempsEcoule;
        do {
          if ( getKeyRec() == 2) {
            // Stoppe l'enregistrement
            recording = false;
            // Marqueur de fin d'enregistrement (pour Python)
            Serial.write("\r\n"); // Retour à la ligne + Saut de ligne            
            valBoutonRec = 0;
            lcd.clear();
            break;  // On sort de la boucle do...while  
          }
          tempsEcoule = millis() - time;     
        } while (tempsEcoule < tempsPause);
        numeroMesure++;
    }  
  } while (recording == true);
}

//////////////////////////////////////////////////////////////////////////////////////////

void nouveauFichier()
{
  String entete = enteteFichier;
  
  // Sortie sur carte SD
  // Effacer le fichier data.txt précédent
  if (SD.exists("data.txt")) {
    SD.remove("data.txt");
  }
  // Créer le fichier data.txt et sa première ligne
  dataFile = SD.open("data.txt", FILE_WRITE);  
  // If the file is available, write to it.
  if (dataFile) {
    dataFile.println(entete);
    dataFile.close(); 
  }
  // If the file isn't open, pop up an error.
  else {
    Serial.println("Error opening data.txt");
  }

  // Sortie sur port série
  Serial.println(entete);
}

//////////////////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////////////////
/*
   *** TRANSFERER FICHIER
*/
//////////////////////////////////////////////////////////////////////////////////////////

void transfererFichier()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TRANSFERT");
  // Lecture du fichier data.txt sur le port série  
  dataFile = SD.open("data.txt");
  if (dataFile) {
    unsigned long tailleFichier = dataFile.size();
    // Read from the file until there's nothing else in it
    while (dataFile.available()) {
      Serial.write(dataFile.read());
      // Affichage du pourcentage de transfert
      lcd.setCursor(11,1);
      byte pourcentage = 100 * dataFile.position() / tailleFichier;
      dtostrf(pourcentage, 3, 0, ligne);
      lcd.print(ligne);
      lcd.print(" %");    
      // Pour abandonner le transfert
      if (getKeyPlay() == 0) {
        break;  // On sort de la boucle while  
      }
    }
    // Marqueur de fin de fichier (pour Python)
    Serial.write("\r\n"); // Retour à la ligne + Saut de ligne
    // Close the file
    dataFile.close();
    delay(3000); // Le temps que le programme de récupération se ferme
    valBoutonPlay = 0;
    lcd.clear();
  }
  // If the file didn't open, print an error
  else { 
    Serial.println("Error opening data.txt");
  }  
}

//////////////////////////////////////////////////////////////////////////////////////////
