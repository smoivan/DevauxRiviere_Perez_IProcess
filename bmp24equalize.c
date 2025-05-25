#include <stdio.h>
#include <stdlib.h>
#include "bmp24.h"
#include <math.h>

t_pixel **bmp24_allouerPixelsData(int largeur, int hauteur) {
    t_pixel **pixels = (t_pixel **)malloc(hauteur * sizeof(t_pixel *));
    if (!pixels) {
        fprintf(stderr, "Échec de l'allocation mémoire pour les lignes de pixels.\n");
        return NULL;
    }
    for (int i = 0; i < hauteur; i++) {
        pixels[i] = (t_pixel *)malloc(largeur * sizeof(t_pixel));
        if (!pixels[i]) {
            fprintf(stderr, "Échec de l'allocation mémoire pour la ligne de pixels %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            return NULL;
        }
    }
    return pixels;
}

void bmp24_libererPixelsData(t_pixel **pixels, int hauteur) {
    if (!pixels) return;
    for (int i = 0; i < hauteur; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

t_bmp24 *bmp24_allouer(int largeur, int hauteur, int profondeurCouleur) {
    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (!img) {
        fprintf(stderr, "Échec de l'allocation mémoire pour l'image.\n");
        return NULL;
    }
    img->largeur = largeur;
    img->hauteur = hauteur;
    img->profondeurCouleur = profondeurCouleur;
    img->donnees = bmp24_allouerPixelsData(largeur, hauteur);
    if (!img->donnees) {
        free(img);
        return NULL;
    }
    return img;
}

void bmp24_liberer(t_bmp24 *img) {
    if (!img) return;
    bmp24_libererPixelsData(img->donnees, img->hauteur);
    free(img);
}

t_bmp24 *bmp24_chargerImage(const char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "rb");
    if (!fichier) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", nomFichier);
        return NULL;
    }
    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (!img) {
        fclose(fichier);
        fprintf(stderr, "Échec de l'allocation mémoire pour l'image.\n");
        return NULL;
    }
    // Lire l'en-tête du fichier BMP
    fread(&img->entete.type, sizeof(uint16_t), 1, fichier);
    fread(&img->entete.taille, sizeof(uint32_t), 1, fichier);
    fread(&img->entete.reserve1, sizeof(uint16_t), 1, fichier);
    fread(&img->entete.reserve2, sizeof(uint16_t), 1, fichier);
    fread(&img->entete.decalage, sizeof(uint32_t), 1, fichier);
    // Lire l'en-tête d'information BMP
    fread(&img->entete_info.taille, sizeof(uint32_t), 1, fichier);
    fread(&img->entete_info.largeur, sizeof(int32_t), 1, fichier);
    fread(&img->entete_info.hauteur, sizeof(int32_t), 1, fichier);
    fread(&img->entete_info.plans, sizeof(uint16_t), 1, fichier);
    fread(&img->entete_info.bits, sizeof(uint16_t), 1, fichier);
    fread(&img->entete_info.compression, sizeof(uint32_t), 1, fichier);
    fread(&img->entete_info.tailleImage, sizeof(uint32_t), 1, fichier);
    fread(&img->entete_info.resolutionX, sizeof(int32_t), 1, fichier);
    fread(&img->entete_info.resolutionY, sizeof(int32_t), 1, fichier);
    fread(&img->entete_info.nbCouleurs, sizeof(uint32_t), 1, fichier);
    fread(&img->entete_info.couleursImportantes, sizeof(uint32_t), 1, fichier);