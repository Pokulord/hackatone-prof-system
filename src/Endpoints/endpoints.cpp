#include "endpoints.h"
#include "AuthEndpoints.h"
#include "UserEndpoints.h"

void registerEndpoints(crow::SimpleApp& app, UserService& userService, JwtUtils& jwtUtils, 
                        RefreshTokenService& refreshService, PgUserRepository& userRepo, RoleService& roleService, Logger& logger) {
    registerAuthEndpoints(app, userService, jwtUtils, refreshService, userRepo, logger);
    registerUserEndpoints(app, userService, jwtUtils, logger);
    registerMonitoringEndpoints(app, logger);
    registerRoleEndpoints(app, roleService, jwtUtils, logger);
}

