#pragma once

#include "../../domain/services/IPasswordHasher.hpp"
#include <string>

namespace infrastructure::adapters {
    /**
     * @brief A simple, insecure password hasher for development and testing purposes.
     * 
     * @warning This implementation is NOT secure and MUST NOT be used in production.
     * It stores passwords in plain text. It should be replaced with a proper
     * implementation like BcryptPasswordHasher.
     */
    class PlaintextPasswordHasher : public domain::services::IPasswordHasher {
    public:
        std::string hashPassword(const std::string& password) override;
        bool verifyPassword(const std::string& password, const std::string& hash) override;
    };
}
