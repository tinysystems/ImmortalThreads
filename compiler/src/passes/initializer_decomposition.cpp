/**
 * \file decl_def.cpp
 * \brief Instruments variable declarations in a immortal function -
 * implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */
#include "initializer_decomposition.hpp"

#include "matchers/matchers.hpp"
#include "utils/error.hpp"
#include "utils/immortal.hpp"
#include "utils/location.hpp"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Lexer.h"

#include <cctype>

using namespace imtc::ast_matchers;
using namespace imtc::utils;

using namespace clang;
using namespace clang::ast_matchers;

static constexpr char IMMORTAL_LOCAL_VAR_DECL_STMT_BINDING[] =
    "immortal_local_var_decl";

/**
 * From
 * https://stackoverflow.com/questions/5689003/how-to-implode-a-vector-of-strings-into-a-string-the-elegant-way
 */
static std::string implode(const std::vector<std::string> &elems, char delim) {
  std::string s;
  for (std::vector<std::string>::const_iterator ii = elems.begin();
       ii != elems.end(); ++ii) {
    s += (*ii);
    if (ii + 1 != elems.end()) {
      s += delim;
    }
  }

  return s;
}

namespace imtc {

void InitializationDecompositionPass::register_matchers(MatchFinder &finder) {
  /**
   * Matches any local variable declaration that is not already declared as
   * immortal value
   */
  auto immortal_function_local_var_decl_matcher =
      for_each_node_inside_immortal_function_definition(
          declStmt(hasDescendant(immortal_var_decl()))
              .bind(IMMORTAL_LOCAL_VAR_DECL_STMT_BINDING));
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               immortal_function_local_var_decl_matcher),
      this);
}

void InitializationDecompositionPass::run(
    const MatchFinder::MatchResult &result) {
  const auto *decl =
      result.Nodes.getNodeAs<DeclStmt>(IMMORTAL_LOCAL_VAR_DECL_STMT_BINDING);
  assert(decl);
  llvm::Error err = llvm::ErrorSuccess();
  /// Handle local variable
  auto initializers = get_decomposed_decl(result, decl);
  if (initializers.size() != 0) {
    auto loc_after_semilocon =
        find_symbol_location(decl->getEndLoc(), ';', *result.SourceManager,
                             result.Context->getLangOpts())
            .getLocWithOffset(1);
    err = this->insert(loc_after_semilocon, "\n");
    assert_no_error(err);
    err = this->insert(loc_after_semilocon, implode(initializers, '\n'));
    assert_no_error(err);
  }
}

/**
 * Removes the initializers from the declarations and returns
 */
std::vector<std::string> InitializationDecompositionPass::get_decomposed_decl(
    const clang::ast_matchers::MatchFinder::MatchResult &result,
    const DeclStmt *decl) {
  std::vector<std::string> res;
  for (const auto &child : decl->decls()) {
    if (auto var_decl = dyn_cast<VarDecl>(child)) {
      if (auto init = var_decl->getInit()) {
        // if has initilization expression

        // It's not documented, but getLocation gives the location we want
        // See also https://stackoverflow.com/a/9054913
        auto identifierBeingLoc = var_decl->getLocation();

        // remove the initializer: start from the first space or the first
        // assignment operator after the identifier until the end of the
        // varDecl
        auto err = this->replace(
            CharSourceRange::getTokenRange(
                find_symbol_location_if(
                    identifierBeingLoc,
                    [](char s) { return std::isspace(s) || s == '='; },
                    *result.SourceManager, result.Context->getLangOpts()),
                var_decl->getEndLoc()),
            "");
        assert_no_error(err);
        // create separate initialization statement
        res.push_back(
            std::string(Lexer::getSourceText(
                CharSourceRange::getTokenRange(var_decl->getLocation(),
                                               var_decl->getEndLoc()),
                *result.SourceManager, result.Context->getLangOpts())) +
            ";");
      }
    }
  }
  return res;
}

} // namespace imtc
