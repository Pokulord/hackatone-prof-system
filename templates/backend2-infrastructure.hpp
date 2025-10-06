-- ===============================
-- POSTGRESQL SCHEMA & MIGRATIONS
-- ===============================

-- sql/001_initial_schema.sql
-- Initial database schema for authentication system

-- Create users table
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    role VARCHAR(10) DEFAULT 'user' CHECK (role IN ('admin', 'user')),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP NULL,
    password_changed BOOLEAN DEFAULT FALSE,
    failed_attempts INTEGER DEFAULT 0,
    locked_until TIMESTAMP NULL
);

-- Create audit_log table
CREATE TABLE IF NOT EXISTS audit_log (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50),
    action VARCHAR(100) NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    ip_address INET DEFAULT '127.0.0.1'::inet,
    success BOOLEAN NOT NULL,
    details TEXT
);

-- Insert default admin user (password: admin123 - MUST BE CHANGED!)
INSERT INTO users (username, password_hash, role, password_changed) 
VALUES ('admin', '$2b$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/LGPCuU5GDGm6FHfh.', 'admin', false)
ON CONFLICT (username) DO NOTHING;

-- ===============================
-- BACKEND #2 - DATA LAYER
-- ===============================

// infrastructure/data/DatabaseManager.hpp
#pragma once
#include <pqxx/pqxx>
#include <string>
#include <memory>

namespace infrastructure::data {
    class DatabaseManager {
    public:
        DatabaseManager(const std::string& connectionString);
        ~DatabaseManager();
        
        bool initialize();
        void createTables();
        void runMigrations();
        bool isConnected();
        void closeConnection();
        
        std::unique_ptr<pqxx::work> beginTransaction();
        void commitTransaction(std::unique_ptr<pqxx::work>& txn);
        void rollbackTransaction(std::unique_ptr<pqxx::work>& txn);
        
        pqxx::connection& getConnection() { return *connection_; }
        
    private:
        std::string connectionString_;
        std::unique_ptr<pqxx::connection> connection_;
        bool connected_;
        
        void executeScriptFile(const std::string& scriptPath);
    };
}

// infrastructure/data/PostgreSqlUserRepository.hpp
#pragma once
#include "domain/repositories/IUserRepository.hpp"
#include "domain/entities/User.hpp"
#include "DatabaseManager.hpp"
#include <memory>

namespace infrastructure::data {
    class PostgreSqlUserRepository : public domain::repositories::IUserRepository {
    public:
        PostgreSqlUserRepository(std::shared_ptr<DatabaseManager> dbManager);
        
        std::optional<domain::entities::User> findByUsername(const std::string& username) override;
        std::optional<domain::entities::User> findById(int id) override;
        std::vector<domain::entities::User> findAll() override;
        bool save(const domain::entities::User& user) override;
        bool deleteByUsername(const std::string& username) override;
        bool exists(const std::string& username) override;
        bool updateFailedAttempts(const std::string& username, int attempts) override;
        bool updateLastLogin(const std::string& username) override;
        int count() override;
        
    private:
        std::shared_ptr<DatabaseManager> dbManager_;
        
        domain::entities::User mapRowToUser(const pqxx::row& row);
        domain::entities::UserRole stringToUserRole(const std::string& roleStr);
        std::string userRoleToString(domain::entities::UserRole role);
    };
}

// infrastructure/config/ConfigManager.hpp
#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace infrastructure::config {
    struct DatabaseConfig {
        std::string host;
        int port;
        std::string database;
        std::string username;
        std::string password;
        
        std::string getConnectionString() const;
    };
    
    struct ServerConfig {
        int port;
        int sessionTimeoutMinutes;
        bool enableCors;
    };
    
    struct SecurityConfig {
        int bcryptCost;
        int maxFailedAttempts;
        int lockoutDurationMinutes;
        std::string jwtSecret;
    };
    
    class ConfigManager {
    public:
        ConfigManager() = default;
        
        bool loadFromFile(const std::string& configPath);
        bool loadFromJson(const nlohmann::json& config);
        
        const DatabaseConfig& getDatabaseConfig() const { return databaseConfig_; }
        const ServerConfig& getServerConfig() const { return serverConfig_; }
        const SecurityConfig& getSecurityConfig() const { return securityConfig_; }
        
        // Convenience methods
        std::string getDatabaseConnectionString() const;
        int getServerPort() const;
        int getSessionTimeout() const;
        int getMaxFailedAttempts() const;
        int getBcryptCost() const;
        std::string getLogLevel() const;
        bool isDebugMode() const;
        
    private:
        DatabaseConfig databaseConfig_;
        ServerConfig serverConfig_;
        SecurityConfig securityConfig_;
        std::string logLevel_ = "INFO";
        bool debugMode_ = false;
        
        void setDefaults();
        void validateConfig();
    };
}

