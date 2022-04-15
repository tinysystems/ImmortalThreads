/**
 * \file force_braces.hpp
 * \brief Force braces around control statements - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#ifndef IMMORTALC_FORCE_BRACES_HPP_
#define IMMORTALC_FORCE_BRACES_HPP_

#include "pass.hpp"

namespace imtc {
class ForceBracesPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

private:
  bool checkStmt(const clang::ast_matchers::MatchFinder::MatchResult &Result,
                 const clang::Stmt *S, clang::SourceLocation StartLoc,
                 clang::SourceLocation EndLocHint = clang::SourceLocation());
  template <typename IfOrWhileStmt>
  clang::SourceLocation findRParenLoc(const IfOrWhileStmt *S,
                                      const clang::SourceManager &SM,
                                      const clang::ASTContext *Context);

private:
  std::set<const clang::Stmt *> ForceBracesStmts;
};

} // namespace imtc

#endif /* ifndef IMMORTALC_FORCE_BRACES_HPP_ */
