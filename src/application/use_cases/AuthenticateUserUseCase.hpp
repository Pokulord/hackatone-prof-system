#pragma once

#include "../ports/IAuthenticationService.hpp"
#include "../../domain/repositories/IUserRepository.hpp"
#include "../../domain/repositories/IAuditRepository.hpp"
#include "../../domain/services/IPasswordHasher.hpp"
#include "../../domain/services/ITokenGenerator.hpp"
#include "../../domain/interfaces/IAuthConfigProvider.hpp"
#include <memory>

namespace application::use_cases {
    class AuthenticateUserUseCase : public ports::IAuthenticationService {
    public:
        AuthenticateUserUseCase(
            std::shared_ptr<domain::repositories::IUserRepository> userRepository,
            std::shared_ptr<domain::repositories::IAuditRepository> auditRepository,
            std::shared_ptr<domain::services::IPasswordHasher> passwordHasher,
            std::shared_ptr<domain::services::ITokenGenerator> tokenGenerator,
            std::shared_ptr<domain::interfaces::IAuthConfigProvider> authConfig
        );

        ports::AuthenticationResult authenticate(const dto::AuthenticationRequest& request) override;

    private:
        std::shared_ptr<domain::repositories::IUserRepository> userRepository_;
        std::shared_ptr<domain::repositories::IAuditRepository> auditRepository_;
        std::shared_ptr<domain::services::IPasswordHasher> passwordHasher_;
        std::shared_ptr<domain::services::ITokenGenerator> tokenGenerator_;
        std::shared_ptr<domain::interfaces::IAuthConfigProvider> authConfig_;
    };
}
