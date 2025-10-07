#include "PgUserRepository.h"
#include "Entities/User.h"
#include <pqxx/pqxx>
#include <iostream>

PgUserRepository::PgUserRepository(const std::string& connStr) : connectionString(connStr) {
    // Конструктор без подготовки запросов, подготовка будет перед выполнением
}

std::optional<User> PgUserRepository::getUserByUsername(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        pqxx::connection c(connectionString);
        pqxx::work txn(c);

        // Подготавливаем запрос перед использованием
        c.prepare("get_user", "SELECT username, password_hash, role, must_change_password FROM users WHERE username = $1");

        pqxx::result r = txn.exec_prepared("get_user", username);
        if (r.empty()) return std::nullopt;

        auto row = r[0];
        User user(
            row["username"].c_str(),
            row["password_hash"].c_str(),
            static_cast<Role>(row["role"].as<int>()),
            row["must_change_password"].as<bool>()
        );
        return user;
    } catch (const std::exception& e) {
        std::cerr << "DB error: " << e.what() << "\n";
        return std::nullopt;
    }
}

bool PgUserRepository::saveUser(const User& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        pqxx::connection c(connectionString);
        pqxx::work txn(c);

        c.prepare("insert_user", "INSERT INTO users (username, password_hash, role, must_change_password) VALUES ($1, $2, $3, $4)");

        txn.exec_prepared("insert_user",
                          user.getUsername(),
                          user.getPasswordHash(),
                          static_cast<int>(user.getRole()),
                          user.getMustChangePassword());

        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "DB error: " << e.what() << "\n";
        return false;
    }
}

bool PgUserRepository::updateUser(const User& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        pqxx::connection c(connectionString);
        pqxx::work txn(c);

        c.prepare("update_user", "UPDATE users SET password_hash = $1, role = $2, must_change_password = $3 WHERE username = $4");

        txn.exec_prepared("update_user",
                          user.getPasswordHash(),
                          static_cast<int>(user.getRole()),
                          user.getMustChangePassword(),
                          user.getUsername());

        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "DB error: " << e.what() << "\n";
        return false;
    }
}
