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
<<<<<<< HEAD
        ERROR
=======
        ERR
>>>>>>> test_ver
    };

    explicit Logger(const std::string& filename);
    void log(Level level, const std::string& message);

private:
    std::string levelToString(Level level);
    std::ofstream outFile;
<<<<<<< HEAD
    std::mutex mtx; // для потокобезопасности
};

=======
    std::mutex mtx;
};
>>>>>>> test_ver
