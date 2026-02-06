#include "configReader.h"
#include "connectionManager.h"

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

    connectionManager test1;
    std::string option = argv[1];
    if(option == "1") {
        test1.startServer(6000, 1000);
    }
    else if (option == "0") {
        test1.connectToPeer(6000, "127.0.0.1", 1001);
    }
    


    return 0;
}