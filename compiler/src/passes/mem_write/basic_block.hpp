/**
 * \file basic_block.hpp
 * \brief Instrument write accesses a bit more smarty - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * Wrap write accesses to non-volatile variables using the appropriate
 * ImmortalThreads runtime macro APIs, avoiding wrapping some uncessary writes
 * within basic blocks.
 */

#ifndef IMMORTALC_MEM_WRITE_BASIC_BLOCK_OPTIMIZATION_HPP_
#define IMMORTALC_MEM_WRITE_BASIC_BLOCK_OPTIMIZATION_HPP_

#include "pass.hpp"

#include "clang/Analysis/AnalysisDeclContext.h"

namespace imtc {
class MemWriteCheckpointMacrosInsertionPassWithBasicBlockOptimization
    : public ImmortalcPass {
  struct PassImpl;
  friend PassImpl;
  std::unique_ptr<PassImpl> p_impl_;

public:
  explicit MemWriteCheckpointMacrosInsertionPassWithBasicBlockOptimization(
      ImmortalcContext &context);
  ~MemWriteCheckpointMacrosInsertionPassWithBasicBlockOptimization();

  void register_matchers(clang::ast_matchers::MatchFinder &finder) override;
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;
  void onEndOfTranslationUnit() override;
};
} // namespace imtc

#endif /* IMMORTALC_MEM_WRITE_BASIC_BLOCK_OPTIMIZATION_HPP_ */
