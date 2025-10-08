#pragma once
#include "IUserRepository.h"
#include "jwt_utils.h"

class RefreshTokenService {
private:
    IUserRepository& userRepo;
    JwtUtils& jwtUtils;

public:
    RefreshTokenService(IUserRepository& repo, JwtUtils& jwt) : userRepo(repo), jwtUtils(jwt) {}

    bool isTokenRevoked(const std::string& token) {
        return userRepo.isTokenRevoked(token);
    }

    bool revokeToken(const std::string& token) {
        return userRepo.addExpiredToken(token);
    }

    std::string refreshToken(const std::string& oldToken) {
        if (isTokenRevoked(oldToken)) {
            throw std::runtime_error("Token revoked");
        }

        std::string username;
        if (!jwtUtils.verifyToken(oldToken, username)) {
            throw std::runtime_error("Invalid token");
        }

        auto userOpt = userRepo.getUserByUsername(username);
        if (!userOpt) {
            throw std::runtime_error("User not found");
        }

        if (!revokeToken(oldToken)) {
            throw std::runtime_error("Failed to revoke token");
        }

        std::string role = (userOpt->getRole() == Role::ADMIN) ? "admin" : "user";
        return jwtUtils.generateToken(username, role);
    }
};
