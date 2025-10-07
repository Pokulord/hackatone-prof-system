#pragma once
#include "../Entities/User.h"
#include <optional>

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual std::optional<User> getUserByUsername(const std::string& username) = 0;
    virtual bool saveUser(const User& user) = 0;
    virtual bool updateUser(const User& user) = 0;
};
