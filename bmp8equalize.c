#include "bmp8equalize.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

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

    // Recherche cdf_min
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

    // Normalisation avec vérification de dépassement
    for (int i = 0; i < 256; i++) {
        double normalized = ((double)(cdf[i] - cdf_min) / 
                           (double)(nombre_pixels_total - cdf_min)) * 255.0;
        cdf[i] = (unsigned int)round(normalized < 0 ? 0 : 
                                   (normalized > 255 ? 255 : normalized));
    }

    return cdf;
}

void bmp8_equalize(t_bmp8 *img) {
    if (!img || !img->data) {
        fprintf(stderr, "Image invalide\n");
        return;
    }

    // Calcul histogramme
    unsigned int *hist = bmp8_computeHistogram(img);
    if (!hist) return;

    // Calcul nombre réel de pixels (sans padding)
    unsigned int real_size = img->width * img->height;
    unsigned int *cdf = bmp8_computeCDF(hist, real_size);
    if (!cdf) {
        free(hist);
        return;
    }

    // Application avec gestion du padding
    int padding = (4 - (img->width % 4)) % 4;
    int rowSize = img->width + padding;

    for (unsigned int y = 0; y < img->height; y++) {
        for (unsigned int x = 0; x < img->width; x++) {
            unsigned int pos = y * rowSize + x;
            img->data[pos] = (unsigned char)cdf[img->data[pos]];
        }
    }

    free(hist);
    free(cdf);
}