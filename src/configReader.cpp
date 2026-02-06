#include "configReader.h"

commonFile returnCommon(std::string path) {
    commonFile currentRun;

    std::fstream config(path);
    if(!config.is_open()) { //maybe add sdl_logerror later
        std::cout << "error opening the file" << std::endl;
        return currentRun;
    }

    std::string varible, val;
    while(config >> varible >> val) {
        if(varible == "NumberOfPreferredNeighbors") { //maybe there is a better way to do this lol
            currentRun.NumberOfPreferredNeighbors = std::stoi(val);
        }
        else if(varible == "UnchokingInterval") {
            currentRun.UnchokingInterval = std::stoi(val);
        }
        else if(varible == "OptimisticUnchokingInterval") {
            currentRun.OptimisticUnchokingInterval = std::stoi(val);
        }
        else if(varible == "FileName") {
            currentRun.FileName = val;
        }
        else if(varible == "FileSize") {
            currentRun.FileSize = std::stoi(val);
        }
        else {
            currentRun.PieceSize = std::stoi(val);
        }
    }

    config.close();

    return currentRun;
}

std::vector<peerInfo> returnPeerInfo(std::string path) {
    std::vector<peerInfo> peers;
    std::fstream config(path);
    if(!config.is_open()) {
        std::cout << "error opening the file" << std::endl;
        return {};
    }

    std::string id, hostname, port, hasfile;
    while(config >> id >> hostname >> port >> hasfile) {
        peerInfo peer;

        peer.hasFile = std::stoi(hasfile);
        peer.hostName = hostname;
        peer.peerID = std::stoi(id);
        peer.port = std::stoi(port);

        peers.push_back(peer);
    }
    
    return peers;
}

void printCommon(commonFile file) {
    std::cout << "Common.cfg" << std::endl;
    std::cout << "NumberOfPreferredNeighbors = "  << file.NumberOfPreferredNeighbors << std::endl;
    std::cout << "UnchokingInterval = "  << file.UnchokingInterval << std::endl;
    std::cout << "OptimisticUnchokingInterval = "  << file.OptimisticUnchokingInterval << std::endl;
    std::cout << "FileName = "  << file.FileName << std::endl;
    std::cout << "FileSize = "  << file.FileSize << std::endl;
    std::cout << "PieceSize = "  << file.PieceSize << "\n" << std::endl;
}

void printPeerInfo(std::vector<peerInfo> peers) {
    for(auto& peerinfo : peers) {
        std::cout << "Peer ID = " << peerinfo.peerID << std::endl;
        std::cout << "hasFile = " << peerinfo.hasFile << std::endl;
        std::cout << "hostName = " << peerinfo.hostName << std::endl;
        std::cout << "port = " << peerinfo.port << "\n" << std::endl;
    }
    std::cout << "" << std::endl;
}