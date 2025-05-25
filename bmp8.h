#ifndef BMP8_H
#define BMP8_H

#include <stdint.h>

/* Constantes pour le format BMP */
#define BMP_HEADER_SIZE 54
#define BMP_COLOR_TABLE_SIZE 1024
#define BITS_PER_PIXEL 8

/*
 * Structure qui représente une image en noir et blanc (8 bits)
 */
typedef struct {
    unsigned char header[BMP_HEADER_SIZE];     // En-tête du fichier BMP
    unsigned char colorTable[BMP_COLOR_TABLE_SIZE]; // Table des couleurs
    unsigned char *data;                       // Les pixels de l'image
    
    uint32_t width;                           // Largeur de l'image en pixels
    uint32_t height;                          // Hauteur de l'image en pixels
    uint16_t colorDepth;                      // Nombre de bits par pixel (8)
    uint32_t dataSize;                        // Taille totale des données
    uint32_t rowPadding;                      // Padding pour alignement 4 octets
} t_bmp8;

/*
 * Ouvre et charge une image depuis un fichier
 * Paramètre :
 *   filename - Chemin du fichier à charger
 * Renvoie : l'image chargée ou NULL si erreur
 */
t_bmp8 *bmp8_loadImage(const char *filename);

/*
 * Enregistre une image dans un fichier
 * Paramètres :
 *   filename - Chemin où sauvegarder l'image
 *   img      - Image à sauvegarder
 * Renvoie : 0 si réussi, -1 si erreur
 */
int bmp8_saveImage(const char *filename, t_bmp8 *img);

/*
 * Libère la mémoire utilisée par une image
 * Paramètre :
 *   img - Image à libérer
 */
void bmp8_free(t_bmp8 *img);

/*
 * Affiche les caractéristiques de l'image
 * Paramètre :
 *   img - Image dont on veut voir les infos
 */
void bmp8_printInfo(const t_bmp8 *img);

/*
 * Inverse les couleurs de l'image (effet négatif)
 * Paramètre :
 *   img - Image à modifier
 */
void bmp8_negative(t_bmp8 *img);

/*
 * Modifie la luminosité de l'image
 * Paramètres :
 *   img   - Image à modifier
 *   value - Ajustement (-255 à +255)
 */
void bmp8_brightness(t_bmp8 *img, int value);

/*
 * Convertit l'image en noir et blanc pur
 * Paramètres :
 *   img       - Image à modifier
 *   threshold - Seuil de conversion (0-255)
 */
void bmp8_threshold(t_bmp8 *img, int threshold);

#endif /* BMP8_H */