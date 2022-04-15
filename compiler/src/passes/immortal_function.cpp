/**
 * \file immortal_function.cpp
 * \brief Instrument function to make them become "immortal functions" -
 * implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 *
 * This pass applies the following transformations
 *
 * * Add `_begin()` and `_end()` delimiter to immortal functions
 * * Wrap the function declaration with the `_immortal_function`,
 * `_immortal_function_with_retval`, etc. macros
 * * Allocate the metadata for each multi-instance immortal function using
 * `_immortal_function_metadata_def`
 */
#include "immortal_function.hpp"

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

static constexpr char IMMORTAL_FUNCTION_DECL_BINDING[] =
    "immortal_function_decl";
static constexpr char IMMORTAL_FUNCTION_DEF_BINDING[] = "immortal_function_def";

void ImmortalFunctionPass::register_matchers(MatchFinder &finder) {
  /**
   * Match any immmortal function declaration
   */
  auto immortal_function_decl =
      immortal_function_declaration().bind(IMMORTAL_FUNCTION_DECL_BINDING);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               immortal_function_decl),
      this);

  /**
   * Match any immmortal function definitions
   */
  auto immortal_function_def =
      immortal_function_definition().bind(IMMORTAL_FUNCTION_DEF_BINDING);

  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               immortal_function_def),
      this);
}

void ImmortalFunctionPass::run(const MatchFinder::MatchResult &result) {
  this->handle_immortal_function_declaration(result);
  this->handle_immortal_function_definition(result);
}

void ImmortalFunctionPass::onEndOfTranslationUnit() {
  this->handle_immortal_function_declarations_in_header_file();
  this->immortal_functions_in_headers_.clear();
}

void ImmortalFunctionPass::
    handle_immortal_function_declarations_in_header_file() {
  auto &compiler_instance = this->context_.get_current_CI();
  auto &sm = compiler_instance.getSourceManager();

  for (const auto &header_file : this->immortal_functions_in_headers_) {
    std::ostringstream os;

    SourceLocation include_location;
    {
      // walk up the inclusion "chain", until getting the inclusion
      // location in the main source file, where we are going to insert
      // these wrapped declarations
      auto main_file_id = sm.getMainFileID();
      FileID file_of_include_location = header_file.first;
      do {
        include_location = sm.getIncludeLoc(file_of_include_location);
        file_of_include_location = sm.getFileID(include_location);
      } while (main_file_id != file_of_include_location);
    }

    for (const auto &declaration : header_file.second) {
      os << get_immortal_function_declaration(
                *declaration, compiler_instance.getSourceManager(),
                compiler_instance.getLangOpts())
         << ";\n";
    }

    auto include_line_number =
        sm.getSpellingLineNumber(include_location, nullptr);
    // the insert point is the next line
    auto insertion_point =
        sm.translateLineCol(sm.getMainFileID(), include_line_number + 1, 1);

    auto err = this->insert(insertion_point, os.str(), false);
    assert_no_error(err);
  }
}

void ImmortalFunctionPass::handle_immortal_function_declaration(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  auto *fn_decl =
      result.Nodes.getNodeAs<FunctionDecl>(IMMORTAL_FUNCTION_DECL_BINDING);
  if (!fn_decl) {
    return;
  }

  if (FileID header_file_id; is_location_in_header_file(
          fn_decl->getLocation(), *result.SourceManager, &header_file_id)) {
    this->immortal_functions_in_headers_[header_file_id].insert(fn_decl);
  } else {
    CharSourceRange range;
    if (fn_decl->doesThisDeclarationHaveABody()) {
      range = CharSourceRange::getTokenRange(
          fn_decl->getBeginLoc(),
          fn_decl->getBody()->getBeginLoc().getLocWithOffset(-1));
    } else {
      range = CharSourceRange::getTokenRange(fn_decl->getSourceRange());
    }
    // substitute function declaration with immortal function declaration
    auto err =
        this->replace(range, get_immortal_function_declaration(
                                 *fn_decl, result.Context->getSourceManager(),
                                 result.Context->getLangOpts()));
    assert_no_error(err);
  }
}
void ImmortalFunctionPass::handle_immortal_function_definition(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  const auto *fn_def =
      result.Nodes.getNodeAs<FunctionDecl>(IMMORTAL_FUNCTION_DEF_BINDING);
  if (!fn_def) {
    return;
  }

  auto fn_name = fn_def->getNameAsString();

  {
    size_t instances = get_immortal_function_instances(
        *fn_def, this->context_.get_current_CI());
    // insert immortal function metadata definition block
    if (instances > 1) {
      auto err = this->insert(fn_def->getBeginLoc(),
                              get_immortal_function_metadata_definition(
                                  *fn_def, instances,
                                  result.Context->getSourceManager(),
                                  result.Context->getLangOpts()),
                              false);
      assert_no_error(err);
    }
  }

  {
    // Note: the order of the following two insertions matter. I don't have
    // time to give a good explanation, but there is some sense behind this.

    // Insert end macro at the end of the function body
    auto err = this->insert(fn_def->getEndLoc(),
                            get_immortal_function_end_macro_call(
                                *fn_def, this->context_.get_current_CI()),
                            false);
    assert_no_error(err);

    // Insert begin macro at the start of the function body
    err = this->insert(
        fn_def->getBody()->getSourceRange().getBegin().getLocWithOffset(1),
        get_immortal_function_begin_macro_call(*fn_def,
                                               this->context_.get_current_CI()),
        false);
    assert_no_error(err);
  }
}
