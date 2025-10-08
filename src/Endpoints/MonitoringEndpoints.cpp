#include "MonitoringEndpoints.h"

void registerMonitoringEndpoints(crow::SimpleApp& app, Logger& logger) {
    CROW_ROUTE(app, "/api/health").methods(crow::HTTPMethod::Get)([&logger](const crow::request& req) {
        logger.log(Logger::Level::INFO, "GET /api/health from " + req.remote_ip_address);
        crow::json::wvalue response;
        response["status"] = "ok";
        return crow::response(200, response);
    });

    CROW_ROUTE(app, "/api/version").methods(crow::HTTPMethod::Get)([&logger](const crow::request& req) {
        logger.log(Logger::Level::INFO, "GET /api/version from " + req.remote_ip_address);
        crow::json::wvalue response;
        response["version"] = "0.1.0";
        return crow::response(200, response);
    });
}
