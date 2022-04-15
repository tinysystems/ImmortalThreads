start_test_case(basic-1 TARGET_NAME)
target_sources(${TARGET_NAME} PRIVATE main.c)
target_compile_definitions(${TARGET_NAME} PRIVATE
  -DIMMORTALITY_MAX_THREAD_COUNT=2
  )
end_test_case(${TARGET_NAME})
