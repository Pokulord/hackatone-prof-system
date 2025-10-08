#pragma once
#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERR
    };

    explicit Logger(const std::string& filename);
    void log(Level level, const std::string& message);

private:
    std::string levelToString(Level level);
    std::ofstream outFile;
    std::mutex mtx;
};