/**
 * \file immortal_function.hpp
 * \brief Instrument function to make them become "immortal functions" -
 * interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#ifndef IMMORTALC_IMMORTAL_FUNCTION_HPP_
#define IMMORTALC_IMMORTAL_FUNCTION_HPP_

#include "pass.hpp"

#include "llvm/ADT/DenseMap.h"

#include <set>

namespace imtc {
class ImmortalFunctionPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
  void onEndOfTranslationUnit() override;

private:
  void handle_immortal_function_declaration(
      const clang::ast_matchers::MatchFinder::MatchResult &result);
  void handle_immortal_function_definition(
      const clang::ast_matchers::MatchFinder::MatchResult &result);
  void handle_immortal_function_declarations_in_header_file();

  llvm::DenseMap<clang::FileID, std::set<const clang::FunctionDecl *>>
      immortal_functions_in_headers_;
};
} // namespace imtc

#endif /* IMMORTALC_IMMORTAL_FUNCTION_HPP_ */
