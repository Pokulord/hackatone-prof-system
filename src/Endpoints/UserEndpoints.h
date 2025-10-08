#pragma once

#include "crow_all.h"
#include "../UseCases/UserService.h"
#include "../jwt_utils.h"

#include "../Logger/Logger.h"

void registerUserEndpoints(crow::SimpleApp& app, UserService& userService, JwtUtils& jwtUtils, Logger& logger);
