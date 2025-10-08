#include "AuthEndpoints.h"
#include "../jwt-cpp/jwt.h"
#include <fstream>
#include <iostream>
#include <string>
#include <optional>

// Saves a token to a file.
bool saveTokenToFile(const std::string& token, const std::string& filepath) {
    std::ofstream ofs(filepath, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open token file for writing: " << filepath << std::endl;
        return false;
    }
    ofs << token;
    ofs.close();
    return true;
}

void registerAuthEndpoints(crow::SimpleApp& app, UserService& userService, 
    JwtUtils& jwtUtils, RefreshTokenService& refreshService, PgUserRepository& userRepo, Logger& logger) {
    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::Post)([&userService, &logger](const crow::request& req) {
        logger.log(Logger::Level::INFO, "POST /api/auth/register from " + req.remote_ip_address);
        auto body = crow::json::load(req.body);
        if (!body || !body.has("username") || !body.has("password")) {
            logger.log(Logger::Level::WARNING, "Registration failed: Invalid JSON. IP: " + req.remote_ip_address);
            return crow::response(400, "Invalid JSON or missing fields");
        }

        std::string username = body["username"].s();
        std::string password = body["password"].s();
        std::string role = "Guest"; // Default role
        if (body.has("role")) {
            role = body["role"].s();
        }

        bool res = userService.createUser(username, password, role, false);
        if (res) {
            logger.log(Logger::Level::INFO, "User registered successfully: " + username);
            return crow::response(200, "Registered");
        } else {
            logger.log(Logger::Level::WARNING, "Registration failed for user: " + username + ". User may already exist or role is invalid.");
            return crow::response(400, "Invalid role or user exists");
        }
    });

    CROW_ROUTE(app, "/api/auth/login").methods(crow::HTTPMethod::Post)([&userService, &jwtUtils, &logger](const crow::request& req) {
        logger.log(Logger::Level::INFO, "POST /api/auth/login from " + req.remote_ip_address);
        auto body = crow::json::load(req.body);
        if (!body || !body.has("username") || !body.has("password")) {
            logger.log(Logger::Level::WARNING, "Login failed: Invalid JSON. IP: " + req.remote_ip_address);
            return crow::response(400, "Invalid JSON or missing fields");
        }

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        auto userOpt = userService.authenticate(username, password);
        if (!userOpt) {
            logger.log(Logger::Level::WARNING, "Authentication failed for user: " + username);
            return crow::response(401, "Auth failed");
        }

        logger.log(Logger::Level::INFO, "User authenticated successfully: " + username);
        std::string role = userOpt->getRole();
        std::string token = jwtUtils.generateToken(username, role);

        const char* userEnv = getenv("USER");
        std::string usrstr = userEnv ? userEnv : "default";
        std::string tokenFilePath = "/home/" + usrstr + "/.myapp_token";
        if (!saveTokenToFile(token, tokenFilePath)) {
            logger.log(Logger::Level::ERR, "Could not save JWT token to file for user: " + username);
        }

        crow::json::wvalue res;
        res["access_token"] = token;
        res["username"] = username;
        res["role"] = role;
        res["mustChangePassword"] = userOpt->getMustChangePassword();
        return crow::response(res);
    });

    CROW_ROUTE(app, "/api/auth/refresh").methods(crow::HTTPMethod::Post)([&refreshService, &logger](const crow::request& req) {
        logger.log(Logger::Level::INFO, "POST /api/auth/refresh from " + req.remote_ip_address);
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.substr(0, 7) != "Bearer ") {
            logger.log(Logger::Level::WARNING, "Refresh token failed: Missing or invalid Authorization header. IP: " + req.remote_ip_address);
            return crow::response(401, "Invalid token");
        }
        std::string oldToken = authHeader.substr(7);

        try {
            std::string newToken = refreshService.refreshToken(oldToken);
            logger.log(Logger::Level::INFO, "Token refreshed successfully.");
            crow::json::wvalue res;
            res["access_token"] = newToken;
            return crow::response(res);
        } catch (const std::exception& e) {
            logger.log(Logger::Level::WARNING, "Token refresh failed: " + std::string(e.what()));
            return crow::response(401, e.what());
        }
    });

    CROW_ROUTE(app, "/api/auth/logout").methods(crow::HTTPMethod::Post)([&jwtUtils, &userRepo, &logger](const crow::request& req) {
        logger.log(Logger::Level::INFO, "POST /api/auth/logout from " + req.remote_ip_address);
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.substr(0,7) != "Bearer ") {
            return crow::response(401, "Unauthorized");
        }

        std::string token = authHeader.substr(7);
        std::string username;
        if (!jwtUtils.verifyToken(token, username)) {
            logger.log(Logger::Level::WARNING, "Logout failed: Invalid token.");
            return crow::response(401, "Unauthorized");
        }

        bool revoked = userRepo.addExpiredToken(token);
        if (revoked) {
            logger.log(Logger::Level::INFO, "User logged out successfully: " + username);
            return crow::response(200, "Logged out. Token revoked");
        } else {
            logger.log(Logger::Level::ERR, "Failed to revoke token for user: " + username);
            return crow::response(500, "Failed to revoke token");
        }
    });

    CROW_ROUTE(app, "/api/auth/revoke").methods(crow::HTTPMethod::Post)([&jwtUtils, &userRepo, &logger](const crow::request& req) {
        logger.log(Logger::Level::INFO, "POST /api/auth/revoke from " + req.remote_ip_address);
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.substr(0,7) != "Bearer ") {
            return crow::response(401, "Unauthorized");
        }

        std::string adminToken = authHeader.substr(7);
        std::string adminUsername;
        try {
            auto decoded = jwt::decode(adminToken);
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{"supersecretkey"})
                .with_issuer("auth_server");
            verifier.verify(decoded);

            adminUsername = decoded.get_payload_claim("username").as_string();
            std::string role = decoded.get_payload_claim("role").as_string();
            if (role != "admin") {
                logger.log(Logger::Level::WARNING, "Token revocation attempt by non-admin user: " + adminUsername);
                return crow::response(403, "Only admin allowed");
            }
        } catch (...) {
            logger.log(Logger::Level::WARNING, "Token revocation failed: Invalid admin token.");
            return crow::response(401, "Invalid or expired admin token");
        }

        auto body = crow::json::load(req.body);
        if (!body || !body.has("token")) {
            logger.log(Logger::Level::WARNING, "Token revocation failed: Missing 'token' field in body.");
            return crow::response(400, "Missing 'token' field in body");
        }
        std::string targetToken = body["token"].s();

        if (userRepo.addExpiredToken(targetToken)) {
            logger.log(Logger::Level::INFO, "Admin '" + adminUsername + "' revoked a token.");
            return crow::response(200, "Target token revoked");
        } else {
            logger.log(Logger::Level::ERR, "Admin '" + adminUsername + "' failed to revoke a token.");
            return crow::response(500, "Failed to revoke token");
        }
    });

    CROW_ROUTE(app, "/api/auth/validate").methods(crow::HTTPMethod::Get)([&jwtUtils, &logger](const crow::request& req) {
        logger.log(Logger::Level::INFO, "GET /api/auth/validate from " + req.remote_ip_address);
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.substr(0, 7) != "Bearer ") {
            return crow::response(401, "Invalid or missing token");
        }
        std::string token = authHeader.substr(7);
        std::string username;

        if (jwtUtils.verifyToken(token, username)) {
            logger.log(Logger::Level::INFO, "Token validated successfully for user: " + username);
            crow::json::wvalue res;
            res["valid"] = true;
            return crow::response(200, res);
        } else {
            logger.log(Logger::Level::INFO, "Token validation failed.");
            return crow::response(401, "Token is invalid or expired");
        }
    });
}
