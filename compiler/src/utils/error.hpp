/**
 * \file error.hpp
 * \brief Common error handling utilities - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#ifndef IMMORTALC_UTILS_ERROR_HPP_
#define IMMORTALC_UTILS_ERROR_HPP_

#include "llvm/Support/Error.h"

namespace imtc::utils {

void assert_no_error(llvm::Error &err);

}

#endif /* ifndef IMMORTALC_UTILS_ERROR_HPP_ */
