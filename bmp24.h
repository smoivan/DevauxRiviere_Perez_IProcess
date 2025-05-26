/**
 * @file bmp24.h
 *
 * @brief
 * Définitions des structures et déclarations des fonctions pour
 * la gestion et la modification des images BMP couleur 24 bits.
 *
 * Ce fichier centralise : les structures de données BMP, la manipulation
 * mémoire, la lecture/écriture de pixels, les effets visuels de base
 * et avancés.
 *
 * @author [Aurelien Devaux-Rivière]
 * @date   [28/04/25]
 */

#ifndef BMP24_H
#define BMP24_H

#include <stdio.h>
#include <stdint.h>

/*
 * Positions des informations importantes dans le fichier BMP
 * Ces valeurs sont en hexadécimal et représentent des positions dans le fichier
 */
#define BITMAP_MAGIC  0x00   // Signature du fichier BMP
#define BITMAP_SIZE   0x02   // Taille totale du fichier
#define BITMAP_OFFSET 0x0A   // Où commencent les données de l'image
#define BITMAP_WIDTH  0x12   // Largeur de l'image
#define BITMAP_HEIGHT 0x16   // Hauteur de l'image
#define BITMAP_DEPTH  0x1C   // Nombre de bits par pixel
#define BITMAP_SIZE_RAW 0x22 // Taille des données de l'image

#define BMP_TYPE 0x4D42     // Signature "BM" en hexadécimal
#define HEADER_SIZE 0x0E    // Taille de l'en-tête principal
#define INFO_SIZE 0x28      // Taille des informations supplémentaires
#define DEFAULT_DEPTH 0x18  // 24 bits par défaut (8 bits par couleur)


#pragma pack(push, 1) // 🔧 Désactive l’alignement automatique
/*
 * Structure de l'en-tête principal du fichier BMP
 */
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;

/*
 * Informations détaillées sur l'image
 */
typedef struct {
    uint32_t size;             // Taille de cette structure (40 octets)
    int32_t width;
    int32_t height;// Hauteur en pixels
    uint16_t planes;// Toujours 1
    uint16_t bits;             // Doit être 24
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t importantColors; // Nombre de couleurs importantes
} t_bmp_info;

#pragma pack(pop)
/*
 * Structure représentant un pixel en couleur (RGB)
 */
typedef struct {
    uint8_t red;    // Rouge (0-255)
    uint8_t green;  // Vert (0-255)
    uint8_t blue;   // Bleu (0-255)
} t_pixel;

/*
 * Structure principale représentant une image BMP 24 bits
 */
typedef struct {
    t_bmp_header header;      // En-tête du fichier
    t_bmp_info header_info;   // Informations sur l'image
    int width;               // Largeur en pixels
    int height;              // Hauteur en pixels
    int colorDepth;          // Profondeur de couleur (24)
    t_pixel **data;          // Tableau 2D des pixels
} t_bmp24;

/* Déclarations des fonctions - groupées par catégorie */

/* Gestion de la mémoire */
t_pixel **bmp24_allocateDataPixels(int width, int height);
void bmp24_freeDataPixels(t_pixel **pixels, int height);
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth);
void bmp24_free(t_bmp24 *img);

/* Lecture/Écriture de fichiers */
/* file_rawRead
 * Rôle : Lit des données dans un fichier à une position spécifique
 * Paramètres :
 *   position - Position dans le fichier où commencer la lecture
 *   buffer   - Pointeur vers la zone mémoire où stocker les données
 *   size     - Taille d'un élément à lire
 *   n        - Nombre d'éléments à lire
 *   file     - Pointeur vers le fichier source
 */
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);
/* file_rawWrite
 * Rôle : Écrit des données dans un fichier à une position spécifique
 * Paramètres : Identiques à file_rawRead mais pour l'écriture
 */
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);

/* Manipulation des pixels */
void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file);
void bmp24_readPixelData(t_bmp24 *image, FILE *file);
void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file);
void bmp24_writePixelData(t_bmp24 *image, FILE *file);

/* Chargement et sauvegarde */
/* FONCTIONS PRINCIPALES */

/* bmp24_loadImage
 * Rôle : Charge une image BMP 24 bits
 * Paramètre :
 *   filename - Chemin du fichier à charger
 * Retour : Structure image ou NULL si erreur
 * Vérifie : Format 24 bits, existence fichier
 */
t_bmp24 *bmp24_loadImage(const char *filename);

/* bmp24_saveImage
 * Rôle : Sauvegarde une image en BMP
 * Paramètres :
 *   img      - Image à sauvegarder
 *   filename - Nom du fichier destination
 */
void bmp24_saveImage(t_bmp24 *img, const char *filename);

/* Effets de base */
/* bmp24_negative
 * Rôle : Inverse les couleurs de l'image
 * Paramètre :
 *   img - Image à modifier
 * Calcul : nouvelle_couleur = 255 - couleur_originale
 */
void bmp24_negative(t_bmp24 *img);  // Inverse les couleurs
/* bmp24_grayscale
 * Rôle : Convertit en niveaux de gris
 * Paramètre :
 *   img - Image à convertir
 * Méthode : Moyenne des composantes RGB
 */
void bmp24_grayscale(t_bmp24 *img); // Convertit en niveaux de gris
/* bmp24_brightness
 * Rôle : Ajuste la luminosité
 * Paramètres :
 *   img   - Image à modifier
 *   value - Ajustement (-255 à +255)
 */
void bmp24_brightness(t_bmp24 *img, int value); // Ajuste la luminosité

/* Effets avancés utilisant des filtres */
void bmp24_boxBlur(t_bmp24 *img);      // Flou simple
void bmp24_gaussianBlur(t_bmp24 *img);  // Flou gaussien
void bmp24_outline(t_bmp24 *img);       // Détection des contours
void bmp24_emboss(t_bmp24 *img);         // Effet de relief
void bmp24_sharpen(t_bmp24 *img);       // Augmente la netteté

#endif


