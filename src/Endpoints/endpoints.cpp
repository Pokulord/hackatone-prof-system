#include "endpoints.h"
#include "AuthEndpoints.h"
#include "UserEndpoints.h"

void registerEndpoints(crow::SimpleApp& app, UserService& userService, JwtUtils& jwtUtils, 
                        RefreshTokenService& refreshService, PgUserRepository& userRepo) {
    registerAuthEndpoints(app, userService, jwtUtils, refreshService, userRepo);
    registerUserEndpoints(app, userService, jwtUtils);
}

