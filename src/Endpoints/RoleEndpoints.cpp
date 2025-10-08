#include "RoleEndpoints.h"
#include "../jwt-cpp/jwt.h"

// Middleware-like function to check for admin role from a token
// It also logs the username for audit purposes
std::optional<std::string> getAdminUsername(const crow::request& req) {
    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.substr(0, 7) != "Bearer ") {
        return std::nullopt;
    }
    std::string token = authHeader.substr(7);
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{"supersecretkey"})
            .with_issuer("auth_server");
        verifier.verify(decoded);
        if (decoded.get_payload_claim("role").as_string() == "admin") {
            return decoded.get_payload_claim("username").as_string();
        }
    } catch (...) {
        // fall through
    }
    return std::nullopt;
}

void registerRoleEndpoints(crow::SimpleApp& app, RoleService& roleService, JwtUtils& jwtUtils, Logger& logger) {

    // GET /api/roles - Get all roles
    CROW_ROUTE(app, "/api/roles").methods(crow::HTTPMethod::Get)([&](const crow::request& req) {
        logger.log(Logger::Level::INFO, "GET /api/roles from " + req.remote_ip_address);
        auto adminUsernameOpt = getAdminUsername(req);
        if (!adminUsernameOpt) {
            logger.log(Logger::Level::WARNING, "Forbidden GET /api/roles attempt from " + req.remote_ip_address);
            return crow::response(403, "Forbidden: Admins only");
        }
        
        logger.log(Logger::Level::INFO, "Admin '" + *adminUsernameOpt + "' requested all roles.");
        auto roles = roleService.getAllRoles();
        crow::json::wvalue::list roleList;
        for (const auto& role : roles) {
            crow::json::wvalue r;
            r["id"] = role.getId();
            r["name"] = role.getName();
            roleList.push_back(std::move(r));
        }
        return crow::response(200, crow::json::wvalue{roleList});
    });

    // POST /api/roles - Create a new role
    CROW_ROUTE(app, "/api/roles").methods(crow::HTTPMethod::Post)([&](const crow::request& req) {
        logger.log(Logger::Level::INFO, "POST /api/roles from " + req.remote_ip_address);
        auto adminUsernameOpt = getAdminUsername(req);
        if (!adminUsernameOpt) {
            logger.log(Logger::Level::WARNING, "Forbidden POST /api/roles attempt from " + req.remote_ip_address);
            return crow::response(403, "Forbidden: Admins only");
        }

        auto body = crow::json::load(req.body);
        if (!body || !body.has("name")) {
            return crow::response(400, "Invalid JSON or missing 'name' field");
        }
        std::string name = body["name"].s();

        if (roleService.createRole(name)) {
            logger.log(Logger::Level::INFO, "Admin '" + *adminUsernameOpt + "' created role: " + name);
            return crow::response(201, "Role created");
        } else {
            logger.log(Logger::Level::WARNING, "Admin '" + *adminUsernameOpt + "' failed to create role '" + name + "'. It may already exist.");
            return crow::response(409, "Role with this name already exists");
        }
    });

    // PUT /api/roles/<int> - Update a role
    CROW_ROUTE(app, "/api/roles/<int>").methods(crow::HTTPMethod::Put)([&](const crow::request& req, int roleId) {
        logger.log(Logger::Level::INFO, "PUT /api/roles/" + std::to_string(roleId) + " from " + req.remote_ip_address);
        auto adminUsernameOpt = getAdminUsername(req);
        if (!adminUsernameOpt) {
            logger.log(Logger::Level::WARNING, "Forbidden PUT /api/roles attempt from " + req.remote_ip_address);
            return crow::response(403, "Forbidden: Admins only");
        }

        auto body = crow::json::load(req.body);
        if (!body || !body.has("name")) {
            return crow::response(400, "Invalid JSON or missing 'name' field");
        }
        std::string newName = body["name"].s();

        if (roleService.updateRole(roleId, newName)) {
            logger.log(Logger::Level::INFO, "Admin '" + *adminUsernameOpt + "' updated role " + std::to_string(roleId) + " to '" + newName + "'.");
            return crow::response(200, "Role updated");
        } else {
            logger.log(Logger::Level::WARNING, "Admin '" + *adminUsernameOpt + "' failed to update role " + std::to_string(roleId) + ". It may not exist or the new name is taken.");
            return crow::response(404, "Role not found or name already exists");
        }
    });

    // DELETE /api/roles/<int> - Delete a role
    CROW_ROUTE(app, "/api/roles/<int>").methods(crow::HTTPMethod::Delete)([&](const crow::request& req, int roleId) {
        logger.log(Logger::Level::INFO, "DELETE /api/roles/" + std::to_string(roleId) + " from " + req.remote_ip_address);
        auto adminUsernameOpt = getAdminUsername(req);
        if (!adminUsernameOpt) {
            logger.log(Logger::Level::WARNING, "Forbidden DELETE /api/roles attempt from " + req.remote_ip_address);
            return crow::response(403, "Forbidden: Admins only");
        }

        if (roleService.deleteRole(roleId)) {
            logger.log(Logger::Level::INFO, "Admin '" + *adminUsernameOpt + "' deleted role " + std::to_string(roleId) + ".");
            return crow::response(200, "Role deleted");
        } else {
            logger.log(Logger::Level::WARNING, "Admin '" + *adminUsernameOpt + "' failed to delete role " + std::to_string(roleId) + ". It may not exist.");
            return crow::response(404, "Role not found");
        }
    });
}