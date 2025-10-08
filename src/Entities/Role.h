#pragma once

#include <string>

class Role {
public:
    Role(int id, const std::string& name);

    int getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);

private:
    int id;
    std::string name;
};
