#include "endpoints.h"
#include "AuthEndpoints.h"
#include "UserEndpoints.h"

void registerEndpoints(crow::SimpleApp& app, UserService& userService, JwtUtils& jwtUtils, 
                        RefreshTokenService& refreshService, PgUserRepository& userRepo, RoleService& roleService) {
    registerAuthEndpoints(app, userService, jwtUtils, refreshService, userRepo);
    registerUserEndpoints(app, userService, jwtUtils);
    registerMonitoringEndpoints(app);
    registerRoleEndpoints(app, roleService, jwtUtils);
}

