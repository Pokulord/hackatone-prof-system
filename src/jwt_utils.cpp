#include "jwt_utils.h"
#include <chrono>

JwtUtils::JwtUtils(std::string secret, int expirationSec) : secretKey(std::move(secret)), tokenExpirationSeconds(expirationSec) {}

std::string JwtUtils::generateToken(const std::string& username, const std::string& role) {
    auto token = jwt::create()
        .set_issuer("auth_server")
        .set_type("JWS")
        .set_payload_claim("username", jwt::claim(username))
        .set_payload_claim("role", jwt::claim(role))
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{tokenExpirationSeconds})
        .sign(jwt::algorithm::hs256{secretKey});
    return token;
}

bool JwtUtils::verifyToken(const std::string& token, std::string& usernameOut) {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{secretKey})
            .with_issuer("auth_server");
        verifier.verify(decoded);

        usernameOut = decoded.get_payload_claim("username").as_string();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "JWT verification failed: " << e.what() << std::endl;
        return false;
    }
}

bool JwtUtils::isTokenExpired(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        auto exp = decoded.get_expires_at(); // Возвращает std::chrono::system_clock::time_point
        return std::chrono::system_clock::now() > exp;
    } catch (...) {
        return true; // при ошибке считаем, что токен истёк или некорректен
    }
}