cmake_minimum_required(VERSION 3.15)
project(AuthServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
    src
    src/Entities
    src/UseCases
    src/InterfaceAdapters
    src/thirdparty
    src/Logger
)

add_executable(AuthServer
    src/main.cpp
    src/Application.cpp
    src/Endpoints/endpoints.cpp
    src/Endpoints/AuthEndpoints.cpp
    src/Endpoints/UserEndpoints.cpp
    src/Endpoints/MonitoringEndpoints.cpp
    src/Endpoints/RoleEndpoints.cpp
    src/Entities/User.cpp
    src/Entities/Role.cpp
    src/UseCases/UserService.cpp
    src/UseCases/RoleService.cpp
    src/InterfaceAdapters/PgUserRepository.cpp
    src/InterfaceAdapters/PgRoleRepository.cpp
    src/InterfaceAdapters/RefreshTokenService.cpp
    src/jwt_utils.cpp
    src/loadenv.cpp
    src/Migrations/MigrationService.cpp
    src/Logger/Logger.cpp
)

find_package(OpenSSL REQUIRED)
find_library(PQXX_LIBRARY NAMES pqxx)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

target_include_directories(AuthServer PRIVATE ${PQXX_INCLUDE_DIRS})

find_package(Threads REQUIRED)
target_link_libraries(AuthServer PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    ${PQXX_LIBRARIES}
    Threads::Threads
)


target_include_directories(AuthServer PRIVATE ${CMAKE_SOURCE_DIR}/jwt-cpp/include)

