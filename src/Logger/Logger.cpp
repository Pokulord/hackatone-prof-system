#include "Logger.h"
#include <iostream>
#include <ctime>
<<<<<<< HEAD
=======
#include <iomanip>
>>>>>>> test_ver

Logger::Logger(const std::string& filename) {
    outFile.open(filename, std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

<<<<<<< HEAD
void Logger::log(Level level, const std::string& message) {
=======
void Logger::log(Logger::Level level, const std::string& message) {
>>>>>>> test_ver
    if (!outFile.is_open()) return;
    std::lock_guard<std::mutex> lock(mtx);

    std::time_t now = std::time(nullptr);
    char timeBuf[20];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
<<<<<<< HEAD
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
=======
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
>>>>>>> test_ver
