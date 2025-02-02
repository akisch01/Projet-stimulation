#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000
#define BUFFER_SIZE 512

int main() {
    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Création du socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erreur : création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Erreur : adresse IP invalide");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Connexion au serveur
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur : connexion au serveur");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connecté au système central. En attente des mises à jour...\n");

    // Réception des données en boucle
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_received = recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            perror("Erreur : réception des données");
            break;
        }
        // Affichage des données reçues
        printf("\n=== Mise à jour ===\n%s\n", buffer);
    }

    // Fermeture du socket
    close(socket_fd);
    return 0;
}