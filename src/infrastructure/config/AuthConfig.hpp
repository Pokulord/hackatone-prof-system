#pragma once
#include "../../domain/interfaces/IAuthConfigProvider.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

namespace infrastructure::config {

class JsonAuthConfigProvider : public domain::interfaces::IAuthConfigProvider {
private:
    int maxAttempts_;
    std::chrono::minutes lockoutDuration_;

public:
    explicit JsonAuthConfigProvider(const std::string& configPath) {
        loadConfig(configPath);
    }

    int getMaxAttempts() const override { return maxAttempts_; }
    std::chrono::minutes getLockoutDuration() const override { return lockoutDuration_; }

private:
    void loadConfig(const std::string& configPath) {
        std::ifstream file(configPath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open auth config file");
        }

        nlohmann::json j;
        file >> j;

        if (!j.contains("max_attempts") || !j.contains("lockout_duration_minutes")) {
            throw std::runtime_error("Invalid config format");
        }

        maxAttempts_ = j["max_attempts"];
        lockoutDuration_ = std::chrono::minutes(j["lockout_duration_minutes"]);
        
        validateConfig();
    }

    void validateConfig() {
        if (maxAttempts_ <= 0) {
            throw std::invalid_argument("max_attempts must be positive");
        }
        if (lockoutDuration_.count() <= 0) {
            throw std::invalid_argument("lockout_duration_minutes must be positive");
        }
    }
};

}