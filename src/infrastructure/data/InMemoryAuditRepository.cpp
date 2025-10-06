#include "InMemoryAuditRepository.hpp"
#include <algorithm>

namespace infrastructure::data {

    void InMemoryAuditRepository::log(const domain::entities::AuditLog& logEntry) {
        std::lock_guard<std::mutex> lock(mutex_);
        logs_.push_back(logEntry);
    }

    std::vector<domain::entities::AuditLog> InMemoryAuditRepository::getRecentLogs(int limit) {
        std::lock_guard<std::mutex> lock(mutex_);
        int startIndex = std::max(0, static_cast<int>(logs_.size()) - limit);
        return std::vector<domain::entities::AuditLog>(logs_.begin() + startIndex, logs_.end());
    }

    std::vector<domain::entities::AuditLog> InMemoryAuditRepository::getLogsByUsername(const std::string& username) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<domain::entities::AuditLog> userLogs;
        for (const auto& log : logs_) {
            if (log.getUsername() == username) {
                userLogs.push_back(log);
            }
        }
        return userLogs;
    }

    std::vector<domain::entities::AuditLog> InMemoryAuditRepository::getLogsByDateRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<domain::entities::AuditLog> dateLogs;
        for (const auto& log : logs_) {
            if (log.getTimestamp() >= start && log.getTimestamp() <= end) {
                dateLogs.push_back(log);
            }
        }
        return dateLogs;
    }

}
