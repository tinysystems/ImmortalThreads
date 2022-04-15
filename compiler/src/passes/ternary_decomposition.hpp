/**
 * \file ternary_decomposition.hpp
 * \brief Ternary decomposition handlers - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 *
 * Decompose expressions that use the ternary operator and that contains syntax
 * constructs that must be wrapped with ImmortalThreads macros.
 *
 * The ternary operator introduce a sequence point, thus it cannot be handled
 * by ExpressionDecompositionPass
 */

#ifndef IMMORTALC_TERNARY_DECOMPOSITION_HPP_
#define IMMORTALC_TERNARY_DECOMPOSITION_HPP_

#include "pass.hpp"

namespace imtc {
class TernaryDecompositionPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
  void onEndOfTranslationUnit() override;

private:
  /**
   * A dictionary containing, for each "top level statement", the nested
   * ternary expressions that contain expressions to be instrumented (and thus
   * decomposed into statements themselves).
   */
  std::unordered_map<const clang::Stmt *, std::vector<const clang::ConditionalOperator *>>
      ternary_expressions;
};
} // namespace imtc

#endif /* ifndef IMMORTALC_TERNARY_DECOMPOSITION_HPP_ */
