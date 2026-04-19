#ifndef PEER_STATE_H
#define PEER_STATE_H

#include <cstdint>
#include <vector>
#include <map>
#include "configReader.h"
#include <mutex>

struct NeighborState {
    int peerID;
    std::vector<uint8_t> bitfield;
    bool isChoked;
    bool isInterested;
    bool iAmChoked;      // are THEY choking ME
    bool iAmInterested;  // am I interested in THEM
};

class PeerState {
private:
    peerInfo myInfo;
    std::vector<uint8_t> myBitfield;
    std::map<int, NeighborState> neighbors;
    std::mutex stateMutex;
    int totalPieces;

public:
    PeerState(peerInfo myInfo, int numOfPieces);
    
    // my bitfield
    bool hasPiece(int index);
    void setPiece(int index);
    bool isComplete();
    std::vector<uint8_t> getMyBitfield();
    
    // neighbor management
    void addNeighbor(int peerID);
    void setNeighborBitfield(int peerID, std::vector<uint8_t> bitfield);
    bool neighborHasPiece(int peerID, int pieceIndex);
    void setNeighborPiece(int peerID, int pieceIndex);
    
    // choke/interest state
    void setIAmChoked(int peerID, bool choked);
    void setTheyAreInterested(int peerID, bool interested);
    bool amIChokedBy(int peerID);
    bool isNeighborInterestedInMe(int peerID);
    
    // mutex
    void lock();
    void unlock();
};


#endif