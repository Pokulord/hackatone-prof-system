#include "RoleEndpoints.h"
#include "../jwt-cpp/jwt.h"

// Middleware-like function to check for admin role from a token
bool isAdmin(const crow::request& req) {
    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.substr(0, 7) != "Bearer ") {
        return false;
    }
    std::string token = authHeader.substr(7);
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{"supersecretkey"})
            .with_issuer("auth_server");
        verifier.verify(decoded);
        std::string requesterRole = decoded.get_payload_claim("role").as_string();
        return requesterRole == "admin";
    } catch (...) {
        return false;
    }
}

void registerRoleEndpoints(crow::SimpleApp& app, RoleService& roleService, JwtUtils& jwtUtils) {

    // GET /api/roles - Get all roles
    CROW_ROUTE(app, "/api/roles").methods(crow::HTTPMethod::Get)([&](const crow::request& req) {
        if (!isAdmin(req)) {
            return crow::response(403, "Forbidden: Admins only");
        }
        
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
        if (!isAdmin(req)) {
            return crow::response(403, "Forbidden: Admins only");
        }

        auto body = crow::json::load(req.body);
        if (!body || !body.has("name")) {
            return crow::response(400, "Invalid JSON or missing 'name' field");
        }
        std::string name = body["name"].s();

        if (roleService.createRole(name)) {
            return crow::response(201, "Role created");
        } else {
            return crow::response(409, "Role with this name already exists");
        }
    });

    // PUT /api/roles/<int> - Update a role
    CROW_ROUTE(app, "/api/roles/<int>").methods(crow::HTTPMethod::Put)([&](const crow::request& req, int roleId) {
        if (!isAdmin(req)) {
            return crow::response(403, "Forbidden: Admins only");
        }

        auto body = crow::json::load(req.body);
        if (!body || !body.has("name")) {
            return crow::response(400, "Invalid JSON or missing 'name' field");
        }
        std::string newName = body["name"].s();

        if (roleService.updateRole(roleId, newName)) {
            return crow::response(200, "Role updated");
        } else {
            return crow::response(404, "Role not found or name already exists");
        }
    });

    // DELETE /api/roles/<int> - Delete a role
    CROW_ROUTE(app, "/api/roles/<int>").methods(crow::HTTPMethod::Delete)([&](const crow::request& req, int roleId) {
        if (!isAdmin(req)) {
            return crow::response(403, "Forbidden: Admins only");
        }

        if (roleService.deleteRole(roleId)) {
            return crow::response(200, "Role deleted");
        } else {
            return crow::response(404, "Role not found");
        }
    });
}
