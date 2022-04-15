/**
 * \file preprocessor.hpp
 * \brief Expand macros - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#ifndef IMMORTALC_MACRO_EXPANSION_HPP_
#define IMMORTALC_MACRO_EXPANSION_HPP_

#include "pass.hpp"

namespace imtc {
class MacroExpansionPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;
  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void onEndOfTranslationUnit() override;
};
} // namespace imtc

#endif /* IMMORTALC_MACRO_EXPANSION_HPP_ */
