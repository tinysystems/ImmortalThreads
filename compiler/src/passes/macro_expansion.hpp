/**
 * \file preprocessor.hpp
 * \brief Expand macros - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
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
