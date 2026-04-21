#include "logger.h"
#include <ctime>

Logger::Logger(int peerID) : myID(peerID) {
    logFile.open("log_peer_" + std::to_string(peerID) + ".log");
}

std::string Logger::timestamp() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return std::string(buf);
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);
    logFile << "[" << timestamp() << "]: " << message << std::endl;
}

Logger::~Logger() {
    if (logFile.is_open()) logFile.close();
}
