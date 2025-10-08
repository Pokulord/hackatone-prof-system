#include "UserEndpoints.h"
#include "../jwt-cpp/jwt.h"
#include <vector>

void registerUserEndpoints(crow::SimpleApp& app, UserService& userService, JwtUtils& jwtUtils, Logger& logger) {
    CROW_ROUTE(app, "/api/users/getAll").methods(crow::HTTPMethod::Get)(
        [&userService, &logger](const crow::request& req) {
            logger.log(Logger::Level::INFO, "GET /api/users/getAll from " + req.remote_ip_address);
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.substr(0, 7) != "Bearer ") {
                return crow::response(401, "Unauthorized");
            }
            std::string token = authHeader.substr(7);
            std::string requesterRole;
            std::string requesterUsername;

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

            if (requesterRole != "admin") {
                logger.log(Logger::Level::WARNING, "Forbidden GET /api/users/getAll attempt by user: " + requesterUsername);
                return crow::response(403, "Forbidden: Admins only");
            }

            logger.log(Logger::Level::INFO, "Admin user '" + requesterUsername + "' requested all users.");
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
        [&userService, &logger](const crow::request& req) {
            logger.log(Logger::Level::INFO, "GET /api/users/me from " + req.remote_ip_address);
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
                logger.log(Logger::Level::ERR, "User from valid token not found in DB: " + currentUsername);
                return crow::response(404, "User not found");
            }

            logger.log(Logger::Level::INFO, "User '" + currentUsername + "' requested their own profile.");
            crow::json::wvalue res;
            res["username"] = userOpt->getUsername();
            res["role"] = userOpt->getRole();
            res["mustChangePassword"] = userOpt->getMustChangePassword();

            return crow::response(res);
        }
    );

    CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::Get)(
        [&jwtUtils, &userService, &logger](const crow::request& req, const std::string& requestedUsername) {
            logger.log(Logger::Level::INFO, "GET /api/users/" + requestedUsername + " from " + req.remote_ip_address);
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

            if (requesterUsername != requestedUsername && requesterRole != "admin") {
                logger.log(Logger::Level::WARNING, "Forbidden GET /api/users/" + requestedUsername + " attempt by user: " + requesterUsername);
                return crow::response(403, "Forbidden");
            }

            auto userOpt = userService.getUserByUsername(requestedUsername);
            if (!userOpt) {
                logger.log(Logger::Level::INFO, "User not found: " + requestedUsername);
                return crow::response(404, "User not found");
            }

            logger.log(Logger::Level::INFO, "User '" + requesterUsername + "' accessed profile of '" + requestedUsername + "'.");
            crow::json::wvalue res;
            res["username"] = userOpt->getUsername();
            res["role"] = userOpt->getRole();
            res["mustChangePassword"] = userOpt->getMustChangePassword();

            return crow::response(res);
        }
    );

    CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::Put)(
        [&jwtUtils, &userService, &logger](const crow::request& req, const std::string& usernameToUpdate) {
            logger.log(Logger::Level::INFO, "PUT /api/users/" + usernameToUpdate + " from " + req.remote_ip_address);
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
                logger.log(Logger::Level::WARNING, "Update user failed: Invalid JSON. User: " + requesterUsername);
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
                    logger.log(Logger::Level::WARNING, "Forbidden attempt by '" + requesterUsername + "' to change password for '" + usernameToUpdate + "'.");
                    return crow::response(403, "Forbidden: You can only change your own password.");
                }
            }

            if (json_body.has("role")) {
                if (isAdmin) {
                    newRole = json_body["role"].s();
                } else {
                    logger.log(Logger::Level::WARNING, "Forbidden attempt by non-admin '" + requesterUsername + "' to change role for '" + usernameToUpdate + "'.");
                    return crow::response(403, "Forbidden: Only admins can change roles.");
                }
            }

            if (json_body.has("mustChangePassword")) {
                if (isAdmin) {
                    newMustChangePassword = json_body["mustChangePassword"].b();
                }
                else {
                    logger.log(Logger::Level::WARNING, "Forbidden attempt by non-admin '" + requesterUsername + "' to change mustChangePassword for '" + usernameToUpdate + "'.");
                    return crow::response(403, "Forbidden: Only admins can change this setting.");
                }
            }

            if (!newPassword && !newRole && !newMustChangePassword) {
                return crow::response(400, "No update data provided.");
            }

            if (userService.updateUser(usernameToUpdate, newPassword, newRole, newMustChangePassword)) {
                logger.log(Logger::Level::INFO, "User '" + usernameToUpdate + "' updated successfully by '" + requesterUsername + "'.");
                return crow::response(200, "User updated successfully.");
            } else {
                logger.log(Logger::Level::ERR, "Update failed for user '" + usernameToUpdate + "'. User may not exist.");
                return crow::response(404, "User not found or update failed.");
            }
        }
    );

    CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::Delete)(
        [&jwtUtils, &userService, &logger](const crow::request& req, const std::string& usernameToDelete) {
            logger.log(Logger::Level::INFO, "DELETE /api/users/" + usernameToDelete + " from " + req.remote_ip_address);
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
                logger.log(Logger::Level::WARNING, "Forbidden DELETE attempt by '" + requesterUsername + "' on user '" + usernameToDelete + "'.");
                return crow::response(403, "Forbidden: You can only delete your own account, or an admin can delete any account.");
            }

            if (userService.deleteUser(usernameToDelete)) {
                logger.log(Logger::Level::INFO, "User '" + usernameToDelete + "' deleted successfully by '" + requesterUsername + "'.");
                return crow::response(200, "User deleted successfully.");
            } else {
                logger.log(Logger::Level::ERR, "Delete failed for user '" + usernameToDelete + "'. User may not exist.");
                return crow::response(404, "User not found or delete failed.");
            }
        }
    );

    CROW_ROUTE(app, "/api/users/<string>/roles").methods(crow::HTTPMethod::Patch)(
        [&jwtUtils, &userService, &logger](const crow::request& req, const std::string& usernameToUpdate) {
            logger.log(Logger::Level::INFO, "PATCH /api/users/" + usernameToUpdate + "/roles from " + req.remote_ip_address);
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.substr(0, 7) != "Bearer ") {
                return crow::response(401, "Unauthorized");
            }
            std::string token = authHeader.substr(7);
            std::string requesterRole, requesterUsername;

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

            if (requesterRole != "admin") {
                logger.log(Logger::Level::WARNING, "Forbidden PATCH role attempt by non-admin '" + requesterUsername + "' on user '" + usernameToUpdate + "'.");
                return crow::response(403, "Forbidden: Admins only");
            }

            auto json_body = crow::json::load(req.body);
            if (!json_body || !json_body.has("role")) {
                return crow::response(400, "Invalid JSON or missing 'role' field");
            }

            std::optional<std::string> newRole = json_body["role"].s();

            if (userService.updateUser(usernameToUpdate, std::nullopt, newRole, std::nullopt)) {
                logger.log(Logger::Level::INFO, "Role for user '" + usernameToUpdate + "' updated to '" + *newRole + "' by admin '" + requesterUsername + "'.");
                return crow::response(200, "User role updated successfully.");
            } else {
                logger.log(Logger::Level::ERR, "Role update failed for user '" + usernameToUpdate + "'. User may not exist.");
                return crow::response(404, "User not found or update failed.");
            }
        }
    );
}