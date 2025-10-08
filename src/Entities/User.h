#pragma once
#include <string>

class User {
private:
    std::string username;
    std::string passwordHash;
    std::string role;
    bool mustChangePassword;

public:
    User(const std::string& username, const std::string& passwordHash, const std::string& role, bool mustChangePassword);

    const std::string& getUsername() const;
    const std::string& getPasswordHash() const;
    const std::string& getRole() const;
    bool getMustChangePassword() const;

    void setPasswordHash(const std::string& newPasswordHash);
    void setMustChangePassword(bool value);
    void setPassword(const std::string& newPassword);
    void setRole(const std::string& newRole);

    bool verifyPassword(const std::string& password) const;
    static std::string hashPassword(const std::string& password);
};
