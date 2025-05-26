/**
 * @file bmp24.c
 *
 * @brief
 * Ce fichier contient toutes les fonctions pour charger, enregistrer et
 * modifier des images couleur BMP 24 bits: lecture, écriture, négatif,
 * niveaux de gris, ajustement de luminosité et filtres (flou, contour, etc.).
 *
 * @author [Aurelien Devaux-Riviere]
 * @date   [27/04/25]
 */

#include "bmp24.h"
#include <string.h>
#include <stdlib.h>




void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}







void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}


/* FONCTIONS DE GESTION MÉMOIRE */

/* bmp24_allocateDataPixels
 * Rôle : Alloue un tableau 2D pour stocker les pixels
 * Paramètres :
 *   width  - Largeur de l'image
 *   height - Hauteur de l'image
 * Retour : Tableau 2D de pixels ou NULL si erreur
 */
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

/* bmp24_freeDataPixels
 * Rôle : Libère la mémoire d'un tableau de pixels
 * Paramètres :
 *   pixels - Tableau à libérer
 *   height - Nombre de lignes du tableau
 */
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


/* FONCTIONS DE LECTURE/ÉCRITURE PIXELS */

/* bmp24_readPixelValue
 * Rôle : Lit un pixel depuis le fichier BMP
 * Paramètres :
 *   image - Image en cours de lecture
 *   x, y  - Coordonnées du pixel
 *   file  - Fichier source
 * Note : Gère l'inversion verticale du format BMP
 */
void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {

    uint32_t position = image->header.offset + ((image->height - 1 - y) * image->width + x) * 3;


    uint8_t bgr[3];
    file_rawRead(position, bgr, sizeof(uint8_t), 3, file);


    image->data[y][x].blue = bgr[0];
    image->data[y][x].green = bgr[1];
    image->data[y][x].red = bgr[2];
}





void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_readPixelValue(image, x, y, file);
        }
    }
}


