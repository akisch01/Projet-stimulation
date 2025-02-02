#include "chauffage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#ifndef SO_REUSEPORT
#define SO_REUSEPORT 15
#endif

#define BUFFER_SIZE 1024
#define MULTICAST_ADDRESS "239.255.255.250"
#define MULTICAST_PORT 5000
#define TCP_PORT 7000

static int heatingLevel = 0;

void sendHeatingLevel(int socket, struct sockaddr_in *multicastAddr, socklen_t addrLen) {
    char message[BUFFER_SIZE];
    snprintf(message, BUFFER_SIZE, "HEATING %d", heatingLevel);
    sendto(socket, message, strlen(message), 0, (struct sockaddr *)multicastAddr, addrLen);
    printf("Message de chauffage envoyé : %s\n", message);
}

void receiveCommands(int socket) {
    char buffer[BUFFER_SIZE];
    int n = recv(socket, buffer, BUFFER_SIZE - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Commande reçue : %s\n", buffer);
        int newLevel = atoi(buffer);
        if (newLevel >= 0 && newLevel <= 5) {
            heatingLevel = newLevel;
            printf("Nouveau niveau de chauffage : %d\n", heatingLevel);
        } else {
            printf("Commande invalide : %s\n", buffer);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <multicastAddress> <multicastPort> <tcpPort>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *multicastAddress = MULTICAST_ADDRESS;
    int multicastPort = MULTICAST_PORT;
    int tcpPort = TCP_PORT;

    int multicastSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (multicastSocket < 0) {
        perror("Erreur lors de la création du socket multicast");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(multicastSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Erreur lors de la configuration de SO_REUSEADDR");
        close(multicastSocket);
        exit(EXIT_FAILURE);
    }
    if (setsockopt(multicastSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("Erreur lors de la configuration de SO_REUSEPORT");
        close(multicastSocket);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in multicastAddr;
    memset(&multicastAddr, 0, sizeof(multicastAddr));
    multicastAddr.sin_family = AF_INET;
    multicastAddr.sin_port = htons(multicastPort);
    if (inet_pton(AF_INET, multicastAddress, &multicastAddr.sin_addr) <= 0) {
        perror("Adresse multicast invalide");
        close(multicastSocket);
        exit(EXIT_FAILURE);
    }

    int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket < 0) {
        perror("Erreur lors de la création du socket TCP");
        close(multicastSocket);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in tcpAddr;
    memset(&tcpAddr, 0, sizeof(tcpAddr));
    tcpAddr.sin_family = AF_INET;
    tcpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    tcpAddr.sin_port = htons(tcpPort);

    if (bind(tcpSocket, (struct sockaddr *)&tcpAddr, sizeof(tcpAddr)) < 0) {
        perror("Erreur lors du bind du socket TCP");
        close(multicastSocket);
        close(tcpSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(tcpSocket, 5) < 0) {
        perror("Erreur lors de l'écoute sur le socket TCP");
        close(multicastSocket);
        close(tcpSocket);
        exit(EXIT_FAILURE);
    }

    printf("Chauffage démarré. En attente de commandes TCP...\n");

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(tcpSocket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            perror("Erreur lors de l'acceptation d'une connexion TCP");
            continue;
        }

        receiveCommands(clientSocket);
        close(clientSocket);

        sendHeatingLevel(multicastSocket, &multicastAddr, sizeof(multicastAddr));
    }

    close(multicastSocket);
    close(tcpSocket);
    return 0;
}