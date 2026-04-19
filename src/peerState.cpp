#include "peerState.h"



PeerState::PeerState(peerInfo myInfo, int numOfPieces) {
    this->myInfo = myInfo;
    this->totalPieces = numOfPieces;


    //check if I have the entire file and if so fill my bitfield with ones; otherwise all 0s
    bool haveFile = myInfo.hasFile;
    int numBytes = (numOfPieces + 7) / 8;

    if (haveFile) {
        myBitfield.assign(numBytes, 0xFF); // 0xFF = 11111111 all bits set to 1
    } else {
        myBitfield.assign(numBytes, 0x00);
    }
}

bool PeerState::hasPiece(int index) {
    
}

void PeerState::setPiece(int index) {

}

bool PeerState::isComplete() {
    //loop through every piece
}

std::vector<uint8_t> PeerState::getMyBitfield() {

}

void PeerState::addNeighbor(int peerID) {
    //insert new NeighborState into the map with defaults
}

void PeerState::setNeighborBitfield(int peerID, std::vector<uint8_t> bitfield) {

}

bool PeerState::neighborHasPiece(int peerID, int pieceIndex) {

}

void PeerState::setNeighborPiece(int peerID, int pieceIndex) {

}

void PeerState::setIAmChoked(int peerID, bool choked) {

}

void PeerState::setTheyAreInterested(int peerID, bool interested) {

}

bool PeerState::amIChokedBy(int peerID) {

}

bool PeerState::isNeighborInterestedInMe(int peerID) {

}

void PeerState::lock() {

}

void PeerState::unlock() {

}