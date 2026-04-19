#include "peerState.h"


/*
BITFIELD LAYOUT

each byte holds 8 pieces. piece 0 is the MOST significant bit (leftmost).

         byte 0              byte 1              byte 2
   [ 7 6 5 4 3 2 1 0 ]  [ 7 6 5 4 3 2 1 0 ]  [ 7 6 5 4 3 2 1 0 ]
   [ 0 1 2 3 4 5 6 7 ]  [ 8 9 ...      15 ]  [ 16 ...    22 23 ]
     ^                                         spare bits at end = 0

to find a piece i:
  bytePos      = i / 8
  bitInByte    = i % 8
  shiftAmount  = 7 - bitInByte
  mask         = 1 << shiftAmount

example: piece 10
  bytePos     = 1
  bitInByte   = 2
  shift       = 5
  mask        = 00100000

check:  (myBitfield[bytePos] & mask) != 0
set:    myBitfield[bytePos] |= mask
*/



PeerState::PeerState(peerInfo myInfo, int numOfPieces) {
    this->myInfo = myInfo;
    this->totalPieces = numOfPieces;


    //check if I have the entire file and if so fill my bitfield with ones; otherwise all 0s
    bool haveFile = myInfo.hasFile;
    int numBytes = (numOfPieces + 7) / 8; //ig i could do the math back in main

    if (haveFile) {
        myBitfield.assign(numBytes, 0x00);
        //ik this loops is a lot of work but instructions say that the last bits need to be 0
        // also this only happens once for the first peer
        for (int i = 0; i < numOfPieces; i++) {
            int bytePos = i /8;
            int bitInByte = i %8;
            int shiftAmount = 7 - bitInByte;
            uint8_t mask = 1 <<shiftAmount;
            myBitfield[bytePos] |= mask;
        }
    } else {
        myBitfield.assign(numBytes, 0x00);
    }
}

bool PeerState::hasPiece(int index) {
    int bytePos = index/8;
    int bitInByte = index % 8;
    int shiftAmount = 7 - bitInByte;
    uint8_t mask = 1 << shiftAmount;
    if (myBitfield[bytePos] & mask) {
        return true;
    }
    else return false;
}

void PeerState::setPiece(int index) {
    int bytePos = index/8;
    int bitInByte = index % 8;
    int shiftAmount = 7 - bitInByte;
    uint8_t mask = 1 << shiftAmount;
    myBitfield[bytePos] |= mask;
}

bool PeerState::isComplete() {
    for (int i = 0; i < totalPieces; ++i) {
        if (!hasPiece(i)) return false;
    }
    return true;
}

std::vector<uint8_t> PeerState::getMyBitfield() {
    return myBitfield;
}

void PeerState::addNeighbor(int peerID) {
    //insert new NeighborState into the map with defaults
    NeighborState newNeighbor;
    newNeighbor.peerID = peerID;
    neighbors[peerID] = newNeighbor;
}

void PeerState::setNeighborBitfield(int peerID, std::vector<uint8_t> bitfield) {
    neighbors[peerID].bitfield = bitfield;
}

bool PeerState::neighborHasPiece(int peerID, int pieceIndex) {


    //prevents bug of calling on a peers bitfield that doesnt exist yet
    if (neighbors.find(peerID) == neighbors.end()) return false;
    if (neighbors[peerID].bitfield.empty()) return false;

    //checking the correct bit
    int bytePos = pieceIndex/8;
    int bitInByte = pieceIndex % 8;
    int shiftAmount = 7 - bitInByte;
    uint8_t mask = 1 << shiftAmount;

    if (neighbors[peerID].bitfield[bytePos] & mask) {
        return true;
    }
    else return false;
}

void PeerState::setNeighborPiece(int peerID, int pieceIndex) {

    //prevents bug of calling on a peers bitfield that doesnt exist yet
    if (neighbors.find(peerID) == neighbors.end()) return;
    if (neighbors[peerID].bitfield.empty()) return;

    
    //checking the correct bit
    int bytePos = pieceIndex/8;
    int bitInByte = pieceIndex % 8;
    int shiftAmount = 7 - bitInByte;
    uint8_t mask = 1 << shiftAmount;

    neighbors[peerID].bitfield[bytePos] |= mask;

}

void PeerState::setIAmChoked(int peerID, bool choked) {
    neighbors[peerID].iAmChoked = choked;
}

void PeerState::setTheyAreInterested(int peerID, bool interested) {
    neighbors[peerID].isInterested = interested;
}

bool PeerState::amIChokedBy(int peerID) {
    return neighbors[peerID].iAmChoked;
}

bool PeerState::isNeighborInterestedInMe(int peerID) {
    return neighbors[peerID].isInterested;
}

void PeerState::lock() {
    stateMutex.lock();
}

void PeerState::unlock() {
    stateMutex.unlock();
}