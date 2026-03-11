#include "connectionManager.h"

/*
connectionManager.cpp
This .cpp file handles the networking layer for the p2p communication.
Key tasks which are in-progress
- Listens for incoming TCP connections from other peers (working - VL Rocket)
- Initiates outgoing TCP connections to peers (TODO-Andy: has started but has a bug in loop)
- Performs handshake exchange on new connections (TODO-Andy send / receive has some layout)
- Maintains socket file descriptors for all active peer connections (TODO-Andy - has some layout)



TODOs that need attention:
- Message sending/receiving (choke, unchoke, interested, etc)
- Tracking which peer is on which connection
*/

int connectionManager::startServer(int port, int id) {
    int connection_fd, opt = 1;
    struct sockaddr_in address;
    struct sockaddr_storage;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    if((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        std::cout << "socket error" << std::endl;
        return -1;
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cout << "setsockopt error" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //any ip address but could change later
    address.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cout << "failed to bind" << std::endl;
        return -1;
    }

    if(listen(server_fd, 4) < 0) {
        std::cout << "listening error" << std::endl;
        return -1;
    }

    while(true) {
        if((connection_fd = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0) {
        std::cout << "accept error" << std:: endl;
        }

        allServerThreads.push_back(connection_fd);
        std::thread serverWorker(&connectionManager::exchange, this, connection_fd, id);
        serverWorker.detach();

    }

    close(server_fd);

    return 0;
}

int connectionManager::connectToPeer(int port, const char* address, int id) {
    struct addrinfo hints, *servinfo, *p;
    int val, client_fd;
    char s[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if((val = getaddrinfo(address, std::to_string(port).c_str(), &hints, &servinfo)) != 0) {
        std::cout << "error getting addrinfo" << std::endl;
        return -1;
    }

    for(p = servinfo; p != NULL; p->ai_addr) { // TODO-Andy Fix: Bug in loop - should be p = p->ai_next
        
        if((client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cout << "next client socket" << std::endl;
            continue;
        }

        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        inet_ntop(p->ai_family, &(ipv4->sin_addr), s, sizeof(s));

        if(connect(client_fd, p->ai_addr, p->ai_addrlen) == -1) {
            std::cout << "error connection to server" << std::endl;
            close(client_fd);
            continue;
        }

        break;
    }

    if(p == nullptr) {
        std::cout << "could not bind a socket" << std::endl;
        return -1;
    }

    freeaddrinfo(servinfo);

    exchange(client_fd, id);

    close(client_fd);    
    return 0;
}

int connectionManager::exchange(int fd, int id) {
    char handshake[32], buffer[33] = { 0 };
    int numberOfBytes;

    std::cout << buffer << std::endl;

    memset(handshake, 0, sizeof(handshake));
// TODO-Andy: Peer ID should be 4-byte binary (network byte order), not ASCII string
    memcpy(handshake, "P2PFILESHARINGPROJ", 18);
    memcpy(handshake + 18, "0000000000", 10);
    std::string insertID = std::to_string(id);
    memcpy(handshake + 28, insertID.c_str(), 4);

    std::cout << "about to send handshake" << std::endl;

    if(send(fd, handshake, sizeof(handshake), 0) == -1) { //or 32 ig
        std::cout << "error sending handshake" << std::endl;
        return -1;
    }

    std::cout << "handshake sent, waiting for recv..." << std::endl;
// TODO-Andy: Validate received handshake - check header is "P2PFILESHARINGPROJ" and
  verify peer ID
    if((numberOfBytes = recv(fd, buffer, 32, 0)) == -1) {
        std::cout << "recv error" << std::endl;
        return -1;
    }

    buffer[numberOfBytes] = '\0';

    std::cout << buffer << std::endl;


    return 0;
}

void connectionManager::closePeers(std::vector<int> threads) {
    for(int thread : threads) {
        close(thread);
    }
}

void connectionManager::closeConnections() {
    closePeers(allServerThreads);
    closePeers(allClientThreads);
}