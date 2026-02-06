#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include <arpa/inet.h>

#include "configReader.h"

class connectionManager{
    private:
        int server_fd;
        std::vector<int> allServerThreads;
        std::vector<int> allClientThreads;
    public:
        connectionManager() = default;
        int connectToPeer(int port, const char* address, int id);
        int startServer(int port, int id);
        int exchange(int fd, int id);
        void closePeers(std::vector<int> threads);
        void closeConnections();
};



#endif