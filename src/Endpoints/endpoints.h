#pragma once

#include "crow_all.h"
#include "../UseCases/UserService.h"
#include "../jwt_utils.h"
#include "../InterfaceAdapters/RefreshTokenService.h"
#include "../InterfaceAdapters/PgUserRepository.h"
#include "UserEndpoints.h"
#include "MonitoringEndpoints.h"
#include "RoleEndpoints.h"

#include "../UseCases/RoleService.h"

#include "../Logger/Logger.h"

void registerEndpoints(crow::SimpleApp& app, UserService& userService, JwtUtils& jwtUtils, 
                        RefreshTokenService& refreshService, PgUserRepository& userRepo, RoleService& roleService, Logger& logger);


