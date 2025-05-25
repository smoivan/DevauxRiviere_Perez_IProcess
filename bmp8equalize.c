#include "equalize8.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

/*
 * Compte combien de fois chaque niveau de gris apparaît dans l'image
 *
 * Ce qu'elle fait :
 * - Crée un tableau de 256 cases (une pour chaque niveau de gris)
 * - Compte combien de fois on trouve chaque niveau de gris dans l'image
 * 
 * Paramètre :
 * - img : l'image à analyser
 * 
 * Renvoie :
 * - Un tableau avec les comptages
 * - NULL si erreur
 */
unsigned int *bmp8_computeHistogram(t_bmp8 *img) {
    // Vérification des paramètres d'entrée
    if (!img || !img->data) return NULL;

    // Allocation de mémoire pour l'histogramme (256 niveaux de gris)
    unsigned int *hist = calloc(256, sizeof(unsigned int));
    if (!hist) return NULL;

    // Parcours de tous les pixels de l'image et comptage des occurrences
    for (unsigned int i = 0; i < img->dataSize; i++) {
        hist[img->data[i]]++;
    }

    return hist;
}

/*
 * Calcule comment mieux répartir les niveaux de gris
 *
 * Ce qu'elle fait :
 * - Prend l'histogramme en entrée
 * - Calcule les sommes cumulées
 * - Ajuste les valeurs pour avoir une meilleure répartition
 * 
 * Paramètres :
 * - hist : l'histogramme calculé précédemment
 * - nombre_pixels_total : nombre total de pixels dans l'image
 * 
 * Renvoie :
 * - Un tableau avec les nouvelles valeurs pour chaque niveau
 * - NULL si erreur
 */
unsigned int *bmp8_computeCDF(unsigned int *hist, unsigned int nombre_pixels_total) {
    // Vérification des paramètres d'entrée
    if (!hist || nombre_pixels_total == 0) return NULL;

    // Allocation de mémoire pour la CDF
    unsigned int *cdf = calloc(256, sizeof(unsigned int));
    if (!cdf) return NULL;

    // Calcul de la CDF : chaque élément est la somme cumulative
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    // Recherche de la première valeur non nulle dans la CDF (cdf_min)
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // Normalisation de la CDF pour maintenir les valeurs dans l'intervalle [0, 255]
    for (int i = 0; i < 256; i++) {
        cdf[i] = round(((float)(cdf[i] - cdf_min) / (nombre_pixels_total - cdf_min)) * 255);
    }

    return cdf;
}

/*
 * Améliore le contraste de l'image
 *
 * Ce qu'elle fait :
 * - Analyse la répartition des niveaux de gris
 * - Recalcule de nouvelles valeurs pour mieux utiliser tous les niveaux
 * - Applique ces changements à l'image
 * 
 * L'image aura un meilleur contraste après cette opération
 * 
 * Paramètre :
 * - img : l'image à modifier
 */
void bmp8_equalize(t_bmp8 *img) {
    // Vérification des paramètres d'entrée
    if (!img || !img->data) return;

    // Calcul de l'histogramme de l'image
    unsigned int *hist = bmp8_computeHistogram(img);
    if (!hist) return;

    // Calcul de la CDF normalisée
    unsigned int *cdf = bmp8_computeCDF(hist, img->width * img->height);
    if (!cdf) {
        free(hist);
        return;
    }

    // Application de la transformation d'égalisation à chaque pixel
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (unsigned char)cdf[img->data[i]];
    }

    // Libération de la mémoire allouée
    free(hist);
    free(cdf);
}