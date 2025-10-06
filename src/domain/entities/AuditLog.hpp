#pragma once

#include <string>
#include <chrono>

namespace domain::entities {
    class AuditLog {
    public:
        AuditLog(int id,
                 std::string username,
                 std::string action,
                 std::chrono::system_clock::time_point timestamp,
                 bool success,
                 std::string details);

        static AuditLog create(std::string username, std::string action, bool success, std::string details = "");

        int getId() const;
        const std::string& getUsername() const;
        const std::string& getAction() const;
        const std::chrono::system_clock::time_point& getTimestamp() const;
        bool isSuccess() const;
        const std::string& getDetails() const;

    private:
        int id_;
        std::string username_;
        std::string action_;
        std::chrono::system_clock::time_point timestamp_;
        bool success_;
        std::string details_;
    };
}
