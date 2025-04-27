#include <stdlib.h>
#include <string.h>
#include "bmp8.h"

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *image = fopen(filename, "rb");


    if (image == NULL) {
        printf("Erreur lors de l'ouverture du fichier, le fichier n'existe pas!\n");
        return NULL;
    }


