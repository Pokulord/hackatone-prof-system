#pragma once

#include "IRoleRepository.h"
#include <string>
#include <pqxx/pqxx>

class PgRoleRepository : public IRoleRepository {
public:
    explicit PgRoleRepository(const std::string& connStr);

    std::optional<Role> findById(int id) override;
    std::optional<Role> findByName(const std::string& name) override;
    std::vector<Role> findAll() override;
    bool save(const Role& role) override;
    bool update(const Role& role) override;
    bool remove(int id) override;

private:
    std::string connectionString;
};
