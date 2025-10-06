// ===============================
// APPLICATION LAYER - USE CASES
// ===============================

// application/use_cases/AuthenticateUserUseCase.hpp
#pragma once
#include "domain/entities/User.hpp"
#include "domain/repositories/IUserRepository.hpp"
#include "domain/services/IPasswordHasher.hpp"
#include "domain/services/ITokenGenerator.hpp"
#include <memory>
#include <string>

namespace application::use_cases {
    struct AuthenticationRequest {
        std::string username;
        std::string password;
    };
    
    struct AuthenticationResult {
        bool success;
        std::string token;
        std::string errorMessage;
        bool requiresPasswordChange;
    };
    
    class AuthenticateUserUseCase {
    public:
        AuthenticateUserUseCase(
            std::shared_ptr<domain::repositories::IUserRepository> userRepo,
            std::shared_ptr<domain::services::IPasswordHasher> hasher,
            std::shared_ptr<domain::services::ITokenGenerator> tokenGen
        );
        
        AuthenticationResult execute(const AuthenticationRequest& request);
        
    private:
        std::shared_ptr<domain::repositories::IUserRepository> userRepository_;
        std::shared_ptr<domain::services::IPasswordHasher> passwordHasher_;
        std::shared_ptr<domain::services::ITokenGenerator> tokenGenerator_;
    };
}

// application/use_cases/CreateUserUseCase.hpp
#pragma once
#include "domain/entities/User.hpp"
#include "domain/entities/UserRole.hpp"
#include "domain/repositories/IUserRepository.hpp"
#include "domain/services/IPasswordHasher.hpp"
#include <memory>
#include <string>

namespace application::use_cases {
    struct CreateUserRequest {
        std::string username;
        std::string password;
        domain::entities::UserRole role;
        std::string adminToken; // For authorization
    };
    
    struct CreateUserResult {
        bool success;
        std::string errorMessage;
        int userId;
    };
    
    class CreateUserUseCase {
    public:
        CreateUserUseCase(
            std::shared_ptr<domain::repositories::IUserRepository> userRepo,
            std::shared_ptr<domain::services::IPasswordHasher> hasher
        );
        
        CreateUserResult execute(const CreateUserRequest& request);
        
    private:
        std::shared_ptr<domain::repositories::IUserRepository> userRepository_;
        std::shared_ptr<domain::services::IPasswordHasher> passwordHasher_;
        
        bool validateCreateUserRequest(const CreateUserRequest& request);
    };
}

// application/use_cases/ChangePasswordUseCase.hpp
#pragma once
#include "domain/repositories/IUserRepository.hpp"
#include "domain/services/IPasswordHasher.hpp"
#include <memory>
#include <string>

namespace application::use_cases {
    struct ChangePasswordRequest {
        std::string username;
        std::string currentPassword;
        std::string newPassword;
        std::string userToken; // For authorization
    };
    
    struct ChangePasswordResult {
        bool success;
        std::string errorMessage;
    };
    
    class ChangePasswordUseCase {
    public:
        ChangePasswordUseCase(
            std::shared_ptr<domain::repositories::IUserRepository> userRepo,
            std::shared_ptr<domain::services::IPasswordHasher> hasher
        );
        
        ChangePasswordResult execute(const ChangePasswordRequest& request);
        
    private:
        std::shared_ptr<domain::repositories::IUserRepository> userRepository_;
        std::shared_ptr<domain::services::IPasswordHasher> passwordHasher_;
    };
}

// ===============================
// INFRASTRUCTURE LAYER - ADAPTERS
// ===============================

// infrastructure/adapters/BcryptPasswordHasher.hpp
#pragma once
#include "domain/services/IPasswordHasher.hpp"
#include <string>

namespace infrastructure::adapters {
    class BcryptPasswordHasher : public domain::services::IPasswordHasher {
    public:
        BcryptPasswordHasher(int defaultCost = 12);
        
        std::string hashPassword(const std::string& password, int cost = 0) override;
        bool verifyPassword(const std::string& password, const std::string& hash) override;
        bool isValidPassword(const std::string& password) override;
        int getRecommendedCost() override;
        
    private:
        int defaultCost_;
        
        bool meetsPasswordRequirements(const std::string& password);
    };
}

// infrastructure/adapters/JwtTokenGenerator.hpp
#pragma once
#include "domain/services/ITokenGenerator.hpp"
#include "domain/entities/User.hpp"
#include <string>

namespace infrastructure::adapters {
    class JwtTokenGenerator : public domain::services::ITokenGenerator {
    public:
        JwtTokenGenerator(const std::string& secretKey, int expirationHours = 24);
        
        std::string generateSessionToken(const domain::entities::User& user) override;
        bool validateToken(const std::string& token) override;
        std::string extractUsername(const std::string& token) override;
        
    private:
        std::string secretKey_;
        int expirationHours_;
    };
}

// ===============================
// INFRASTRUCTURE LAYER - WEB
// ===============================

