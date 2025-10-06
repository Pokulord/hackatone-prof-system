#include "Session.hpp"
#include <stdexcept>

namespace domain::entities {

    Session::Session(std::string token, 
                     std::string username, 
                     UserRole role, 
                     std::chrono::system_clock::time_point createdAt,
                     std::chrono::system_clock::time_point expiresAt)
        : token_(std::move(token)),
          username_(std::move(username)),
          userRole_(role),
          createdAt_(createdAt),
          expiresAt_(expiresAt) {
        if (token_.empty()) {
            throw std::invalid_argument("Token cannot be empty.");
        }
        if (username_.empty()) {
            throw std::invalid_argument("Username cannot be empty.");
        }
    }

    Session Session::create(std::string token, std::string username, UserRole role, std::chrono::minutes validityPeriod) {
        const auto now = std::chrono::system_clock::now();
        return Session(std::move(token), 
                       std::move(username), 
                       role, 
                       now, 
                       now + validityPeriod);
    }

    const std::string& Session::getToken() const { return token_; }
    const std::string& Session::getUsername() const { return username_; }
    UserRole Session::getUserRole() const { return userRole_; }
    const std::chrono::system_clock::time_point& Session::getCreatedAt() const { return createdAt_; }
    const std::chrono::system_clock::time_point& Session::getExpiresAt() const { return expiresAt_; }

    bool Session::isExpired() const {
        return expiresAt_ <= std::chrono::system_clock::now();
    }

    void Session::extend(std::chrono::minutes additionalTime) {
        if (isExpired()) {
            // Or throw an exception, depending on business rules
            return; 
        }
        expiresAt_ += additionalTime;
    }

}
