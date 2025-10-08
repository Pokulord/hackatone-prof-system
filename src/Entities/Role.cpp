#include "Role.h"

Role::Role(int id, const std::string& name) : id(id), name(name) {}

int Role::getId() const {
    return id;
}

const std::string& Role::getName() const {
    return name;
}

void Role::setName(const std::string& newName) {
    name = newName;
}
