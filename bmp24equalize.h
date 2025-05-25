/**
 * @file bmp24equalize.h
 *
 * @brief
 * Déclaration de la fonction d’égalisation d’histogramme pour image BMP 24 bits.
 * L’égalisation est réalisée sur la luminance (canal Y de l’espace YUV), afin
 * d’améliorer le contraste sans altérer la colorimétrie de l’image.
 *
 * Auteur : [Ivan Perez]
 * Date   : [21/05/25]
 */

#ifndef BMP24EQUALIZE_H
#define BMP24EQUALIZE_H

#include "bmp24.h"

// Réalise l'égalisation d'histogramme sur le canal luminance (Y) d'une image 24 bits.
// Cela améliore le contraste tout en préservant les couleurs.
void bmp24_equalize(t_bmp24 *img);

#endif // BMP24EQUALIZE_H