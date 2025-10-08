#pragma once

#include "../Entities/Role.h"
#include <vector>
#include <optional>
#include <string>

class IRoleRepository {
public:
    virtual ~IRoleRepository() = default;

    virtual std::optional<Role> findById(int id) = 0;
    virtual std::optional<Role> findByName(const std::string& name) = 0;
    virtual std::vector<Role> findAll() = 0;
    virtual bool save(const Role& role) = 0;
    virtual bool update(const Role& role) = 0;
    virtual bool remove(int id) = 0;
};
