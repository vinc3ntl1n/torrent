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
#include "peerState.h"
#include "fileManager.h"

class connectionManager{
    private:
        int server_fd;
        std::vector<int> allServerThreads;
        std::vector<int> allClientThreads;
        //pointer to filemanager for this guy
        FileManager *fileManager;
        //pointer to shared state for this guy
        PeerState *peerState;
    public: 
        connectionManager(PeerState *ps, FileManager *fm); // constructor no longer default. takes in shared statae and file manager
        int connectToPeer(int port, const char* address, int id);
        int startServer(int port, int id);
        int exchange(int fd, int id);
        void closePeers(std::vector<int> threads);
        void closeConnections();
};



#endif