#include "crow_all.h"
#include "UseCases/UserService.h"
#include "InterfaceAdapters/PgUserRepository.h"
#include <memory>
#include <iostream>
#include <optional>
#include <string>
#include "jwt_utils.h"
#include "RefreshTokenService.h"
#include <fstream>
#include <sys/stat.h>

// Проверка прав администратора
bool isAdmin(const User& user) {
    return user.getRole() == Role::ADMIN;
}

bool saveTokenToFile(const std::string& token, const std::string& filepath) {
    std::ofstream ofs(filepath, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open token file for writing: " << filepath << std::endl;
        return false;
    }
    ofs << token;
    ofs.close();
    if (chmod(filepath.c_str(), S_IRUSR | S_IWUSR) != 0) {
        std::cerr << "Failed to set file permissions on token file" << std::endl;
        return false;
    }
    return true;
}

std::optional<std::string> getUserId(const crow::request& req) {
    auto uid = req.get_header_value("X-User-Id");
    if (uid.empty()) return std::nullopt;
    return uid;
}

bool isUsersTableEmpty(const std::string& connStr) {
    try {
        pqxx::connection c(connStr);
        pqxx::work txn(c);
        pqxx::result r = txn.exec("SELECT COUNT(*) FROM users");
        int count = r[0][0].as<int>();
        return count == 0;
    } catch (const std::exception& e) {
        std::cerr << "DB error during user count check: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    JwtUtils jwtUtils("supersecretkey");
    std::string connStr = "dbname=hackathon user=pc_shop_admin password=123 hostaddr=127.0.0.1 port=5432";
    auto userRepo = std::make_shared<PgUserRepository>(connStr);
    UserService userService(userRepo);
    RefreshTokenService refreshService(*userRepo, jwtUtils);

    if (isUsersTableEmpty(connStr)) {
        std::cout << "Users table is empty. Creating default admin user." << std::endl;
        bool created = userService.createUser("admin", "admin123", Role::ADMIN, true);
        if (!created) {
            std::cerr << "Failed to create admin user" << std::endl;
        }
    }

    crow::SimpleApp app;

    // Лямбда для проверки и обновления токена
    auto checkAndUpdateToken = [&jwtUtils, &userRepo](const crow::request& req) -> std::optional<std::string> {
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.substr(0, 7) != "Bearer ")
            return std::nullopt;

        std::string token = authHeader.substr(7);
        std::string username;

        if (!jwtUtils.verifyToken(token, username)) {
            if (userRepo->isTokenRevoked(token) || jwtUtils.isTokenExpired(token)) {
                userRepo->addExpiredToken(token);
                return std::nullopt;
            }
            return std::nullopt;
        }

        if (userRepo->isTokenRevoked(token)) {
            return std::nullopt;
        }
        return username;
    };

    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::POST)([&userService](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("username") || !body.has("password"))
            return crow::response(400, "Invalid JSON or missing fields");

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        bool res = userService.createUser(username, password, Role::USER, false);
        return res ? crow::response(200, "Registered") : crow::response(409, "User exists");
    });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)([&userService, &jwtUtils](const crow::request& req) {
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

    CROW_ROUTE(app, "/refresh").methods(crow::HTTPMethod::POST)([&refreshService](const crow::request& req) {
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

    std::cout << "Server running on port 18080\n";
    app.port(18080).multithreaded().run();
}
