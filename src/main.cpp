#include <iostream>
#include <memory>



#include "application/use_cases/AuthenticateUserUseCase.hpp"
#include "infrastructure/data/InMemoryUserRepository.hpp"
#include "infrastructure/data/InMemoryAuditRepository.hpp"
#include "infrastructure/adapters/PlaintextPasswordHasher.hpp"
#include "infrastructure/adapters/SimpleTokenGenerator.hpp"
#include "infrastructure/config/AuthConfig.hpp"

void demonstrateAuthentication() {
    // --- Dependency Injection ---
    auto userRepository = std::make_shared<infrastructure::data::InMemoryUserRepository>();
    auto auditRepository = std::make_shared<infrastructure::data::InMemoryAuditRepository>();
    auto passwordHasher = std::make_shared<infrastructure::adapters::PlaintextPasswordHasher>();
    auto tokenGenerator = std::make_shared<infrastructure::adapters::SimpleTokenGenerator>();
    auto authConfig = std::make_shared<infrastructure::config::JsonAuthConfigProvider>("config/auth_server.json");

    auto authService = std::make_shared<application::use_cases::AuthenticateUserUseCase>(
        userRepository,
        auditRepository,
        passwordHasher,
        tokenGenerator,
        authConfig
    );

    // --- Use Case Execution ---
    std::cout << "--- Demonstrating Authentication ---" << std::endl;

    // 1. Successful Login
    std::cout << "\n[1] Attempting successful login for 'admin'..." << std::endl;
    application::dto::AuthenticationRequest goodRequest = {"admin", "admin123"};
    auto goodResult = authService->authenticate(goodRequest);
    std::cout << "    Success: " << (goodResult.success ? "Yes" : "No") << std::endl;
    std::cout << "    Message: " << goodResult.message << std::endl;
    std::cout << "    Token:   " << goodResult.token << std::endl;

    // 2. Failed Login (Wrong Password)
    std::cout << "\n[2] Attempting failed login for 'admin' (wrong password)..." << std::endl;
    application::dto::AuthenticationRequest badPasswordRequest = {"admin", "wrongpass"};
    auto badPasswordResult = authService->authenticate(badPasswordRequest);
    std::cout << "    Success: " << (badPasswordResult.success ? "Yes" : "No") << std::endl;
    std::cout << "    Message: " << badPasswordResult.message << std::endl;

    // 3. Failed Login (User Not Found)
    std::cout << "\n[3] Attempting failed login for non-existent user 'nouser'..." << std::endl;
    application::dto::AuthenticationRequest nonExistentUserRequest = {"nouser", "password"};
    auto nonExistentUserResult = authService->authenticate(nonExistentUserRequest);
    std::cout << "    Success: " << (nonExistentUserResult.success ? "Yes" : "No") << std::endl;
    std::cout << "    Message: " << nonExistentUserResult.message << std::endl;
    
    // --- Audit Log ---
    std::cout << "\n--- Audit Log Contents ---" << std::endl;
    auto auditLogs = auditRepository->getRecentLogs(10);
    for(const auto& log : auditLogs) {
        std::cout << "    User: " << log.getUsername() 
                  << ", Action: " << log.getAction() 
                  << ", Success: " << (log.isSuccess() ? "Yes" : "No")
                  << ", Details: " << log.getDetails() << std::endl;
    }
}

int main() {
    try {
        demonstrateAuthentication();
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
