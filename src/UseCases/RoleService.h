#pragma once

#include "../Entities/Role.h"
#include "../InterfaceAdapters/IRoleRepository.h"
#include <memory>
#include <vector>
#include <optional>
#include <string>

class RoleService {
public:
    explicit RoleService(std::shared_ptr<IRoleRepository> repo);

    std::optional<Role> getRoleById(int id);
    std::vector<Role> getAllRoles();
    bool createRole(const std::string& name);
    bool updateRole(int id, const std::string& newName);
    bool deleteRole(int id);

private:
    std::shared_ptr<IRoleRepository> roleRepository;
};
