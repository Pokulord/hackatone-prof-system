#include "crow_all.h"
#include "UseCases/UserService.h"
#include "InterfaceAdapters/PgUserRepository.h"
#include "loadenv.cpp"
#include <memory>
#include <iostream>
#include <optional>
#include <string>

// Проверка прав администратора
bool isAdmin(const User& user) {
    return user.getRole() == Role::ADMIN;
}

// Извлечение username из заголовка запроса для аутентификации (упрощенно)
std::optional<std::string> getUserId(const crow::request& req) {
    auto uid = req.get_header_value("X-User-Id");
    if (uid.empty()) return std::nullopt;
    return uid;
}


// Функция проверки, пуста ли таблица users
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
    setlocale(LC_ALL, "ru_RU.utf8");
    int n = envloader::load_file(".env");
    if (n < 0) {
        std::cerr << "Failed to open .env\n";
        return 1;
    }

    std::string dbname = std::getenv("DB_NAME");
    std::string dbuser = std::getenv("DB_USER");
    std::string dbpass = std::getenv("DB_PASSWORD");
    std::string dbhost = std::getenv("DB_HOST");
    std::string dbport = std::getenv("DB_PORT");
    if (dbname.empty() || dbuser.empty() || dbpass.empty() || dbhost.empty() || dbport.empty()) {
        std::cerr << "Database configuration missing in .env\n";
        return 1;
    }

    std::string connStr = "dbname=" + dbname + " user=" + dbuser + " password=" + dbpass +
                          " hostaddr=" + dbhost + " port=" + dbport;

    // std::string connStr = "dbname=hackathon user=pc_shop_admin password=123 hostaddr=127.0.0.1 port=5432";
    auto userRepo = std::make_shared<PgUserRepository>(connStr);
    UserService userService(userRepo);

    // Проверяем пустая ли таблица users и создаём admin если нужно
    if (isUsersTableEmpty(connStr)) {
        std::cout << "Users table is empty. Creating default admin user." << std::endl;
        bool created = userService.createUser("admin", "admin123", Role::ADMIN, true);
        if (!created) {
            std::cerr << "Failed to create admin user" << std::endl;
        }
    }

    crow::SimpleApp app;


    // register
    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::Post)([&userService](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("username") || !body.has("password"))
            return crow::response(400, "Invalid JSON or missing fields");

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        bool res = userService.createUser(username, password, Role::USER, false);
        return res ? crow::response(200, "Registered") : crow::response(409, "User exists");
    });

    // login
    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::Post)([&userService](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("username") || !body.has("password"))
            return crow::response(400, "Invalid JSON or missing fields");

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        auto userOpt = userService.authenticate(username, password);
        if (!userOpt)
            return crow::response(401, "Auth failed");

        crow::json::wvalue res;
        res["username"] = userOpt->getUsername();
        res["role"] = (userOpt->getRole() == Role::ADMIN) ? "admin" : "user";
        res["mustChangePassword"] = userOpt->getMustChangePassword();
        return crow::response(res);
    });

    // Endpoint: изменить пароль (реализован)
    CROW_ROUTE(app, "/change_password").methods(crow::HTTPMethod::Post)([&userService](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        std::string username = body["username"].s();
        std::string newPassword = body["newPassword"].s();
        bool res = userService.changePassword(username, newPassword);
        return res ? crow::response(200, "Password changed") : crow::response(404, "User not found");
    });

    // Endpoint: выход (logout) - заглушка
    CROW_ROUTE(app, "/logout").methods(crow::HTTPMethod::Post)([](const crow::request&){
        return crow::response(200, "Logged out");
    });

    // Endpoint: обновление токена (refresh) - заглушка
    CROW_ROUTE(app, "/refresh").methods(crow::HTTPMethod::Post)([](const crow::request&){
        return crow::response(200, "Token refreshed");
    });

    // Endpoint: проверка токена - заглушка
    CROW_ROUTE(app, "/validate").methods(crow::HTTPMethod::Get)([](const crow::request&){
        return crow::response(200, "Token valid");
    });

    // Endpoint: отзыв токена (revoke) - заглушка
    CROW_ROUTE(app, "/revoke").methods(crow::HTTPMethod::Post)([](const crow::request&){
        return crow::response(200, "Token revoked");
    });

    // Endpoint: получить информацию о пользователе по имени
    CROW_ROUTE(app, "/user/<string>")([&userService](const crow::request& req, std::string username){
        auto userIdOpt = getUserId(req);
        if (!userIdOpt) return crow::response(401, "Unauthorized");

        auto currentUserOpt = userService.userExists(userIdOpt.value()) ? std::optional<User>(userService.getUserByUsername(userIdOpt.value())) : std::nullopt;
        if (!currentUserOpt) return crow::response(401, "Unauthorized");
        // Можно смотреть свою информацию и администраторы
        if (*userIdOpt != username && !isAdmin(*currentUserOpt)) return crow::response(403, "Forbidden");

        auto userOpt = userService.userExists(username) ? std::optional<User>(userService.getUserByUsername(username)) : std::nullopt;
        if (!userOpt) return crow::response(404, "User not found");

        crow::json::wvalue res;
        res["username"] = username;
        res["role"] = (userOpt->getRole() == Role::ADMIN) ? "admin" : "user";
        res["mustChangePassword"] = userOpt->getMustChangePassword();
        return crow::response(res);
    });

    // Endpoint: получить список пользователей (только администратор)
    CROW_ROUTE(app, "/users").methods(crow::HTTPMethod::Get)([&userService](const crow::request& req) {
        auto userIdOpt = getUserId(req);
        if (!userIdOpt) return crow::response(401, "Unauthorized");

        auto currentUserOpt = userService.userExists(userIdOpt.value()) ? std::optional<User>(userService.getUserByUsername(userIdOpt.value())) : std::nullopt;
        if (!currentUserOpt || !isAdmin(*currentUserOpt)) return crow::response(403, "Forbidden");

        // В реальном приложении получаем из репозитория полный список
        // Здесь заглушка с примером
        crow::json::wvalue res;
        res["users"] = crow::json::wvalue::list({"user1", "admin", "user2"});
        return crow::response(res);
    });

    // Endpoint: удалить пользователя (себя или админу)
    CROW_ROUTE(app, "/user/<string>").methods(crow::HTTPMethod::Delete)([&userService](const crow::request& req, std::string username){
        auto userIdOpt = getUserId(req);
        if (!userIdOpt) return crow::response(401, "Unauthorized");

        auto currentUserOpt = userService.userExists(userIdOpt.value()) ? std::optional<User>(userService.getUserByUsername(userIdOpt.value())) : std::nullopt;
        if (!currentUserOpt) return crow::response(401, "Unauthorized");

        if (*userIdOpt != username && !isAdmin(*currentUserOpt)) return crow::response(403, "Forbidden");

        // Нет метода deleteUser, поэтому просто заглушка
        return crow::response(200, "User deleted (not actually, implement deleteUser in repo)");
    });

    // Endpoint: изменить роли пользователя - заглушка
    CROW_ROUTE(app, "/user/<string>/roles").methods(crow::HTTPMethod::Patch)([&userService](const crow::request& req, std::string username){
        auto userIdOpt = getUserId(req);
        if (!userIdOpt) return crow::response(401, "Unauthorized");

        auto currentUserOpt = userService.userExists(userIdOpt.value()) ? std::optional<User>(userService.getUserByUsername(userIdOpt.value())) : std::nullopt;
        if (!currentUserOpt || !isAdmin(*currentUserOpt)) return crow::response(403, "Forbidden");

        auto body = crow::json::load(req.body);
        if (!body || !body.has("role")) return crow::response(400, "Invalid JSON");

        // Здесь можно реализовать смену роли, но в User класса нет setter для роли
        return crow::response(200, "Role changed (stub)");
    });

    // Endpoint: health check
    CROW_ROUTE(app, "/health").methods(crow::HTTPMethod::Get)([](const crow::request&){
        return crow::response(200, "OK");
    });

    // Endpoint: версия API
    CROW_ROUTE(app, "/version").methods(crow::HTTPMethod::Get)([](const crow::request&){
        return crow::response(200, "API Version 1.0");
    });


    std::cout << "Server running on port 18080\n";
    app.port(18080).multithreaded().run();
}