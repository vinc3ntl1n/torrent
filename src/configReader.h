#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

struct commonFile {
    int NumberOfPreferredNeighbors;
    int UnchokingInterval;
    int OptimisticUnchokingInterval;
    std::string FileName;
    int FileSize;
    int PieceSize;
};

struct peerInfo {
    int peerID;
    std::string hostName;
    int port;
    bool hasFile;
};

commonFile returnCommon(std::string path);
std::vector<peerInfo> returnPeerInfo(std::string path);
void printCommon(commonFile file);
void printPeerInfo(std::vector<peerInfo> peers);

#endif