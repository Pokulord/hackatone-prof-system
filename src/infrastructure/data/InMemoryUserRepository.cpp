#include "InMemoryUserRepository.hpp"

#include "../adapters/PlaintextPasswordHasher.hpp" // For default user

namespace infrastructure::data {

InMemoryUserRepository::InMemoryUserRepository() {
    // Add a default admin user for testing, as per requirements
    adapters::PlaintextPasswordHasher hasher;
    auto adminUser = domain::entities::User::createUser(
        "admin",
        hasher.hashPassword("admin123"),
        domain::entities::UserRole::ADMIN
    );
    save(adminUser);
}

std::optional<domain::entities::User> InMemoryUserRepository::findByUsername(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = users_.find(username);
    if (it != users_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<domain::entities::User> InMemoryUserRepository::findById(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& pair : users_) {
        if (pair.second.getId() == id) {
            return pair.second;
        }
    }
    return std::nullopt;
}

std::vector<domain::entities::User> InMemoryUserRepository::findAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::entities::User> allUsers;
    for (const auto& pair : users_) {
        allUsers.push_back(pair.second);
    }
    return allUsers;
}

void InMemoryUserRepository::save(const domain::entities::User& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (user.getId() == 0) { // New user
        domain::entities::User newUser(
            nextId_++,
            user.getUsername(),
            user.getPasswordHash(),
            user.getRole(),
            user.getCreatedAt(),
            user.getLastLogin(),
            user.isPasswordChanged(),
            user.getFailedAttempts(),
            user.getLockedUntil()
        );
        // Use insert or emplace to avoid default constructor requirement
        users_.insert({newUser.getUsername(), newUser});
    } else { // Existing user
        // Use insert_or_assign instead of operator[]
        users_.insert_or_assign(user.getUsername(), user);
    }
}

void InMemoryUserRepository::remove(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    users_.erase(username);
}

bool InMemoryUserRepository::exists(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    return users_.count(username) > 0;
}

}