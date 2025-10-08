#pragma once
#include <string>

enum class Role { ADMIN, ENGINEER, GUEST };

class User {
private:
    std::string username;
    std::string passwordHash;
    Role role;
    bool mustChangePassword;

public:
    User(const std::string& username, const std::string& passwordHash, Role role, bool mustChangePassword);

    const std::string& getUsername() const;
    const std::string& getPasswordHash() const;
    Role getRole() const;
    bool getMustChangePassword() const;

    void setPasswordHash(const std::string& newPasswordHash);
    void setMustChangePassword(bool value);

    bool verifyPassword(const std::string& password) const;
    static std::string hashPassword(const std::string& password);
};
