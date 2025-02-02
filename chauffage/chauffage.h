#ifndef CHAUFFAGE_H
#define CHAUFFAGE_H

#include <netinet/in.h>

void sendHeatingLevel(int socket, struct sockaddr_in *multicastAddr, socklen_t addrLen);
void receiveCommands(int socket);

#endif // CHAUFFAGE_H