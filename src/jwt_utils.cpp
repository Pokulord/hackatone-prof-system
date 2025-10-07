#include "jwt_utils.h"

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

        // Получаем username из токена
        usernameOut = decoded.get_payload_claim("username").as_string();

        // Можно добавить проверку срока действия если нужно (jwt-cpp проверяет автоматически)
        return true;
    } catch (...) {
        return false;
    }
}
