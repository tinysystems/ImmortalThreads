/**
 * \file naive.cpp
 * \brief Naively instrument all write accesses - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * This pass:
 *
 * * Wraps memory write accesses with ImmortalThreads macros
 *
 * This pass assumes that:
 *
 * * All memory write accesses are under the form of simple assignment
 * expressions and each such expression resides in a separate expression
 * statement
 */
#include "naive.hpp"

#include "matchers/matchers.hpp"
#include "utils/error.hpp"
#include "utils/immortal.hpp"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"

using namespace imtc::ast_matchers;
using namespace imtc::utils;

using namespace clang;
using namespace clang::ast_matchers;

static constexpr char WR_BINDING[] = "wr";
static constexpr char WR_SELF_BINDING[] = "wr_self";
static constexpr char IMMORTAL_FUNCTION_DEFINITION[] = "immortal_function_body";

namespace imtc {
void NaiveMemWriteCheckpointMacrosInsertionPass::wrap_wr_macro(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  const auto *assignment = result.Nodes.getNodeAs<BinaryOperator>(WR_BINDING);
  assert(assignment);

  const auto *fn =
      result.Nodes.getNodeAs<FunctionDecl>(IMMORTAL_FUNCTION_DEFINITION);
  assert(fn);

  std::string new_source =
      get_immortal_write(*assignment, *fn, this->context_.get_current_CI());

  auto err =
      this->replace(*result.SourceManager,
                    CharSourceRange::getTokenRange(assignment->getBeginLoc(),
                                                   assignment->getEndLoc()),
                    new_source);
  assert_no_error(err);
}
void NaiveMemWriteCheckpointMacrosInsertionPass::wrap_wr_self_macro(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  const auto *assignment =
      result.Nodes.getNodeAs<BinaryOperator>(WR_SELF_BINDING);
  assert(assignment);
  const auto *fn =
      result.Nodes.getNodeAs<FunctionDecl>(IMMORTAL_FUNCTION_DEFINITION);
  assert(fn);

  std::string new_source = get_immortal_write_self(
      *assignment, *fn, this->context_.get_current_CI());

  auto err =
      this->replace(*result.SourceManager,
                    CharSourceRange::getTokenRange(assignment->getBeginLoc(),
                                                   assignment->getEndLoc()),
                    new_source);
}

static auto rhs_is_self(std::string bound_self) {
  auto reference_to_self =
      declRefExpr(to(varDecl(equalsBoundNode(bound_self))));
  return hasRHS(anyOf(reference_to_self, hasDescendant(reference_to_self)));
}

void NaiveMemWriteCheckpointMacrosInsertionPass::register_matchers(
    MatchFinder &finder) {
  auto rhs_is_immortal_fn_call =
      hasRHS(anyOf(immortal_function_call_expr(),
                   hasDescendant(immortal_function_call_expr())));

  auto non_volatile_write =
      for_each_node_inside_immortal_function_definition(
          assignment_to_non_volatile_var(
              allOf(anything(),
                    unless(rhs_is_self(NON_VOLATILE_VAR_DECL_BINDING)),
                    unless(rhs_is_immortal_fn_call)))
              .bind(WR_BINDING))
          .bind(IMMORTAL_FUNCTION_DEFINITION);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               non_volatile_write),
      this);

  auto non_volatile_self_write =
      for_each_node_inside_immortal_function_definition(
          assignment_to_non_volatile_var(
              allOf(rhs_is_self(NON_VOLATILE_VAR_DECL_BINDING),
                    unless(hasRHS(immortal_function_call_expr()))))
              .bind(WR_SELF_BINDING))
          .bind(IMMORTAL_FUNCTION_DEFINITION);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               non_volatile_self_write),
      this);
}

void NaiveMemWriteCheckpointMacrosInsertionPass::run(
    const MatchFinder::MatchResult &result) {
  auto bound_map = result.Nodes.getMap();
  if (bound_map.count(WR_BINDING)) {
    this->wrap_wr_macro(result);
  }
  if (bound_map.count(WR_SELF_BINDING)) {
    this->wrap_wr_self_macro(result);
  }
}
} // namespace imtc
