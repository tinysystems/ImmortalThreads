start_test_case(bc TARGET_NAME)
target_sources(${TARGET_NAME} PRIVATE main.c)
end_test_case(${TARGET_NAME})
