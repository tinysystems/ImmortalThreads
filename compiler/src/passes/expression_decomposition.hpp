/**
 * \file expression_decomposition.hpp
 * \brief Expression decomposition handlers - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 *
 * Decompose expressions that contain side effects
 */

#ifndef IMMORTALC_EXPRESSION_DECOMPOSITION_HPP_
#define IMMORTALC_EXPRESSION_DECOMPOSITION_HPP_

#include "pass.hpp"
#include "clang/Rewrite/Core/Rewriter.h"

namespace imtc {
class ExpressionDecompositionPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
  void onEndOfTranslationUnit() override;

private:
  /**
   * A dictionary containing, for each "top level statement", the nested
   * expressions that need to be instrumented (and thus decomposed into
   * statements themselves).
   */
  std::unordered_map<const clang::Stmt *, std::vector<const clang::Expr *>>
      expressions;
  std::unique_ptr<clang::Rewriter> rewriter_;
};
} // namespace imtc

#endif /* ifndef IMMORTALC_EXPRESSION_DECOMPOSITION_HPP_ */
