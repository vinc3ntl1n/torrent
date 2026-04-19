#include "fileManager.h"

FileManager::FileManager(int peerID, std::string fileName, int fileSize, int pieceSize, bool hasFile) {

    //store parameters as fieldsn int his class
    this->fileSize = fileSize;
    this->pieceSize = pieceSize;
    this->totalPieces = (fileSize + pieceSize - 1) / pieceSize;

    //file path using peer id and file name
    this->filePath = "unzipped/project_config_file_small/" + std::to_string(peerID) + "/" + fileName;


    if (hasFile) {
        // file should already exist we just open it
        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary);
    } else {
        // create an empty file of the right size
        std::fstream creator(filePath, std::ios::out | std::ios::binary);
        creator.close();
        
        //trick to fill wiht zeroes of the size of the file 
        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(fileSize - 1);
        file.write("", 1);
        file.flush();
    }

    if (!file.is_open()) {
    std::cerr << "failed to open " << filePath << std::endl;
    return;
    }

}

std::vector<uint8_t> FileManager::readPiece(int pieceIndex) {
    std::lock_guard<std::mutex> guard(fileMutex); // auto unlocks at end of functoin no matter what
    fileMutex.lock();
    int offset = pieceIndex * pieceSize;



    //handles last piece whihc is not exaclty 16k size
    int thisPieceSize = pieceSize;
    if (pieceIndex == totalPieces - 1) {
        thisPieceSize = fileSize - (pieceIndex * pieceSize);
    }   


    std::vector<uint8_t> output(thisPieceSize);
    file.seekg(offset);
    file.read((char*)output.data(), thisPieceSize);
    fileMutex.unlock();
    return output;
}

void FileManager::writePiece(int pieceIndex, std::vector<uint8_t> data) {
    std::lock_guard<std::mutex> guard(fileMutex);
    fileMutex.lock();
    int offset = pieceIndex * pieceSize;

    //handles last piece whihc is not exaclty 16k size
    int thisPieceSize = pieceSize;
    if (pieceIndex == totalPieces - 1) {
        thisPieceSize = fileSize - (pieceIndex * pieceSize);
    }  

    file.seekp(offset);
    file.write((char*)data.data(), data.size());
    file.flush(); //immediately writes
    fileMutex.unlock();
}

FileManager::~FileManager() {
    if (file.is_open()) {
        file.close();
    }
}