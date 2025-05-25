#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bmp8.h"
#include "bmp24.h"
#include "bmp24equalize.h"
#include "bmp8equalize.h"

// Detection de la profondeur de couleur d'une image BMP (8 ou 24 bits)
int detectBitDepth(const char *filename) {
    FILE *f = fopen(filename, "rb"); // Ouvrir en binaire
    if (!f) return -1; // Erreur d’ouverture
    fseek(f, 28, SEEK_SET); // Se placer sur le champ des bits
    uint16_t bits;
    fread(&bits, sizeof(uint16_t), 1, f); // Lire la profondeur
    printf("DEBUG bits = %d\n", bits);
    fclose(f); // Fermer le fichier
    return (bits == 8 || bits == 24) ? bits : -1;
}

// Menu de filtres pour les images en 8 bits
void applyFilters8(t_bmp8 *img) {
    int choix;
    while (1) {
        printf("\nSelectionnez un filtre a appliquer :\n");
        printf("1. Negatif\n");
        printf("2. Luminosite\n");
        printf("3. Noir et blanc\n");
        printf("4. Revenir au menu principal\n");
        printf(">>> Votre choix : ");
        scanf("%d", &choix);
        getchar(); // Absorber le saut de ligne

        switch (choix) {
            case 1: bmp8_negative(img); printf("Negatif applique.\n"); break;
            case 2: {
                int valeur;
                printf("Valeur de luminosite (-255 a 255) : ");
                scanf("%d", &valeur); getchar();
                bmp8_brightness(img, valeur);
                printf("Luminosite modifiee.\n");
                break;
            }
            case 3: {
                int seuil;
                printf("Valeur de seuil (0 a 255) : ");
                scanf("%d", &seuil); getchar();
                bmp8_threshold(img, seuil);
                printf("Conversion en noir et blanc terminee.\n");
                break;
            }
            case 4: return;
            default: printf("Option invalide.\n");
        }
    }
}

// Menu de filtres pour les images en 24 bits
void applyFilters24(t_bmp24 *img) {
    int choix;
    while (1) {
        printf("\nSelectionnez un filtre a appliquer :\n");
        printf("1. Negatif\n");
        printf("2. Luminosite\n");
        printf("3. Niveaux de gris\n");
        printf("4. Egalisation d'histogramme\n");
        printf("5. Revenir au menu principal\n");
        printf(">>> Votre choix : ");
        scanf("%d", &choix);
        getchar();

        switch (choix) {
            case 1: bmp24_negative(img); printf("Negatif applique.\n"); break;
            case 2: {
                int valeur;
                printf("Valeur de luminosite (-255 a 255) : ");
                scanf("%d", &valeur); getchar();
                bmp24_brightness(img, valeur);
                printf("Luminosite modifiee.\n");
                break;
            }
            case 3: bmp24_grayscale(img); printf("Image convertie en niveaux de gris.\n"); break;
            case 4: bmp24_equalize(img); printf("Egalisation d'histogramme effectuee.\n"); break;
            case 5: return;
            default: printf("Option invalide.\n");
        }
    }
}

// Fonction principale
int main(void) {
    char cheminFichier[256]; // Stockage du chemin
    int choix;
    int bits = -1; // Profondeur detectee
    t_bmp8 *img8 = NULL;
    t_bmp24 *img24 = NULL;

    printf("\nBienvenue dans l'application d'edition d'images BMP !\n");

    while (1) {
        printf("\nVeuillez choisir une option\n");
        printf("1. Ouvrir une image\n");
        printf("2. Sauvegarder une image\n");
        printf("3. Appliquer un filtre\n");
        printf("4. Afficher les informations\n");
        printf("5. Quitter\n");
        printf(">>> Votre choix : ");

        char entree[10];
        fgets(entree, sizeof(entree), stdin);
        if (sscanf(entree, "%d", &choix) != 1) {
            printf("Saisie incorrecte. Veuillez entrer un nombre de 1 a 5.\n");
            continue;
        }

        switch (choix) {
            case 1: {
                printf("Chemin vers l'image : ");
                scanf("%255s", cheminFichier); getchar();

                bits = detectBitDepth(cheminFichier);
                printf("DEBUG : profondeur detectee = %d\n", bits);

                // Liberer les anciennes images
                if (img8) { bmp8_free(img8); img8 = NULL; }
                if (img24) { bmp24_free(img24); img24 = NULL; }

                if (bits == 8) {
                    img8 = bmp8_loadImage(cheminFichier);
                    if (!img8) {
                        printf("Erreur : impossible de charger l'image 8 bits.\n");
                        bits = -1;
                    } else {
                        printf("Image 8 bits chargee avec succes !\n");
                    }
                } else if (bits == 24) {
                    img24 = bmp24_loadImage(cheminFichier);
                    if (!img24) {
                        printf("Erreur : chargement de l'image 24 bits echoue.\n");
                        bits = -1;
                    } else {
                        printf("Image 24 bits chargee avec succes !\n");
                        printf("DEBUG – Premier pixel : R=%d G=%d B=%d\n",
                               img24->data[0][0].red,
                               img24->data[0][0].green,
                               img24->data[0][0].blue);
                    }
                } else {
                    printf("Format non reconnu. Seuls les fichiers BMP 8 et 24 bits sont supportes.\n");
                }
                break;
            }

            case 2: {
                if (bits == 8 && img8) {
                    printf("Nom du fichier de sortie : ");
                    scanf("%255s", cheminFichier); getchar();
                    bmp8_saveImage(cheminFichier, img8);
                } else if (bits == 24 && img24) {
                    printf("Nom du fichier de sortie : ");
                    scanf("%255s", cheminFichier); getchar();
                    bmp24_saveImage(img24, cheminFichier);
                } else {
                    printf("Veuillez d'abord charger une image.\n");
                }
                break;
            }

            case 3:
                if (bits == 8 && img8) applyFilters8(img8);
                else if (bits == 24 && img24) applyFilters24(img24);
                else printf("Aucune image n'est chargee.\n");
                break;

            case 4:
                if (bits == 8 && img8) {
                    bmp8_printInfo(img8);
                } else if (bits == 24 && img24) {
                    printf("Informations sur l'image :\n");
                    printf("Largeur       : %d px\n", img24->width);
                    printf("Hauteur       : %d px\n", img24->height);
                    printf("Profondeur    : %d bits\n", img24->colorDepth);
                } else {
                    printf("Veuillez charger une image pour afficher les infos.\n");
                }
                break;

            case 5:
                if (img8) bmp8_free(img8);
                if (img24) bmp24_free(img24);
                printf("Fermeture du programme. A bientot !\n");
                return 0;

            default:
                printf("Choix invalide. Entrez un nombre entre 1 et 5.\n");
        }
    }
}
