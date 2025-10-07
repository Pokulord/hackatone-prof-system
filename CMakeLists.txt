cmake_minimum_required(VERSION 3.21)
project(AuthServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# --- DEPENDENCIES ---
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# find_package(Crow REQUIRED)
# find_package(libpqxx REQUIRED)
# find_package(OpenSSL REQUIRED)
# find_package(Bcrypt REQUIRED)
# find_package(cpp-uuid REQUIRED)

# --- PROJECT STRUCTURE ---
# Define source files for each layer of the Clean Architecture

# Domain Layer
file(GLOB_RECURSE DOMAIN_FILES
    "src/domain/entities/*.cpp"
    "src/domain/services/*.cpp"
)

# Application Layer
file(GLOB_RECURSE APPLICATION_FILES
    "src/application/use_cases/*.cpp"
    "src/application/dto/*.cpp"
)

# Infrastructure Layer
file(GLOB_RECURSE INFRASTRUCTURE_FILES
    "src/infrastructure/adapters/*.cpp"
    "src/infrastructure/data/*.cpp"
    "src/infrastructure/config/*.cpp"
    "src/infrastructure/logging/*.cpp"
    "src/infrastructure/web/*.cpp"
)

# Presentation Layer
file(GLOB_RECURSE PRESENTATION_FILES
    "src/presentation/dto/*.cpp"
)

# Main application entry point
set(MAIN_FILE src/main.cpp)

# --- EXECUTABLE TARGET ---
add_executable(AuthServer
    ${MAIN_FILE}
    ${DOMAIN_FILES}
    ${APPLICATION_FILES}
    ${INFRASTRUCTURE_FILES}
    ${PRESENTATION_FILES}
)

# --- INCLUDE DIRECTORIES ---
# Allows #include<> to find headers from our source directories
target_include_directories(AuthServer PUBLIC
    "${PROJECT_SOURCE_DIR}/src"
)

# --- LINK LIBRARIES ---
# Link the executable with the libraries found by vcpkg
target_link_libraries(AuthServer PRIVATE
    nlohmann_json::nlohmann_json
    # Crow::crow
    # libpqxx::pqxx
    # OpenSSL::SSL
    # OpenSSL::Crypto
    # bcrypt::bcrypt
    # cpp-uuid::uuid
)

# --- TESTING ---
# enable_testing()

# add_executable(AuthServerTests
#     # src/tests/AuthenticateUserUseCase.test.cpp
#     ${DOMAIN_FILES}
#     ${APPLICATION_FILES}
#     ${INFRASTRUCTURE_FILES}
# )

# target_include_directories(AuthServerTests PUBLIC
#     "${PROJECT_SOURCE_DIR}/src"
#     ${Boost_INCLUDE_DIRS}
# )

# target_link_libraries(AuthServerTests PRIVATE
#     nlohmann_json::nlohmann_json
#     Boost::unit_test_framework
# )

# add_test(NAME AuthServerTests COMMAND AuthServerTests)