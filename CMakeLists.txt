cmake_minimum_required(VERSION 3.0)

project(
    sqcli
    DESCRIPTION "CLI utility that allows you to work with the SQLite DBMS"
    HOMEPAGE_URL "https://github.com/dan4ik605743/SQCli"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(sqcli)
add_subdirectory(modules)
