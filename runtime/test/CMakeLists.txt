set(_CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})

include(../cmake/ImmortalCompiler.cmake)

#.rst:
# AddTestCase
# -----------
# Utility function that helps to create a test executable.
#
# Signature::
#
#   add_test_case(<TEST_CASE_NAME> <TARGET_NAME>)
#
# The required parameters are:
#
# - ``TEST_CASE_NAME`` (input): the name of the test case
# - ``TARGET_NAME`` (output): the name of the executable target created for
# this test case. The developer can use this name to specificy ad-hoc
# requirements for the test case (e.g. main source file, additional source
# files, extra include directories and linked libraries, etc.)
function(START_TEST_CASE _TEST_CASE_NAME _TARGET_NAME)
  set(TARGET_NAME immortality-test-${_TEST_CASE_NAME})
  set(${_TARGET_NAME} ${TARGET_NAME} PARENT_SCOPE)

  add_executable(${TARGET_NAME}
    # By default we compile only the immortal versions
    EXCLUDE_FROM_ALL
    )

  if("${IMMORTALITY_PORT}" STREQUAL "msp430")
    set_target_properties(
      ${TARGET_NAME}
      PROPERTIES
      SUFFIX ".out"
      )
  endif()

  add_test(NAME ${TARGET_NAME}
    COMMAND ${TARGET_NAME}
    )
  target_link_libraries(${TARGET_NAME}
    PRIVATE
    immortality::immortality
    )
  target_compile_options(${TARGET_NAME}
    PRIVATE
    -Wall -Wextra -Wno-implicit-fallthrough
    )
  target_link_options(${TARGET_NAME}
    PRIVATE
    LINKER:-Map=${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.map
    )
  if (IMMORTALITY_PORT STREQUAL "msp430")
    target_link_options(${TARGET_NAME}
      PRIVATE
      # Specify custom linker script
      -T${_CURRENT_DIR}/../src/port/msp430/linker.ld
      )
  endif()
endfunction()
function(END_TEST_CASE _TARGET_NAME)
  cmake_parse_arguments(PARSE_ARGV 1
    "_FUNC_NAMED_PARAMETERS"
    ""
    "OPTIMIZE"
    "INSTRUMENTABLE_DIRECTORIES;EXTRA_ARGS")

  set(INSTRUMENTABLE_DIRECTORIES "")
  if (_FUNC_NAMED_PARAMETERS_INSTRUMENTABLE_DIRECTORIES)
    set(INSTRUMENTABLE_DIRECTORIES ${_FUNC_NAMED_PARAMETERS_INSTRUMENTABLE_DIRECTORIES})
  endif()

  set(EXTRA_ARGS "")
  if (_FUNC_NAMED_PARAMETERS_EXTRA_ARGS)
    set(EXTRA_ARGS ${_FUNC_NAMED_PARAMETERS_EXTRA_ARGS})
  endif()

  set(OPTIMIZE "forward-progress")
  if (_FUNC_NAMED_PARAMETERS_OPTIMIZE)
    set(OPTIMIZE ${_FUNC_NAMED_PARAMETERS_OPTIMIZE})
  endif()

  set(INFIX "immortal")

  if ("${IMMORTALITY_PORT}" STREQUAL "linux")
    set(INFIX "linux.immortal")
  elseif("${IMMORTALITY_PORT}" STREQUAL "msp430")
    set(INFIX "msp430.immortal")
    list(APPEND EXTRA_ARGS "-D__MSP430__" "-D__MSP430FR5994__")
  endif()
  add_immortal_executable(${_TARGET_NAME}
    INFIX "${INFIX}"
    EXTRA_ARGS ${EXTRA_ARGS}
    OPTIMIZE "${OPTIMIZE}"
    INSTRUMENTABLE_DIRECTORIES ${INSTRUMENTABLE_DIRECTORIES}
    )
endfunction()

add_subdirectory(benchmarks)
if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/${IMMORTALITY_PORT})
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${IMMORTALITY_PORT})
endif()
