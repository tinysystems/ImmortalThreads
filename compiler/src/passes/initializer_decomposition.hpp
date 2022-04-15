/**
 * \file initializer_decomposition.hpp
 * \brief Decomposes initialization from declaration - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#ifndef IMMORTALC_INITIALIZER_DECOMPOSITION_HPP_
#define IMMORTALC_INITIALIZER_DECOMPOSITION_HPP_

#include "pass.hpp"

namespace imtc {
class InitializationDecompositionPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

private:
  std::vector<std::string> get_decomposed_decl(
      const clang::ast_matchers::MatchFinder::MatchResult &result,
      const clang::DeclStmt *decl);
};
} // namespace imtc

#endif /* ifndef IMMORTALC_INITIALIZER_DECOMPOSITION_HPP_ */
