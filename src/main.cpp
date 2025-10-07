#include "crow_all.h"
#include "UseCases/UserService.h"
#include "InterfaceAdapters/PgUserRepository.h"
#include <memory>
#include <iostream>

int main() {
    auto userRepo = std::make_shared<PgUserRepository>("dbname=hackathon user=pc_shop_admin password=123 hostaddr=127.0.0.1 port=5432");
    UserService userService(userRepo);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::POST)([&userService](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        std::string username = body["username"].s();
        std::string password = body["password"].s();
        bool res = userService.createUser(username, password, Role::USER, true);
        return res ? crow::response(200, "Registered") : crow::response(409, "User exists");
    });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)([&userService](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        std::string username = body["username"].s();
        std::string password = body["password"].s();
        auto userOpt = userService.authenticate(username, password);
        if (!userOpt) return crow::response(401, "Auth failed");
        crow::json::wvalue res;
        res["username"] = userOpt->getUsername();
        res["role"] = (userOpt->getRole() == Role::ADMIN) ? "admin" : "user";
        res["mustChangePassword"] = userOpt->getMustChangePassword();
        return crow::response(res);
    });

    CROW_ROUTE(app, "/change_password").methods(crow::HTTPMethod::POST)([&userService](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        std::string username = body["username"].s();
        std::string newPassword = body["newPassword"].s();
        bool res = userService.changePassword(username, newPassword);
        return res ? crow::response(200, "Password changed") : crow::response(404, "User not found");
    });

    std::cout << "Server running on port 18080\n";
    app.port(18080).multithreaded().run();
}

