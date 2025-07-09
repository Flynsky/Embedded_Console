cmake_minimum_required(VERSION 3.16)
project(ConsoleLib LANGUAGES CXX)

# Compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the actual library (static)
add_library(embedded_console STATIC
    src/Console.cpp
)

# Set public headers so other projects can use them
target_include_directories(embedded_console
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(embedded_console)
