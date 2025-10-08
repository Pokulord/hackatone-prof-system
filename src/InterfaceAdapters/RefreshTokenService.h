#pragma once
#include "IUserRepository.h"
#include "jwt_utils.h"

class RefreshTokenService {
private:
    IUserRepository& userRepo;
    JwtUtils& jwtUtils;

public:
    RefreshTokenService(IUserRepository& repo, JwtUtils& jwt);

    bool isTokenRevoked(const std::string& token);
    bool revokeToken(const std::string& token);
    std::string refreshToken(const std::string& oldToken);
};