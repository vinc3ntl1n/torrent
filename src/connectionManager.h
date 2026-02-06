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

class connectionManager{
    private:
        std::vector<int> allThreads;
    public:
        int connectToPeer(int port, const char* address);
        int startServer(int port);
        int serverFunctions(int fd);
        int clientFunctions(int fd);
        void closeConnections();
};



#endif