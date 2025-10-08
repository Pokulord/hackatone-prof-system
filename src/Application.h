#pragma once

#include "crow_all.h"
#include "UseCases/UserService.h"
#include "InterfaceAdapters/PgUserRepository.h"
#include "jwt_utils.h"
#include "InterfaceAdapters/RefreshTokenService.h"
#include <memory>

class Application {
public:
    Application();
    void run();

private:
    crow::SimpleApp app;
    std::string connStr;
    std::shared_ptr<PgUserRepository> userRepo;
    std::unique_ptr<UserService> userService;
    std::unique_ptr<JwtUtils> jwtUtils;
    std::unique_ptr<RefreshTokenService> refreshService;
};
