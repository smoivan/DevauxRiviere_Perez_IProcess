#include <stdlib.h>
#include <string.h>
#include "bmp8.h"

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");


    if (file == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, file    );


    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned short *)&img->header[28];
    img->dataSize = *(unsigned int *)&img->header[34];

    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (img == NULL) {
        printf("Erreur: Allocation mémoire échouée\n");
        fclose(file);
        return NULL;
    }
    if (fread(img->header, sizeof(unsigned char), 54, file) != 54) {
        printf("Erreur: Le fichier n'est pas au format BMP valide\n");
        free(img);
        fclose(file);
        return NULL;
    }

    if (img->header[0] != 'B' || img->header[1] != 'M') {
        printf("Erreur: Le fichier n'est pas au format BMP\n");
        free(img);
        fclose(file);
        return NULL;
    }


    if (img->colorDepth != 8) {
        printf("Erreur: L'image n'est pas en 8 bits\n");
        free(img);
        fclose(file);
        return NULL;
    }

    if (fread(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        printf("Erreur: Impossible de lire la table de couleurs\n");
        free(img);
        fclose(file);
        return NULL;
    }

    img->data = (unsigned char *)malloc(img->dataSize * sizeof(unsigned char));
    if (img->data == NULL) {
        printf("Erreur: Allocation mémoire échouée pour les données\n");
        free(img);
        fclose(file);
        return NULL;
    }


    if (fread(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        printf("Erreur: Impossible de lire les données de l'image\n");
        free(img->data);
        free(img);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return img;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (img == NULL) {
        printf("Erreur: Image invalide\n");
        return;
    }
    size_t header = fwrite(img->header, sizeof(unsigned char), 54, file);
        if (header != 54) {
            printf("Error occured while writing the header");
            fclose(file);
            return;
        }
