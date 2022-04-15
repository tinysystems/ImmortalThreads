cmake_minimum_required(VERSION 3.16)

set(IMMORTALTHREADS_RUNTIME_DIR ${CMAKE_CURRENT_LIST_DIR}/../../runtime)
set(IMMORTALTHREADS_COMPILER_DIR ${CMAKE_CURRENT_LIST_DIR}/../../compiler)

set(CMAKE_TOOLCHAIN_FILE ${IMMORTALTHREADS_RUNTIME_DIR}/cmake/toolchains/msp430fr5994.cmake)
project(sem_example)

set(IMMORTALITY_PORT "msp430")
add_subdirectory(../../runtime "runtime")

add_executable(sem_example
  main.c
  th1.c
  th2.c
  )
target_link_libraries(sem_example
  PRIVATE
  immortality::immortality
)
target_link_options(sem_example
  PRIVATE
  # Specify custom linker script
  -T${IMMORTALTHREADS_RUNTIME_DIR}/src/port/msp430/linker.ld
  )
set_target_properties(
  sem_example
  PROPERTIES
  SUFFIX ".out"
  )

include(${IMMORTALTHREADS_RUNTIME_DIR}/cmake/ImmortalCompiler.cmake)
add_immortal_executable(
  sem_example
  EXTRA_ARGS -D__MSP430__ -D__MSP430FR5994__
  )
