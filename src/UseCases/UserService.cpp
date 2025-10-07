#include "UserService.h"
#include "IUserRepository.h"
#include <mutex>

UserService::UserService(std::shared_ptr<IUserRepository> repo)
    : userRepository(std::move(repo)), maxFailedAttempts(5) {}

unsigned int UserService::getFailedAttempts(const std::string& username) {
    std::lock_guard<std::mutex> lock(failedAttemptsMutex);
    auto it = failedAttemptsMap.find(username);
    if (it == failedAttemptsMap.end()) return 0;
    return it->second;
}

void UserService::incrementFailedAttempts(const std::string& username) {
    std::lock_guard<std::mutex> lock(failedAttemptsMutex);
    failedAttemptsMap[username]++;
}

void UserService::resetFailedAttempts(const std::string& username) {
    std::lock_guard<std::mutex> lock(failedAttemptsMutex);
    failedAttemptsMap.erase(username);
}

std::optional<User> UserService::authenticate(const std::string& username, const std::string& password) {
    if (getFailedAttempts(username) >= maxFailedAttempts) {
        return std::nullopt;  // Заблокирован
    }
    auto userOpt = userRepository->getUserByUsername(username);
    if (!userOpt || !userOpt->verifyPassword(password)) {
        incrementFailedAttempts(username);
        return std::nullopt;
    }
    resetFailedAttempts(username);
    return userOpt;
}

std::optional<User> UserService::getUserByUsername(const std::string& username) {
    return userRepository->getUserByUsername(username);
}

bool UserService::createUser(const std::string& username, const std::string& password, Role role, bool mustChangePassword) {
    if (userRepository->getUserByUsername(username)) return false;
    std::string hashed = User::hashPassword(password);
    User user(username, hashed, role, mustChangePassword);
    return userRepository->saveUser(user);
}

bool UserService::changePassword(const std::string& username, const std::string& newPassword) {
    auto userOpt = userRepository->getUserByUsername(username);
    if (!userOpt) return false;
    userOpt->setPasswordHash(User::hashPassword(newPassword));
    userOpt->setMustChangePassword(false);
    return userRepository->updateUser(userOpt.value());
}

bool UserService::userExists(const std::string& username) {
    return userRepository->getUserByUsername(username).has_value();
}
