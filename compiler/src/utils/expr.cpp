/**
 * \file expr.cpp
 * \brief Common utility related to expressions - implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#include "expr.hpp"

#include "fmt/core.h"
#include "immortal.hpp"

#include "clang/Lex/Lexer.h"

using namespace clang;
using namespace clang::ast_matchers;

namespace imtc::utils {

bool does_expr_create_tmp_var(const Expr *e) {
  if (auto expr = dyn_cast<CallExpr>(e)) {
    auto type = expr->getCalleeDecl()->getAsFunction()->getReturnType();
    return !type->isVoidType();
  }
  if (auto expr = dyn_cast<UnaryOperator>(e)) {
    return expr->isPostfix();
  }
  return false;
}

std::string expr_tmp_var_assignment(const Expr *e,
                                    const std::string &expr_content,
                                    const std::string &tmp_var_name,
                                    const clang::Rewriter &rewriter) {
  if (auto expr = dyn_cast<UnaryOperator>(e)) {
    assert(expr->isPostfix());
    std::string operand = Lexer::getSourceText(
        CharSourceRange::getTokenRange(expr->getSubExpr()->getSourceRange()),
        rewriter.getSourceMgr(), rewriter.getLangOpts());
    std::string type = get_type_of_the_expression(e).getAsString();
    return fmt::format("{} {};\n{} = {};\n{}", type, tmp_var_name, tmp_var_name,
                       operand, expr_content);
  }
  if (dyn_cast<CallExpr>(e)) {
    std::string type = get_type_of_the_expression(e).getAsString();
    return fmt::format("{} {};\n{} = {}", type, tmp_var_name, tmp_var_name,
                       expr_content);
  }
  assert(0);
}

bool does_expr_have_value(const clang::Expr *e) {
  if (auto expr = dyn_cast<CallExpr>(e)) {
    auto type = expr->getCalleeDecl()->getAsFunction()->getReturnType();
    return !type->isVoidType();
  }
  return true;
}

clang::Expr *get_value_of_the_expression(const clang::Expr *e) {
  if (auto expr = dyn_cast<UnaryOperator>(e)) {
    assert(expr->isPrefix());
    return expr->getSubExpr();
  }
  if (auto expr = dyn_cast<BinaryOperator>(e)) {
    if (expr->isCompoundAssignmentOp() || expr->isAssignmentOp()) {
      return expr->getLHS();
    }
  }
  assert(false && "Unexpected expression");
}

clang::QualType get_type_of_the_expression(const clang::Expr *e) {
  if (auto expr = dyn_cast<UnaryOperator>(e)) {
    return expr->getSubExpr()->getType();
  }
  if (auto expr = dyn_cast<BinaryOperator>(e)) {
    if (expr->isCompoundAssignmentOp() || expr->isAssignmentOp()) {
      return expr->getLHS()->getType();
    }
  }
  if (auto expr = dyn_cast<CallExpr>(e)) {
    return expr->getCalleeDecl()->getFunctionType()->getReturnType();
  }
  assert(false && "Unexpected expression");
}

} // namespace imtc::utils
