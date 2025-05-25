#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Déclaration des prototypes des fonctions (qui devraient être dans des headers)
int lire_image8(const char* nom_fichier);  // pour bmp8.c
int lire_image24(const char* nom_fichier); // pour bmp24.c
void equalizer8(void);                     // pour equalize8.c
void equalizer24(void);                    // pour equalize24.c

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <fichier_image.bmp> <type>\n", argv[0]);
        printf("type: 8 pour BMP 8-bit ou 24 pour BMP 24-bit\n");
        return 1;
    }

    const char* nom_fichier = argv[1];
    int type = atoi(argv[2]);

    // Vérification de l'extension .bmp
    if (strlen(nom_fichier) < 4 || strcmp(nom_fichier + strlen(nom_fichier) - 4, ".bmp") != 0) {
        printf("Erreur : Le fichier doit avoir l'extension .bmp\n");
        return 1;
    }

    int resultat;
    switch (type) {
        case 8:
            // Traitement pour image 8-bit
            resultat = lire_image8(nom_fichier);
            if (resultat == 0) {
                equalizer8();
                printf("Traitement de l'image 8-bit réussi\n");
            } else {
                printf("Erreur lors du traitement de l'image 8-bit\n");
                return 1;
            }
            break;

        case 24:
            // Traitement pour image 24-bit
            resultat = lire_image24(nom_fichier);
            if (resultat == 0) {
                equalizer24();
                printf("Traitement de l'image 24-bit réussi\n");
            } else {
                printf("Erreur lors du traitement de l'image 24-bit\n");
                return 1;
            }
            break;

        default:
            printf("Type non valide. Utilisez 8 pour BMP 8-bit ou 24 pour BMP 24-bit\n");
            return 1;
    }

    return 0;
}