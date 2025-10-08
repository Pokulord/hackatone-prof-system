#pragma once

#include "../crow_all.h"
#include "../UseCases/RoleService.h"
#include "../jwt_utils.h"

#include "../Logger/Logger.h"

void registerRoleEndpoints(crow::SimpleApp& app, RoleService& roleService, JwtUtils& jwtUtils, Logger& logger);
