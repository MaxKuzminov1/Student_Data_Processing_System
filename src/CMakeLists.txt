cmake_minimum_required(VERSION 3.10)
project(StudentProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим зависимости
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
find_package(nlohmann_json 3.11.2 REQUIRED)

# Добавляем поддиректории
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(test)