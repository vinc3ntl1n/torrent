#include "fileManager.h"

FileManager::FileManager(int peerID, std::string fileName, int fileSize, int pieceSize, bool hasFile) {

    //file path using peer id and file name
    std::string filePath = "unzipped/project_config_file_small" + std::to_string(peerID) + "/" + fileName;

    //file object 
    std::fstream file;
    file.open(filePath, std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open()) {
    std::cerr << "failed to open " << filePath << std::endl;
    return;
    }


}