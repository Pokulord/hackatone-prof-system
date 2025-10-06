// ===============================
// DOMAIN LAYER - ENTITIES  
// ===============================

// domain/entities/User.hpp
#pragma once
#include <string>
#include <chrono>
#include "UserRole.hpp"

namespace domain::entities {
    class User {
    public:
        User(int id, std::string username, std::string passwordHash, UserRole role);
        
        // Getters
        int getId() const { return id_; }
        const std::string& getUsername() const { return username_; }
        const std::string& getPasswordHash() const { return passwordHash_; }
        UserRole getRole() const { return role_; }
        const std::chrono::system_clock::time_point& getCreatedAt() const { return createdAt_; }
        const std::chrono::system_clock::time_point& getLastLogin() const { return lastLogin_; }
        bool isPasswordChanged() const { return passwordChanged_; }
        int getFailedAttempts() const { return failedAttempts_; }
        const std::chrono::system_clock::time_point& getLockedUntil() const { return lockedUntil_; }
        
        // Business methods
        bool isLocked() const;
        bool requiresPasswordChange() const;
        void recordFailedLogin();
        void resetFailedAttempts();
        void updateLastLogin();
        void changePassword(const std::string& newPasswordHash);
        void forcePasswordChange();
        
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

// domain/entities/UserRole.hpp
#pragma once

namespace domain::entities {
    enum class UserRole {
        ADMIN,
        USER
    };
    
    std::string userRoleToString(UserRole role);
    UserRole stringToUserRole(const std::string& str);
}

// domain/entities/Session.hpp  
#pragma once
#include <string>
#include <chrono>
#include "UserRole.hpp"

namespace domain::entities {
    class Session {
    public:
        Session(std::string token, std::string username, UserRole role, 
               std::chrono::minutes validityPeriod = std::chrono::minutes(30));
        
        const std::string& getToken() const { return token_; }
        const std::string& getUsername() const { return username_; }
        UserRole getUserRole() const { return userRole_; }
        bool isExpired() const;
        void extend(std::chrono::minutes additionalTime);
        
    private:
        std::string token_;
        std::string username_;
        UserRole userRole_;
        std::chrono::system_clock::time_point createdAt_;
        std::chrono::system_clock::time_point expiresAt_;
    };
}

// ===============================
// DOMAIN LAYER - REPOSITORY INTERFACES
// ===============================

// domain/repositories/IUserRepository.hpp
#pragma once
#include "domain/entities/User.hpp"
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
        virtual bool save(const entities::User& user) = 0;
        virtual bool deleteByUsername(const std::string& username) = 0;
        virtual bool exists(const std::string& username) = 0;
        virtual bool updateFailedAttempts(const std::string& username, int attempts) = 0;
        virtual bool updateLastLogin(const std::string& username) = 0;
        virtual int count() = 0;
    };
}

// domain/repositories/IAuditRepository.hpp
#pragma once
#include "domain/entities/AuditLog.hpp"
#include <vector>
#include <string>
#include <chrono>

namespace domain::repositories {
    class IAuditRepository {
    public:
        virtual ~IAuditRepository() = default;
        
        virtual bool logEvent(const std::string& username, const std::string& action, 
                            bool success, const std::string& details = "") = 0;
        virtual std::vector<entities::AuditLog> getRecentLogs(int limit = 100) = 0;
        virtual std::vector<entities::AuditLog> getLogsByUsername(const std::string& username) = 0;
        virtual std::vector<entities::AuditLog> getLogsByDateRange(
            const std::chrono::system_clock::time_point& start,
            const std::chrono::system_clock::time_point& end) = 0;
        virtual bool cleanupOldLogs(int daysToKeep = 30) = 0;
    };
}

// ===============================
// DOMAIN LAYER - SERVICE INTERFACES
// ===============================

// domain/services/IPasswordHasher.hpp
#pragma once
#include <string>

namespace domain::services {
    class IPasswordHasher {
    public:
        virtual ~IPasswordHasher() = default;
        
        virtual std::string hashPassword(const std::string& password, int cost = 12) = 0;
        virtual bool verifyPassword(const std::string& password, const std::string& hash) = 0;
        virtual bool isValidPassword(const std::string& password) = 0;
        virtual int getRecommendedCost() = 0;
    };
}

// domain/services/ITokenGenerator.hpp
#pragma once
#include <string>
#include "domain/entities/User.hpp"

namespace domain::services {
    class ITokenGenerator {
    public:
        virtual ~ITokenGenerator() = default;
        
        virtual std::string generateSessionToken(const entities::User& user) = 0;
        virtual bool validateToken(const std::string& token) = 0;
        virtual std::string extractUsername(const std::string& token) = 0;
    };
}