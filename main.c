#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

// Definition des longeurs maximales
#define max_username 100
#define max_password (SHA256_DIGEST_LENGTH * 2 + 1)
#define max_captcha 10

struct Account {
    char Username[max_username];
    char* Password;
};

typedef struct cellule {
    struct Account compte;
    struct cellule* suiv;
} CELLULE;

typedef CELLULE* list;

list comptes = NULL; // Liste chaînée où sauvegarder les comptes créés.

int Counter = 0; // Compteur de comptes créés.
int Connected = 0; // 0 : Déconnecté , 1 : Connecté.
char Session[max_username]; //Session de l'utilisateur.

//Fonction qui HASH les mots de passes.
unsigned char* hash(const char* mdp) {
    unsigned char hashedpwd[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)mdp, strlen(mdp), hashedpwd);

    // Convertir le tableau d'octets en une chaîne hexadécimale / +1 pour le caractère '\0'
    char* hashedpwds = malloc(SHA256_DIGEST_LENGTH * 2 + 1); //Tableau de Caractères hashés.

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hashedpwds[i * 2], "%02x", hashedpwd[i]);
    }

    return hashedpwds;
}

//Fonction qui génère une CAPTCHA aléatoire.
char* genrand(int n) {
    // Allouer de la mémoire pour la captcha (n caractères + '\0').
    char* captcha = (char*)malloc((n + 1) * sizeof(char));

    const char ens[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@&$!#?";
    const int len = strlen(ens);
    srand(time(0)); // Modifier le seed/graine à chaque fois pour avoir des CAPTCHAs différents.
    for (int i = 0; i < n; i++) {
        int j = rand() % len;
        *(captcha + i) = ens[j];
    }

    *(captcha + n) = '\0'; // Fin de la chaîne

    return captcha;
}

//Fonction qui enregistre les comptes dans un fichier texte.
void saveAccounts() {
    FILE* file = fopen("accounts.txt", "w");

    list aux = comptes;
    int i = 1;
    while  (aux != NULL) {
        fprintf(file, "%i. Username : %s , Password : %s\n", i, aux->compte.Username, aux->compte.Password);
        aux = aux->suiv;
        i++;
    }
    fclose(file);
}

// Charger les comptes enregistrés.
void loadAccounts() {
    FILE* file = fopen("accounts.txt", "r");
    if (file == NULL) {
        FILE* file = fopen("accounts.txt", "w");
        return;
    }

    // Réinitialiser Counter pour stocker les comptes chargés.
    Counter = 0;

    char line[200];

    while (Counter < 100 && fgets(line, sizeof(line), file) != NULL) {
        char username[max_username];
        char password[max_password];
        if (sscanf(line, " %*d. Username : %99s , Password : %64s", username, password) == 2) {
            // Ajouter le compte à la fin de la liste chaînée
            list nouv = (list) malloc(sizeof(CELLULE));
            strcpy(nouv->compte.Username, username);
            nouv->compte.Password = malloc(strlen(password) + 1);
            strcpy(nouv->compte.Password, password);

            nouv->suiv = NULL;

            if (comptes == NULL) {
                comptes = nouv;
            } else {
                list aux = comptes;
                while (aux->suiv != NULL) {
                    aux = aux->suiv;
                }
                aux->suiv = nouv;
            }

            Counter++;
        }
    }
    fclose(file);
}

//Fonction qui affiche les comptes existants.
void affiche() {
    system("cls");
    printf("\n\n");
    printf("                 La liste des comptes : \n");
    
    list aux = comptes;
    int i = 1;
    while (aux != NULL) {
        printf("+---|---------------------------------------------------------+\n");
        printf(" %d. |                         %s\n", i, aux->compte.Username);
        aux = aux->suiv;
        i++;
    }
    printf("+---|---------------------------------------------------------+\n");
}

// Vérification de l'existence de l'utilisateur.
char* verif(char* name) {
    if (Counter == 0)
        return NULL;
    list aux = comptes;
    while (aux != NULL) {
        if (strcmp(aux->compte.Username, name) == 0) {
            return aux->compte.Password;
        }
        aux = aux->suiv;
    }
    return NULL;
}

// Fonction qui crée des comptes.
void reg() {
    struct Account a;
    char passwd[16];
    printf("\n---------# Inscription : #---------\n");
    printf("Entrer votre nom : ");
    scanf("%s", a.Username);
    do {
        printf("Entrer votre mot de passe (de taille maximale 15) : ");
        scanf("%s", passwd);
    } while (strlen(passwd) > 15);

    a.Password = hash(passwd);

    printf("\n");

    char* UserCaptcha = genrand(10);

    printf("         ******************************\n");
    printf("         *                            *\n");
    printf("         *    CAPTCHA : %s    *\n", UserCaptcha);
    printf("         *                            *\n");
    printf("         ******************************\n\n");

    char CaptchaTyped[max_captcha + 1];
    printf("Veuillez entrer le CAPTCHA ci-dessus : ");
    scanf("%s", CaptchaTyped);

    // Verification du CAPTCHA
    if (strcmp(UserCaptcha, CaptchaTyped) == 0) {
        if (verif(a.Username) == NULL) {
            printf("CAPTCHA correct. Le compte a bien ete cree.\n");

            // Ajouter le compte à la fin de la liste chaînée
            list nouv = (list) malloc(sizeof(CELLULE));
            strcpy(nouv->compte.Username, a.Username);
            nouv->compte.Password = malloc(strlen(a.Password) + 1);
            strcpy(nouv->compte.Password, a.Password);

            nouv->suiv = NULL;

            if (comptes == NULL) {
                comptes = nouv;
            } else {
                list aux = comptes;
                while (aux->suiv != NULL) {
                    aux = aux->suiv;
                }
                aux->suiv = nouv;
            }

            Counter++;
        }
        else
            printf("\nCompte existe deja. Veuillez reessayer.\n");
    }
    else {
        printf("CAPTCHA invalide. Le compte n'a pas ete cree.\n");
    }
    free(UserCaptcha);
}

// Connexion aux comptes.
void connexion() {
    char user[max_username];
    char pass[max_password];
    system("cls");
    printf("\n\n---------# Connexion : #---------\n");
    printf("Donner votre nom : ");
    scanf("%s", user);

    printf("Donner votre mot de passe : ");
    scanf("%s", pass);

    if (verif(user) != NULL && strcmp(hash(pass), verif(user)) == 0) {
        Connected = 1;
        strcpy(Session, user);
        system("cls");
        printf("\nConnexion Reussite !\n");
    }
    else {
        Connected = 0;
        printf("\nVeuillez verifier vos coordonnees !\n");
    }
}

// Fonction pour changer le mot de passe d'un utilisateur
void ChangerMdp() {
    char ancien_mdp[max_password];
    char nouv_mdp[max_password];

    printf("\n---------# Changer le mot de passe : #---------\n");

    printf("Donner votre ancien mot de passe : ");
    scanf("%s", ancien_mdp);

    // Vérifier si l'utilisateur existe et si le mot de passe correspond
    if (verif(Session) != NULL && strcmp(hash(ancien_mdp), verif(Session)) == 0) {
        printf("Donner votre nouveau mot de passe : ");
        scanf("%s", nouv_mdp);

        // Mettre à jour le mot de passe dans la liste chaînée
        list aux = comptes;
        while (aux != NULL) {
            if (strcmp(aux->compte.Username, Session) == 0) {
                free(aux->compte.Password);
                aux->compte.Password = hash(nouv_mdp);
                printf("\nLe mot de passe a ete change avec succes.\n");
                break;
            }
            aux = aux->suiv;
        }
    }
    else {
        printf("Veuillez verifier vos informations d'identification.\n");
    }
}

void menu() {
    int c;
    do {
        printf("\n  +------------------~~~---------------+");
        printf("\n  |            MENU PRINCIPAL          |\n");
        printf("  +------------------~~~---------------+\n");
        printf("  | 1. Creation de compte.             |\n");
        printf("  | 2. Connexion.                      |\n");
        printf("  | 3. Afficher les comptes.           |\n");
        printf("  |                                    |\n");
        printf("  |                                    |\n");
        printf("  | 9. Effacer l'ecran.                |\n");
        printf("  | 0. Exit.                           |\n");
        printf("  +-----------------#------------------+\n\n");
        do {
            printf("Saisissez votre choix : ");
            scanf("%d", &c);
        } while (c != 0 && c != 1 && c != 2 && c != 3 && c != 9);
        switch (c) {
        case 1:
            reg();
            saveAccounts();
            break;
        case 2:
            connexion();
            break;
        case 3:
            affiche();
            break;
        case 9:
            system("cls");
            break;
        }
    } while (c != 0 && Connected == 0);
}

void menuc() {
    int c, m;
    do {
        printf("\n  +------------------~~~---------------+");
        printf("\n  |            MENU CONNEXION          |\n");
        printf("  +------------------~~~---------------+\n");
        printf("  | 1. Generer un MDP robuste          |\n");
        printf("  | 2. Modifier votre MDP              |\n");
        printf("  |                                    |\n");
        printf("  | 8. Deconnexion.                    |\n");
        printf("  | 9. Effacer l'ecran.                |\n");
        printf("  | 0. Exit.                           |\n");
        printf("  +-----------------#------------------+\n\n");

        printf("Saisissez votre choix : ");
        scanf("%d", &c);
        switch (c) {
        case 1:
            printf("Donner la longueur desiree : ");
            scanf("%d", &m);
            printf("\n------------# Mot de Passe #------------\n");
            printf("               %s                 \n", genrand(m));
            printf("----------------------------------------\n");
            break;
        case 2:
            ChangerMdp();
            saveAccounts();
            break;
        case 8:
            Connected = 0;
            system("cls");
            menu();
            break;
        case 9:
            system("cls");
            break;
        }
    } while (c != 0);
}


int main() {
    loadAccounts();
    menu();
    if (Connected == 1) {
        menuc();
    }

    // Libérer la mémoire allouée pour la liste chaînée à la fin du programme
    list aux = comptes;
    CELLULE* next;
    while (aux != NULL) {
        next = aux->suiv;
        free(aux->compte.Password);
        free(aux);
        aux = next;
    }

    return 0;
}