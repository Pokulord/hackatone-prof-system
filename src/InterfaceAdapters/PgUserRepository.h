#pragma once
#include "IUserRepository.h"
#include <pqxx/pqxx>
#include <optional>
#include <mutex>

class PgUserRepository : public IUserRepository {
private:
    std::string connectionString;
    std::mutex mutex_;

public:
    explicit PgUserRepository(const std::string& connStr);
    std::optional<User> getUserByUsername(const std::string& username) override;
    bool saveUser(const User& user) override;
    bool updateUser(const User& user) override;
};
