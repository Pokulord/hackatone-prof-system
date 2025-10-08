#pragma once
#include "../Entities/User.h"
#include <optional>
#include <vector>
#include <string>
class IUserRepository {
public:
    virtual std::optional<User> getUserByUsername(const std::string& username) = 0;
    virtual bool saveUser(const User& user) = 0;
    virtual bool updateUser(const User& user) = 0;
    virtual bool deleteUser(const std::string& username) = 0;
    virtual std::vector<User> getAllUsers() = 0;
    virtual bool addExpiredToken(const std::string& token) = 0;
    virtual bool isTokenRevoked(const std::string& token) = 0;
    virtual ~IUserRepository() = default;
};
