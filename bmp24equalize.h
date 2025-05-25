#ifndef BMP24_H
#define BMP24_H

#include <stdint.h>

// Structure pour l'en-tête BMP
typedef struct {
    uint16_t type;
    uint32_t taille;
    uint16_t reserve1;
    uint16_t reserve2;
    uint32_t decalage_donnees;
} t_entete_bmp;

// Structure pour l'en-tête d'informations BMP
typedef struct {
    uint32_t taille;
    int32_t largeur;
    int32_t hauteur;
    uint16_t plans;
    uint16_t bits_par_pixel;
    uint32_t compression;
    uint32_t taille_image;
    int32_t resolution_x;
    int32_t resolution_y;
    uint32_t nb_couleurs;
    uint32_t couleurs_importantes;
} t_info_bmp;

// Structure représentant un pixel (composantes RVB)
typedef struct {
    uint8_t rouge;
    uint8_t vert;
    uint8_t bleu;
} t_pixel;

// Structure représentant une image BMP 24 bits
typedef struct {
    t_entete_bmp entete;
    t_info_bmp info;
    int largeur;
    int hauteur;
    int profondeur_couleur;
    t_pixel **donnees;
} t_bmp24;

// Déclarations des fonctions
t_pixel **allouerPixels(int largeur, int hauteur);
void libererPixels(t_pixel **pixels, int hauteur);

t_bmp24 *allouerImage24(int largeur, int hauteur, int profondeur_couleur);
void libererImage24(t_bmp24 *img);

t_bmp24 *chargerImage24(const char *nom_fichier);
int sauvegarderImage24(const char *nom_fichier, t_bmp24 *img);

void negatif24(t_bmp24 *img);
void niveauxDeGris24(t_bmp24 *img);
void luminosite24(t_bmp24 *img, int valeur);

void flouMoyenneur24(t_bmp24 *img);
void flouGaussien24(t_bmp24 *img);
void contours24(t_bmp24 *img);
void relief24(t_bmp24 *img);
void nettete24(t_bmp24 *img);

t_pixel convolution24(t_bmp24 *img, int x, int y, float *noyau, int tailleNoyau);
void appliquerFiltre24(t_bmp24 *img, float *noyau, int tailleNoyau);

#endif // BMP24_H