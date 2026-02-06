#include "configReader.h"
#include "connectionManager.h"

#include <iostream>
#include <vector>
#include <thread>

int main(int argc, char* argv[]) {
    commonFile commonConfig = returnCommon("config/unzipped/project_config_file_small/project_config_file_small/Common.cfg");
    std::vector<peerInfo> peers = returnPeerInfo("config/unzipped/project_config_file_small/project_config_file_small/PeerInfo.cfg");
    printCommon(commonConfig);
    printPeerInfo(peers);

    
    
    return 0;
}