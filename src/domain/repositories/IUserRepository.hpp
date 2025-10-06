#pragma once

#include "../entities/User.hpp"
#include <optional>
#include <vector>
#include <string>

namespace domain::repositories {
    class IUserRepository {
    public:
        virtual ~IUserRepository() = default;
        
        virtual std::optional<entities::User> findByUsername(const std::string& username) = 0;
        virtual std::optional<entities::User> findById(int id) = 0;
        virtual std::vector<entities::User> findAll() = 0;
        virtual void save(const entities::User& user) = 0;
        virtual void remove(const std::string& username) = 0;
        virtual bool exists(const std::string& username) = 0;
    };
}