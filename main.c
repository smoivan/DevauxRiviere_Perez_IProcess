#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constantes
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define MAX_FILENAME 256
#define EXTENSION ".bmp"

// Codes d'erreur
typedef enum {
    ERR_OK = 0,
    ERR_FORMAT,
    ERR_LECTURE,
    ERR_TRAITEMENT
} ErrorCode;

// Types d'image supportés
typedef enum {
    BMP_8BIT = 8,
    BMP_24BIT = 24
} BmpType;

// Prototypes
int lire_image8(const char* nom_fichier);
int lire_image24(const char* nom_fichier);
void equalizer8(void);
void equalizer24(void);

// Fonction de vérification du type
static int verifier_type(const char* type_str) {
    // Vérification que la chaîne contient uniquement des chiffres
    for (int i = 0; type_str[i] != '\0'; i++) {
        if (!isdigit(type_str[i])) return 0;
    }
    
    int type = atoi(type_str);
    return (type == BMP_8BIT || type == BMP_24BIT);
}

// Fonction de vérification du fichier
static ErrorCode verifier_fichier(const char* nom_fichier) {
    if (!nom_fichier) return ERR_FORMAT;
    
    size_t len = strlen(nom_fichier);
    if (len >= MAX_FILENAME) {
        fprintf(stderr, "Erreur : Nom de fichier trop long\n");
        return ERR_FORMAT;
    }
    
    if (len < strlen(EXTENSION) || 
        strcasecmp(nom_fichier + len - strlen(EXTENSION), EXTENSION) != 0) {
        fprintf(stderr, "Erreur : Le fichier doit avoir l'extension %s\n", EXTENSION);
        return ERR_FORMAT;
    }
    
    FILE* test = fopen(nom_fichier, "rb");
    if (!test) {
        perror("Erreur d'ouverture du fichier");
        return ERR_LECTURE;
    }
    fclose(test);
    
    return ERR_OK;
}

// Fonction de traitement
static ErrorCode traiter_image(const char* nom_fichier, BmpType type) {
    int resultat;
    
    switch (type) {
        case BMP_8BIT:
            resultat = lire_image8(nom_fichier);
            if (resultat == 0) {
                equalizer8();
                printf("Égalisation de l'image 8 bits réussie\n");
                return ERR_OK;
            }
            fprintf(stderr, "Échec du traitement de l'image 8 bits\n");
            return ERR_TRAITEMENT;

        case BMP_24BIT:
            resultat = lire_image24(nom_fichier);
            if (resultat == 0) {
                equalizer24();
                printf("Égalisation de l'image 24 bits réussie\n");
                return ERR_OK;
            }
            fprintf(stderr, "Échec du traitement de l'image 24 bits\n");
            return ERR_TRAITEMENT;

        default:
            return ERR_FORMAT;
    }
}

int main(int argc, char *argv[]) {
    // Vérification des arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <fichier_image%s> <type>\n", argv[0], EXTENSION);
        fprintf(stderr, "type: %d pour BMP %d-bit ou %d pour BMP %d-bit\n", 
                BMP_8BIT, BMP_8BIT, BMP_24BIT, BMP_24BIT);
        return EXIT_FAILURE;
    }

    // Validation du type
    if (!verifier_type(argv[2])) {
        fprintf(stderr, "Type non valide. Utilisez %d ou %d\n", BMP_8BIT, BMP_24BIT);
        return EXIT_FAILURE;
    }
    BmpType type = (BmpType)atoi(argv[2]);

    // Vérification du fichier
    ErrorCode err = verifier_fichier(argv[1]);
    if (err != ERR_OK) {
        return EXIT_FAILURE;
    }

    // Traitement de l'image
    err = traiter_image(argv[1], type);
    if (err != ERR_OK) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}