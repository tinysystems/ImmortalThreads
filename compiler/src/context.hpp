/**
 * \file context.hpp
 * \brief Immortalc context - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */
#ifndef IMMORTALC_CONTEXT_HPP_
#define IMMORTALC_CONTEXT_HPP_

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Tooling.h"

#include <filesystem>

namespace imtc {
/// This class is where the "state" of the whole transformation lies.
/// This class is a singleton
///
/// Every transformation reports errors through a \c DiagnosticsEngine provided
/// by this context.
/// Every transformation register changes to this context.
///
/// A \c imtc::ImmortalcPass always has access to the active context.
class ImmortalcContext {
public:
  /// This class adds a sub context id to ImmortalcContext when it is
  /// constructured. When it is destructed, the sub context id is removed.
  class ScopedDumpSubContext {
    friend ImmortalcContext;

  public:
    ScopedDumpSubContext(ImmortalcContext &context, const std::string &id)
        : context_(context), context_id_(id) {
      context_.dump_context_chain_.push_back(this);
    }
    ~ScopedDumpSubContext() { context_.dump_context_chain_.remove(this); }

  private:
    ImmortalcContext &context_;
    std::string context_id_;
  };

  class CurrentFrontendAction {

  public:
    ///
    /// \param action Expects a fully constructured clang::FrontendAction where
    /// the compiler instance has already been registered registered.
    ///
    CurrentFrontendAction(ImmortalcContext &context,
                          const clang::FrontendAction &action)
        : context_(context) {
      assert(!context.current_frontend_action_);
      this->context_.set_current_frontend_action(&action);
    }
    ~CurrentFrontendAction() {
      this->context_.set_current_frontend_action(nullptr);
    }

  private:
    ImmortalcContext &context_;
  };

  /// Get the singleton instance
  static ImmortalcContext &get_instance() {
    static ImmortalcContext instance;
    return instance;
  }
  ImmortalcContext(ImmortalcContext const &) = delete;
  void operator=(ImmortalcContext const &) = delete;

  /// Initialize singleton. Call at the beginning.
  void init(bool force_output, const std::string &instrumented_file_infix,
            const std::vector<std::string> &instrumentable_header_paths);

  /// Add a change
  void add_change(const clang::tooling::AtomicChange &change);
  /// Apply all the changes added so far
  void apply_changes(clang::Rewriter &rewriter,
                     const std::string *dump_directory = nullptr);
  /// Whether there is any changed
  bool has_changes();
  /// Clear the changes that have been added so far
  void clear_changes();
  /// Given a ClangTool instance, replace the original files with their
  /// transformed versions, so that ClangTool picks up the transformed source
  /// text, instead of the original source text that lies in the real file
  /// system.
  void map_changed_files(clang::tooling::ClangTool &tool);
  /// Write all the transformed files alongside the original ones, with an
  /// added infix "immortal" before the extension.
  /// E.g. for "file.c", the file "file.immortal.c" would be created in the same
  /// directory.
  void write_changed_files();
  /// Print the transformed files to the given output stream
  void print_changed_files(std::ostream &out);
  /// Given a declaration, return whether the location is in a header file and
  /// the header file is not instrumentable. Return false if any of the two
  /// conditions is false.
  bool is_declared_in_non_instrumentable_headers(const clang::Decl &decl);

  /// Get current compiler instance
  const clang::CompilerInstance &get_current_CI();
  /// Set the current frontend action (can also set it to nullptr)
  void set_current_frontend_action(const clang::FrontendAction *);

  static ImmortalcContext &get_application_immortalc_context() {
    // when this method is invoked, unique_global_instance_ shall not be
    //  non-nullptr
    assert(ImmortalcContext::unique_global_instance_);
    return *ImmortalcContext::unique_global_instance_;
  }

  const std::vector<std::filesystem::path> &get_instrumentable_header_paths() {
    return this->instrumentable_header_paths_;
  }

private:
  static ImmortalcContext *unique_global_instance_;

  ImmortalcContext() {}
  std::string get_dump_context();
  void mark_main_file_as_changed();
  /// Infix to be used in the file name of the instrumented source file
  std::string instrumented_file_infix_;
  /// Paths that contain header files that can be instrumented by immortalc
  std::vector<std::filesystem::path> instrumentable_header_paths_;
  /// Map of all the files changed by the transformation.
  /// Key: the path of the changed file
  /// Value: the modified source text of the changed file
  std::map<std::string, std::string> changed_files_;
  clang::tooling::AtomicChanges changes_;
  std::list<const ScopedDumpSubContext *> dump_context_chain_;
  const clang::FrontendAction *current_frontend_action_ = nullptr;
  bool force_output_ = false;
};

} // namespace imtc

#endif /* ifndef IMMORTALC_CONTEXT_HPP_ */
