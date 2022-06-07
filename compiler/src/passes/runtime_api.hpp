/**
 * \file immortal_runtime_api.hpp
 * \brief ImmortalThreads runtime API instrumentation - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * Wrap elementary syntax construct using the appropriate ImmortalThreads
 * runtime macro APIs
 */

#ifndef IMMORTALC_RUNTIME_API_HPP_
#define IMMORTALC_RUNTIME_API_HPP_

#include "pass.hpp"

namespace imtc {
class ImmortalRuntimeApiPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

private:
  void mark_def(const clang::ast_matchers::MatchFinder::MatchResult &result);
  void mark_gdef(const clang::ast_matchers::MatchFinder::MatchResult &result);
  void wrap_call(const clang::ast_matchers::MatchFinder::MatchResult &result);
  void wrap_call_with_retval(
      const clang::ast_matchers::MatchFinder::MatchResult &result);
  void wrap_return(const clang::ast_matchers::MatchFinder::MatchResult &result);
};
} // namespace imtc

#endif /* IMMORTALC_RUNTIME_API_HPP_ */
