#include "MigrationService.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <set>

namespace fs = std::filesystem;

MigrationService::MigrationService(const std::string& connStr, const std::string& migrationsPath)
    : connectionString(connStr), migrationsDir(migrationsPath) {}

void MigrationService::createMigrationsTable() {
    pqxx::connection c(connectionString);
    pqxx::work txn(c);
    txn.exec(R"(
        CREATE TABLE IF NOT EXISTS schema_migrations (
            version VARCHAR(255) PRIMARY KEY
        );
    )");
    txn.commit();
}

std::vector<std::string> MigrationService::getAppliedMigrations() {
    std::vector<std::string> versions;
    pqxx::connection c(connectionString);
    pqxx::nontransaction n(c);
    pqxx::result r = n.exec("SELECT version FROM schema_migrations");
    
    for (const auto& row : r) {
        versions.push_back(row[0].as<std::string>());
    }
    return versions;
}

void MigrationService::runMigrations() {
    try {
        createMigrationsTable();
        
        std::set<std::string> appliedMigrations;
        for(const auto& version : getAppliedMigrations()) {
            appliedMigrations.insert(version);
        }

        std::vector<fs::path> migrationFiles;
        for (const auto& entry : fs::directory_iterator(migrationsDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".sql") {
                migrationFiles.push_back(entry.path());
            }
        }
        std::sort(migrationFiles.begin(), migrationFiles.end());

        for (const auto& filePath : migrationFiles) {
            std::string filename = filePath.filename().string();
            if (appliedMigrations.find(filename) == appliedMigrations.end()) {
                std::cout << "Applying migration: " << filename << std::endl;
                
                std::ifstream file(filePath);
                if (!file.is_open()) {
                    throw std::runtime_error("Could not open migration file: " + filename);
                }
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                pqxx::connection c(connectionString);
                pqxx::work txn(c);
                txn.exec(content);
                txn.exec_params("INSERT INTO schema_migrations (version) VALUES ($1)", filename);
                txn.commit();
                
                std::cout << "Successfully applied migration: " << filename << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Migration failed: " << e.what() << std::endl;
        // It's often a good idea to re-throw or exit here to prevent the application
        // from running with a partially migrated or incorrect schema.
        throw;
    }
}
