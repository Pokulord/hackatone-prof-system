#pragma once

#include "../../domain/repositories/IAuditRepository.hpp"
#include <vector>
#include <mutex>

namespace infrastructure::data {
    class InMemoryAuditRepository : public domain::repositories::IAuditRepository {
    public:
        void log(const domain::entities::AuditLog& logEntry) override;
        std::vector<domain::entities::AuditLog> getRecentLogs(int limit) override;
        std::vector<domain::entities::AuditLog> getLogsByUsername(const std::string& username) override;
        std::vector<domain::entities::AuditLog> getLogsByDateRange(
            const std::chrono::system_clock::time_point& start,
            const std::chrono::system_clock::time_point& end) override;

    private:
        std::vector<domain::entities::AuditLog> logs_;
        std::mutex mutex_;
    };
}
