/**
 * \file immortal_runtime_api.cpp
 * \brief ImmortalThreads runtime API instrumentation - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * This pass:
 *
 * * Wraps function calls with ImmortalThreads macros
 *
 * This pass assumes that:
 *
 * * All function calls resides in a separate expression statement
 */
#include "runtime_api.hpp"

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

static constexpr char IMMORTAL_FUNCTION_DEFINITION[] =
    "immortal_function_definition";
static constexpr char CALL_WITH_RETVAL_BINDING[] = "call_with_retval";
static constexpr char CALL_BINDING[] = "call";
static constexpr char LOCAL_IMMORTAL_VAR_DECL_STMT[] =
    "local_immortal_var_decl_stmt";
static constexpr char GLOBAL_IMMORTAL_VAR_DECL[] = "global_immortal_var_decl";
static constexpr char RETURN_STMT_BINDING[] = "return_stmt";

namespace imtc {
void ImmortalRuntimeApiPass::wrap_call(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  const auto *call = result.Nodes.getNodeAs<CallExpr>(CALL_BINDING);
  const auto *fn =
      result.Nodes.getNodeAs<FunctionDecl>(IMMORTAL_FUNCTION_DEFINITION);
  assert(fn);

  std::string new_source =
      get_immortal_call(*call, *fn, this->context_.get_current_CI());

  auto err = this->replace(
      *result.SourceManager,
      CharSourceRange::getTokenRange(call->getBeginLoc(), call->getEndLoc()),
      new_source);
  assert_no_error(err);
}
void ImmortalRuntimeApiPass::wrap_call_with_retval(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  const auto *assignment =
      result.Nodes.getNodeAs<BinaryOperator>(CALL_WITH_RETVAL_BINDING);
  assert(assignment);
  const auto *fn =
      result.Nodes.getNodeAs<FunctionDecl>(IMMORTAL_FUNCTION_DEFINITION);
  assert(fn);

  std::string new_source = get_immortal_call_with_retval(
      *assignment, *fn, this->context_.get_current_CI());

  auto err =
      this->replace(*result.SourceManager,
                    CharSourceRange::getTokenRange(assignment->getBeginLoc(),
                                                   assignment->getEndLoc()),
                    new_source);
  assert_no_error(err);
}

void ImmortalRuntimeApiPass::mark_def(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  auto decl = result.Nodes.getNodeAs<DeclStmt>(LOCAL_IMMORTAL_VAR_DECL_STMT);
  assert(decl);
  auto err =
      this->insert(decl->getBeginLoc(),
                   std::string(IMMORTAL_NVVAR_DECL_DEF_MACRO) + " ", false);
  assert_no_error(err);
}
void ImmortalRuntimeApiPass::mark_gdef(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  auto decl = result.Nodes.getNodeAs<VarDecl>(GLOBAL_IMMORTAL_VAR_DECL);
  assert(decl);
  auto err = this->insert(
      decl->getBeginLoc(),
      std::string(IMMORTAL_GLOBAL_NVVAR_DECL_DEF_MACRO) + " ", false);
  assert_no_error(err);
}

void ImmortalRuntimeApiPass::wrap_return(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  const auto *fn_def =
      result.Nodes.getNodeAs<FunctionDecl>(IMMORTAL_FUNCTION_DEFINITION);
  assert(fn_def);
  const auto *return_stmt =
      result.Nodes.getNodeAs<ReturnStmt>(RETURN_STMT_BINDING);
  assert(return_stmt);

  auto err = this->replace(
      result.Context->getSourceManager(),
      CharSourceRange::getTokenRange(return_stmt->getSourceRange()),
      get_immortal_function_return_stmt(*return_stmt, *fn_def,
                                        this->context_.get_current_CI()));
  assert_no_error(err);
}

static auto rhs_is_self(std::string bound_self) {
  auto reference_to_self =
      declRefExpr(to(varDecl(equalsBoundNode(bound_self))));
  return hasRHS(anyOf(reference_to_self, hasDescendant(reference_to_self)));
}

void ImmortalRuntimeApiPass::register_matchers(MatchFinder &finder) {
  auto rhs_is_immortal_fn_call =
      hasRHS(anyOf(immortal_function_call_expr(),
                   hasDescendant(immortal_function_call_expr())));

  auto immortal_function_call =
      for_each_node_inside_immortal_function_definition(
          immortal_function_call_expr(
              unless(hasAncestor(assignment_operator())))
              .bind(CALL_BINDING))
          .bind(IMMORTAL_FUNCTION_DEFINITION);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               immortal_function_call),
      this);

  auto non_volatile_write_with_retval =
      for_each_node_inside_immortal_function_definition(
          assignment_to_non_volatile_var(rhs_is_immortal_fn_call)
              .bind(CALL_WITH_RETVAL_BINDING))
          .bind(IMMORTAL_FUNCTION_DEFINITION);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               non_volatile_write_with_retval),
      this);

  auto local_immortal_var_decl =
      declStmt(hasDescendant(immortal_var_decl(
                   hasAncestor(immortal_function_definition()))))
          .bind(LOCAL_IMMORTAL_VAR_DECL_STMT);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               local_immortal_var_decl),
      this);

  auto non_marked_global_immortal_var_decl =
      immortal_var_decl(hasDeclContext(translationUnitDecl()),
                        unless(is_located_in_header_file()),
                        // those that haven't been manually marked by
                        // the developer yet
                        unless(hasAttr(clang::attr::Section)))
          .bind(GLOBAL_IMMORTAL_VAR_DECL);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               non_marked_global_immortal_var_decl),
      this);

  auto immortal_function_return_stmt =
      for_each_node_inside_immortal_function_definition(
          returnStmt().bind(RETURN_STMT_BINDING))
          .bind(IMMORTAL_FUNCTION_DEFINITION);

  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               immortal_function_return_stmt),
      this);
}

void ImmortalRuntimeApiPass::run(const MatchFinder::MatchResult &result) {
  auto bound_map = result.Nodes.getMap();
  if (bound_map.count(CALL_BINDING)) {
    this->wrap_call(result);
  }
  if (bound_map.count(CALL_WITH_RETVAL_BINDING)) {
    this->wrap_call_with_retval(result);
  }
  if (bound_map.count(GLOBAL_IMMORTAL_VAR_DECL)) {
    this->mark_gdef(result);
  }
  if (bound_map.count(LOCAL_IMMORTAL_VAR_DECL_STMT)) {
    this->mark_def(result);
  }
  if (bound_map.count(RETURN_STMT_BINDING)) {
    this->wrap_return(result);
  }
}
} // namespace imtc
