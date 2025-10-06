#pragma once

#include <string>

namespace domain::services {
    class IPasswordHasher {
    public:
        virtual ~IPasswordHasher() = default;
        
        virtual std::string hashPassword(const std::string& password) = 0;
        virtual bool verifyPassword(const std::string& password, const std::string& hash) = 0;
    };
}