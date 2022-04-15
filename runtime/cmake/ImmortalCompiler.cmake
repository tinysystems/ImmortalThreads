#.rst:
# _ImmortalInstrumentSourceFiles
# ------------------------------
# Private function
#
# Instrument all the source files of the given target
#
# The parameters are:
#
# - ``TARGET`` (input): library or executable target
# - ``NEW_TARGET`` (input): new new library or executable that will hold the instrumented sources
# - ``EXTRA_ARGS`` (input): extra arguments
# - ``INSTRUMENTABLE_DIRECTORIES`` (input): instrumentable header directories
# - ``IGNORE_SOURCES`` (input): sources to be ignored
function(_IMMORTAL_ADD_INSTRUMENTED_SOURCE_FILES _TARGET _NEW_TARGET _INFIX _EXTRA_ARGS _INSTRUMENTABLE_DIRECTORIES _IGNORE_SOURCES)
	get_target_property(SOURCE_FILES ${_TARGET} SOURCES)
	set(IGNORE_SOURCES "")
	foreach(IGNORED_SOURCE ${_IGNORE_SOURCES})
		get_source_file_property(PATH "${IGNORED_SOURCE}"
			LOCATION
			)
		list(APPEND IGNORE_SOURCES "${PATH}")
	endforeach()
	foreach(SOURCE_FILE ${SOURCE_FILES})
		get_source_file_property(FULL_PATH "${SOURCE_FILE}"
			LOCATION
			)
		if ("${FULL_PATH}" IN_LIST IGNORE_SOURCES)
			# Add the original source file to the new target
			target_sources(${_NEW_TARGET}
				PRIVATE
				${SOURCE_FILE}
				)
			continue()
		endif()
		get_filename_component(NAME ${FULL_PATH} NAME_WE)
		get_filename_component(DIRECTORY ${FULL_PATH} DIRECTORY)
		get_filename_component(EXTENSION ${FULL_PATH} EXT)
		set(INSTRUMENTED_FILE_NAME ${NAME}.${_INFIX}${EXTENSION})
		set(INSTRUMENTED_FILE_FULL_PATH ${DIRECTORY}/${NAME}.${_INFIX}${EXTENSION})

		set(DIRS_ARG_LIST "")
		list(LENGTH _INSTRUMENTABLE_DIRECTORIES DIRS_ARG_LIST_LENGTH)
		if (${DIRS_ARG_LIST_LENGTH} GREATER_EQUAL "1")
			list(JOIN _INSTRUMENTABLE_DIRECTORIES ";-H;" DIRS_ARG_LIST)
			set(DIRS_ARG_LIST "-H;${DIRS_ARG_LIST}")
		endif()

		set(EXTRA_ARG_LIST "")
		list(LENGTH _EXTRA_ARGS EXTRA_ARGS_LENGTH)
		if (${EXTRA_ARGS_LENGTH} GREATER_EQUAL "1")
			list(JOIN _EXTRA_ARGS ";--extra-arg=" EXTRA_ARG_LIST)
			set(EXTRA_ARG_LIST "--extra-arg=${EXTRA_ARG_LIST}")
		endif()

		find_program(IMMORTALC immortalc REQUIRED)
		if (NOT IMMORTALC)
			message(FATAL_ERROR "Unable to find \"immortalc\". Did you install it?")
		endif()

		# CMake doesn't allow us to express that a custom command must be
		# re-executed when some build configuration (compiler flags, include paths)
		# of the input file changes. What we do is to generate a file that contains
		# the build configuration of the input and use the generated file also as
		# dependency for the custom command. When the build configuration changes,
		# the file will be re-generated and the custom command will detect that one
		# of its dependency has changed and thus re-executed.
		#
		# Credits: QT4_CREATE_MOC_COMMAND from Qt4Macros.cmake
		file(RELATIVE_PATH RELATIVE_TO_SOURCE_DIR ${CMAKE_SOURCE_DIR} ${INSTRUMENTED_FILE_FULL_PATH})
		set(IMMORTALC_BUILD_CONFIG_FILE "${CMAKE_BINARY_DIR}/${RELATIVE_TO_SOURCE_DIR}/${INSTRUMENTED_FILE_NAME}.build_config")
		file (GENERATE
			OUTPUT ${IMMORTALC_BUILD_CONFIG_FILE}
			CONTENT "
			$<TARGET_PROPERTY:${_TARGET},LINK_LIBRARIES>
			$<TARGET_PROPERTY:${_TARGET},INTERFACE_LINK_LIBRARIES>
			$<TARGET_PROPERTY:${_TARGET},LINK_OPTIONS>
			$<TARGET_PROPERTY:${_TARGET},INTERFACE_LINK_OPTIONS>
			$<TARGET_PROPERTY:${_TARGET},INCLUDE_DIRECTORIES>
			$<TARGET_PROPERTY:${_TARGET},INTERFACE_INCLUDE_DIRECTORIES>
			$<TARGET_PROPERTY:${_TARGET},COMPILE_DEFINITIONS>
			$<TARGET_PROPERTY:${_TARGET},INTERFACE_COMPILE_DEFINITIONS>
			$<TARGET_PROPERTY:${_TARGET},INTERFACE_COMPILE_FLAGS>
			$<TARGET_PROPERTY:${_TARGET},COMPILE_FLAGS>
			$<TARGET_PROPERTY:${_TARGET},COMPILE_OPTIONS>
			$<TARGET_PROPERTY:${_TARGET},INTERFACE_COMPILE_OPTIONS>
			$<TARGET_PROPERTY:${_TARGET},COMPILE_FEATURES>
			$<TARGET_PROPERTY:${_TARGET},INTERFACE_COMPILE_FEATURES>
			"
			)

		add_custom_command(OUTPUT ${INSTRUMENTED_FILE_FULL_PATH}
			COMMAND ${IMMORTALC}
			-p "${CMAKE_BINARY_DIR}"
			--force-output
			--infix "${_INFIX}"
			"${DIRS_ARG_LIST}"
			"${EXTRA_ARG_LIST}"
			"${FULL_PATH}"
			MAIN_DEPENDENCY ${FULL_PATH}
			DEPENDS ${IMMORTALC_BUILD_CONFIG_FILE}
			IMPLICIT_DEPENDS C ${FULL_PATH}
			VERBATIM
			COMMAND_EXPAND_LISTS
			)
		target_sources(${_NEW_TARGET}
			PRIVATE
			${INSTRUMENTED_FILE_FULL_PATH}
			)
	endforeach()
