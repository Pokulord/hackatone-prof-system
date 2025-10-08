#pragma once

#include "crow_all.h"
#include "UseCases/UserService.h"
#include "InterfaceAdapters/PgUserRepository.h"
#include "jwt_utils.h"
#include "InterfaceAdapters/RefreshTokenService.h"
#include <memory>

#include "InterfaceAdapters/PgRoleRepository.h"
#include "UseCases/RoleService.h"
#include "Logger/Logger.h"

class Application {
public:
    Application();
    void run();

private:
    crow::SimpleApp app;
    std::string connStr;
    std::unique_ptr<Logger> logger;
    std::shared_ptr<PgUserRepository> userRepo;
    std::shared_ptr<PgRoleRepository> roleRepo;
    std::unique_ptr<UserService> userService;
    std::unique_ptr<RoleService> roleService;
    std::unique_ptr<JwtUtils> jwtUtils;
    std::unique_ptr<RefreshTokenService> refreshService;
};