/* bmp24_writePixelValue
 * Rôle : Écrit un pixel dans le fichier BMP
 * Paramètres : Similaires à readPixelValue mais pour l'écriture
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
        printf("Erreur lors de l'ouverture du fichier!\n");
        return NULL;
    }


    t_bmp_header header;
    t_bmp_info header_info;

    // Read headers
    file_rawRead(0, &header, sizeof(t_bmp_header), 1, file);
    file_rawRead(sizeof(t_bmp_header), &header_info, sizeof(t_bmp_info), 1, file);

    if (header.type != 0x4D42) {  // "BM" en hexadécimal
        printf("Erreur: Le fichier n'est pas au format BMP\n");
        fclose(file);
        return NULL;
    }


    int width = header_info.width;
    int height = header_info.height;
    int colorDepth = header_info.bits;

    if (colorDepth != 24) {
        printf("Cette image ne fait pas 24 bits");
        fclose(file);
        return NULL;
    }

    t_bmp24 *image = bmp24_allocate(width, height, colorDepth);
    if (image == NULL) {
        fclose(file);
        return NULL;
    }

    // Set headers
    image->header = header;
    image->header_info = header_info;

    // Read pixel data
    bmp24_readPixelData(image, file);

    fclose(file);
    return image;
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





t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    t_pixel result = {0, 0, 0};
    float red_sum = 0.0f, green_sum = 0.0f, blue_sum = 0.0f;
    int n = kernelSize / 2;


    for (int j = -n; j <= n; j++) {
        for (int i = -n; i <= n; i++) {

            if (x + i >= 0 && x + i < img->width && y + j >= 0 && y + j < img->height) {
                int kernel_x = i + n;
                int kernel_y = j + n;

                red_sum += img->data[y + j][x + i].red * kernel[kernel_y][kernel_x];
                green_sum += img->data[y + j][x + i].green * kernel[kernel_y][kernel_x];
                blue_sum += img->data[y + j][x + i].blue * kernel[kernel_y][kernel_x];
            }
        }
    }


    result.red = (red_sum > 255.0f) ? 255 : ((red_sum < 0.0f) ? 0 : (uint8_t)red_sum);
    result.green = (green_sum > 255.0f) ? 255 : ((green_sum < 0.0f) ? 0 : (uint8_t)green_sum);
    result.blue = (blue_sum > 255.0f) ? 255 : ((blue_sum < 0.0f) ? 0 : (uint8_t)blue_sum);

    return result;
}






float **create_box_blur_kernel() {
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }
    return kernel;
}








float **create_gaussian_blur_kernel() {
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
    }

    kernel[0][0] = 1.0f / 16.0f; kernel[0][1] = 2.0f / 16.0f; kernel[0][2] = 1.0f / 16.0f;
    kernel[1][0] = 2.0f / 16.0f; kernel[1][1] = 4.0f / 16.0f; kernel[1][2] = 2.0f / 16.0f;
    kernel[2][0] = 1.0f / 16.0f; kernel[2][1] = 2.0f / 16.0f; kernel[2][2] = 1.0f / 16.0f;

    return kernel;
}







float **create_outline_kernel() {
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
    }

    kernel[0][0] = -1.0f; kernel[0][1] = -1.0f; kernel[0][2] = -1.0f;
    kernel[1][0] = -1.0f; kernel[1][1] =  8.0f; kernel[1][2] = -1.0f;
    kernel[2][0] = -1.0f; kernel[2][1] = -1.0f; kernel[2][2] = -1.0f;

    return kernel;
}







float **create_emboss_kernel() {
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
    }

    kernel[0][0] = -2.0f; kernel[0][1] = -1.0f; kernel[0][2] = 0.0f;
    kernel[1][0] = -1.0f; kernel[1][1] =  1.0f; kernel[1][2] = 1.0f;
    kernel[2][0] =  0.0f; kernel[2][1] =  1.0f; kernel[2][2] = 2.0f;

    return kernel;
}







float **create_sharpen_kernel() {
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
    }

    kernel[0][0] =  0.0f; kernel[0][1] = -1.0f; kernel[0][2] =  0.0f;
    kernel[1][0] = -1.0f; kernel[1][1] =  5.0f; kernel[1][2] = -1.0f;
    kernel[2][0] =  0.0f; kernel[2][1] = -1.0f; kernel[2][2] =  0.0f;

    return kernel;
}








void free_kernel(float **kernel, int size) {
    for (int i = 0; i < size; i++) {
        free(kernel[i]);
    }
    free(kernel);
}







void apply_filter(t_bmp24 *img, float **kernel, int kernelSize) {
    if (img == NULL || img->data == NULL || kernel == NULL) {
        printf("Erreur: Paramètres invalides\n");
        return;
    }


    t_pixel **result = bmp24_allocateDataPixels(img->width, img->height);
    if (result == NULL) {
        return;
    }


    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            result[y][x] = bmp24_convolution(img, x, y, kernel, kernelSize);
        }
    }


    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = result[y][x];
        }
    }


    bmp24_freeDataPixels(result, img->height);
}






void bmp24_boxBlur(t_bmp24 *img) {
    float **kernel = create_box_blur_kernel();
    apply_filter(img, kernel, 3);
    free_kernel(kernel, 3);
}







void bmp24_gaussianBlur(t_bmp24 *img) {
    float **kernel = create_gaussian_blur_kernel();
    apply_filter(img, kernel, 3);
    free_kernel(kernel, 3);
}







void bmp24_outline(t_bmp24 *img) {
    float **kernel = create_outline_kernel();
    apply_filter(img, kernel, 3);
    free_kernel(kernel, 3);
}






void bmp24_emboss(t_bmp24 *img) {
    float **kernel = create_emboss_kernel();
    apply_filter(img, kernel, 3);
    free_kernel(kernel, 3);
}






void bmp24_sharpen(t_bmp24 *img) {
    float **kernel = create_sharpen_kernel();
    apply_filter(img, kernel, 3);
    free_kernel(kernel, 3);
}
