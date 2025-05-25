/**
 * @file bmp8equalize.c
 *
 * @brief
 * Ce fichier permet de rendre le contraste des images BMP 8 bits plus uniforme.
 *
 * @author [Ivan Pere]
 * @date   [20/05/25]
 */

#include "bmp8equalize.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

/**
 * Calcule l'histogramme des niveaux de gris d'une image BMP 8 bits.
 *
 * @param img Pointeur vers la structure t_bmp8 représentant l'image à traiter.
 * @return Pointeur vers un tableau de 256 entiers non signés représentant l'histogramme, 
 *         ou NULL en cas d'erreur (image invalide ou échec d'allocation).
 * 
 * L'histogramme indique combien de fois chaque valeur de gris (0 à 255) apparaît dans l'image.
 * La fonction prend en compte le padding de chaque ligne BMP pour un comptage correct.
 */
unsigned int *bmp8_computeHistogram(t_bmp8 *img) {
    if (!img || !img->data) return NULL;

    unsigned int *hist = calloc(256, sizeof(unsigned int));
    if (!hist) {
        perror("Erreur d'allocation de l'histogramme");
        return NULL;
    }

    // Tenir compte du padding dans le calcul
    int padding = (4 - (img->width % 4)) % 4;
    int rowSize = img->width + padding;

    // Parcours ligne par ligne pour respecter le padding
    for (unsigned int y = 0; y < img->height; y++) {
        for (unsigned int x = 0; x < img->width; x++) {
            hist[img->data[y * rowSize + x]]++;
        }
    }

    return hist;
}

/**
 * Calcule la Fonction de Répartition Cumulative (CDF) normalisée à partir d'un histogramme.
 *
 * @param hist Pointeur vers un tableau de 256 entiers non signés (l'histogramme).
 * @param nombre_pixels_total Nombre total de pixels dans l'image.
 * @return Pointeur vers un tableau de 256 entiers non signés représentant la CDF normalisée sur [0, 255],
 *         ou NULL en cas d'erreur (entrée invalide ou division par zéro possible).
 * 
 * La CDF permet de redistribuer les niveaux de gris pour égaliser l'histogramme.
 */
unsigned int *bmp8_computeCDF(unsigned int *hist, unsigned int nombre_pixels_total) {
    if (!hist || nombre_pixels_total == 0) return NULL;

    unsigned int *cdf = calloc(256, sizeof(unsigned int));
    if (!cdf) {
        perror("Erreur d'allocation de la CDF");
        return NULL;
    }

    // Calcul CDF
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    // Recherche cdf_min (plus petite valeur CDF non-nulle)
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // Protection contre division par zéro
    if (nombre_pixels_total == cdf_min) {
        free(cdf);
        return NULL;
    }

    // Normalisation de la CDF entre 0 et 255
    for (int i = 0; i < 256; i++) {
        double normalized = ((double)(cdf[i] - cdf_min) /
                           (double)(nombre_pixels_total - cdf_min)) * 255.0;
        cdf[i] = (unsigned int)round(normalized < 0 ? 0 :
                                   (normalized > 255 ? 255 : normalized));
    }

    return cdf;
}

/**
 * Applique l'égalisation d'histogramme sur une image BMP 8 bits.
 *
 * @param img Pointeur vers la structure t_bmp8 représentant l'image à égaliser.
 * 
 * Cette fonction modifie directement les données de l'image (img->data),
 * en remplaçant chaque valeur de pixel par la valeur calculée grâce à la CDF,
 * permettant d'obtenir une répartition plus uniforme des niveaux de gris.
 */
void bmp8_equalize(t_bmp8 *img) {
    if (!img || !img->data) return;

    unsigned int *hist = bmp8_computeHistogram(img);
    if (!hist) return;

    unsigned int *cdf = bmp8_computeCDF(hist, img->width * img->height);
    if (!cdf) {
        free(hist);
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (unsigned char)cdf[img->data[i]];
    }

    free(hist);
    free(cdf);
}