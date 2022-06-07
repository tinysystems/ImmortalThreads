/**
 * \file function_copy.hpp
 * \brief Instrument multi-instance immortal functions - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#ifndef IMMORTALC_FUNCTION_COPY_HPP_
#define IMMORTALC_FUNCTION_COPY_HPP_

#include "pass.hpp"

#include "llvm/ADT/DenseMap.h"

#include <set>

namespace imtc {
class FunctionCopyPass : public ImmortalcPass {
public:
  using ImmortalcPass::ImmortalcPass;

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

private:
  void handle_function_copy_via_data_copy(
      const clang::ast_matchers::MatchFinder::MatchResult &result);
  void handle_function_copy_via_whole_body_copy(
      const clang::ast_matchers::MatchFinder::MatchResult &result);
};
} // namespace imtc

#endif /* IMMORTALC_FUNCTION_COPY_HPP_ */
