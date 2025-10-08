#include "RefreshTokenService.h"
#include <stdexcept>

RefreshTokenService::RefreshTokenService(IUserRepository& repo, JwtUtils& jwt)
    : userRepo(repo), jwtUtils(jwt) {}

bool RefreshTokenService::isTokenRevoked(const std::string& token) {
    return userRepo.isTokenRevoked(token);
}

bool RefreshTokenService::revokeToken(const std::string& token) {
    return userRepo.addExpiredToken(token);
}

std::string RefreshTokenService::refreshToken(const std::string& oldToken) {
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

    std::string role = userOpt->getRole();
    return jwtUtils.generateToken(username, role);
}