#include "Logger.h"
#include <iostream>
#include <ctime>

Logger::Logger(const std::string& filename) {
    outFile.open(filename, std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::log(Level level, const std::string& message) {
    if (!outFile.is_open()) return;
    std::lock_guard<std::mutex> lock(mtx);

    std::time_t now = std::time(nullptr);
    char timeBuf[20];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    outFile << "[" << timeBuf << "] [" << levelToString(level) << "] " << message << "\n";
    outFile.flush();
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case Level::DEBUG: return "DEBUG";
        case Level::INFO: return "INFO";
        case Level::WARNING: return "WARNING";
        case Level::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
