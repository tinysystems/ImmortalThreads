/**
 * \file switch_transformation.hpp
 * \brief Transform switch statement info if/else - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#ifndef IMMORTALC_SWITCH_TRANSFORMATION_HPP_
#define IMMORTALC_SWITCH_TRANSFORMATION_HPP_

#include "pass.hpp"

namespace imtc {
class SwitchTransformationPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
  void onEndOfTranslationUnit() override;

  typedef std::pair<const clang::SwitchStmt *, std::vector<const clang::Stmt *>>
      SwitchStmtInfo;

private:
  /**
   * A dictionary containing, for each switch statement, the sequence of case
   * statements,  break statements and default statements.
   */
  std::unordered_map<SwitchStmtInfo::first_type, SwitchStmtInfo::second_type>
      switch_stmts;

  /**
   * Check whether the given switch statement is supported
   */
  bool check_support(const SwitchStmtInfo &info);
  /**
   * Given a supported switch statement, transform it
   */
  void transformt_switch_stmt(const SwitchStmtInfo &info);
};
} // namespace imtc

#endif /* IMMORTALC_SWITCH_TRANSFORMATION_HPP_ */
