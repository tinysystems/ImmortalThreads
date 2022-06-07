/**
 * \file pass.cpp
 * \brief ImmortalcPass - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "pass.hpp"
#include "utils/immortal.hpp"

#include "clang/Tooling/Refactoring/AtomicChange.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

/// Checkes whether the given location is the expansion location of an
/// macro. If not, then the change is not skippable. If yes, then the change can
/// be skipped if the macro in question is a macro from ImmortalThreads,
/// otherwise abort.
static bool is_change_skippable(clang::SourceLocation loc,
                                const clang::CompilerInstance &CI) {
  if (loc.isMacroID()) {
    auto callee = CI.getSourceManager().getTopMacroCallerLoc(loc);
    auto &PP = CI.getPreprocessor();
    auto macros = imtc::utils::get_runtime_macros();
    assert(std::find(macros.cbegin(), macros.cend(),
                     PP.getImmediateMacroName(callee)) != macros.cend() &&
           "Unexpected macro");
    return true;
  }
  return false;
}

namespace imtc {
void ImmortalcPass::register_changes() {
  for (auto &change : this->file_changes_) {
    this->context_.add_change(change.second);
  }
  this->file_changes_.clear();
}

void ImmortalcPass::create_atomic_change_if_missing(
    const clang::SourceManager &SM, clang::FileID file_id) {
  if (this->file_changes_.find(file_id) == this->file_changes_.end()) {
    auto change =
        clang::tooling::AtomicChange(SM, SM.getLocForStartOfFile(file_id));
    this->file_changes_.insert({
        file_id,
        change,
    });
  }
}

llvm::Error ImmortalcPass::replace(const clang::SourceManager &SM,
                                   const clang::CharSourceRange &Range,
                                   llvm::StringRef ReplacementText) {
  bool begin_skppable =
      is_change_skippable(Range.getBegin(), this->context_.get_current_CI());
  bool end_skppable =
      is_change_skippable(Range.getEnd(), this->context_.get_current_CI());
  assert(begin_skppable == end_skppable);
  if (begin_skppable) {
    return llvm::ErrorSuccess();
  }
  auto file = SM.getFileID(Range.getBegin());
  this->create_atomic_change_if_missing(SM, file);
  return this->file_changes_.at(file).replace(SM, Range, ReplacementText);
}

llvm::Error ImmortalcPass::replace(const clang::CharSourceRange &Range,
                                   llvm::StringRef ReplacementText) {
  return this->replace(this->context_.get_current_CI().getSourceManager(),
                       Range, ReplacementText);
}
llvm::Error ImmortalcPass::replace(const clang::SourceManager &SM,
                                   clang::SourceLocation Loc, unsigned Length,
                                   llvm::StringRef Text) {
  if (is_change_skippable(Loc, this->context_.get_current_CI())) {
    return llvm::ErrorSuccess();
  }
  auto file = SM.getFileID(Loc);
  this->create_atomic_change_if_missing(SM, file);
  return this->file_changes_.at(file).replace(SM, Loc, Length, Text);
}
llvm::Error ImmortalcPass::replace(clang::SourceLocation Loc, unsigned Length,
                                   llvm::StringRef Text) {
  return this->replace(this->context_.get_current_CI().getSourceManager(), Loc,
                       Length, Text);
}
llvm::Error ImmortalcPass::insert(clang::SourceLocation Loc,
                                  llvm::StringRef Text, bool InsertAfter) {
  return this->insert(this->context_.get_current_CI().getSourceManager(), Loc,
                      Text, InsertAfter);
}
llvm::Error ImmortalcPass::insert(const clang::SourceManager &SM,
                                  clang::SourceLocation Loc,
                                  llvm::StringRef Text, bool InsertAfter) {
  if (is_change_skippable(Loc, this->context_.get_current_CI())) {
    return llvm::ErrorSuccess();
  }
  auto file = SM.getFileID(Loc);
  this->create_atomic_change_if_missing(SM, file);
  return this->file_changes_.at(file).insert(SM, Loc, Text, InsertAfter);
}

} // namespace imtc
