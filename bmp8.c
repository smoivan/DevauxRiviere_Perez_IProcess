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
    fread(header, sizeof(unsigned char), 54, file);


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

    fwrite(img->header, sizeof(unsigned char), 54, file);


    fwrite(img->colorTable, sizeof(unsigned char), 1024, file);


    fwrite(img->data, sizeof(unsigned char), img->dataSize, file);

    fclose(file);
}






void bmp8_free(t_bmp8 *img) {
    if (img != NULL) {
        if (img->data != NULL) {
            free(img->data);
        }
        free(img);
    }
}






void bmp8_printInfo(t_bmp8 *img) {
    if (img == NULL) {
        printf("Erreur: Image invalide\n");
        return;
    }

    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u\n", img->dataSize);
}







void bmp8_negative(t_bmp8 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur: Image invalide\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
}







void bmp8_brightness(t_bmp8 *img, int value) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur: Image invalide\n");
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
        printf("Erreur: Image invalide\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
    }
}