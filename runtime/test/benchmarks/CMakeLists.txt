if (IMMORTALITY_PORT STREQUAL "msp430" AND
    IMMORTALITY_TEST_BENCHMARK_EXECUTION_MODE STREQUAL "intermittent")
  add_definitions(-DPF_SIMULATION)
endif()
add_subdirectory(ar)
add_subdirectory(bc)
add_subdirectory(cuckoo)
add_subdirectory(dnn)
