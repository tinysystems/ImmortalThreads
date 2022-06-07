/**
 * \file self_write_operators.hpp
 * \brief Handle "self write" operators such as "++", "+=", etc.
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#ifndef IMMORTALC_SELF_WRITE_OPERATORS_HPP_
#define IMMORTALC_SELF_WRITE_OPERATORS_HPP_

#include "pass.hpp"

namespace imtc {
class SelfWritePass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

private:
  void handle_unary_operator(
      const clang::ast_matchers::MatchFinder::MatchResult &result);
  void handle_compound_operator(
      const clang::ast_matchers::MatchFinder::MatchResult &result);
};
} // namespace imtc

#endif /* IMMORTALC_SELF_WRITE_OPERATORS_HPP_ */
