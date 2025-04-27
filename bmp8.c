#include <stdlib.h>
#include <string.h>
#include "bmp8.h"

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *image = fopen(filename, "rb");


    if (image == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }


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

    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned short *)&img->header[28];
    img->dataSize = *(unsigned int *)&img->header[34];

