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
#include "messageHandler.h"
#include "logger.h"
#include <map>
#include <set>
#include <mutex>

class connectionManager{
    private:
        int server_fd;
        std::vector<int> allServerThreads;
        std::vector<int> allClientThreads;
        FileManager *fileManager;
        PeerState *peerState;
        Logger *logger;
        std::map<int, int> peerFDs;
        std::mutex fdMutex;
        std::set<int> requestedPieces;
        std::map<int, int> pendingPerPeer;
        std::mutex requestMutex;

        bool recvAll(int fd, uint8_t* buf, int len);
        bool sendAll(int fd, const uint8_t* buf, int len);
        void sendMessage(int fd, Message& msg);
        void broadcastHave(int pieceIndex, int excludePeer);
        int optimisticPeer = -1;
        int totalPeers = 0;
        int myID = 0;
    public:
        connectionManager(PeerState *ps, FileManager *fm, Logger *lg, int numPeers, int peerID);
        int connectToPeer(int port, const char* address, int id);
        int startServer(int port, int id);
        int exchange(int fd, int id, bool isServer);
        void closePeers(std::vector<int> threads);
        void closeConnections();
        void unchokingTimer(int interval, int k);
        void optimisticTimer(int interval);
};



#endif