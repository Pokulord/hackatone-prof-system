#include "User.hpp"
#include <stdexcept>

namespace domain::entities {

    User::User(int id, 
               std::string username, 
               std::string passwordHash, 
               UserRole role,
               std::chrono::system_clock::time_point createdAt,
               std::chrono::system_clock::time_point lastLogin,
               bool passwordChanged,
               int failedAttempts,
               std::chrono::system_clock::time_point lockedUntil)
        : id_(id),
          username_(std::move(username)),
          passwordHash_(std::move(passwordHash)),
          role_(role),
          createdAt_(createdAt),
          lastLogin_(lastLogin),
          passwordChanged_(passwordChanged),
          failedAttempts_(failedAttempts),
          lockedUntil_(lockedUntil) {
        if (username_.empty()) {
            throw std::invalid_argument("Username cannot be empty.");
        }
        if (passwordHash_.empty()) {
            throw std::invalid_argument("Password hash cannot be empty.");
        }
    }

    User User::createUser(std::string username, std::string passwordHash, UserRole role) {
        return User(0, // ID is typically set by the database
                    std::move(username), 
                    std::move(passwordHash), 
                    role,
                    std::chrono::system_clock::now(), // createdAt
                    {}, // lastLogin
                    false, // passwordChanged
                    0, // failedAttempts
                    {}); // lockedUntil
    }

    int User::getId() const { return id_; }
    const std::string& User::getUsername() const { return username_; }
    const std::string& User::getPasswordHash() const { return passwordHash_; }
    UserRole User::getRole() const { return role_; }
    const std::chrono::system_clock::time_point& User::getCreatedAt() const { return createdAt_; }
    const std::chrono::system_clock::time_point& User::getLastLogin() const { return lastLogin_; }
    bool User::isPasswordChanged() const { return passwordChanged_; }
    int User::getFailedAttempts() const { return failedAttempts_; }
    const std::chrono::system_clock::time_point& User::getLockedUntil() const { return lockedUntil_; }

    bool User::isLocked() const {
        return lockedUntil_ > std::chrono::system_clock::now();
    }

    bool User::requiresPasswordChange() const {
        // Example policy: password must be changed after creation
        return !passwordChanged_;
    }

    void User::recordFailedLoginAttempt(int maxAttempts, std::chrono::minutes lockoutDuration) {
        if (isLocked()) {
            return;
        }
        failedAttempts_++;
        if (failedAttempts_ >= maxAttempts) {
            lockedUntil_ = std::chrono::system_clock::now() + lockoutDuration;
        }
    }

    void User::resetFailedLoginAttempts() {
        failedAttempts_ = 0;
        lockedUntil_ = {}; // Reset lock time
    }

    void User::updateLastLogin() {
        lastLogin_ = std::chrono::system_clock::now();
    }

    void User::changePassword(const std::string& newPasswordHash) {
        if (newPasswordHash.empty()) {
            throw std::invalid_argument("New password hash cannot be empty.");
        }
        passwordHash_ = newPasswordHash;
        passwordChanged_ = true;
    }
    
    void User::lock() {
        // Manually lock account, e.g., for 24 hours
        lockedUntil_ = std::chrono::system_clock::now() + std::chrono::hours(24);
    }

}
