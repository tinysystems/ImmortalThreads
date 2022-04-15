cmake_minimum_required(VERSION 3.0.0)

# Generate compile_commands.json, which is also required by immortalc
# when performing code instrumenting
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "")

project(immortalc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_subdirectory(src)
enable_testing()
add_subdirectory(test)
