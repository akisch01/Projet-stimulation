#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

void afficher_menu() {
    printf("\n=== Console de Commande ===\n");
    printf("1. Modifier le niveau de chauffage\n");
    printf("2. Quitter\n");
    printf("Choisissez une option : ");
}

void envoyer_commande(int socket_fd, const char *commande) {
    if (send(socket_fd, commande, strlen(commande), 0) == -1) {
        perror("Erreur lors de l'envoi de la commande");
        close(socket_fd);
        exit(EXIT_FAILURE);
    } else {
        printf("Commande envoyée : %s\n", commande);
    }
}

int main() {
    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[256];
    int choix, niveau;
    char piece[50];

    // Création du socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Adresse IP invalide");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Connexion au serveur
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur de connexion au système central");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connexion établie avec le système central.\n");

    while (1) {
        afficher_menu();
        scanf("%d", &choix);

        if (choix == 1) {
            printf("Entrez le nom de la pièce (max 49 caractères) : ");
            scanf("%49s", piece); // Limite à 49 caractères
            printf("Entrez le niveau de chauffage (0-5) : ");
            scanf("%d", &niveau);

            if (niveau < 0 || niveau > 5) {
                printf("Niveau de chauffage invalide. Réessayez.\n");
                continue;
            }

            snprintf(buffer, sizeof(buffer), "PIECE:%s;NIVEAU:%d", piece, niveau);
            envoyer_commande(socket_fd, buffer);

        } else if (choix == 2) {
            printf("Fermeture de la console.\n");
            break;
        } else {
            printf("Option invalide. Réessayez.\n");
        }
    }

    close(socket_fd);
    return 0;
}