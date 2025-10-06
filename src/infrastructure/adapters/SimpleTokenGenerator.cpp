#include "SimpleTokenGenerator.hpp"
#include "../../domain/entities/UserRole.hpp"
#include <sstream>
#include <vector>

namespace infrastructure::adapters {

    std::string SimpleTokenGenerator::generateToken(const domain::entities::User& user) {
        // WARNING: Insecure. For development only.
        // Format: "token_for_USERNAME_with_role_ROLENAME"
        return "token_for_" + user.getUsername() + "_with_role_" + domain::entities::userRoleToString(user.getRole());
    }

    bool SimpleTokenGenerator::validateToken(const std::string& token) {
        // WARNING: Insecure. For development only.
        return token.rfind("token_for_", 0) == 0;
    }

    std::optional<domain::services::DecodedToken> SimpleTokenGenerator::decodeToken(const std::string& token) {
        // WARNING: Insecure. For development only.
        if (!validateToken(token)) {
            return std::nullopt;
        }

        std::string temp = token;
        // Remove "token_for_"
        temp.erase(0, 10); 
        
        size_t rolePos = temp.find("_with_role_");
        if (rolePos == std::string::npos) {
            return std::nullopt;
        }

        std::string username = temp.substr(0, rolePos);
        std::string roleStr = temp.substr(rolePos + 11);

        return domain::services::DecodedToken{
            username,
            domain::entities::stringToUserRole(roleStr)
        };
    }

}
