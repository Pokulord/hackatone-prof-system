cmake_minimum_required(VERSION 3.15)
project(AuthServer)

set(CMAKE_CXX_STANDARD 17)
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
if(OpenSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    target_link_libraries(AuthServer OpenSSL::SSL OpenSSL::Crypto)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx)

include_directories(${LIBPQXX_INCLUDE_DIRS})
link_directories(${LIBPQXX_LIBRARY_DIRS})

target_link_libraries(AuthServer ${LIBPQXX_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(AuthServer Threads::Threads)

target_include_directories(AuthServer PRIVATE ${CMAKE_SOURCE_DIR}/jwt-cpp/include)


