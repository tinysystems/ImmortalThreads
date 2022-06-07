/**
 * \file naive.hpp
 * \brief Naively instrument all write accesses - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * Wrap write accesses to non-volatile variables using the appropriate
 * ImmortalThreads runtime macro APIs
 */

#ifndef IMMORTALC_MEM_WRITE_NAIVE_HPP_
#define IMMORTALC_MEM_WRITE_NAIVE_HPP_

#include "pass.hpp"

namespace imtc {
class NaiveMemWriteCheckpointMacrosInsertionPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

private:
  void
  wrap_wr_macro(const clang::ast_matchers::MatchFinder::MatchResult &result);
  void wrap_wr_self_macro(
      const clang::ast_matchers::MatchFinder::MatchResult &result);
};
} // namespace imtc

#endif /* IMMORTALC_MEM_WRITE_NAIVE_HPP_ */
