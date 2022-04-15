set(TARGET_NAME ${PROJECT_NAME})

add_library(${TARGET_NAME} STATIC "")
add_library(${PROJECT_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})
add_subdirectory(${IMMORTALITY_PORT_DIR})

if (EXISTS ${IMMORTALITY_PORT_DIR}/port_special.h)
	target_compile_definitions(${TARGET_NAME}
		PUBLIC
		IMMORTALITY_PORT_SPECIAL_AVAILABLE
		)
endif()

string(MAKE_C_IDENTIFIER "${IMMORTALITY_PORT}" PORT)
string(TOUPPER "${PORT}" PORT_UPPER)
target_compile_definitions(${TARGET_NAME}
	PUBLIC
	"IMMORTALITY_PORT_${PORT_UPPER}"
	)

target_sources(${TARGET_NAME}
	PRIVATE
	sched.c
	thread.c
	)

target_include_directories(${TARGET_NAME}
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	${IMMORTALITY_PORT_DIR}
	)
target_compile_features(${TARGET_NAME}
	PRIVATE
	c_std_99
	)
