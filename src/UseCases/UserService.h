#pragma once
#include "../Entities/User.h"
#include <memory>
#include <string>
#include <optional>
#include <unordered_map>
#include <mutex>

class IUserRepository;

class UserService {
private:
    std::shared_ptr<IUserRepository> userRepository;
    unsigned int maxFailedAttempts;

    std::unordered_map<std::string, unsigned int> failedAttemptsMap;
    std::mutex failedAttemptsMutex;

    unsigned int getFailedAttempts(const std::string& username);
    void incrementFailedAttempts(const std::string& username);
    void resetFailedAttempts(const std::string& username);

public:
    explicit UserService(std::shared_ptr<IUserRepository> repo);

    bool createUser(const std::string& username, const std::string& password, Role role, bool mustChangePassword);
    std::optional<User> authenticate(const std::string& username, const std::string& password);
    bool changePassword(const std::string& username, const std::string& newPassword);                                      
    bool userExists(const std::string& username);
    std::optional<User> getUserByUsername(const std::string& username);
    std::vector<User> getAllUsers();
    bool updateUser(const std::string& username, const std::optional<std::string>& password, const std::optional<Role>& role, const std::optional<bool>& mustChangePassword);
    bool deleteUser(const std::string& username);
};
