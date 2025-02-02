#ifndef SYSTEME_CENTRAL_H
#define SYSTEME_CENTRAL_H

#include <pthread.h>
#include <netinet/in.h>

struct SharedData {
    int socket_tcp;               // Socket TCP pour les connexions des consoles
    int multicastSocket;          // Socket multicast pour envoyer des commandes au chauffage
    struct sockaddr_in multicastAddr; // Adresse multicast pour le chauffage
    pthread_mutex_t lock;         // Mutex pour la synchronisation
};

void *gestion_consoles(void *arg);
void *ecoute_thermometres(void *arg);

#endif // SYSTEME_CENTRAL_H