#include "thermometre.h"
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
#define TCP_SERVER_IP "127.0.0.1"
#define TCP_SERVER_PORT 6000

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <multicastAddress> <multicastPort> <tcpAddress> <tcpPort>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *multicastAddress = MULTICAST_ADDRESS;
    int multicastPort = MULTICAST_PORT;
    const char *tcpAddress = TCP_SERVER_IP;
    int tcpPort = TCP_SERVER_PORT;

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

    if (bind(multicastSocket, (struct sockaddr *)&multicastAddr, sizeof(multicastAddr)) < 0) {
        perror("Erreur lors du bind du socket multicast");
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
    tcpAddr.sin_port = htons(tcpPort);
    if (inet_pton(AF_INET, tcpAddress, &tcpAddr.sin_addr) <= 0) {
        perror("Adresse TCP invalide");
        close(multicastSocket);
        close(tcpSocket);
        exit(EXIT_FAILURE);
    }

    if (connect(tcpSocket, (struct sockaddr *)&tcpAddr, sizeof(tcpAddr)) < 0) {
        perror("Erreur lors de la connexion au système central");
        close(multicastSocket);
        close(tcpSocket);
        exit(EXIT_FAILURE);
    }

    printf("Thermomètre connecté. En attente de données multicast...\n");

    while (1) {
        char buffer[BUFFER_SIZE];
        socklen_t addrLen = sizeof(multicastAddr);
        int n = recvfrom(multicastSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&multicastAddr, &addrLen);
        if (n < 0) {
            perror("Erreur lors de la réception des données multicast");
            continue;
        }

        buffer[n] = '\0';
        printf("Données reçues du multicast : %s\n", buffer); // Log supplémentaire

        if (strncmp(buffer, "TEMP", 4) == 0) {
            printf("Température reçue : %s\n", buffer);
            if (send(tcpSocket, buffer, n, 0) < 0) {
                perror("Erreur lors de l'envoi des données au système central");
            } else {
                printf("Température envoyée au système central : %s\n", buffer); // Log supplémentaire
            }
        }
    }

    close(multicastSocket);
    close(tcpSocket);
    return 0;
}