// infrastructure/web/RestApiServer.hpp
#pragma once
#include <httplib.h>
#include <memory>
#include <functional>

namespace infrastructure::web {
    class AuthController;
    class UserController;
    class AuditController;
    
    class RestApiServer {
    public:
        RestApiServer();
        ~RestApiServer();
        
        void start(int port = 8080);
        void stop();
        
        void setupRoutes(
            std::shared_ptr<AuthController> authController,
            std::shared_ptr<UserController> userController,
            std::shared_ptr<AuditController> auditController
        );
        
    private:
        std::unique_ptr<httplib::Server> server_;
        bool running_;
        
        void setupCorsHeaders();
        void setupErrorHandlers();
    };
}

// infrastructure/web/AuthController.hpp
#pragma once
#include "application/use_cases/AuthenticateUserUseCase.hpp"
#include "application/use_cases/ChangePasswordUseCase.hpp"
#include <httplib.h>
#include <memory>

namespace infrastructure::web {
    class AuthController {
    public:
        AuthController(
            std::shared_ptr<application::use_cases::AuthenticateUserUseCase> authUseCase,
            std::shared_ptr<application::use_cases::ChangePasswordUseCase> changePassUseCase
        );
        
        void handleLogin(const httplib::Request& req, httplib::Response& res);
        void handleLogout(const httplib::Request& req, httplib::Response& res);
        void handleChangePassword(const httplib::Request& req, httplib::Response& res);
        
    private:
        std::shared_ptr<application::use_cases::AuthenticateUserUseCase> authenticateUseCase_;
        std::shared_ptr<application::use_cases::ChangePasswordUseCase> changePasswordUseCase_;
        
        // Helper methods
        nlohmann::json parseAuthRequest(const std::string& body);
        void sendAuthResponse(httplib::Response& res, const application::use_cases::AuthenticationResult& result);
        void sendErrorResponse(httplib::Response& res, int statusCode, const std::string& message);
        bool extractTokenFromHeader(const httplib::Request& req, std::string& token);
    };
}

// ===============================
// MAIN APPLICATION ENTRY POINT
// ===============================

// main.cpp
#include "infrastructure/adapters/BcryptPasswordHasher.hpp"
#include "infrastructure/adapters/JwtTokenGenerator.hpp"
#include "application/use_cases/AuthenticateUserUseCase.hpp"
#include "application/use_cases/CreateUserUseCase.hpp"
#include "application/use_cases/ChangePasswordUseCase.hpp"
#include "infrastructure/web/RestApiServer.hpp"
#include "infrastructure/web/AuthController.hpp"
#include "infrastructure/web/UserController.hpp"
#include "infrastructure/web/AuditController.hpp"
// Note: These will be implemented by Backend #2
// #include "infrastructure/data/PostgreSqlUserRepository.hpp"
// #include "infrastructure/data/PostgreSqlAuditRepository.hpp"
// #include "infrastructure/config/ConfigManager.hpp"

int main() {
    try {
        // Configuration (will be implemented by Backend #2)
        // auto config = std::make_shared<infrastructure::config::ConfigManager>();
        // config->loadFromFile("config/auth_server.json");
        
        // Infrastructure adapters (Backend #1)
        auto passwordHasher = std::make_shared<infrastructure::adapters::BcryptPasswordHasher>(12);
        auto tokenGenerator = std::make_shared<infrastructure::adapters::JwtTokenGenerator>("secret_key_here", 24);
        
        // Data repositories (will be implemented by Backend #2) 
        // auto userRepository = std::make_shared<infrastructure::data::PostgreSqlUserRepository>(
        //     config->getDatabaseConnectionString()
        // );
        // auto auditRepository = std::make_shared<infrastructure::data::PostgreSqlAuditRepository>(
        //     config->getDatabaseConnectionString()
        // );
        
        // For now, use placeholder - Backend #2 will replace this
        // std::shared_ptr<domain::repositories::IUserRepository> userRepository = nullptr;
        
        // Application use cases (Backend #1)
        // auto authUseCase = std::make_shared<application::use_cases::AuthenticateUserUseCase>(
        //     userRepository, passwordHasher, tokenGenerator
        // );
        // auto createUserUseCase = std::make_shared<application::use_cases::CreateUserUseCase>(
        //     userRepository, passwordHasher
        // );
        // auto changePasswordUseCase = std::make_shared<application::use_cases::ChangePasswordUseCase>(
        //     userRepository, passwordHasher
        // );
        
        // Web controllers (Backend #1)
        // auto authController = std::make_shared<infrastructure::web::AuthController>(
        //     authUseCase, changePasswordUseCase
        // );
        
        // REST API Server (Backend #1)
        auto server = std::make_shared<infrastructure::web::RestApiServer>();
        // server->setupRoutes(authController, userController, auditController);
        
        std::cout << "Starting Auth Server on port 8080..." << std::endl;
        server->start(8080);
        
    } catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}