#include "connectionManager.h"

int connectionManager::startServer(int port) {
    int server_fd, new_fd, opt = 1;
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
        if((new_fd = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0) {
        std::cout << "accept error" << std:: endl;
        }

        allThreads.push_back(new_fd);
        std::thread serverWorker(serverFunctions, new_fd);
        serverWorker.detach();

    }
    
    closeConnections();

    close(server_fd);

    return 0;
}


int connectionManager::serverFunctions(int fd) {

}

int connectionManager::connectToPeer(int port, const char* address) {
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

    for(p = servinfo; p != NULL; p->ai_addr) {
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

    

    close(client_fd);    

    return 0;
}

int connectionManager::clientFunctions(int fd) {
    return -1;
}

void connectionManager::closeConnections() {
    for(int thread : allThreads) {
        close(thread);
    }
}