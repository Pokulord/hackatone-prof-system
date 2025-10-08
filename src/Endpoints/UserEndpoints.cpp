#include "UserEndpoints.h"
#include "../jwt-cpp/jwt.h"
#include <vector>

void registerUserEndpoints(crow::SimpleApp& app, UserService& userService, JwtUtils& jwtUtils) {
    CROW_ROUTE(app, "/api/users/getAll").methods(crow::HTTPMethod::Get)(
        [&](const crow::request& req) {
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.substr(0, 7) != "Bearer ") {
                return crow::response(401, "Unauthorized");
            }
            std::string token = authHeader.substr(7);
            std::string requesterRole;

            try {
                auto decoded = jwt::decode(token);
                auto verifier = jwt::verify()
                    .allow_algorithm(jwt::algorithm::hs256{"supersecretkey"})
                    .with_issuer("auth_server");
                verifier.verify(decoded);
                requesterRole = decoded.get_payload_claim("role").as_string();
            } catch (...) {
                return crow::response(401, "Invalid token");
            }

            if (requesterRole != "admin") {
                return crow::response(403, "Forbidden: Admins only");
            }

            std::vector<User> users = userService.getAllUsers();
            crow::json::wvalue::list userList;
            for (const auto& user : users) {
                crow::json::wvalue u;
                u["username"] = user.getUsername();
                u["role"] = user.getRole();
                u["mustChangePassword"] = user.getMustChangePassword();
                userList.push_back(std::move(u));
            }
            
            return crow::response(crow::json::wvalue{userList});
        }
    );

    CROW_ROUTE(app, "/api/users/me").methods(crow::HTTPMethod::Get)(
        [&](const crow::request& req) {
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.substr(0, 7) != "Bearer ") {
                return crow::response(401, "Unauthorized");
            }
            std::string token = authHeader.substr(7);
            std::string currentUsername;

            try {
                auto decoded = jwt::decode(token);
                auto verifier = jwt::verify()
                    .allow_algorithm(jwt::algorithm::hs256{"supersecretkey"})
                    .with_issuer("auth_server");
                verifier.verify(decoded);
                currentUsername = decoded.get_payload_claim("username").as_string();
            } catch (...) {
                return crow::response(401, "Invalid token");
            }

            auto userOpt = userService.getUserByUsername(currentUsername);
            if (!userOpt) {
                return crow::response(404, "User not found");
            }

            crow::json::wvalue res;
            res["username"] = userOpt->getUsername();
            res["role"] = userOpt->getRole();
            res["mustChangePassword"] = userOpt->getMustChangePassword();

            return crow::response(res);
        }
    );

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
            res["role"] = userOpt->getRole();
            res["mustChangePassword"] = userOpt->getMustChangePassword();

            return crow::response(res);
        }
    );

    CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::Put)(
        [&jwtUtils, &userService](const crow::request& req, const std::string& usernameToUpdate) {
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.substr(0, 7) != "Bearer ") {
                return crow::response(401, "Unauthorized");
            }
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

            auto json_body = crow::json::load(req.body);
            if (!json_body) {
                return crow::response(400, "Invalid JSON");
            }

            std::optional<std::string> newPassword;
            std::optional<std::string> newRole;
            std::optional<bool> newMustChangePassword;

            bool isOwnProfile = (requesterUsername == usernameToUpdate);
            bool isAdmin = (requesterRole == "admin");

            if (json_body.has("password")) {
                if (isOwnProfile) {
                    newPassword = json_body["password"].s();
                } else {
                    return crow::response(403, "Forbidden: You can only change your own password.");
                }
            }

            if (json_body.has("role")) {
                if (isAdmin) {
                    newRole = json_body["role"].s();
                } else {
                    return crow::response(403, "Forbidden: Only admins can change roles.");
                }
            }

            if (json_body.has("mustChangePassword")) {
                if (isAdmin) {
                    newMustChangePassword = json_body["mustChangePassword"].b();
                }
                else {
                    return crow::response(403, "Forbidden: Only admins can change this setting.");
                }
            }

            if (!newPassword && !newRole && !newMustChangePassword) {
                return crow::response(400, "No update data provided.");
            }

            if (userService.updateUser(usernameToUpdate, newPassword, newRole, newMustChangePassword)) {
                return crow::response(200, "User updated successfully.");
            } else {
                return crow::response(404, "User not found or update failed.");
            }
        }
    );

    CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::Delete)(
        [&jwtUtils, &userService](const crow::request& req, const std::string& usernameToDelete) {
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.substr(0, 7) != "Bearer ") {
                return crow::response(401, "Unauthorized");
            }
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

            bool isOwnProfile = (requesterUsername == usernameToDelete);
            bool isAdmin = (requesterRole == "admin");

            if (!isOwnProfile && !isAdmin) {
                return crow::response(403, "Forbidden: You can only delete your own account, or an admin can delete any account.");
            }

            if (userService.deleteUser(usernameToDelete)) {
                return crow::response(200, "User deleted successfully.");
            } else {
                return crow::response(404, "User not found or delete failed.");
            }
        }
    );

    CROW_ROUTE(app, "/api/users/<string>/roles").methods(crow::HTTPMethod::Patch)(
        [&jwtUtils, &userService](const crow::request& req, const std::string& usernameToUpdate) {
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.substr(0, 7) != "Bearer ") {
                return crow::response(401, "Unauthorized");
            }
            std::string token = authHeader.substr(7);
            std::string requesterRole;

            try {
                auto decoded = jwt::decode(token);
                auto verifier = jwt::verify()
                    .allow_algorithm(jwt::algorithm::hs256{"supersecretkey"})
                    .with_issuer("auth_server");
                verifier.verify(decoded);
                requesterRole = decoded.get_payload_claim("role").as_string();
            } catch (...) {
                return crow::response(401, "Invalid token");
            }

            if (requesterRole != "admin") {
                return crow::response(403, "Forbidden: Admins only");
            }

            auto json_body = crow::json::load(req.body);
            if (!json_body || !json_body.has("role")) {
                return crow::response(400, "Invalid JSON or missing 'role' field");
            }

            std::optional<std::string> newRole = json_body["role"].s();

            if (userService.updateUser(usernameToUpdate, std::nullopt, newRole, std::nullopt)) {
                return crow::response(200, "User role updated successfully.");
            } else {
                return crow::response(404, "User not found or update failed.");
            }
        }
    );
}