endfunction()
function(_IMMORTAL_APPLY_PROPERTY _TARGET _NEW_TARGET PROPERTY)
	get_target_property(PROPERTY_VAR ${_TARGET} ${PROPERTY})
	if (PROPERTY_VAR)
		set_target_properties(${_NEW_TARGET}
			PROPERTIES
			${PROPERTY} "${PROPERTY_VAR}"
			)
	endif()
endfunction()
function(_IMMORTAL_CLONE_RELEVANT_TARGET_PROPERTIES _TARGET _NEW_TARGET)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} LINK_LIBRARIES)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} INTERFACE_LINK_LIBRARIES)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} LINK_OPTIONS)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} INTERFACE_LINK_OPTIONS)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} INCLUDE_DIRECTORIES)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} INTERFACE_INCLUDE_DIRECTORIES)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} COMPILE_DEFINITIONS)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} INTERFACE_COMPILE_DEFINITIONS)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} INTERFACE_COMPILE_FLAGS)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} COMPILE_FLAGS)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} COMPILE_OPTIONS)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} INTERFACE_COMPILE_OPTIONS)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} COMPILE_FEATURES)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} INTERFACE_COMPILE_FEATURES)
	_immortal_apply_property(${_TARGET} ${_NEW_TARGET} SUFFIX)
endfunction()

