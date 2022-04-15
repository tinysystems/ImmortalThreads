/**
 * \file self_write_operators.cpp
 * \brief Handle "self write" operators such as "++", "+=", etc.
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#include "self_write_operators.hpp"
#include "matchers/matchers.hpp"
#include "utils/error.hpp"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/Lex/Lexer.h"

using namespace imtc::ast_matchers;
using namespace imtc::utils;

using namespace clang;
using namespace clang::ast_matchers;

static constexpr char UNARY_OPERATOR_BINDING[] = "unary_operator_binding";
static constexpr char COMPOUND_OPERATOR_BINDING[] =
    "compound_assignment_operator_binding";

namespace imtc {
void SelfWritePass::handle_unary_operator(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {

  const auto *op =
      result.Nodes.getNodeAs<UnaryOperator>(UNARY_OPERATOR_BINDING);
  std::string op_str;
  switch (op->getOpcode()) {
  case UnaryOperator::Opcode::UO_PreInc:
    [[fallthrough]];
  case UnaryOperator::Opcode::UO_PostInc:
    op_str = "+";
    break;
  case UnaryOperator::Opcode::UO_PreDec:
    [[fallthrough]];
  case UnaryOperator::Opcode::UO_PostDec:
    op_str = "-";
    break;
  default:
    llvm_unreachable("Unexpected unary operator");
  }
  std::string operand = Lexer::getSourceText(
      CharSourceRange::getTokenRange(op->getSubExpr()->getSourceRange()),
      *result.SourceManager, result.Context->getLangOpts());

  std::string new_source = operand + " = " + operand + " " + op_str + " 1";
  auto err = this->replace(
      *result.SourceManager,
      CharSourceRange::getTokenRange(op->getBeginLoc(), op->getEndLoc()),
      new_source);
  assert_no_error(err);
}
void SelfWritePass::handle_compound_operator(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {

  const auto *op =
      result.Nodes.getNodeAs<BinaryOperator>(COMPOUND_OPERATOR_BINDING);
  assert(op->isCompoundAssignmentOp() && "Unexpected operator");
  std::string op_str = op->getOpcodeStr();
  // remove the last '=' of the compound assignment operator
  op_str.pop_back();

  std::string lhs = Lexer::getSourceText(
      CharSourceRange::getTokenRange(op->getLHS()->getSourceRange()),
      *result.SourceManager, result.Context->getLangOpts());
  std::string rhs = Lexer::getSourceText(
      CharSourceRange::getTokenRange(op->getRHS()->getSourceRange()),
      *result.SourceManager, result.Context->getLangOpts());

  std::string new_source = lhs + " = " + lhs + " " + op_str + " " + rhs;
  auto err = this->replace(
      *result.SourceManager,
      CharSourceRange::getTokenRange(op->getBeginLoc(), op->getEndLoc()),
      new_source);
  assert_no_error(err);
}

void SelfWritePass::register_matchers(MatchFinder &finder) {
  finder.addMatcher(
      for_each_node_inside_immortal_function_definition(
          unaryOperator(anyOf(hasOperatorName("++"), hasOperatorName("--")),
                        hasDescendant(non_volatile_var_reference()))
              .bind(UNARY_OPERATOR_BINDING)),
      this);

  /*
   * FIXME: hasDescendant() should be hasLHS(hasDescendant())
   * Is this really relevant? Define a test case if it is.
   */
  finder.addMatcher(
      for_each_node_inside_immortal_function_definition(
          binaryOperator(isAssignmentOperator(), unless(hasOperatorName("=")),
                         hasDescendant(non_volatile_var_reference()))
              .bind(COMPOUND_OPERATOR_BINDING)),
      this);
}

void SelfWritePass::run(const MatchFinder::MatchResult &result) {
  auto bound_map = result.Nodes.getMap();
  if (bound_map.count(UNARY_OPERATOR_BINDING)) {
    this->handle_unary_operator(result);
  }
  if (bound_map.count(COMPOUND_OPERATOR_BINDING)) {
    this->handle_compound_operator(result);
  }
}
} // namespace imtc
