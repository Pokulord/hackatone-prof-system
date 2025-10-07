#include "User.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

User::User(const std::string& username, const std::string& passwordHash, Role role, bool mustChangePassword)
    : username(username), passwordHash(passwordHash), role(role), mustChangePassword(mustChangePassword) {}

const std::string& User::getUsername() const { return username; }
const std::string& User::getPasswordHash() const { return passwordHash; }
Role User::getRole() const { return role; }
bool User::getMustChangePassword() const { return mustChangePassword; }
void User::setPasswordHash(const std::string& newPasswordHash) { passwordHash = newPasswordHash; }
void User::setMustChangePassword(bool value) { mustChangePassword = value; }

bool User::verifyPassword(const std::string& password) const {
    return hashPassword(password) == passwordHash;
}

std::string User::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}
