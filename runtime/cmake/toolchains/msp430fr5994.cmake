# CMAKE toolchain for the MSP430FR microcontroller
#
# Adapted from: https://github.com/TUDSSL/TICS/blob/master/tics/arch/msp430x/msp430-toolchain.cmake

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR msp430)

if(NOT TOOLCHAIN_PREFIX)
	set(TOOLCHAIN_PREFIX "/opt/msp430-gcc")
	message(STATUS "No TOOLCHAIN_PREFIX specified, using default: " ${TOOLCHAIN_PREFIX})
else()
	file(TO_CMAKE_PATH "${TOOLCHAIN_PREFIX}" TOOLCHAIN_PREFIX)
endif()

set(TOOLCHAIN_BIN_DIR "${TOOLCHAIN_PREFIX}/bin")

set(CMAKE_C_COMPILER    "${TOOLCHAIN_BIN_DIR}/msp430-elf-gcc")
set(CMAKE_CXX_COMPILER  "${TOOLCHAIN_BIN_DIR}/msp430-elf-g++")
set(CMAKE_AR            "${TOOLCHAIN_BIN_DIR}/msp430-elf-ar")
set(CMAKE_LINKER        "${TOOLCHAIN_BIN_DIR}/msp430-elf-ld")
set(CMAKE_NM            "${TOOLCHAIN_BIN_DIR}/msp430-elf-nm")
set(CMAKE_OBJDUMP       "${TOOLCHAIN_BIN_DIR}/msp430-elf-objdump")
set(CMAKE_STRIP         "${TOOLCHAIN_BIN_DIR}/msp430-elf-strip")
set(CMAKE_RANLIB        "${TOOLCHAIN_BIN_DIR}/msp430-elf-ranlib")
set(CMAKE_SIZE          "${TOOLCHAIN_BIN_DIR}/msp430-elf-size")

#
# Compiler flags.
# See https://gcc.gnu.org/onlinedocs/gcc/MSP430-Options.html
# and https://www.ti.com/lit/ug/slau646f/slau646f.pdf
#

# Set MCU target
set(MCU_SPECIFIC_CFLAGS "-mmcu=msp430fr5994")
# Set type of hardware multiply supported by the target.
string(APPEND MCU_SPECIFIC_CFLAGS " -mhwmult=f5series")
# Use large-model addressing
string(APPEND MCU_SPECIFIC_CFLAGS " -mlarge")
# Toolchain header files
string(APPEND MCU_SPECIFIC_CFLAGS " -I${TOOLCHAIN_PREFIX}/include")

string(APPEND MCU_SPECIFIC_CFLAGS " -mcode-region=none")
string(APPEND MCU_SPECIFIC_CFLAGS " -mdata-region=lower")

set(CMAKE_C_FLAGS "${MCU_SPECIFIC_CFLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_DEBUG "-Og -g -gdwarf-3 -gstrict-dwarf" CACHE INTERNAL "c compiler flags debug")
set(CMAKE_C_FLAGS_RELEASE "-Os -flto -DNDEBUG" CACHE INTERNAL "c compiler flags release")

#
# Linker flags.
#
# This path contains default linker scripts
set(MCU_SPECIFIC_LINKER_FLAGS "-L${TOOLCHAIN_PREFIX}/include")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_SPECIFIC_LINKER_FLAGS} -Wl,--gc-sections -Wl,-lgcc -Wl,-lc" CACHE STRING "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
