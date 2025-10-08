#include "PgRoleRepository.h"
#include <iostream>

PgRoleRepository::PgRoleRepository(const std::string& connStr) : connectionString(connStr) {}

std::optional<Role> PgRoleRepository::findById(int id) {
    try {
        pqxx::connection c(connectionString);
        pqxx::work txn(c);
        pqxx::result r = txn.exec_params("SELECT id, name FROM roles WHERE id = $1", id);
        txn.commit();

        if (!r.empty()) {
            return Role(r[0][0].as<int>(), r[0][1].as<std::string>());
        }
    } catch (const std::exception& e) {
        std::cerr << "Database error in findById: " << e.what() << std::endl;
    }
    return std::nullopt;
}

std::optional<Role> PgRoleRepository::findByName(const std::string& name) {
    try {
        pqxx::connection c(connectionString);
        pqxx::work txn(c);
        pqxx::result r = txn.exec_params("SELECT id, name FROM roles WHERE name = $1", name);
        txn.commit();

        if (!r.empty()) {
            return Role(r[0][0].as<int>(), r[0][1].as<std::string>());
        }
    } catch (const std::exception& e) {
        std::cerr << "Database error in findByName: " << e.what() << std::endl;
    }
    return std::nullopt;
}

std::vector<Role> PgRoleRepository::findAll() {
    std::vector<Role> roles;
    try {
        pqxx::connection c(connectionString);
        pqxx::work txn(c);
        pqxx::result r = txn.exec("SELECT id, name FROM roles ORDER BY id");
        txn.commit();

        for (const auto& row : r) {
            roles.emplace_back(row[0].as<int>(), row[1].as<std::string>());
        }
    } catch (const std::exception& e) {
        std::cerr << "Database error in findAll: " << e.what() << std::endl;
    }
    return roles;
}

bool PgRoleRepository::save(const Role& role) {
    try {
        pqxx::connection c(connectionString);
        pqxx::work txn(c);
        txn.exec_params("INSERT INTO roles (name) VALUES ($1)", role.getName());
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Database error in save: " << e.what() << std::endl;
        return false;
    }
}

bool PgRoleRepository::update(const Role& role) {
    try {
        pqxx::connection c(connectionString);
        pqxx::work txn(c);
        auto res = txn.exec_params("UPDATE roles SET name = $1 WHERE id = $2", role.getName(), role.getId());
        txn.commit();
        return res.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Database error in update: " << e.what() << std::endl;
        return false;
    }
}

bool PgRoleRepository::remove(int id) {
    try {
        pqxx::connection c(connectionString);
        pqxx::work txn(c);
        auto res = txn.exec_params("DELETE FROM roles WHERE id = $1", id);
        txn.commit();
        return res.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Database error in remove: " << e.what() << std::endl;
        return false;
    }
}
