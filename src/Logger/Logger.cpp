#include "Logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>

Logger::Logger(const std::string& filename) {
    outFile.open(filename, std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::log(Logger::Level level, const std::string& message) {
    if (!outFile.is_open()) return;
    std::lock_guard<std::mutex> lock(mtx);

    std::time_t now = std::time(nullptr);
    char timeBuf[20];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    outFile << "[" << timeBuf << "] [" << levelToString(level) << "] " << message << std::endl;
}

std::string Logger::levelToString(Logger::Level level) {
    switch (level) {
        case Logger::Level::DEBUG: return "DEBUG";
        case Logger::Level::INFO: return "INFO";
        case Logger::Level::WARNING: return "WARNING";
        case Logger::Level::ERR: return "ERROR";
        default: return "UNKNOWN";
    }
}