#pragma once

#include <string>

namespace domain::entities {
    enum class UserRole {
        ADMIN,
        ENGINEER,
        GUEST
    };
    
    inline std::string userRoleToString(UserRole role) {
        switch (role) {
            case UserRole::ADMIN: return "admin";
            case UserRole::ENGINEER: return "engineer";
            case UserRole::GUEST: return "guest";
            default: return "unknown";
        }
    }
    
    inline UserRole stringToUserRole(const std::string& str) {
        if (str == "admin") return UserRole::ADMIN;
        if (str == "engineer") return UserRole::ENGINEER;
        if (str == "guest") return UserRole::GUEST;
        // Default to a restricted role
        return UserRole::GUEST;
    }
}