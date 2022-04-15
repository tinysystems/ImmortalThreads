/**
 * \file shim_api_replacement.cpp
 * \brief Replace shim APIs with true APIs - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#include "shim_api_replacement.hpp"

#include "matchers/matchers.hpp"
#include "utils/error.hpp"
#include "utils/immortal.hpp"

#include "fmt/core.h"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/Lex/Lexer.h"

using namespace imtc;
using namespace imtc::ast_matchers;
using namespace imtc::utils;

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

static constexpr char IMT_THREAD_INIT_WRONG_CALL[] =
    "imt_thread_init_wrong_call";
static constexpr char IMT_THREAD_INIT_CORRECT_CALL[] =
    "imt_thread_init_correct_call";
static constexpr char REFERENCED_IMMORTAL_THREAD_DECL[] =
    "referenced_immortal_thread_decl";
static constexpr char SIMPLE_SHIM_API_CALL[] = "simple_shim_api_call";

static const char *SIMPLE_SHIM_APIS[] = {
    "CHECKPOINT",

    // mutex
    "ENTER",
    "LOCK_INIT",
    "LEAVE",

    // event
    "EVENT_INIT",
    "EVENT_SET_TIMESTAMP",
    "EVENT_GET_BUFFER",
    "EVENT_SIGNAL",
    "EVENT_GET_TIMESTAMP",
    "EVENT_WAIT_EXPIRES",
    "EVENT_WAIT",

    // semaphore
    "SEM_INIT",
    "SEM_WAIT",
    "SEM_POST_ISR",
    "SEM_POST",
};

void ShimApiReplacementPass::register_matchers(MatchFinder &finder) {
  //
  // Matchers for handle_shim_api_imt_thread_init
  //
  auto immortal_thread_reference =
      declRefExpr(to(functionDecl(annotated_with_immortalc_fn_thread())
                         .bind(REFERENCED_IMMORTAL_THREAD_DECL)));
  auto immortal_thread_reference_argument =
      hasArgument(1, immortal_thread_reference);
  auto wrong_imt_thread_init_invocation =
      callExpr(allOf(callee(functionDecl(hasName("imt_thread_init"))),
                     unless(immortal_thread_reference_argument)))
          .bind(IMT_THREAD_INIT_WRONG_CALL);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               wrong_imt_thread_init_invocation),
      this);

  auto correct_imt_thread_init_invocation =
      callExpr(allOf(callee(functionDecl(hasName("imt_thread_init"))),
                     immortal_thread_reference_argument))
          .bind(IMT_THREAD_INIT_CORRECT_CALL);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               correct_imt_thread_init_invocation),
      this);

  for (auto simple_shim_api : SIMPLE_SHIM_APIS) {
    auto checkpoint_invocation =
        callExpr(callee(functionDecl(hasName(simple_shim_api))))
            .bind(SIMPLE_SHIM_API_CALL);
    finder.addMatcher(
        traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
                 checkpoint_invocation),
        this);
  }
}

void ShimApiReplacementPass::run(const MatchFinder::MatchResult &result) {
  if (result.Nodes.getNodeAs<FunctionDecl>(IMT_THREAD_INIT_WRONG_CALL)) {
    assert(0);
  }

  if (auto imt_thread = result.Nodes.getNodeAs<FunctionDecl>(
          REFERENCED_IMMORTAL_THREAD_DECL)) {

    assert(imt_thread);
    size_t num_instances = get_immortal_function_instances(
        *imt_thread, this->context_.get_current_CI());
    assert(num_instances >= 1);

    auto call_expr =
        result.Nodes.getNodeAs<CallExpr>(IMT_THREAD_INIT_CORRECT_CALL);
    assert(call_expr);

    auto true_call = get_true_imt_thread_init(*call_expr, num_instances > 1,
                                              *result.SourceManager,
                                              result.Context->getLangOpts());

    auto err = this->replace(
        CharSourceRange::getTokenRange(call_expr->getSourceRange()), true_call);
    assert_no_error(err);
  }
}

void SimpleShimApiReplacementPass::register_matchers(MatchFinder &finder) {
  for (auto simple_shim_api : SIMPLE_SHIM_APIS) {
    auto checkpoint_invocation =
        callExpr(callee(functionDecl(hasName(simple_shim_api))))
            .bind(SIMPLE_SHIM_API_CALL);
    finder.addMatcher(
        traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
                 checkpoint_invocation),
        this);
  }
}

void SimpleShimApiReplacementPass::run(const MatchFinder::MatchResult &result) {
  if (auto call_expr = result.Nodes.getNodeAs<CallExpr>(SIMPLE_SHIM_API_CALL)) {
    auto loc = result.SourceManager->getSpellingLoc(call_expr->getBeginLoc());
    auto err = this->insert(loc, "_", false);
    assert_no_error(err);
  }
}
