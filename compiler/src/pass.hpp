/**
 * \file pass.hpp
 * \brief ImmortalcPass - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#ifndef IMMORTALC_PASS_HPP_
#define IMMORTALC_PASS_HPP_

#include "context.hpp"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

#include <memory>

namespace imtc {

/// Base class for all immortalc passes.
/// A pass is something that typically registers some AST matchers and acts on
/// the matches.
///
/// To implement a ``ImmortalcPass``, write a subclass and override some of the
/// base class's methods. E.g. ``registerMatchers``:
/// and then override ``run(const MatchResult &Result)`` to do the actual
/// transformation for each match.
///
/// ImmortalcPasses can modify the source file using the methods `replace` and
/// `insert`, which are wrappers around \c AtomicChange. ImmortalcPass keeps a
/// map of AtomicChange per source file and when the method register_changes is
/// called the registered changes are passed to the ImmortalcContext.
/// ImmortalcPass's AtomicChange wrappers ensure that only changes outside
/// macros are kept. Changes happening at expansion location are ignored if
/// the macros that are involved correspond to macros from the ImmortalThreads
/// runtime. Otherwise the program is aborted, since we expect that the macros
/// in the original source file were all expanded as first step in the program.
class ImmortalcPass : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  /// Initializes the check with \p context.
  ///
  /// Derived classes must implement the constructor with this signature or
  /// delegate it.
  ImmortalcPass(ImmortalcContext &context) : context_(context) {}

  /// Override this to register AST matchers with \p finder.
  ///
  /// This should be used by passes that analyze code properties that
  /// dependent on AST knowledge.
  ///
  /// You can register as many matchers as necessary with \p finder. Usually,
  /// "this" will be used as callback, but you can also specify other callback
  /// classes. Thereby, different matchers can trigger different callbacks.
  ///
  /// If you need to merge information between the different matchers, you can
  /// store these as members of the derived class. However, note that all
  /// matches occur in the order of the AST traversal.
  virtual void register_matchers(clang::ast_matchers::MatchFinder &finder) {}

  /// Register changes to ImmortalcContext
  void register_changes();

  /// ``ClangTidyChecks`` that register ASTMatchers should do the actual
  /// work in here.
  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {}

protected:
  /// Wrapper of \c AtomicChange
  llvm::Error replace(const clang::SourceManager &SM,
                      const clang::CharSourceRange &Range,
                      llvm::StringRef ReplacementText);
  llvm::Error replace(const clang::CharSourceRange &Range,
                      llvm::StringRef ReplacementText);
  /// Wrapper of \c AtomicChange
  llvm::Error replace(const clang::SourceManager &SM, clang::SourceLocation Loc,
                      unsigned Length, llvm::StringRef Text);
  llvm::Error replace(clang::SourceLocation Loc, unsigned Length,
                      llvm::StringRef Text);
  /// Wrapper of \c AtomicChange
  llvm::Error insert(const clang::SourceManager &SM, clang::SourceLocation Loc,
                     llvm::StringRef Text, bool InsertAfter = true);
  llvm::Error insert(clang::SourceLocation Loc, llvm::StringRef Text,
                     bool InsertAfter = true);

  template <typename T> std::string get_ast_node_string(const T &node) {
    return clang::Lexer::getSourceText(
        clang::CharSourceRange::getTokenRange(node.getSourceRange()),
        this->context_.get_current_CI().getSourceManager(),
        this->context_.get_current_CI().getLangOpts());
  }

  ImmortalcContext &context_;

private:
  void create_atomic_change_if_missing(const clang::SourceManager &SM,
                                       clang::FileID file_id);
  std::map<clang::FileID, clang::tooling::AtomicChange> file_changes_;
};

} // namespace imtc

#endif /* ifndef IMMORTALC_PASS_HPP_ */
