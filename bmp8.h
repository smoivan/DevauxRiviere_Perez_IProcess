/*
 * Structure qui représente une image en noir et blanc (8 bits)
 */
typedef struct {
    unsigned char header[54];        // En-tête du fichier BMP
    unsigned char colorTable[1024];  // Table des couleurs pour le noir et blanc
    unsigned char * data;           // Les pixels de l'image

    unsigned int width;             // Largeur de l'image en pixels
    unsigned int height;            // Hauteur de l'image en pixels
    unsigned int colorDepth;        // Nombre de bits par pixel (ici 8)
    unsigned int dataSize;          // Taille totale des données de l'image
} t_bmp8;

/*
 * Ouvre et charge une image depuis un fichier
 * Renvoie : l'image chargée ou NULL si erreur
 */
t_bmp8 * bmp8_loadImage(const char * filename);

/*
 * Enregistre une image dans un fichier
 * Renvoie : 0 si réussi, -1 si erreur
 */
int bmp8_saveImage(const char * filename, t_bmp8 * img);

/*
 * Libère la mémoire utilisée par une image
 */
void bmp8_free(t_bmp8 * img);

/*
 * Affiche les caractéristiques de l'image (taille, etc.)
 */
void bmp8_printInfo(t_bmp8 * img);

/*
 * Inverse les couleurs de l'image (effet négatif)
 */
void bmp8_negative(t_bmp8 * img);

/*
 * Modifie la luminosité de l'image
 * value : positif pour éclaircir, négatif pour assombrir
 */
void bmp8_brightness(t_bmp8 * img, int value);

/*
 * Convertit l'image en noir et blanc pur selon un seuil
 * threshold : valeur entre 0 et 255
 */
void bmp8_threshold(t_bmp8 * img, int threshold);