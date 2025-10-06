#pragma once

#include "../entities/User.hpp"
#include <string>
#include <optional>

namespace domain::services {
    struct DecodedToken {
        std::string username;
        entities::UserRole role;
    };

    class ITokenGenerator {
    public:
        virtual ~ITokenGenerator() = default;
        
        virtual std::string generateToken(const entities::User& user) = 0;
        virtual bool validateToken(const std::string& token) = 0;
        virtual std::optional<DecodedToken> decodeToken(const std::string& token) = 0;
    };
}
