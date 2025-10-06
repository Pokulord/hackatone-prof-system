#include "PlaintextPasswordHasher.hpp"

namespace infrastructure::adapters {

    std::string PlaintextPasswordHasher::hashPassword(const std::string& password) {
        // WARNING: Insecure. For development only.
        return password;
    }

    bool PlaintextPasswordHasher::verifyPassword(const std::string& password, const std::string& hash) {
        // WARNING: Insecure. For development only.
        return password == hash;
    }

}
