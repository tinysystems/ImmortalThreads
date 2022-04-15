/**
 * \file error.hpp
 * \brief Common error handling utilities - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#ifndef IMMORTALC_UTILS_ERROR_HPP_
#define IMMORTALC_UTILS_ERROR_HPP_

#include "llvm/Support/Error.h"

namespace imtc::utils {

void assert_no_error(llvm::Error &err);

}

#endif /* ifndef IMMORTALC_UTILS_ERROR_HPP_ */
