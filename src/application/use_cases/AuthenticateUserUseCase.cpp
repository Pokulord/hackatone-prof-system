#include "AuthenticateUserUseCase.hpp"
#include "../../domain/entities/AuditLog.hpp"
#include <stdexcept>

namespace application::use_cases {

    AuthenticateUserUseCase::AuthenticateUserUseCase(
        std::shared_ptr<domain::repositories::IUserRepository> userRepository,
        std::shared_ptr<domain::repositories::IAuditRepository> auditRepository,
        std::shared_ptr<domain::services::IPasswordHasher> passwordHasher,
        std::shared_ptr<domain::services::ITokenGenerator> tokenGenerator,
        std::shared_ptr<domain::interfaces::IAuthConfigProvider> authConfig)
        : userRepository_(std::move(userRepository)),
          auditRepository_(std::move(auditRepository)),
          passwordHasher_(std::move(passwordHasher)),
          tokenGenerator_(std::move(tokenGenerator)),
          authConfig_(std::move(authConfig)) {
              if (!userRepository_ || !auditRepository_ || !passwordHasher_ || !tokenGenerator_) {
                  throw std::invalid_argument("All dependencies must be provided.");
              }
          }

    ports::AuthenticationResult AuthenticateUserUseCase::authenticate(const dto::AuthenticationRequest& request) {
        auto userOpt = userRepository_->findByUsername(request.username);

        if (!userOpt) {
            auto log = domain::entities::AuditLog::create(request.username, "LOGIN_ATTEMPT", false, "User not found");
            auditRepository_->log(log);
            return {false, "", "Invalid username or password."};
        }

        auto& user = *userOpt;

        if (user.isLocked()) {
            auto log = domain::entities::AuditLog::create(request.username, "LOGIN_ATTEMPT", false, "Account is locked");
            auditRepository_->log(log);
            return {false, "", "Account is locked. Please try again later."};
        }

        if (!passwordHasher_->verifyPassword(request.password, user.getPasswordHash())) {
                        
            user.recordFailedLoginAttempt(authConfig_->getMaxAttempts(), authConfig_->getLockoutDuration());
            userRepository_->save(user);
            
            auto log = domain::entities::AuditLog::create(request.username, "LOGIN_ATTEMPT", false, "Invalid password");
            auditRepository_->log(log);
            return {false, "", "Invalid username or password."};
        }

        user.resetFailedLoginAttempts();
        user.updateLastLogin();
        userRepository_->save(user);

        auto token = tokenGenerator_->generateToken(user);
        
        auto log = domain::entities::AuditLog::create(request.username, "LOGIN_SUCCESS", true);
        auditRepository_->log(log);

        return {true, token, "Authentication successful."};
    }

}
