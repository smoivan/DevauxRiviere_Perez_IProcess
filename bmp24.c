#include "bmp24.h"
#include <string.h>


 */
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}


 */
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = (t_pixel **)malloc(height * sizeof(t_pixel *));
    if (pixels == NULL) {
        printf("Erreur: Impossible d'allouer de la mémoire pour les pixels\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        pixels[i] = (t_pixel *)malloc(width * sizeof(t_pixel));
        if (pixels[i] == NULL) {
            printf("Erreur: Impossible d'allouer de la mémoire pour les pixels de la ligne %d\n", i);
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            return NULL;
        }
    }

    return pixels;
}

void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (pixels == NULL) {
        return;
    }

    for (int i = 0; i < height; i++) {
        if (pixels[i] != NULL) {
            free(pixels[i]);
        }
    }
    free(pixels);
}


 */
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (img == NULL) {
        printf("Erreur: Impossible d'allouer de la mémoire pour l'image\n");
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;

    img->data = bmp24_allocateDataPixels(width, height);
    if (img->data == NULL) {
        free(img);
        return NULL;
    }

    return img;
}


void bmp24_free(t_bmp24 *img) {
    if (img == NULL) {
        return;
    }

    if (img->data != NULL) {
        bmp24_freeDataPixels(img->data, img->height);
    }

    free(img);
}

void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {

    uint32_t position = image->header.offset + ((image->height - 1 - y) * image->width + x) * 3;


    uint8_t bgr[3];
    file_rawRead(position, bgr, sizeof(uint8_t), 3, file);


    image->data[y][x].blue = bgr[0];
    image->data[y][x].green = bgr[1];
    image->data[y][x].red = bgr[2];
}


 */
void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_readPixelValue(image, x, y, file);
        }
    }
}


 */
void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {

    uint32_t position = image->header.offset + ((image->height - 1 - y) * image->width + x) * 3;


    uint8_t bgr[3];
    bgr[0] = image->data[y][x].blue;
    bgr[1] = image->data[y][x].green;
    bgr[2] = image->data[y][x].red;

    file_rawWrite(position, bgr, sizeof(uint8_t), 3, file);
}



void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }


    uint16_t type;
    int32_t width, height;
    uint16_t bits;

    file_rawRead(BITMAP_MAGIC, &type, sizeof(uint16_t), 1, file);
    file_rawRead(BITMAP_WIDTH, &width, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_HEIGHT, &height, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_DEPTH, &bits, sizeof(uint16_t), 1, file);

    if (type != BMP_TYPE) {
        printf("Erreur: Le fichier n'est pas au format BMP valide\n");
        fclose(file);
        return NULL;
    }

    if (bits != DEFAULT_DEPTH) {
        printf("Erreur: L'image n'est pas en 24 bits (profondeur: %d)\n", bits);
        fclose(file);
        return NULL;
    }


    t_bmp24 *img = bmp24_allocate(width, height, bits);
    if (img == NULL) {
        fclose(file);
        return NULL;
    }


    file_rawRead(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawRead(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);


    bmp24_readPixelData(img, file);

    fclose(file);
    return img;
}


void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    if (img == NULL) {
        printf("Erreur: Image invalide\n");
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Erreur: Impossible de créer le fichier %s\n", filename);
        return;
    }


    file_rawWrite(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawWrite(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);


    bmp24_writePixelData(img, file);

    fclose(file);
}


void bmp24_negative(t_bmp24 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur: Image invalide\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
}


void bmp24_grayscale(t_bmp24 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur: Image invalide\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {

            uint8_t gray_value = (img->data[y][x].red + img->data[y][x].green + img->data[y][x].blue) / 3;


            img->data[y][x].red = gray_value;
            img->data[y][x].green = gray_value;
            img->data[y][x].blue = gray_value;
        }
    }
}


void bmp24_brightness(t_bmp24 *img, int value) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur: Image invalide\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {

            int new_red = img->data[y][x].red + value;
            int new_green = img->data[y][x].green + value;
            int new_blue = img->data[y][x].blue + value;


            img->data[y][x].red = (new_red > 255) ? 255 : ((new_red < 0) ? 0 : new_red);
            img->data[y][x].green = (new_green > 255) ? 255 : ((new_green < 0) ? 0 : new_green);
            img->data[y][x].blue = (new_blue > 255) ? 255 : ((new_blue < 0) ? 0 : new_blue);
        }
    }
}