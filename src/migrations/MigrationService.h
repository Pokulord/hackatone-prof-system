#pragma once

#include <string>
#include <pqxx/pqxx>

class MigrationService {
public:
    explicit MigrationService(const std::string& connStr, const std::string& migrationsPath);
    void runMigrations();

private:
    void createMigrationsTable();
    std::vector<std::string> getAppliedMigrations();
    
    std::string connectionString;
    std::string migrationsDir;
};
