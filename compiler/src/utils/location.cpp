/**
 * \file location.cpp
 * \brief SourceLocation related utilities - implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#include "location.hpp"

using namespace clang;

namespace imtc::utils {

bool is_location_in_header_file(const SourceLocation &loc,
                                const SourceManager &manager,
                                FileID *header_file_id) {

  auto main_file_id = manager.getMainFileID();
  auto expanded_loc = manager.getExpansionLoc(loc);
  auto loc_file_id = manager.getFileID(expanded_loc);
  bool is_in_header = main_file_id != loc_file_id;
  if (is_in_header) {
    if (header_file_id) {
      *header_file_id = loc_file_id;
    }
  }
  return is_in_header;
}

SourceLocation find_symbol_location(const SourceLocation &loc, char symbol,
                                    const SourceManager &manager,
                                    const LangOptions &opts) {
  std::string c;
  c.reserve(1);
  for (size_t i = 0;; ++i) {
    c = Lexer::getSourceText(
        CharSourceRange::getCharRange(loc.getLocWithOffset(i),
                                      loc.getLocWithOffset(i + 1)),
        manager, opts);
    assert(c.size() == 1);
    if (c[0] == symbol) {
      return loc.getLocWithOffset(i);
    }
  }
}

} // namespace imtc::utils
