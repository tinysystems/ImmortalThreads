/**
 * \file expr.hpp
 * \brief Common utility related to AST walking - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#ifndef IMMORTALC_AST_HPP_
#define IMMORTALC_AST_HPP_

#include "clang/AST/AST.h"

namespace imtc::utils {

const clang::ast_type_traits::DynTypedNode *
find_parent(const clang::ast_type_traits::DynTypedNode &node,
            clang::ASTContext &context,
            std::function<bool(const clang::ast_type_traits::DynTypedNode &)>
                predicate);

}

#endif /* ifndef IMMORTALC_AST_HPP_ */
