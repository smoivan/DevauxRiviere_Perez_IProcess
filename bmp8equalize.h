/#ifndef EQUALIZE8_H
#define EQUALIZE8_H
#include "bmp8.h"

// Calcule l'histogramme pour les images 8-bits en niveaux de gris
unsigned int *bmp8_computeHistogram(t_bmp8 *img);

// Calcule la CDF à partir d'un histogramme donné
unsigned int *bmp8_computeCDF(unsigned int *hist, unsigned int total_pixels);

// Applique l'égalisation d'histogramme à l'image en niveaux de gris
void bmp8_equalize(t_bmp8 *img);

#endif // EQUALIZE8_H