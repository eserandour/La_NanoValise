//////////////////////////////////////////////////////////////////////////////////////

byte getKeyRec() {
  boolean etatBoutonRec = digitalRead(BOUTON_REC);
  if((etatBoutonRec != memoireBoutonRec) && (etatBoutonRec == HIGH)) {
    valBoutonRec++;
    valBoutonRec = valBoutonRec % 3; // 3 valeurs possibles : 0, 1, 2
  }
  // On enregistre l'état du bouton pour le tour suivant
  memoireBoutonRec = etatBoutonRec;
  return valBoutonRec;
}

//////////////////////////////////////////////////////////////////////////////////////

byte getKeyPlay() {
  boolean etatBoutonPlay = digitalRead(BOUTON_PLAY);
  if((etatBoutonPlay != memoireBoutonPlay) && (etatBoutonPlay == HIGH)) {
    valBoutonPlay++;
    valBoutonPlay = valBoutonPlay % 2; // 2 valeurs possibles : 0, 1
  }
  // On enregistre l'état du bouton pour le tour suivant
  memoireBoutonPlay = etatBoutonPlay;
  return valBoutonPlay;
}

//////////////////////////////////////////////////////////////////////////////////////
