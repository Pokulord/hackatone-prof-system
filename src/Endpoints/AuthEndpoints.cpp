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
    JwtUtils& jwtUtils, RefreshTokenService& refreshService, PgUserRepository& userRepo) {
    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::Post)([&userService](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("username") || !body.has("password"))
            return crow::response(400, "Invalid JSON or missing fields");

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        bool res = userService.createUser(username, password, Role::ENGINEER, false);
        return res ? crow::response(200, "Registered") : crow::response(409, "User exists");
    });

    CROW_ROUTE(app, "/api/auth/login").methods(crow::HTTPMethod::Post)([&userService, &jwtUtils](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("username") || !body.has("password"))
            return crow::response(400, "Invalid JSON or missing fields");

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        auto userOpt = userService.authenticate(username, password);
        if (!userOpt) return crow::response(401, "Auth failed");

        std::string role = (userOpt->getRole() == Role::ADMIN) ? "admin" : "user";
        std::string token = jwtUtils.generateToken(username, role);

        const char* userEnv = getenv("USER");
        std::string usrstr = userEnv ? userEnv : "default";
        std::string tokenFilePath = "/home/" + usrstr + "/.myapp_token";
        if (!saveTokenToFile(token, tokenFilePath)) {
            std::cerr << "Warning: could not save JWT token to file" << std::endl;
        }

        crow::json::wvalue res;
        res["access_token"] = token;
        res["username"] = username;
        res["role"] = role;
        res["mustChangePassword"] = userOpt->getMustChangePassword();
        return crow::response(res);
    });

    CROW_ROUTE(app, "/api/auth/refresh").methods(crow::HTTPMethod::Post)([&refreshService](const crow::request& req) {
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.substr(0, 7) != "Bearer ")
            return crow::response(401, "Invalid token");
        std::string oldToken = authHeader.substr(7);

        try {
            std::string newToken = refreshService.refreshToken(oldToken);
            crow::json::wvalue res;
            res["access_token"] = newToken;
            return crow::response(res);
        } catch (const std::exception& e) {
            return crow::response(401, e.what());
        }
    });

    CROW_ROUTE(app, "/api/auth/logout").methods(crow::HTTPMethod::Post)([&jwtUtils, &userRepo](const crow::request& req) {
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.substr(0,7) != "Bearer ")
            return crow::response(401, "Unauthorized");

        std::string token = authHeader.substr(7);
        std::string username;
        if (!jwtUtils.verifyToken(token, username)) {
            return crow::response(401, "Unauthorized");
        }

        bool revoked = userRepo.addExpiredToken(token);
        if (revoked) {
            return crow::response(200, "Logged out. Token revoked");
        } else {
            return crow::response(500, "Failed to revoke token");
        }
    });

    CROW_ROUTE(app, "/api/auth/revoke").methods(crow::HTTPMethod::Post)([&jwtUtils, &userRepo](const crow::request& req) {
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.substr(0,7) != "Bearer ")
            return crow::response(401, "Unauthorized");

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
                return crow::response(403, "Only admin allowed");
            }
        } catch (...) {
            return crow::response(401, "Invalid or expired admin token");
        }

        auto body = crow::json::load(req.body);
        if (!body || !body.has("token"))
            return crow::response(400, "Missing 'token' field in body");
        std::string targetToken = body["token"].s();

        try {
            jwt::decode(targetToken);
        } catch (...) {
            return crow::response(400, "Invalid target token format");
        }

        if (userRepo.addExpiredToken(targetToken)) {
            return crow::response(200, "Target token revoked");
        } else {
            return crow::response(500, "Failed to revoke token");
        }
    });
}
