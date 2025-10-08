#pragma once

#include "../crow_all.h"

#include "../Logger/Logger.h"

void registerMonitoringEndpoints(crow::SimpleApp& app, Logger& logger);
