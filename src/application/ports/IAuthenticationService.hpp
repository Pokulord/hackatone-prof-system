#pragma once

#include "../dto/AuthenticationRequest.hpp"
#include <string>
#include <stdexcept>

namespace application::ports {

    // Result of the authentication attempt
    struct AuthenticationResult {
        bool success;
        std::string token;
        std::string message;
    };

    class AuthenticationException : public std::runtime_error {
    public:
        explicit AuthenticationException(const std::string& message) 
            : std::runtime_error(message) {}
    };

    class IAuthenticationService {
    public:
        virtual ~IAuthenticationService() = default;
        
        virtual AuthenticationResult authenticate(const dto::AuthenticationRequest& request) = 0;
    };
}
