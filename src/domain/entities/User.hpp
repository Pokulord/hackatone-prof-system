#pragma once

#include <string>
#include <chrono>
#include "../../domain/entities/UserRole.hpp"

namespace domain::entities {
    class User {
    public:
        User(int id, 
             std::string username, 
             std::string passwordHash, 
             UserRole role,
             std::chrono::system_clock::time_point createdAt,
             std::chrono::system_clock::time_point lastLogin,
             bool passwordChanged,
             int failedAttempts,
             std::chrono::system_clock::time_point lockedUntil);
        
        // Factory method for creating a new user
        static User createUser(std::string username, std::string passwordHash, UserRole role);

        // Getters
        int getId() const;
        const std::string& getUsername() const;
        const std::string& getPasswordHash() const;
        UserRole getRole() const;
        const std::chrono::system_clock::time_point& getCreatedAt() const;
        const std::chrono::system_clock::time_point& getLastLogin() const;
        bool isPasswordChanged() const;
        int getFailedAttempts() const;
        const std::chrono::system_clock::time_point& getLockedUntil() const;
        
        // Business methods
        bool isLocked() const;
        bool requiresPasswordChange() const;
        void recordFailedLoginAttempt(int maxAttempts, std::chrono::minutes lockoutDuration);
        void resetFailedLoginAttempts();
        void updateLastLogin();
        void changePassword(const std::string& newPasswordHash);
        void lock();
        
    private:
        int id_;
        std::string username_;
        std::string passwordHash_;
        UserRole role_;
        std::chrono::system_clock::time_point createdAt_;
        std::chrono::system_clock::time_point lastLogin_;
        bool passwordChanged_;
        int failedAttempts_;
        std::chrono::system_clock::time_point lockedUntil_;
    };
}