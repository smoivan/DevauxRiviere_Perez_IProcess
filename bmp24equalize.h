#ifndef EQUALIZE24_H
#define EQUALIZE24_H

#include "bmp24.h"

// Réalise l'égalisation d'histogramme sur le canal luminance (Y) d'une image 24 bits.
// Cela améliore le contraste tout en préservant les couleurs.
void bmp24_equalize(t_bmp24 *img);

#endif // EQUALIZE24_H