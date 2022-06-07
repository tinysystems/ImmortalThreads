/**
 * \file error.cpp
 * \brief Common error handling utilities - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */
#include "error.hpp"

#include "clang/Tooling/Core/Replacement.h"

namespace imtc::utils {

void assert_no_error(llvm::Error &err) {
  auto error = llvm::handleErrors(std::move(err),
                                  [](clang::tooling::ReplacementError &err) {
                                    llvm::errs() << err.message();
                                    assert(0);
                                  });
  assert(!error);
}
} // namespace imtc::utils
