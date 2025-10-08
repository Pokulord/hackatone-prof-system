#include "Application.h"
#include "loadenv.h"
#include "Endpoints/endpoints.h"
#include "migrations/MigrationService.h"
#include <iostream>
#include <stdexcept>
#include <pqxx/pqxx>

// Checks if the users table is empty in the database.
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

Application::Application() {
    setlocale(LC_ALL, "ru_RU.utf8");
    if (EnvLoader::load_file(".env") < 0) {
        throw std::runtime_error("Failed to open .env");
    }

    const char* dbname_env = std::getenv("DB_NAME");
    const char* dbuser_env = std::getenv("DB_USER");
    const char* dbpass_env = std::getenv("DB_PASSWORD");
    const char* dbhost_env = std::getenv("DB_HOST");
    const char* dbport_env = std::getenv("DB_PORT");

    if (!dbname_env || !dbuser_env || !dbpass_env || !dbhost_env || !dbport_env) {
        throw std::runtime_error("Database configuration missing in .env");
    }

    connStr = "dbname=" + std::string(dbname_env) +
              " user=" + std::string(dbuser_env) +
              " password=" + std::string(dbpass_env) +
              " hostaddr=" + std::string(dbhost_env) +
              " port=" + std::string(dbport_env);

    try {
        MigrationService migrationService(connStr, "migrations");
        migrationService.runMigrations();
    } catch (const std::exception& e) {
        std::cerr << "Failed to run migrations: " << e.what() << std::endl;
        throw;
    }

    logger = std::make_unique<Logger>("app.log");

    jwtUtils = std::make_unique<JwtUtils>("supersecretkey");
    
    userRepo = std::make_shared<PgUserRepository>(connStr);
    roleRepo = std::make_shared<PgRoleRepository>(connStr);
    userService = std::make_unique<UserService>(userRepo);
    roleService = std::make_unique<RoleService>(roleRepo);
    refreshService = std::make_unique<RefreshTokenService>(*userRepo, *jwtUtils);

    if (isUsersTableEmpty(connStr)) {
        std::cout << "Users table is empty. Creating default admin user." << std::endl;
        if (!userService->createUser("admin", "admin123", "admin", true, true)) {
            std::cerr << "Failed to create admin user" << std::endl;
        }
    }
}

void Application::run() {
    registerEndpoints(app, *userService, *jwtUtils, *refreshService, *userRepo, *roleService, *logger);

    std::cout << "Server running on port 18080" << std::endl;
    app.port(18080).multithreaded().run();
}
