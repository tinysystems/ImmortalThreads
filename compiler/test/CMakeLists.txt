if (NOT TARGET immortality::immortality)
	set(IMMORTALITY_PORT "linux")
	add_subdirectory(../../runtime "immortality")
endif()

file(GLOB_RECURSE TEST_INPUT_FILES input.c)
set(TARGET_NAME test_target)
add_library(${TARGET_NAME} STATIC EXCLUDE_FROM_ALL "${TEST_INPUT_FILES}")
target_link_libraries(${TARGET_NAME}
	PUBLIC
	immortality::immortality
	)
add_test(NAME e2e-test
	COMMAND pytest -vv ${CMAKE_CURRENT_SOURCE_DIR}/test.py
	WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	)
add_custom_target(e2e-test-update-snapshots
	COMMAND pytest -vv ${CMAKE_CURRENT_SOURCE_DIR}/test.py --snapshot-update
	WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	)
