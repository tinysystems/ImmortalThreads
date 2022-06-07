/**
 * \file expr.hpp
 * \brief Common utility related to expressions - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#ifndef IMMORTALC_EXPR_HPP_
#define IMMORTALC_EXPR_HPP_

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include <string>
#include <vector>

namespace imtc::utils {

/**
 * Checkes whether an expression that needs to be instrumented creates a
 * temporary value.
 * Examples:
 *
 * * compound assignment expressions don't create temporary values.
 * * postfix increment expressions create temporary values
 * * function call expressions with return values create temporary values
 */
bool does_expr_create_tmp_var(const clang::Expr *expr);
/**
 * Given an expression that creates temporary variables, return a valid C
 * snippet in which the temporary variable is defined and assigned.
 *
 * \param [in] expr the expression
 * \param [in] expr_content the content of the expression. This function could
 * obtain the original content of the expression from \p rewriter, but it may
 * be possible that the developer wants to use a modifed expression content to
 * construct the tmp var assignment snippet. Thus we ask the expression content
 * to be passed.
 * \param [in] tmp_var_name the name to be used for the defined temporary
 * variable
 */
std::string expr_tmp_var_assignment(const clang::Expr *expr,
                                    const std::string &expr_content,
                                    const std::string &tmp_var_name,
                                    const clang::Rewriter &rewriter);
/**
 * Checkes whether an expression that needs to be instrumented "carries" a
 * value.
 *
 * AFAICT this should return false only for function calls where there is no
 * return value (void functions).
 */
bool does_expr_have_value(const clang::Expr *expr);
/**
 * Given an expression that does not create temporary values (e.g. prefix
 * increment, compound assignments, etc), return the node that carries the value
 * after the expression is evaluated.
 */
clang::Expr *get_value_of_the_expression(const clang::Expr *expr);
/**
 * Given an expression that "carries" a value, return the type of such value.
 */
clang::QualType get_type_of_the_expression(const clang::Expr *expr);
/**
 * Whether the given expression is a l-value and is a pointer dereference
 */
bool is_pointer_dereference_lvalue(const clang::Expr *expr);

} // namespace imtc::utils

#endif /* ifndef IMMORTALC_EXPR_HPP_ */
