#pragma once

#include "domain/services/ITokenGenerator.hpp"

namespace infrastructure::adapters {
    /**
     * @brief A simple, insecure token generator for development and testing.
     * 
     * @warning This implementation is NOT secure and MUST NOT be used in production.
     * It creates predictable tokens. It should be replaced with a proper
     * JWT implementation.
     */
    class SimpleTokenGenerator : public domain::services::ITokenGenerator {
    public:
        std::string generateToken(const domain::entities::User& user) override;
        bool validateToken(const std::string& token) override;
        std::optional<domain::services::DecodedToken> decodeToken(const std::string& token) override;
    };
}
