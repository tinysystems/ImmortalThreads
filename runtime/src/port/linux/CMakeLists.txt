target_sources(${TARGET_NAME}
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/port.c
	${CMAKE_CURRENT_LIST_DIR}/port_special.c
	)
target_link_libraries(${TARGET_NAME}
	PRIVATE
	pthread
	)
