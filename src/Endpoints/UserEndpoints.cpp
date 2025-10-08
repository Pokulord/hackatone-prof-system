#include "UserEndpoints.h"
#include "../jwt-cpp/jwt.h"

void registerUserEndpoints(crow::SimpleApp& app, UserService& userService, JwtUtils& jwtUtils) {
    CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::Get)(
        [&jwtUtils, &userService](const crow::request& req, const std::string& requestedUsername) {
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.substr(0, 7) != "Bearer ")
                return crow::response(401, "Unauthorized");

            std::string token = authHeader.substr(7);
            std::string requesterUsername, requesterRole;

            try {
                auto decoded = jwt::decode(token);
                auto verifier = jwt::verify()
                    .allow_algorithm(jwt::algorithm::hs256{"supersecretkey"})
                    .with_issuer("auth_server");
                verifier.verify(decoded);

                requesterUsername = decoded.get_payload_claim("username").as_string();
                requesterRole = decoded.get_payload_claim("role").as_string();
            } catch (...) {
                return crow::response(401, "Invalid token");
            }

            if (requesterUsername != requestedUsername && requesterRole != "admin")
                return crow::response(403, "Forbidden");

            auto userOpt = userService.getUserByUsername(requestedUsername);
            if (!userOpt)
                return crow::response(404, "User not found");

            crow::json::wvalue res;
            res["username"] = userOpt->getUsername();
            res["role"] = (userOpt->getRole() == Role::ADMIN) ? "admin" : "user";
            res["mustChangePassword"] = userOpt->getMustChangePassword();

            return crow::response(res);
        }
    );
}
