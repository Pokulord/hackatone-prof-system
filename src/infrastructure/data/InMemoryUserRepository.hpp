#pragma once

#include "../../domain/repositories/IUserRepository.hpp"
#include <vector>
#include <map>
#include <mutex>

namespace infrastructure::data {
    /**
     * @brief In-memory implementation of IUserRepository for testing and development.
     * 
     * @note This implementation is not thread-safe for all operations.
     * A mutex is included for basic safety, but complex transactions are not supported.
     */
    class InMemoryUserRepository : public domain::repositories::IUserRepository {
    public:
        InMemoryUserRepository();

        std::optional<domain::entities::User> findByUsername(const std::string& username) override;
        std::optional<domain::entities::User> findById(int id) override;
        std::vector<domain::entities::User> findAll() override;
        void save(const domain::entities::User& user) override;
        void remove(const std::string& username) override;
        bool exists(const std::string& username) override;

    private:
        std::map<std::string, domain::entities::User> users_;
        int nextId_ = 1;
        std::mutex mutex_;
    };
}
