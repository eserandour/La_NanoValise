# -*- coding: utf-8 -*-
"""
########################################################################
#
#  La NanoValise : Récupération et traitement des données brutes
#  Version 2021.04.06c
#  Copyright 2019-2021 - Eric Sérandour
#  http://3615.entropie.org
#
#  Programme écrit en Python 3
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 3 of
#  the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public
#  License along with this program; if not, write to the Free
#  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#  Boston, MA 02110-1301, USA.
#
########################################################################
"""

"""
########################################################################
#
#  Pour régler certains paramètres, voir vers le bas de ce fichier :
#  Dans la marge, j'indique "A modifier éventuellement".
#
########################################################################
"""
#!/usr/bin/env python

import serial
import time
import csv
import matplotlib.pyplot as plt  # Pour faire des graphiques
import numpy

########################################################################






########################################################################
#  FONCTION DE RECUPERATION DES DONNEES DU PORT SERIE => FICHIER CSV
########################################################################

def enregistrerDonnees(nomPort, vitessePort, nomFichier):
    """Enregistrement des données dans un fichier CSV"""
    # Ouverture du port série
    serialPort = serial.Serial(port = nomPort, baudrate = vitessePort)
    # Réinitialisation du microcontrôleur via la broche DTR
    serialPort.setDTR(False)
    time.sleep(0.1)
    serialPort.setDTR(True)
    # On vide le tampon (buffer)
    serialPort.flushInput()
    # -----------------------------------------------------------------
    # Enregistrement dans le fichier CSV
    file = open(nomFichier, "wb")  # Ecriture en mode binaire
    finFichier = False
    data = False
    while not finFichier:
        # Lecture du port série
        ligne = serialPort.readline()
        # Affichage des données en provenance d'Arduino dans la console
        if data == False:
            print("Données brutes :")
            data = True
        print(ligne)
        if ligne == b'\r\n':
            finFichier = True
        else:
            # Ecriture dans le fichier CSV
            file.write(ligne)
    file.close();
    # -----------------------------------------------------------------
    # Fermeture du port série
    serialPort.close()

########################################################################






########################################################################
#  FONCTION D'EXTRACTION DES DONNEES (NOMBRES) DEPUIS LE .CSV
########################################################################

def readColCSV(nomFichier, numCol):
    """Lit une colonne du fichier CSV (la numérotation commence à 0)"""
    file = open(nomFichier, "r")
    reader = csv.reader(file, delimiter =";")
    colonne = []  # Création de la liste "colonne" (vide)
    for row in reader:  # On balaye toutes les lignes du fichier CSV
        try:
            # On remplace les virgules éventuelles par des points 
            notationPoint = row[numCol].replace(",", ".")
            # Remplissage de la liste "colonne" avec des réels
            colonne.append(float(notationPoint))
        except:
            pass
    file.close()
    return colonne

########################################################################

def extraireDonnees(nomFichier, colX, colY):
    """Extraction des données depuis le fichier CSV"""
    listeX = readColCSV(nomFichier, colX)  # Colonne choisie pour x
    listeY = readColCSV(nomFichier, colY)  # Colonne choisie pour y
    x = numpy.array(listeX)    # Liste => Tableau
    y = numpy.array(listeY)    # Liste => Tableau
    return numpy.array([x,y])

########################################################################

def selectionnerZoneDonnees(x, y, debut, fin):
    """Sélection d'une zone de données"""
    x = x[debut:fin]
    y = y[debut:fin]
    return numpy.array([x,y])

########################################################################






########################################################################
#  AFFICHAGE DES DONNEES EXTRAITES ET TRAITEES
########################################################################

def afficherDonnees(message, x, y):
    """Affichage des données"""
    print(message)
    print("Abscisses :", x)
    print("Ordonnées :", y)
    print("-----------------------------------------------------------")

########################################################################







"""
########################################################################
#  DEBUT DU PROGRAMME
########################################################################
"""






########################################################################
#  TRAITEMENT DES DONNEES BRUTES
########################################################################

# Enregistrement des données Arduino dans un fichier CSV
print ("En attente de données...")
print ()
# Indiquer le port sélectionné dans le menu Arduino (Outils >  Port) :
# Sous Linux : /dev/ttyACM ou ou /dev/ttyUSB suivis d'un numéro (0,1,...)
# Sous Windows : COM suivi d'un numéro (1,2,...)
PORT = "/dev/ttyUSB0"                                                   # A modifier éventuellement
VITESSE = 9600  # Vitesse en bauds                                      
FICHIER_CSV = "data.csv"                                                # A modifier éventuellement
enregistrerDonnees(PORT, VITESSE, FICHIER_CSV)                          # A mettre en commentaire si on veut travailler sur un fichier CSV déjà existant
print("-----------------------------------------------------------")

# Extraction du fichier CSV
COLONNE_X = 0                                                           # A modifier éventuellement
COLONNE_Y = 2                                                           # A modifier éventuellement
x, y = extraireDonnees(FICHIER_CSV, COLONNE_X, COLONNE_Y)
"""afficherDonnees("Données extraites :", x, y)"""

# Sélectionner une zone de données (x, y, ligne début, ligne fin)
DEBUT = 0
FIN = numpy.size(x)
x, y = selectionnerZoneDonnees(x, y, DEBUT, FIN)                        # A modifier éventuellement
"""afficherDonnees("Données sélectionnées :", x, y)"""

# Conversion des données
# x : Temps en secondes
# Pour une temporisation de 100 ms, choisir 0.1 s
temporisation = 5  # en s, min, ou h                                    # A modifier éventuellement
x = x * temporisation
# y :
# y =                                                                   # A modifier éventuellement
"""afficherDonnees("Données converties :", x, y)""" 

########################################################################







########################################################################
#  AFFICHAGE DU GRAPHIQUE
########################################################################

plt.title("Taux de CO2 en fonction du temps")                           # A modifier (Titre)
plt.xlabel("Temps (s)")                                                 # A modifier (Abscisses)
plt.ylabel("Taux de CO2 (ppm)")                                         # A modifier (Ordonnées)

#plt.plot(x, y, ".r")  # Les points ne sont pas reliés (r : rouge)
plt.plot(x,y)  # Les points sont reliés
plt.grid(True)  # Grille
plt.savefig("graphique.png")  # Sauvegarde du graphique au format PNG
plt.show()

########################################################################
