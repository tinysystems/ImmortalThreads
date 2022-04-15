find_package(LLVM 10.0.0 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

find_package(fmt)

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

SET(BIN_NAME ${PROJECT_NAME})

add_executable("${BIN_NAME}"
	main.cpp
	context.cpp
	pass.cpp
	passes/macro_expansion.cpp
	passes/function_copy.cpp
	passes/initializer_decomposition.cpp
	passes/runtime_api.cpp
	passes/immortal_function.cpp
	passes/self_write_operators.cpp
	passes/control_stmt.cpp
	passes/shim_api_replacement.cpp
	passes/switch_transformation.cpp
	passes/expression_decomposition.cpp
	passes/force_braces.cpp
	passes/ternary_decomposition.cpp
	matchers/matchers.cpp
	utils/immortal.cpp
	utils/expr.cpp
	utils/location.cpp
	utils/ast.cpp
	utils/error.cpp
	)
target_include_directories(${BIN_NAME}
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
	)
install(TARGETS "${BIN_NAME}" RUNTIME DESTINATION bin)

target_link_libraries(${BIN_NAME} PRIVATE fmt::fmt)
target_link_libraries(${BIN_NAME}
	PRIVATE
	-Wl,--start-group
	LLVM
	clangAST
	clangASTMatchers
	clangAnalysis
	clangBasic
	clangDriver
	clangEdit
	clangFrontend
	clangFrontendTool
	clangLex
	clangParse
	clangSema
	clangEdit
	clangRewrite
	clangRewriteFrontend
	clangStaticAnalyzerFrontend
	clangStaticAnalyzerCheckers
	clangStaticAnalyzerCore
	clangCrossTU
	clangIndex
	clangSerialization
	clangToolingCore
	clangToolingInclusions
	clangToolingRefactoring
	clangTooling
	clangFormat
	-Wl,--end-group
	)
target_compile_features(${BIN_NAME}
	PRIVATE
	cxx_std_17
	)
