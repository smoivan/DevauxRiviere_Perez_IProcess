/**
 * @file bmp8.c
 *
 * @brief
 * Ce fichier gère les images au format BMP 8 bits (noir et blanc ou niveaux de gris).
 * Il permet de charger et de sauvegarder ce type d’image, d’afficher des informations (taille, profondeur),
 * et de faire quelques modifications simples (négatif, luminosité, seuil, filtres).
 * 
 * Toutes les fonctions présentes ici servent à manipuler facilement les images BMP dans le projet.
 *
 * @author  [Aureilien Devaux-Riviere]
 * @date    [27/04/25]
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "bmp8.h"


/*
 * Charge une image en noir et blanc (format BMP 8 bits)
 * 
 * Ce qu'elle fait :
 * - Ouvre le fichier image
 * - Vérifie que c'est bien une image BMP en noir et blanc
 * - Charge l'image en mémoire
 * 
 * Paramètre :
 * - filename : le nom du fichier image à ouvrir
 * 
 * Renvoie :
 * - L'image chargée si tout va bien
 * - NULL si il y a eu un problème
 */
t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        perror("Erreur: Impossible d'ouvrir le fichier");
        return NULL;
    }

    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (img == NULL) {
        perror("Erreur: Allocation mémoire échouée");
        fclose(file);
        return NULL;
    }

    // Lecture de l'en-tête BMP
    if (fread(img->header, sizeof(unsigned char), 54, file) != 54) {
        fprintf(stderr, "Erreur: Le fichier n'est pas au format BMP valide\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Vérification de la signature BMP
    if (img->header[0] != 'B' || img->header[1] != 'M') {
        fprintf(stderr, "Erreur: Le fichier n'est pas au format BMP\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Extraction des informations de l'image
    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned short *)&img->header[28];
    img->dataSize = *(unsigned int *)&img->header[34];

    // Fallback pour dataSize si elle vaut 0
    if (img->dataSize == 0) {
        img->dataSize = img->width * img->height;
    }

    // Informations de debug
    printf("DEBUG: Largeur=%u, Hauteur=%u, Profondeur=%u, Taille données=%u\n",
           img->width, img->height, img->colorDepth, img->dataSize);

    // Vérification que l'image est en 8 bits
    if (img->colorDepth != 8) {
        fprintf(stderr, "Erreur: L'image n'est pas en 8 bits\n");
        free(img);
        fclose(file);
        return NULL;
    }

	// Calcul du padding (à ajouter après width)
	int padding = (4 - (img->width % 4)) % 4;
	img->dataSize = (img->width + padding) * img->height;

    // Lecture de la table de couleurs
    if (fread(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        fprintf(stderr, "Erreur: Impossible de lire la table de couleurs\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Allocation mémoire pour les données de l'image
    img->data = (unsigned char *)malloc(img->dataSize * sizeof(unsigned char));
    if (img->data == NULL) {
        perror("Erreur: Allocation mémoire échouée pour les données");
        free(img);
        fclose(file);
        return NULL;
    }

    // Lecture des données de l'image
    if (fread(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Erreur: Impossible de lire les données de l'image\n");
        free(img->data);
        free(img);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return img;
}

/*
 * Sauvegarde une image en noir et blanc dans un fichier
 * 
 * Ce qu'elle fait :
 * - Crée un nouveau fichier
 * - Écrit l'image dedans au format BMP
 * 
 * Paramètres :
 * - filename : nom du fichier où sauvegarder
 * - img : l'image à sauvegarder
 * 
 * Renvoie :
 * - 0 si la sauvegarde a réussi
 * - -1 si il y a eu une erreur
 */
int bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (img == NULL) {
        fprintf(stderr, "Erreur: Image invalide\n");
        return -1;
    }

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Erreur: Impossible d'ouvrir le fichier pour l'écriture");
        return -1;
    }

    // Écriture de l'en-tête
    if (fwrite(img->header, sizeof(unsigned char), 54, file) != 54) {
        perror("Erreur: Impossible d'écrire l'en-tête");
        fclose(file);
        return -1;
    }

    // Écriture de la table de couleurs
    if (fwrite(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        perror("Erreur: Impossible d'écrire la table de couleurs");
        fclose(file);
        return -1;
    }

    // Écriture des données de l'image
    if (fwrite(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        perror("Erreur: Impossible d'écrire les données de l'image");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

/*
 * Libère la mémoire utilisée par une image
 * 
 * Ce qu'elle fait :
 * - Supprime les données de l'image de la mémoire
 * - À utiliser quand on n'a plus besoin de l'image
 * 
 * Paramètre :
 * - img : l'image à supprimer
 */
void bmp8_free(t_bmp8 *img) {
    if (img != NULL) {
        if (img->data != NULL) {
            free(img->data);
        }
        free(img);
    }
}

/*
 * Affiche les informations sur l'image
 * 
 * Ce qu'elle fait :
 * Montre à l'écran :
 * - La largeur de l'image
 * - La hauteur de l'image
 * - Le type d'image (8 bits)
 * - La taille des données
 * 
 * Paramètre :
 * - img : l'image dont on veut voir les infos
 */
void bmp8_printInfo(t_bmp8 *img) {
    if (img == NULL) {
        fprintf(stderr, "Erreur: Image invalide\n");
        return;
    }

    printf("Informations de l'image:\n");
    printf("    Largeur: %u\n", img->width);
    printf("    Hauteur: %u\n", img->height);
    printf("    Profondeur de couleur: %u\n", img->colorDepth);
    printf("    Taille des données: %u\n", img->dataSize);
}

/*
 * Inverse les couleurs de l'image (effet négatif)
 * 
 * Ce qu'elle fait :
 * - Transforme chaque pixel en son opposé
 * - Le blanc devient noir et inversement
 * 
 * Paramètre :
 * - img : l'image à modifier
 */
void bmp8_negative(t_bmp8 *img) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Erreur: Image invalide\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
}

/*
 * Change la luminosité de l'image
 * 
 * Ce qu'elle fait :
 * - Rend l'image plus claire (valeur positive)
 * - Ou plus sombre (valeur négative)
 * 
 * Paramètres :
 * - img : l'image à modifier
 * - value : combien on veut éclaircir ou assombrir
 */
void bmp8_brightness(t_bmp8 *img, int value) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Erreur: Image invalide\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        int newValue = img->data[i] + value;

        // Ajustement pour rester dans les limites [0, 255]
        if (newValue > 255) {
            newValue = 255;
        } else if (newValue < 0) {
            newValue = 0;
        }

        img->data[i] = (unsigned char)newValue;
    }
}

void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Erreur: Image invalide\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
    }
}

void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (img == NULL || img->data == NULL || kernel == NULL) {
        fprintf(stderr, "Erreur: Paramètres invalides\n");
        return;
    }

    unsigned char *tempData = (unsigned char *)malloc(img->dataSize * sizeof(unsigned char));
    if (tempData == NULL) {
        perror("Erreur: Allocation mémoire échouée");
        return;
    }

    // Copie des données originales
    memcpy(tempData, img->data, img->dataSize * sizeof(unsigned char));

    int n = kernelSize / 2;

    // Application du filtre seulement aux pixels intérieurs
    for (unsigned int y = n; y < img->height - n; y++) {
        for (unsigned int x = n; x < img->width - n; x++) {
            float sum = 0.0f;

            for (int j = -n; j <= n; j++) {
                for (int i = -n; i <= n; i++) {
                    unsigned int pixelPos = (y + j) * img->width + (x + i);
                    sum += img->data[pixelPos] * kernel[j + n][i + n];
                }
            }

            // Clamping des valeurs
            if (sum > 255) {
                sum = 255;
            } else if (sum < 0) {
                sum = 0;
            }

            tempData[y * img->width + x] = (unsigned char)sum;
        }
    }

    // Remplacement des données
    free(img->data);
    img->data = tempData;
}