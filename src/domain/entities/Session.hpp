#pragma once

#include <string>
#include <chrono>
#include "UserRole.hpp"

namespace domain::entities {
    class Session {
    public:
        Session(std::string token, 
                std::string username, 
                UserRole role, 
                std::chrono::system_clock::time_point createdAt,
                std::chrono::system_clock::time_point expiresAt);

        static Session create(std::string token, std::string username, UserRole role, std::chrono::minutes validityPeriod);

        const std::string& getToken() const;
        const std::string& getUsername() const;
        UserRole getUserRole() const;
        const std::chrono::system_clock::time_point& getCreatedAt() const;
        const std::chrono::system_clock::time_point& getExpiresAt() const;
        
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
