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
)

add_executable(AuthServer
    src/main.cpp
    src/Application.cpp
    src/Endpoints/endpoints.cpp
    src/Endpoints/AuthEndpoints.cpp
    src/Endpoints/UserEndpoints.cpp
    src/Entities/User.cpp
    src/UseCases/UserService.cpp
    src/InterfaceAdapters/PgUserRepository.cpp
    src/jwt_utils.cpp
    src/loadenv.cpp
)

find_package(OpenSSL REQUIRED)
find_path(PQXX_INCLUDE_DIR NAMES pqxx/pqxx PATHS "C:/Users/Merrow/vcpkg/installed/x64-windows/include" NO_DEFAULT_PATH)
find_library(PQXX_LIBRARY NAMES pqxx)

target_include_directories(AuthServer PRIVATE ${PQXX_INCLUDE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(AuthServer PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    ${PQXX_LIBRARY}
    Threads::Threads
)


target_include_directories(AuthServer PRIVATE ${CMAKE_SOURCE_DIR}/jwt-cpp/include)

