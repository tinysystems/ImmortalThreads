cmake_minimum_required(VERSION 3.13)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "" FORCE)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
	set(IS_ROOT_PROJECT TRUE)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

set(IMMORTALITY_PORT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/port/${IMMORTALITY_PORT})
if (NOT DEFINED IMMORTALITY_PORT)
	message(FATAL_ERROR "IMMORTALITY_PORT is required")
elseif (NOT EXISTS ${IMMORTALITY_PORT_DIR}/portmacro.h)
	message(FATAL_ERROR "Invalid value for IMMORTALITY_PORT: ${IMMORTALITY_PORT}")
endif()

# If needed, set toolchain file for cross-compilation.
# Note that the toolchain file must be set before calling project()
if (IS_ROOT_PROJECT AND IMMORTALITY_PORT STREQUAL "msp430")
	set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/toolchains/msp430fr5994.cmake)
endif()

project(immortality)

add_compile_options(-Wall -Wextra -Wno-unused)

add_subdirectory(src)
if (IS_ROOT_PROJECT)
	add_subdirectory(test)
endif()
