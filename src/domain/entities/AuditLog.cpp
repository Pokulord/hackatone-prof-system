#include "AuditLog.hpp"
#include <stdexcept>

namespace domain::entities {

    AuditLog::AuditLog(int id,
                     std::string username,
                     std::string action,
                     std::chrono::system_clock::time_point timestamp,
                     bool success,
                     std::string details)
        : id_(id),
          username_(std::move(username)),
          action_(std::move(action)),
          timestamp_(timestamp),
          success_(success),
          details_(std::move(details)) {
        if (action_.empty()) {
            throw std::invalid_argument("Action cannot be empty.");
        }
    }

    AuditLog AuditLog::create(std::string username, std::string action, bool success, std::string details) {
        return AuditLog(0, // ID is set by the database
                        std::move(username),
                        std::move(action),
                        std::chrono::system_clock::now(),
                        success,
                        std::move(details));
    }

    int AuditLog::getId() const { return id_; }
    const std::string& AuditLog::getUsername() const { return username_; }
    const std::string& AuditLog::getAction() const { return action_; }
    const std::chrono::system_clock::time_point& AuditLog::getTimestamp() const { return timestamp_; }
    bool AuditLog::isSuccess() const { return success_; }
    const std::string& AuditLog::getDetails() const { return details_; }

}
