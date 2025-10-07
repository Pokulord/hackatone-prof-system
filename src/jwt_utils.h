#pragma once
#include <string>
#include "jwt-cpp/jwt.h"

class JwtUtils {
private:
    std::string secretKey;
    int tokenExpirationSeconds;

public:
    JwtUtils(std::string secret, int expirationSec = 3600);

    std::string generateToken(const std::string& username, const std::string& role);
    bool verifyToken(const std::string& token, std::string& usernameOut);
};


