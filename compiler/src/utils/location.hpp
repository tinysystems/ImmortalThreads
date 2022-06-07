/**
 * \file location.hpp
 * \brief SourceLocation related utilities - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */
#ifndef IMMORTALC_UTILS_LOCATION_HPP_
#define IMMORTALC_UTILS_LOCATION_HPP_

#include "clang/AST/ASTContext.h"
#include "clang/Lex/Lexer.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include <string>
#include <vector>

namespace imtc::utils {

/**
 * Given a location, determine whether the location is in a header file.
 *
 * \param [out] header_file_id If the location is in a header file and this a
 * valid pointer is passed, the file id of the header file is set
 */
bool is_location_in_header_file(const clang::SourceLocation &loc,
                                const clang::SourceManager &manager,
                                clang::FileID *header_file_id = nullptr);

/**
 * Given a location return the location of the next \p symbol.
 *
 * \param [in] loc the location to start from
 * \param [in] symbol the symbol you're looking for
 *
 * \return the location of the symbol. Note: if you want the location right
 * after the symbol, just call `getLocWithOffset`.
 */
clang::SourceLocation find_symbol_location(const clang::SourceLocation &loc,
                                           char symbol,
                                           const clang::SourceManager &manager,
                                           const clang::LangOptions &opts);
/**
 * Given a location return the location of the next symbol that matches the
 * given predicate.
 *
 * \param [in] loc the location to start from
 * \param [in] symbols the symbols you're looking for.
 *
 * \return the location of the symbol that is encountered
 *
 * See also https://en.cppreference.com/w/cpp/named_req/Predicate
 * for what a predicate can be.
 */
template <class UnaryPredicate>
clang::SourceLocation find_symbol_location_if(
    const clang::SourceLocation &loc, UnaryPredicate predicate,
    const clang::SourceManager &manager, const clang::LangOptions &opts) {
  std::string c;
  c.reserve(1);
  for (size_t i = 0;; ++i) {
    c = clang::Lexer::getSourceText(
        clang::CharSourceRange::getCharRange(loc.getLocWithOffset(i),
                                             loc.getLocWithOffset(i + 1)),
        manager, opts);
    assert(c.size() == 1);
    if (predicate(c[0])) {
      return loc.getLocWithOffset(i);
    }
  }
  // TODO handle invalid locations
  assert(0);
}
} // namespace imtc::utils

#endif /* ifndef IMMORTALC_UTILS_LOCATION_HPP_ */
