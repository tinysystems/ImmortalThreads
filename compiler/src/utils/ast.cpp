/**
 * \file ast.cpp
 * \brief Common utility related to AST walking - implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#include "ast.hpp"

#include "fmt/core.h"
#include "immortal.hpp"

using namespace clang;
using namespace clang::ast_type_traits;

namespace imtc::utils {

const DynTypedNode *
find_parent(const DynTypedNode &node, ASTContext &context,
            std::function<bool(const DynTypedNode &)> predicate) {
  auto parents = context.getParents(node);
  for (auto &parent : parents) {
    if (predicate(parent)) {
      return &parent;
    }
  }
  return nullptr;
}
} // namespace imtc::utils
