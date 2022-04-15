/**
 * \file shim_api_replacement.hpp
 * \brief Replace shim APIs with true APIs - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#ifndef IMMORTALC_SHIM_API_REPLACEMENT_HPP_
#define IMMORTALC_SHIM_API_REPLACEMENT_HPP_

#include "pass.hpp"

namespace imtc {
class ShimApiReplacementPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
};

class SimpleShimApiReplacementPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
};
} // namespace imtc

#endif /* IMMORTALC_SHIM_API_REPLACEMENT_HPP_ */
