start_test_case(dnn TARGET_NAME)
target_sources(${TARGET_NAME}
	PRIVATE
	dnn.c
	dnn_calc.c
	fixed.c
	main.c
	mat.c
	)
target_include_directories(${TARGET_NAME}
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
	)
end_test_case(${TARGET_NAME}
	INSTRUMENTABLE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/includes/dnnlib"
	)
