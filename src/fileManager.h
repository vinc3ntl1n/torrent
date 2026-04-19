#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <cstdint>
#include <vector>
#include <map>
#include "configReader.h"
#include <mutex>




class FileManager {
private:
    std::fstream file;
    std::string filePath;
    int pieceSize;
    int fileSize;
    int totalPieces;
    std::mutex fileMutex;
    
public:
    FileManager(int peerID, std::string fileName, int fileSize, int pieceSize, bool hasFile);
    std::vector<uint8_t> readPiece(int pieceIndex);
    void writePiece(int pieceIndex, std::vector<uint8_t> data);
    ~FileManager();
};



#endif