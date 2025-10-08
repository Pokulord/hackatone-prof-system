#pragma once

#include "crow_all.h"
#include "../UseCases/UserService.h"
#include "../jwt_utils.h"
#include "../InterfaceAdapters/RefreshTokenService.h"
#include "../InterfaceAdapters/PgUserRepository.h"

#include "../Logger/Logger.h"



void registerAuthEndpoints(crow::SimpleApp& app, UserService& userService, 

    JwtUtils& jwtUtils, RefreshTokenService& refreshService, PgUserRepository& userRepo, Logger& logger);
