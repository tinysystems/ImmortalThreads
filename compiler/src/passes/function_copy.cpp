/**
 * \file function_copy.cpp
 * \brief Instrument multi-instance immortal functions - implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#include "function_copy.hpp"

#include "matchers/matchers.hpp"
#include "utils/error.hpp"
#include "utils/immortal.hpp"
#include "utils/location.hpp"

#include "fmt/core.h"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/Lex/Lexer.h"

using namespace imtc;
using namespace imtc::ast_matchers;
using namespace imtc::utils;

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

static constexpr char MULTI_INSTANCE_FUNCTION_COPY_VIA_DATA_COPY[] =
    "multi_instance_function_copy_via_data_copy";
static constexpr char VAR_DECL_STMT[] = "var_decl_stmt";
static constexpr char VAR_REF[] = "var_ref_stmt";

void FunctionCopyPass::register_matchers(MatchFinder &finder) {
  auto local_immortal_var_ref =
      declRefExpr(to(varDecl(immortal_var_decl(hasAncestor(compoundStmt())),
                             unless(parmVarDecl()))))
          .bind(VAR_REF);
  auto data_copy_multi_instance_function_copy =
      immortal_function_definition(
          is_multiinstance_immortal_function(
              ImmortalFunctionMultiInstanceTypeFlags::DATA_COPY),
          has(compoundStmt(forEachDescendant(stmt(anyOf(
              declStmt(hasDescendant(immortal_var_decl())).bind(VAR_DECL_STMT),
              local_immortal_var_ref))))))
          .bind(MULTI_INSTANCE_FUNCTION_COPY_VIA_DATA_COPY);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               data_copy_multi_instance_function_copy),
      this);
}

void FunctionCopyPass::run(const MatchFinder::MatchResult &result) {
  this->handle_function_copy_via_data_copy(result);
  this->handle_function_copy_via_whole_body_copy(result);
}

void FunctionCopyPass::handle_function_copy_via_data_copy(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  if (const auto *fn_def = result.Nodes.getNodeAs<FunctionDecl>(
          MULTI_INSTANCE_FUNCTION_COPY_VIA_DATA_COPY)) {
    if (auto decl_stmt = result.Nodes.getNodeAs<DeclStmt>(VAR_DECL_STMT)) {
      for (const auto &decl : decl_stmt->decls()) {
        if (auto var_decl = dyn_cast<VarDecl>(decl)) {
          // I don't know why getEndLoc doesn't work...
          auto end_loc = var_decl->getLocation().getLocWithOffset(
              var_decl->getNameAsString().size());
          auto err = this->insert(
              end_loc,
              fmt::format("[{}]",
                          get_immortal_function_instances(
                              *fn_def, this->context_.get_current_CI())));
          assert_no_error(err);
        }
      }
    }
    if (auto var_ref = result.Nodes.getNodeAs<DeclRefExpr>(VAR_REF)) {
      auto err = this->replace(
          CharSourceRange::getTokenRange(var_ref->getSourceRange()),
          fmt::format("({}[{}])", var_ref->getNameInfo().getAsString(), "_id"));
      assert_no_error(err);
    }
  }
}
void FunctionCopyPass::handle_function_copy_via_whole_body_copy(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  // TODO
}
