cmake_minimum_required(VERSION 3.21)
project(MyApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указать путь к исходникам
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(MyApp ${SOURCES})

# Найти пакеты через find_package с поддержкой vcpkg
find_package(OpenSSL REQUIRED)
find_package(httplib REQUIRED)

target_link_libraries(MyApp PRIVATE OpenSSL::SSL OpenSSL::Crypto httplib::httplib)
