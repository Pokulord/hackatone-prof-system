#include "RefreshTokenService.h"

RefreshTokenService::RefreshTokenService(IUserRepository& repo, JwtUtils& jwt)
    : tokenRepo(repo), jwtUtils(jwt) {}

bool RefreshTokenService::isTokenRevoked(const std::string& token) {
    return tokenRepo.isTokenRevoked(token);
}

bool RefreshTokenService::revokeToken(const std::string& token) {
    return tokenRepo.revokeToken(token);
}

std::string RefreshTokenService::refreshToken(const std::string& oldToken) {
    if (isTokenRevoked(oldToken)) {
        throw std::runtime_error("Token revoked");
    }

    std::string username;
    if (!jwtUtils.verifyToken(oldToken, username)) {
        throw std::runtime_error("Invalid token");
    }

    auto userOpt = tokenRepo.getUserByUsername(username);
    if (!userOpt) {
        throw std::runtime_error("User not found");
    }

    if (!revokeToken(oldToken)) {
        throw std::runtime_error("Failed to revoke token");
    }

    std::string role = (userOpt->getRole() == Role::ADMIN) ? "admin" : "user";
    return jwtUtils.generateToken(username, role);
}
