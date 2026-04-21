#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger {
private:
    std::ofstream logFile;
    int myID;
    std::mutex logMutex;
    std::string timestamp();

public:
    Logger(int peerID);
    void log(const std::string& message);
    ~Logger();
};

#endif