#.rst:
# AddImmortalLibrary
# ------------------
# Create an instrumented version of a static library
#
#   add_immortal_library(<MODULE_NAME> <LIBRARY_TARGET>)
#
# All the source files specified for the static library will be instrumented
# and put alongside the original files
#
# The parameters are:
#
# - ``LIBRARY_TARGET`` (input): the target name of the library target to
#   be instrumented
#
# The optional parameters are:
#
# - ``KEEP_ORIGINAL_TARGET_IN_ALL`` (input): pass this option to keep the
#   original target (that you passed) in ALL, so that it is included in the
#   default build.
# - ``INFIX`` (input): string to be put in to "${original_source_file_name}.${infix}.c".
#   Useful if you have multiple build configurations on which you are working
#   at the same time and you don't want the filename of the generated sources
#   to conflict.
# - ``INSTRUMENTABLE_DIRECTORIES`` (input): list of directories, which will be
#   passes to immortalc as `-H` parameter.
# - ``EXTRA_ARGS`` (input): list of extra arguments to be passed to immortalc.
# - ``IGNORE_SOURCES`` (input): source files that must not be instrumented
#   (e.g. a source file with all interrupt handlers)
function(ADD_IMMORTAL_LIBRARY _LIBRARY_TARGET)
	cmake_parse_arguments(PARSE_ARGV 1 "_FUNC_NAMED_PARAMETERS" "KEEP_ORIGINAL_TARGET_IN_ALL" "INFIX" "EXTRA_ARGS;INSTRUMENTABLE_DIRECTORIES;IGNORE_SOURCES")

	if (NOT _FUNC_NAMED_PARAMETERS_KEEP_ORIGINAL_TARGET_IN_ALL)
		set_target_properties(${_LIBRARY_TARGET}
			PROPERTIES
			EXCLUDE_FROM_ALL TRUE
			)
	endif()

	set(_EXTRA_ARGS "")
	if (_FUNC_NAMED_PARAMETERS_EXTRA_ARGS)
		set(_EXTRA_ARGS ${_FUNC_NAMED_PARAMETERS_EXTRA_ARGS})
	endif()

	set(_INFIX "immortal")
	if (_FUNC_NAMED_PARAMETERS_INFIX)
		set(_INFIX ${_FUNC_NAMED_PARAMETERS_INFIX})
	endif()

	set(INSTRUMENTABLE_DIRECTORIES "")
	if (_FUNC_NAMED_PARAMETERS_INSTRUMENTABLE_DIRECTORIES)
		set(INSTRUMENTABLE_DIRECTORIES ${_FUNC_NAMED_PARAMETERS_INSTRUMENTABLE_DIRECTORIES})
	endif()

	set(IGNORE_SOURCES "")
	if (_FUNC_NAMED_PARAMETERS_IGNORE_SOURCES)
		set(IGNORE_SOURCES ${_FUNC_NAMED_PARAMETERS_IGNORE_SOURCES})
	endif()

	set(TARGET_NAME ${_LIBRARY_TARGET}_immortal)
	add_library(${TARGET_NAME} STATIC "")
	_IMMORTAL_ADD_INSTRUMENTED_SOURCE_FILES(${_EXECUTABLE_TARGET} ${TARGET_NAME} "${_INFIX}" "${_EXTRA_ARGS}" "${INSTRUMENTABLE_DIRECTORIES}" "${IGNORE_SOURCES}")
	_IMMORTAL_CLONE_RELEVANT_TARGET_PROPERTIES(${_LIBRARY_TARGET} ${TARGET_NAME})
endfunction()

#.rst:
# AddImmortalExecutable
# ---------------------
#
# Same as AddImmortalLibrary, but for executable rather than library.
function(ADD_IMMORTAL_EXECUTABLE _EXECUTABLE_TARGET)
	cmake_parse_arguments(PARSE_ARGV 1 "_FUNC_NAMED_PARAMETERS" "KEEP_ORIGINAL_TARGET_IN_ALL" "INFIX;" "EXTRA_ARGS;INSTRUMENTABLE_DIRECTORIES;IGNORE_SOURCES")

	if (NOT _FUNC_NAMED_PARAMETERS_KEEP_ORIGINAL_TARGET_IN_ALL)
		set_target_properties(${_EXECUTABLE_TARGET}
			PROPERTIES
			EXCLUDE_FROM_ALL TRUE
			)
	endif()

	set(_EXTRA_ARGS "")
	if (_FUNC_NAMED_PARAMETERS_EXTRA_ARGS)
		set(_EXTRA_ARGS ${_FUNC_NAMED_PARAMETERS_EXTRA_ARGS})
	endif()

	set(_INFIX "immortal")
	if (_FUNC_NAMED_PARAMETERS_INFIX)
		set(_INFIX ${_FUNC_NAMED_PARAMETERS_INFIX})
	endif()

	set(INSTRUMENTABLE_DIRECTORIES "")
	if (_FUNC_NAMED_PARAMETERS_INSTRUMENTABLE_DIRECTORIES)
		set(INSTRUMENTABLE_DIRECTORIES ${_FUNC_NAMED_PARAMETERS_INSTRUMENTABLE_DIRECTORIES})
	endif()

	set(IGNORE_SOURCES "")
	if (_FUNC_NAMED_PARAMETERS_IGNORE_SOURCES)
		set(IGNORE_SOURCES ${_FUNC_NAMED_PARAMETERS_IGNORE_SOURCES})
	endif()

	set(TARGET_NAME ${_EXECUTABLE_TARGET}_immortal)
	add_executable(${TARGET_NAME})
	_IMMORTAL_ADD_INSTRUMENTED_SOURCE_FILES(${_EXECUTABLE_TARGET} ${TARGET_NAME} "${_INFIX}" "${_EXTRA_ARGS}" "${INSTRUMENTABLE_DIRECTORIES}" "${IGNORE_SOURCES}")
	_IMMORTAL_CLONE_RELEVANT_TARGET_PROPERTIES(${_EXECUTABLE_TARGET} ${TARGET_NAME})
endfunction()
