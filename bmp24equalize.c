#include "bmp24equalize.h"
#include <stdlib.h>
#include <math.h>

#define RGB_MAX 255
#define NIVEAUX_GRIS 256

/* FONCTIONS DE CONVERSION COULEUR */

/* rgb2yuv
 * Rôle : Convertit une couleur de l'espace RGB vers YUV
 * Paramètres :
 *   R, G, B - Composantes RGB d'entrée (0-255)
 *   Y, U, V - Pointeurs vers les composantes YUV résultantes
 * Formules :
 *   Y = 0.299R + 0.587G + 0.114B     (luminance)
 *   U = -0.14713R - 0.28886G + 0.436B (chrominance bleue)
 *   V = 0.615R - 0.51499G - 0.10001B  (chrominance rouge)
 */
static void rgb2yuv(uint8_t R, uint8_t G, uint8_t B,
                    float *Y, float *U, float *V)
{
    *Y =  0.299f * R + 0.587f * G + 0.114f * B;
    *U = -0.14713f * R - 0.28886f * G + 0.436f * B;
    *V =  0.615f * R - 0.51499f * G - 0.10001f * B;
}

/* yuv2rgb
 * Rôle : Convertit une couleur de l'espace YUV vers RGB
 * Paramètres :
 *   Y, U, V - Composantes YUV d'entrée
 *   R, G, B - Pointeurs vers les composantes RGB résultantes (0-255)
 * Formules :
 *   R = Y + 1.13983V
 *   G = Y - 0.39465U - 0.58060V
 *   B = Y + 2.03211U
 * Note : Les valeurs sont automatiquement bornées entre 0 et 255
 */
static void yuv2rgb(float Y, float U, float V,
                    uint8_t *R, uint8_t *G, uint8_t *B)
{
    int r = (int)roundf(Y + 1.13983f * V);
    int g = (int)roundf(Y - 0.39465f * U - 0.58060f * V);
    int b = (int)roundf(Y + 2.03211f * U);

    // Clamp les valeurs entre 0 et 255
    *R = (uint8_t)(r < 0 ? 0 : (r > 255 ? 255 : r));
    *G = (uint8_t)(g < 0 ? 0 : (g > 255 ? 255 : g));
    *B = (uint8_t)(b < 0 ? 0 : (b > 255 ? 255 : b));
}

/* FONCTION PRINCIPALE D'ÉGALISATION */

/* bmp24_equalize
 * Rôle : Égalise l'histogramme d'une image couleur
 * Paramètre :
 *   img - Image à traiter
 * Processus en 4 étapes :
 * 
 * 1. Construction de l'histogramme
 *    - Conversion RGB→YUV pour chaque pixel
 *    - Comptage des occurrences de Y (luminance)
 *    - Tableau hist[256] contenant les fréquences
 *
 * 2. Calcul de la fonction de répartition (CDF)
 *    - Somme cumulée de l'histogramme
 *    - Recherche du premier niveau non nul (cdf_min)
 *
 * 3. Création de la table de correspondance
 *    - Pour chaque niveau i :
 *      map[i] = ((cdf[i] - cdf_min)/(N - cdf_min)) * 255
 *    - N = nombre total de pixels
 *
 * 4. Application de la transformation
 *    - Pour chaque pixel :
 *      - Conversion RGB→YUV
 *      - Remplacement de Y par sa valeur égalisée
 *      - Conversion YUV→RGB
 *
 * Gestion mémoire :
 *   - Allocation dynamique pour hist[] et cdf[]
 *   - Libération en fin de fonction
 *   - Vérification des allocations
 */
void bmp24_equalize(t_bmp24 *img) {
    if (!img || !img->data) return;

    int largeur = img->width;
    int hauteur = img->height;
    int N = largeur * hauteur;

    // 1) Construction de l'histogramme sur le canal Y (luminance)
    unsigned int *hist = calloc(256, sizeof(unsigned int));
    if (!hist) return;

    for (int y = 0; y < hauteur; y++) {
        for (int x = 0; x < largeur; x++) {
            t_pixel p = img->data[y][x];
            float Yf, Uf, Vf;
            rgb2yuv(p.red, p.green, p.blue, &Yf, &Uf, &Vf);
            int Yi = (int)roundf(Yf);
            if (Yi < 0) Yi = 0;
            else if (Yi > 255) Yi = 255;
            hist[Yi]++;
        }
    }

    // 2) Calcul de la fonction de répartition cumulée (CDF)
    unsigned int *cdf = calloc(256, sizeof(unsigned int));
    if (!cdf) {
        free(hist);
        return;
    }
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    // Recherche du plus petit cdf non nul (cdf_min)
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // 3) Construction de la table de correspondance (lookup table)
    uint8_t map[256];
    for (int i = 0; i < 256; i++) {
        map[i] = (uint8_t)roundf(((float)(cdf[i] - cdf_min) / (float)(N - cdf_min)) * 255.0f);
    }

    // 4) Application de l'égalisation à l'image
    for (int y = 0; y < hauteur; y++) {
        for (int x = 0; x < largeur; x++) {
            t_pixel p = img->data[y][x];
            float Yf, Uf, Vf;
            rgb2yuv(p.red, p.green, p.blue, &Yf, &Uf, &Vf);
            int Yi = (int)roundf(Yf);
            if (Yi < 0) Yi = 0;
            else if (Yi > 255) Yi = 255;

            // On remplace Y par la valeur égalisée tout en gardant U et V inchangés
            yuv2rgb(map[Yi], Uf, Vf,
                    &img->data[y][x].red,
                    &img->data[y][x].green,
                    &img->data[y][x].blue);
        }
    }

    free(hist);
    free(cdf);
}