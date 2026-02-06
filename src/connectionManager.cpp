#include "connectionManager.h"

int connectionManager::startServer(int port) {
    int server_fd, new_fd, opt = 1;
    ssize_t valread;
    struct sockaddr_in address;
    struct addrinfo hints, *servinfo, *p;
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

    if(listen(server_fd, 4) < 0) { //number is how many many other programs can be in the queue
        std::cout << "listening error" << std::endl;
        return -1;
    }

    if((new_fd = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0) {
        std::cout << "accept error" << std:: endl;
    }

    char temp[] = "temp";
    valread = read(new_fd, buffer, 1024 - 1);
    std::cout << buffer << std::endl;
    send(new_fd, temp, strlen(temp), 0);

    close(new_fd);

    close(server_fd);

    return 0;
}