// ===============================
// DOCKER & BUILD CONFIGURATION
// ===============================

// CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(AuthServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

# Find libpqxx
pkg_check_modules(PQXX REQUIRED libpqxx)

# Find httplib
find_path(HTTPLIB_INCLUDE_DIR httplib.h)
if(NOT HTTPLIB_INCLUDE_DIR)
    message(FATAL_ERROR "httplib.h not found. Please install cpp-httplib")
endif()

# Source files
set(DOMAIN_SOURCES
    src/domain/entities/User.cpp
    src/domain/entities/UserRole.cpp
    src/domain/entities/Session.cpp
    src/domain/entities/AuditLog.cpp
)

set(APPLICATION_SOURCES
    src/application/use_cases/AuthenticateUserUseCase.cpp
    src/application/use_cases/CreateUserUseCase.cpp
    src/application/use_cases/ChangePasswordUseCase.cpp
    src/application/use_cases/DeleteUserUseCase.cpp
)

set(INFRASTRUCTURE_SOURCES
    src/infrastructure/adapters/BcryptPasswordHasher.cpp
    src/infrastructure/adapters/JwtTokenGenerator.cpp
    src/infrastructure/web/RestApiServer.cpp
    src/infrastructure/web/AuthController.cpp
    src/infrastructure/web/UserController.cpp
    src/infrastructure/data/DatabaseManager.cpp
    src/infrastructure/data/PostgreSqlUserRepository.cpp
    src/infrastructure/data/PostgreSqlAuditRepository.cpp
    src/infrastructure/config/ConfigManager.cpp
    src/infrastructure/logging/Logger.cpp
)

set(ALL_SOURCES
    src/main.cpp
    ${DOMAIN_SOURCES}
    ${APPLICATION_SOURCES}
    ${INFRASTRUCTURE_SOURCES}
)

# Create executable
add_executable(AuthServer ${ALL_SOURCES})

# Include directories
target_include_directories(AuthServer PRIVATE
    src/
    ${HTTPLIB_INCLUDE_DIR}
    ${PQXX_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(AuthServer PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    ${PQXX_LIBRARIES}
    pthread
)

# Compiler flags
target_compile_options(AuthServer PRIVATE ${PQXX_CFLAGS_OTHER})

# Install target
install(TARGETS AuthServer DESTINATION bin)

# Copy config files
install(FILES 
    config/auth_server.json 
    config/database.json 
    DESTINATION share/authserver/config
)

# docker-compose.yml
version: '3.8'

services:
  postgres:
    image: postgres:15
    container_name: auth_postgres
    environment:
      POSTGRES_DB: auth_db
      POSTGRES_USER: auth_user
      POSTGRES_PASSWORD: secure_password123
    ports:
      - "5432:5432"
    volumes:
      - postgres_data:/var/lib/postgresql/data
      - ./sql:/docker-entrypoint-initdb.d
    networks:
      - auth_network

  auth_server:
    build: .
    container_name: auth_server
    ports:
      - "8080:8080"
    depends_on:
      - postgres
    environment:
      DATABASE_URL: "postgresql://auth_user:secure_password123@postgres:5432/auth_db"
      SERVER_PORT: 8080
    volumes:
      - ./config:/app/config
      - ./logs:/app/logs
    networks:
      - auth_network

volumes:
  postgres_data:

networks:
  auth_network:
    driver: bridge

# Dockerfile
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libpqxx-dev \
    libssl-dev \
    nlohmann-json3-dev \
    libspdlog-dev \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Install cpp-httplib (header-only library)
RUN wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h \
    -O /usr/local/include/httplib.h

# Set working directory
WORKDIR /app

# Copy source code
COPY src/ ./src/
COPY CMakeLists.txt .
COPY config/ ./config/
COPY sql/ ./sql/

# Build application
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Create runtime directories
RUN mkdir -p /app/logs

# Expose port
EXPOSE 8080

# Run application
CMD ["./build/AuthServer"]

# ===============================
-- CONFIGURATION FILES
# ===============================

# config/auth_server.json
{
    "database": {
        "host": "localhost",
        "port": 5432,
        "database": "auth_db",
        "username": "auth_user",
        "password": "secure_password123"
    },
    "server": {
        "port": 8080,
        "sessionTimeoutMinutes": 30,
        "enableCors": true
    },
    "security": {
        "bcryptCost": 12,
        "maxFailedAttempts": 5,
        "lockoutDurationMinutes": 15,
        "jwtSecret": "your-256-bit-secret-key-here"
    },
    "logging": {
        "level": "INFO",
        "enableFileLogging": true,
        "logFilePath": "/app/logs/auth_server.log",
        "enableConsoleLogging": true
    },
    "debug": false
}