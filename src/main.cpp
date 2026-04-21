#include "configReader.h"
#include "connectionManager.h"
#include "peerState.h"
#include "fileManager.h"
#include "logger.h"

#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    commonFile commonConfig = returnCommon("Common.cfg");
    std::vector<peerInfo> peers = returnPeerInfo("PeerInfo.cfg");

    int myID = std::stoi(argv[1]);

    peerInfo myInfo;
    for (const auto& peer: peers) {
        if (peer.peerID == myID) {
            myInfo = peer;
        }
    }

    int totalPieces = (commonConfig.FileSize + commonConfig.PieceSize - 1) / commonConfig.PieceSize;

    PeerState myState(myInfo, totalPieces);
    FileManager myFileManager(myID, commonConfig.FileName, commonConfig.FileSize, commonConfig.PieceSize, myInfo.hasFile);
    Logger myLogger(myID);
    connectionManager connMgr(&myState, &myFileManager, &myLogger, (int)peers.size(), myID);

    std::thread serverThread(&connectionManager::startServer, &connMgr, myInfo.port, myID);

    std::thread unchokThread(&connectionManager::unchokingTimer, &connMgr,
        commonConfig.UnchokingInterval, commonConfig.NumberOfPreferredNeighbors);
    std::thread optThread(&connectionManager::optimisticTimer, &connMgr,
        commonConfig.OptimisticUnchokingInterval);
    unchokThread.detach();
    optThread.detach();

    for (const auto& peer : peers) {
        if (peer.peerID == myID) break;
        connMgr.connectToPeer(peer.port, peer.hostName.c_str(), myID);
    }

    serverThread.join();

    return 0;
}