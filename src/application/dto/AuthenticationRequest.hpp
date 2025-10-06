#pragma once

#include <string>

namespace application::dto {
    struct AuthenticationRequest {
        std::string username;
        std::string password;
    };
}
