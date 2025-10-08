#include "MonitoringEndpoints.h"

void registerMonitoringEndpoints(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/health").methods(crow::HTTPMethod::Get)([]() {
        crow::json::wvalue response;
        response["status"] = "ok";
        return crow::response(200, response);
    });

    CROW_ROUTE(app, "/api/version").methods(crow::HTTPMethod::Get)([]() {
        crow::json::wvalue response;
        response["version"] = "0.1.0";
        return crow::response(200, response);
    });
}
