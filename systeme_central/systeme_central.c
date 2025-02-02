#include "systeme_central.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#ifndef SO_REUSEPORT
#define SO_REUSEPORT 15
#endif
#define BUFFER_SIZE 1024
#define MULTICAST_ADDRESS "239.255.255.250"
#define MULTICAST_PORT 5000
#define TCP_PORT 6000
#define JAVA_SERVER_IP "127.0.0.1"
#define JAVA_SERVER_PORT 5001

void sendToJavaServer(const char *data) {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de la création du socket");
        return;
    }


    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(JAVA_SERVER_PORT);
    inet_pton(AF_INET, JAVA_SERVER_IP, &servaddr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Erreur lors de la connexion au serveur Java");
        close(sockfd);
        return;
    }

    if (send(sockfd, data, strlen(data), 0) < 0) {
        perror("Erreur lors de l'envoi des données");
    } else {
        printf("Données envoyées au serveur Java : %s\n", data); // Log supplémentaire
    }

    close(sockfd);
}

void *gestion_consoles(void *arg) {
    struct SharedData *data = (struct SharedData *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(data->socket_tcp, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            perror("Erreur lors de l'acceptation d'une connexion console");
            continue;
        }

        int n = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Commande reçue de la console : %s\n", buffer);

            char roomName[50];
            int heatingLevel;
            if (sscanf(buffer, "PIECE:%[^;];NIVEAU:%d", roomName, &heatingLevel) == 2) {
                printf("Commande extraite : Pièce = %s, Niveau = %d\n", roomName, heatingLevel);

                char data[BUFFER_SIZE];
                snprintf(data, BUFFER_SIZE, "%s;20.0;%d", roomName, heatingLevel);
                sendToJavaServer(data);
            } else {
                printf("Format de commande invalide : %s\n", buffer);
            }
        }

        close(clientSocket);
    }

    return NULL;
}

void *ecoute_thermometres(void *arg) {
    char *config = (char *)arg;
    char multicastAddress[BUFFER_SIZE];
    int multicastPort;
    sscanf(config, "%s %d", multicastAddress, &multicastPort);

    int multicastSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (multicastSocket < 0) {
        perror("Erreur lors de la création du socket multicast");
        pthread_exit(NULL);
    }

    int opt = 1;
    if (setsockopt(multicastSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Erreur lors de la configuration de SO_REUSEADDR");
        close(multicastSocket);
        pthread_exit(NULL);
    }
    if (setsockopt(multicastSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("Erreur lors de la configuration de SO_REUSEPORT");
        close(multicastSocket);
        pthread_exit(NULL);
    }

    struct sockaddr_in multicastAddr;
    memset(&multicastAddr, 0, sizeof(multicastAddr));
    multicastAddr.sin_family = AF_INET;
    multicastAddr.sin_port = htons(multicastPort);
    if (inet_pton(AF_INET, multicastAddress, &multicastAddr.sin_addr) <= 0) {
        perror("Adresse multicast invalide");
        close(multicastSocket);
        pthread_exit(NULL);
    }

    if (bind(multicastSocket, (struct sockaddr *)&multicastAddr, sizeof(multicastAddr)) < 0) {
        perror("Erreur lors du bind du socket multicast");
        close(multicastSocket);
        pthread_exit(NULL);
    }

    printf("Écoute des mises à jour des thermomètres sur %s:%d\n", multicastAddress, multicastPort);

    char buffer[BUFFER_SIZE];
    while (1) {
        socklen_t addrLen = sizeof(multicastAddr);
        int n = recvfrom(multicastSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&multicastAddr, &addrLen);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Données reçues d'un thermomètre : %s\n", buffer);

            double temperature;
            if (sscanf(buffer, "TEMP %lf", &temperature) == 1) {
                printf("Température extraite : %.2f\n", temperature);

                char data[BUFFER_SIZE];
                snprintf(data, BUFFER_SIZE, "Salon;%.2f;0", temperature);
                sendToJavaServer(data);
            } else {
                printf("Format de données invalide : %s\n", buffer);
            }
        }
    }

    printf("Données reçues du thermomètre : %s\n", buffer);

    close(multicastSocket);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <multicastAddress> <multicastPort> <tcpPort>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *multicastAddress = MULTICAST_ADDRESS;
    int multicastPort = MULTICAST_PORT;
    int tcpPort = TCP_PORT;

    struct SharedData data;
    pthread_mutex_init(&data.lock, NULL);

    data.socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (data.socket_tcp < 0) {
        perror("Erreur lors de la création du socket TCP");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in tcpAddr;
    memset(&tcpAddr, 0, sizeof(tcpAddr));
    tcpAddr.sin_family = AF_INET;
    tcpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    tcpAddr.sin_port = htons(tcpPort);

    if (bind(data.socket_tcp, (struct sockaddr *)&tcpAddr, sizeof(tcpAddr)) < 0) {
        perror("Erreur lors du bind du socket TCP");
        exit(EXIT_FAILURE);
    }

    if (listen(data.socket_tcp, 5) < 0) {
        perror("Erreur lors de l'écoute sur le socket TCP");
        exit(EXIT_FAILURE);
    }

    printf("Système central prêt pour les connexions TCP sur le port %d.\n", tcpPort);

    pthread_t thread_console, thread_thermometre;
    pthread_create(&thread_console, NULL, gestion_consoles, &data);

    char config_multicast[BUFFER_SIZE];
    snprintf(config_multicast, BUFFER_SIZE, "%s %d", multicastAddress, multicastPort);
    pthread_create(&thread_thermometre, NULL, ecoute_thermometres, config_multicast);

    pthread_join(thread_console, NULL);
    pthread_join(thread_thermometre, NULL);

    close(data.socket_tcp);
    pthread_mutex_destroy(&data.lock);

    return 0;
}