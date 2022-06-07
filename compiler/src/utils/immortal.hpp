/**
 * \file immortal.hpp
 * \brief Utility functions for working with ImmortalThreads's runtime interface
 * - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#ifndef IMMORTALC_UTILTS_IMMORTAL_HPP_
#define IMMORTALC_UTILTS_IMMORTAL_HPP_

#include <string>

#include "clang/AST/AST.h"
#include "clang/Frontend/CompilerInstance.h"

namespace imtc::utils {

constexpr const char *IMMORTAL_NVVAR_DECL_DEF_MACRO = "_def";
constexpr const char *IMMORTAL_GLOBAL_NVVAR_DECL_DEF_MACRO = "_gdef";

/**
 * Return the macro APIs used provided by the ImmortalThreads runtime
 */
std::vector<std::string> get_runtime_macros();

/**
 * Given a function declaration return the macro-wrapped immortal function
 * declaration.
 *
 * \param [in] fn the immortal function in question
 */
std::string
get_immortal_function_declaration(const clang::FunctionDecl &fn,
                                  const clang::SourceManager &manager,
                                  const clang::LangOptions &opts);

/**
 * Given a function definition (so there is the function body), return the
 * macro call that defines the metadata block for the function
 *
 * \param [in] fn the immortal function in question
 * \param [in] max_instances_num maximum number of instances of the function
 */
std::string get_immortal_function_metadata_definition(
    const clang::FunctionDecl &fn, size_t max_instances_num,
    const clang::SourceManager &manager, const clang::LangOptions &opts);

/**
 * Given an assignment expression to a non-volatile variable, return
 * the instrumented assignment.
 *
 *
 * \returns the instrumented assignment
 */
std::string get_immortal_write(const clang::BinaryOperator &assignment,
                               const clang::FunctionDecl &immortal_function,
                               const clang::CompilerInstance &ci);

/**
 * Given an assignment expression to a non-volatile variable, where the
 * variable (or its potential alias) appears also on the RHS, return the
 * instrumented assignment.
 *
 * \returns the instrumented assignment
 */
std::string
get_immortal_write_self(const clang::BinaryOperator &assignment,
                        const clang::FunctionDecl &immortal_function,
                        const clang::CompilerInstance &ci);

/**
 * Given multiple consecutive assignment expressions to non-volatile variables,
 * where the variable (or its potential alias) appears also on the RHS, return
 * the instrumented assignments.
 *
 * \returns the instrumented assignments
 */
std::string get_immortal_coalesced_write_selfs(
    const std::vector<const clang::BinaryOperator *> assignments,
    const clang::FunctionDecl &immortal_function,
    const clang::CompilerInstance &ci);

/**
 * Given an assignment expression to a non-volatile variable, where the
 * RHS is a call expression, return the instrumented assignment
 *
 * \returns the instrumented assignment
 */
std::string
get_immortal_call_with_retval(const clang::BinaryOperator &assignment,
                              const clang::FunctionDecl &immortal_function,
                              const clang::CompilerInstance &ci);

/**
 * Given a call expression to an immortal function, return the instrumented call
 *
 * \returns the instrumented assignment
 */
std::string get_immortal_call(const clang::CallExpr &call_expr,
                              const clang::FunctionDecl &immortal_function,
                              const clang::CompilerInstance &ci);

/**
 * Given a return statement inside an immortal function, return the instrumented
 * return statement.
 *
 * \param [in] ret_stmt the return statement to be instrumented
 * \param [in] immortal_function the immortal function in which the return
 * statement resides
 *
 * \returns the instrumented return statement
 */
std::string
get_immortal_function_return_stmt(const clang::ReturnStmt &ret_stmt,
                                  const clang::FunctionDecl &immortal_function,
                                  const clang::CompilerInstance &ci);

/**
 * Given an immortal function, return the begin macro that should be placed at
 * the start of the function
 */
std::string get_immortal_function_begin_macro_call(
    const clang::FunctionDecl &immortal_function,
    const clang::CompilerInstance &ci);

/**
 * Given an immortal function, return the begin macro that should be placed at
 * the end of the function
 */
std::string get_immortal_function_end_macro_call(
    const clang::FunctionDecl &immortal_function,
    const clang::CompilerInstance &ci);

/**
 * Given an call to the imt_thread_init shim API, get the correct call
 * expression to the true runtime API.
 *
 * \returns the correct call
 */
std::string get_true_imt_thread_init(
    const clang::CallExpr &imt_thread_init_call, bool is_multi_instance_thread,
    const clang::SourceManager &manager, const clang::LangOptions &opts);

/**
 * Given an immortal function, return its maximum number of instances.
 */
size_t get_immortal_function_instances(const clang::FunctionDecl &fn_decl,
                                       const clang::CompilerInstance &ci);

/**
 * Given an immortal function, return whether potential function copying should
 * be performed via whole body copy (whether the application developer asked to
 * do so)
 */
bool immortal_function_do_whole_body_copy(const clang::FunctionDecl &fn_decl,
                                          const clang::CompilerInstance &ci);

} // namespace imtc::utils

#endif /* ifndef IMMORTALC_UTILTS_IMMORTAL_HPP_ */
