#pragma once

#include "../entities/AuditLog.hpp"
#include <vector>
#include <string>
#include <chrono>

namespace domain::repositories {
    class IAuditRepository {
    public:
        virtual ~IAuditRepository() = default;
        
        virtual void log(const entities::AuditLog& logEntry) = 0;
        virtual std::vector<entities::AuditLog> getRecentLogs(int limit) = 0;
        virtual std::vector<entities::AuditLog> getLogsByUsername(const std::string& username) = 0;
        virtual std::vector<entities::AuditLog> getLogsByDateRange(
            const std::chrono::system_clock::time_point& start,
            const std::chrono::system_clock::time_point& end) = 0;
    };
}
