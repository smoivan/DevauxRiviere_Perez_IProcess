/**
 * @file bmp8equalize.h
 *
 * @brief
 * Déclarations des fonctions pour améliorer automatiquement le contraste
 * des images BMP 8 bits en noir et blanc.
 *
 * @author [Ivan]
 * @date   [Perez]
 */

#ifndef EQUALIZE8_H
#define EQUALIZE8_H
#include "bmp8.h"

/*
 * Ces fonctions permettent d'améliorer automatiquement le contraste des images
 * en noir et blanc
 */

/*
 * Analyse l'image pour compter l'utilisation des différents niveaux de gris
 * Renvoie un tableau de 256 valeurs (une pour chaque niveau de gris)
 */
unsigned int *bmp8_computeHistogram(t_bmp8 *img);

/*
 * Prépare les nouvelles valeurs pour améliorer le contraste
 * Utilise l'histogramme pour calculer comment redistribuer les niveaux de gris
 */
unsigned int *bmp8_computeCDF(unsigned int *hist, unsigned int total_pixels);

/*
 * Améliore automatiquement le contraste de l'image
 * Cette fonction utilise les deux fonctions précédentes pour :
 * - Analyser l'image
 * - Calculer les meilleures valeurs
 * - Appliquer les modifications
 */
void    bmp8_equalize(t_bmp8 *img);

#endif // EQUALIZE8_H