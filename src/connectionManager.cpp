#include "connectionManager.h"

int connectionManager::startServer(int port) {
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage
}