start_test_case(ar TARGET_NAME)
target_sources(${TARGET_NAME} PRIVATE main.c)
end_test_case(${TARGET_NAME}
  OPTIMIZE execution-time
  )
