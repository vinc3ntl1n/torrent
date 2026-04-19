#include "configReader.h"
#include "connectionManager.h"
#include "peerState.h"
#include "fileManager.h"

#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <bitset>
#include <string.h>
#include <string>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    commonFile commonConfig = returnCommon("config/unzipped/project_config_file_small/project_config_file_small/Common.cfg");
    std::vector<peerInfo> peers = returnPeerInfo("config/unzipped/project_config_file_small/project_config_file_small/PeerInfo.cfg");
    //printCommon(commonConfig);
    //printPeerInfo(peers);

    // connectionManager test1;
    // std::string option = argv[1];
    // if(option == "1") {
    //     test1.startServer(6000, 1001);
    // }
    // else if (option == "0") {
    //     test1.connectToPeer(6000, "127.0.0.1", 1002);
    // }
    
    // 1. get your peer ID from command line
    int myID = std::stoi(argv[1]);


    peerInfo myInfo; //static information from config file
    
    for (const auto& peer: peers) {
        if (peer.peerID == myID) {
            myInfo = peer; 
        } 
    }
    //calculate how many pieces there are (weird ceiling division i found online)
    int totalPieces = (commonConfig.FileSize + commonConfig.PieceSize - 1) / commonConfig.PieceSize;

//live runtime state of this peer
    PeerState myState(peerInfo myInfo, int totalPieces);




    return 0;
}