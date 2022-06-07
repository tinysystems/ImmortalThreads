/**
 * \file context.cpp
 * \brief Immortalc context - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "context.hpp"

#include "utils/error.hpp"

#include "fmt/core.h"

#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/ReplacementsYaml.h"

#include <filesystem>
#include <fstream>

using namespace clang;
using namespace clang::tooling;
namespace fs = std::filesystem;

static inline bool is_header_file(const fs::path &path) {
  return path.extension().string().rfind(".h", 0) == 0;
}

/// Given a path to a directory, perform the necesary actions
/// to ensure that it is possible to create files in such directory
static void ensure_directory_is_ok(const fs::path &dir) {
  llvm::ExitOnError exit_on_error(
      fmt::format("Unable to create directory '{}': ", dir.string()));
  exit_on_error(
      llvm::errorCodeToError(llvm::sys::fs::create_directories(dir.string())));
}

/// Write the given content to a file at the given path
static void write_to_file(const fs::path &path, const std::string &content) {
  ensure_directory_is_ok(path.parent_path());

  llvm::ExitOnError exit_on_error("Unable to write to file '{}': ");
  std::error_code code;
  llvm::raw_fd_ostream dump_out(path.string(), code);
  exit_on_error(llvm::errorCodeToError(code));
  dump_out << content;
  exit_on_error(llvm::errorCodeToError(dump_out.error()));
}

static auto get_output_file_path(const fs::path &input_path,
                                 const std::string &infix) {
  auto path = fs::path(input_path);
  path.replace_extension(infix + path.extension().string());
  return path;
}

static auto get_canonical_paths(const std::vector<std::string> &paths) {
  std::vector<fs::path> normalized_paths;
  normalized_paths.reserve(paths.size());
  std::transform(
      paths.cbegin(), paths.cend(), std::back_inserter(normalized_paths),
      [](const auto &path) { return fs::canonical(fs::path(path)); });
  return normalized_paths;
}

/// Check whether the file is contained in one of the directories in the given
/// list of directories
/// \param [in] directories Assumed to be canonical
static auto
is_file_contained_in_directories(const fs::path &file,
                                 const std::vector<fs::path> &directories) {
  auto path = fs::canonical(file);
  for (const auto &dir : directories) {
    auto [mismatch, _] =
        std::mismatch(dir.begin(), dir.end(), path.begin(), path.end());
    if (mismatch == dir.end()) {
      return true;
    }
  }
  return false;
}

namespace imtc {

void ImmortalcContext::init(
    bool force_output, const std::string &instrumented_file_infix,
    const std::vector<std::string> &instrumentable_header_paths) {
  this->force_output_ = force_output;
  this->instrumented_file_infix_ = instrumented_file_infix;
  this->instrumentable_header_paths_ =
      get_canonical_paths(instrumentable_header_paths);
}

void ImmortalcContext::set_current_frontend_action(
    const clang::FrontendAction *action) {
  this->current_frontend_action_ = action;
  if (this->force_output_) {
    this->mark_main_file_as_changed();
  }
}

void ImmortalcContext::add_change(const clang::tooling::AtomicChange &change) {
  auto changed_path = fs::path(change.getFilePath());
  if (is_header_file(changed_path) &&
      !is_file_contained_in_directories(changed_path,
                                        this->instrumentable_header_paths_)) {
    // ignore non instrumentable header files
    return;
  }
  this->changes_.push_back(change);
}
bool ImmortalcContext::has_changes() { return this->changes_.size() != 0; }
void ImmortalcContext::clear_changes() { this->changes_.clear(); }

void ImmortalcContext::apply_changes(clang::Rewriter &rewriter,
                                     const std::string *dump_directory) {
  std::map<std::string, Replacements> files_replacements;

  for (auto &change : this->changes_) {
    for (auto &replacement : change.getReplacements()) {
      auto err = files_replacements[change.getFilePath()].add(replacement);
      utils::assert_no_error(err);
    }
  }

  formatAndApplyAllReplacements(files_replacements, rewriter);

  for (auto &file_replacements : files_replacements) {
    SourceManager &manager = rewriter.getSourceMgr();
    llvm::ExitOnError exit_on_error(fmt::format(
        "Unable to get file reference ''", file_replacements.first));
    clang::FileEntryRef file = exit_on_error(
        manager.getFileManager().getFileRef(file_replacements.first));
    FileID id = manager.translateFile(&file.getFileEntry());
    auto file_range = CharSourceRange::getCharRange(
        manager.getLocForStartOfFile(id), manager.getLocForEndOfFile(id));
    std::string edited_src = rewriter.getRewrittenText(file_range);
    this->changed_files_[file_replacements.first] = edited_src;
  }

  if (dump_directory) {
    fs::path dump_dir(*dump_directory);
    std::string dump_context = this->get_dump_context();

    std::vector<TranslationUnitReplacements> reps;
    reps.reserve(files_replacements.size());
    for (auto &file_replacement : files_replacements) {
      auto file_name = fs::path(file_replacement.first).filename().string();
      {
        // dump YAML changes
        TranslationUnitReplacements rep;
        rep.MainSourceFile = file_replacement.first;
        std::copy(file_replacement.second.begin(),
                  file_replacement.second.end(),
                  std::back_inserter(rep.Replacements));
        std::string yaml_content;
        llvm::raw_string_ostream yaml_stream(yaml_content);
        llvm::yaml::Output yaml(yaml_stream);
        yaml << rep;
        write_to_file(dump_dir / dump_context / (file_name + ".changes.yaml"),
                      yaml_stream.str());
      }
      {
        // dump the instrumented file itself
        write_to_file(dump_dir / dump_context / file_name,
                      this->changed_files_[file_replacement.first]);
      }
    }
  }
}
void ImmortalcContext::map_changed_files(clang::tooling::ClangTool &tool) {
  for (auto &change : this->changed_files_) {
    tool.mapVirtualFile(change.first, change.second);
  }
}

void ImmortalcContext::write_changed_files() {
  for (auto &file : this->changed_files_) {
    write_to_file(
        get_output_file_path(file.first, this->instrumented_file_infix_),
        file.second);
  }
}
void ImmortalcContext::print_changed_files(std::ostream &out) {
  for (auto &file : this->changed_files_) {
    out << "## "
        << get_output_file_path(file.first, this->instrumented_file_infix_)
        << "\n";
    out << file.second << "\n";
  }
}

bool ImmortalcContext::is_declared_in_non_instrumentable_headers(
    const clang::Decl &decl) {
  auto canonical_decl = decl.getCanonicalDecl();
  assert(canonical_decl);

  auto &sm = this->get_current_CI().getSourceManager();
  auto decl_loc = canonical_decl->getBeginLoc();
  auto decl_file_id = sm.getFileID(sm.getExpansionLoc(decl_loc));
  auto main_file_id = sm.getMainFileID();
  if (decl_file_id == main_file_id) {
    // location is in main file (i.e. the source file), not header file
    return true;
  }

  auto file_entry = sm.getFileEntryForID(decl_file_id);
  assert(file_entry);
  auto header_file_path = fs::path(file_entry->getName());

  assert(is_header_file(header_file_path));
  return is_file_contained_in_directories(header_file_path,
                                          this->instrumentable_header_paths_);
}

std::string ImmortalcContext::get_dump_context() {
  std::string context;

  for (auto c : this->dump_context_chain_) {
    context += c->context_id_ + "/";
  }
  // remove last slash
  return context.erase(context.size() - 1);
}

void ImmortalcContext::mark_main_file_as_changed() {
  if (this->changed_files_.size() == 0) {
    auto &src_manager = this->get_current_CI().getSourceManager();
    auto file_id = src_manager.getMainFileID();
    auto main_file_path =
        fs::path(src_manager.getFileEntryForID(file_id)->getName());
    Rewriter rewriter(src_manager, this->get_current_CI().getLangOpts());
    auto file_range =
        CharSourceRange::getCharRange(src_manager.getLocForStartOfFile(file_id),
                                      src_manager.getLocForEndOfFile(file_id));
    auto content = rewriter.getRewrittenText(file_range);
    this->changed_files_[main_file_path] = content;
  }
}

const clang::CompilerInstance &ImmortalcContext::get_current_CI() {
  assert(this->current_frontend_action_);
  return this->current_frontend_action_->getCompilerInstance();
}
} // namespace imtc
