/**
 * \file control_stmt.hpp
 * \brief Instrument control statements - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#ifndef IMMORTALC_CONTROL_STMT_HPP_
#define IMMORTALC_CONTROL_STMT_HPP_

#include "pass.hpp"

namespace imtc {
class ControlStmtPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

private:
  void
  decompose_if(const clang::ast_matchers::MatchFinder::MatchResult &result);
  void
  decompose_switch(const clang::ast_matchers::MatchFinder::MatchResult &result);
  void
  decompose_for(const clang::ast_matchers::MatchFinder::MatchResult &result);
  void
  decompose_while(const clang::ast_matchers::MatchFinder::MatchResult &result);
  template <typename T>
  void handle_loop_condition_to_be_instrumented(
      const T *loop_stmt,
      const clang::ast_matchers::MatchFinder::MatchResult &result);
};
} // namespace imtc

#endif /* ifndef IMMORTALC_CONTROL_STMT_HPP_ */
