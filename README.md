# Projet de traitement d’images BMP

**Auteurs : Ivan Perez & Aurelien Devaux-Rivière**

---

## Présentation

Ce projet est une application complète de traitement d’images écrite en langage C.  
Elle propose deux interfaces:
- Une interface en ligne de commande (CLI)
- Une interface graphique (GUI) basée sur GTK3

L’outil se concentre sur la manipulation d’images au format **BMP** (8 bits en niveaux de gris et 24 bits en couleur).

---

## Fonctionnalités

- **Prise en charge des images BMP 8 bits et 24 bits**
- **Charger et sauvegarder** des images BMP (noir et blanc ou couleur)
- **Afficher les informations** de l’image (dimensions, profondeur, etc.)

#### Filtres et traitements disponibles :
- Négatif
- Ajustement de la luminosité
- Noir et blanc (seuil)
- Flou par boîte (*Box blur*)
- Flou gaussien
- Détection de contours (*Outline*)
- Estampage (*Emboss*)
- Netteté (*Sharpen*)
- Égalisation d’histogramme (amélioration automatique du contraste)
  - Compatible avec les images en niveaux de gris et en couleur (traitement via l’espace YUV pour les images couleur)

## Organisation du projet
- `bmp8.c:` : Gestion des images BMP 8 bits et filtres associés
- `bmp24.c` : Gestion des images BMP 24 bits et filtres associés
- `bmp24.h` : Structures et prototypes pour la manipulation d’images BMP (lecture, écriture, effets, filtres, gestion mémoire…)
- `bmp24equalize.h` : Déclaration de la fonction d’égalisation d’histogramme.
- `bmp24equalize.c` : Implémentation de l’égalisation d’histogramme (et conversions RGB <-> YUV).
