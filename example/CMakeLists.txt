cmake_minimum_required(VERSION 3.10)

project(hello LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(hello main.cpp)
set_target_properties(hello PROPERTIES CXX_STANDARD 20)
target_compile_options(hello PRIVATE -Wall -Wextra -g)
