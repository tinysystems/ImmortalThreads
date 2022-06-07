/**
 * \file main.cpp
 * \brief Main entrypoint of immortalc
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "context.hpp"

#include "passes/control_stmt.hpp"
#include "passes/expression_decomposition.hpp"
#include "passes/force_braces.hpp"
#include "passes/function_copy.hpp"
#include "passes/immortal_function.hpp"
#include "passes/initializer_decomposition.hpp"
#include "passes/macro_expansion.hpp"
#include "passes/runtime_api.hpp"
#include "passes/self_write_operators.hpp"
#include "passes/shim_api_replacement.hpp"
#include "passes/switch_transformation.hpp"
#include "passes/ternary_decomposition.hpp"

#include "passes/mem_write/basic_block.hpp"
#include "passes/mem_write/naive.hpp"

#include "fmt/core.h"
#include "fmt/ranges.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Pass.h"
#include "llvm/Support/InitLLVM.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;
using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;
using namespace imtc;

static cl::OptionCategory ImmortalcCategory("immortalc options");
static cl::opt<bool>
    dump_passes("dump-passes", cl::init(false), cl::cat(ImmortalcCategory),
                cl::desc("Whether to dump each source to source "
                         "transformation pass performed by immortalc"));
static cl::opt<std::string> g_arg_dump_passes_dir(
    "dump-passes-dir", cl::init("immortalc-passes"), cl::cat(ImmortalcCategory),
    cl::desc("When --dump-passes is used, this option determines in which "
             "directory each pass will be dumped. By default is "
             "\"immortalc-passes\" in the current working directory"));

static cl::opt<std::string> g_arg_infix(
    "infix", cl::init("immortal"), cl::cat(ImmortalcCategory),
    cl::desc("Infix to use in the name of the instrumented files before the "
             "extension. The default is \"immortalc\", so the instrumented "
             "version of \"main.c\" will be \"main.immortal.c\". By default "
             "all the instrumented source files are written to a new file "
             "along side the original source file."));

static cl::opt<bool> g_arg_stdout(
    "stdout", cl::init(false), cl::cat(ImmortalcCategory),
    cl::desc("By default, the transformed files are written to a new file "
             "alongside the original one. Use this option to avoid this "
             "behaviour and to output all the transformations to stdout"));

static cl::opt<bool> g_arg_force_output(
    "force-output", cl::init(false), cl::cat(ImmortalcCategory),
    cl::desc("If this option is passed, even if no instrumentation happens, "
             "the unmodified files are outputed. By default, if no file needs "
             "to be instrumented, immortalc doesn't output anything or write "
             "to any file."));

static cl::opt<std::string> g_optimize(
    "optimize", cl::init("forward-progress"), cl::cat(ImmortalcCategory),
    cl::desc(
        "In general, the more checkpoints are placed, the less work is wasted "
        "on power failure, at the cost of higher execution overhead due to "
        "checkpoints. And, viceversa, fewer checkpoints imply less overhead, "
        "but more wasted work on power failure. The developer can choose which "
        "aspect to optimize using the option. The supported values are:\n"
        "- \"forward-progress\" (default): to place more checkpoints than "
        "required, to keep forward progress\n"
        "- \"execution-time\": to place as few checkpoints as possible (while "
        "ensuring memory consistency), to decrease execution-time\"\n"));

static cl::list<std::string> g_arg_project_header_files_dir(
    "H", cl::cat(ImmortalcCategory),
    cl::desc("List of paths which contain header files that can be "
             "instrumented by immortalc"));

/// A way to simulate a string based enum with namespaces
/// See https://stackoverflow.com/a/60169407
namespace optimization_type {
typedef std::string Type;
Type FORWARD_PROGRESS = "forward-progress";
Type EXECUTION_TIME = "execution-time";
} // namespace optimization_type

/// Apparently the `-p` flag causes cwd to change in some places of the
/// program. So we need to save the initial cwd.
static fs::path g_cwd_at_app_init;
/// Absolutized dump passes dir
static fs::path g_absolute_dump_passes_dir;

namespace imtc {
using ImmortalcPasses = std::vector<std::unique_ptr<ImmortalcPass>>;

/// Implementation of the ASTConsumer interface for reading an AST produced by
/// the Clang parser. It registers the matchers of the given passes and runs
/// them on the AST.
class ImmortalPassASTConsumer : public ASTConsumer {
private:
public:
  ImmortalPassASTConsumer(ImmortalcPasses &passes) : passes_(passes) {}

  void HandleTranslationUnit(ASTContext &context) override {
    for (auto &pass : passes_) {
      pass->register_matchers(this->finder_);
    }
    this->finder_.matchAST(context);
    for (auto &pass : passes_) {
      pass->register_changes();
    }
  }

private:
  MatchFinder finder_;
  ImmortalcPasses &passes_;
};

// For each source file provided to the tool, a new FrontendAction is created.
class PassFrontendAction : public ASTFrontendAction {
public:
  PassFrontendAction(ImmortalcContext &context, ImmortalcPasses &passes)
      : context_(context), passes_(passes) {}
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    current_action_ = std::make_unique<ImmortalcContext::CurrentFrontendAction>(
        this->context_, *this);

    this->rewriter_.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<ImmortalPassASTConsumer>(this->passes_);
  }

  void EndSourceFileAction() override {
    std::string dump_passes_dir;
    if (dump_passes) {
      dump_passes_dir = g_absolute_dump_passes_dir.string();
    }

    ImmortalcContext::ScopedDumpSubContext dump_context(
        this->context_, fs::path(this->getCurrentFile()).filename());
    this->context_.apply_changes(this->rewriter_,
                                 dump_passes ? &dump_passes_dir : nullptr);
    this->context_.clear_changes();
  }

private:
  std::unique_ptr<ImmortalcContext::CurrentFrontendAction> current_action_;
  ImmortalcContext &context_;
  ImmortalcPasses &passes_;
  Rewriter rewriter_;
};

class PassFrontendActionFactory : public FrontendActionFactory {
public:
  PassFrontendActionFactory(ImmortalcContext &context, ImmortalcPasses &passes)
      : context_(context), passes_(passes) {}
  std::unique_ptr<FrontendAction> create() override {
    return std::make_unique<PassFrontendAction>(this->context_, this->passes_);
  }

private:
  ImmortalcContext &context_;
  ImmortalcPasses &passes_;
};

std::unique_ptr<FrontendActionFactory>
newPassFrontendActionFactory(ImmortalcContext &context,
                             ImmortalcPasses &passes) {
  return std::unique_ptr<FrontendActionFactory>(
      new PassFrontendActionFactory(context, passes));
}

} // namespace imtc

int main(int argc, const char **argv) {
  g_cwd_at_app_init = fs::current_path();
  llvm::InitLLVM X(argc, argv);
  CommonOptionsParser op(argc, argv, ImmortalcCategory);

  {
    fs::path dir(g_arg_dump_passes_dir.getValue());
    if (dir.is_relative()) {
      g_absolute_dump_passes_dir = g_cwd_at_app_init / dir;
    } else {
      g_absolute_dump_passes_dir = dir;
    }
  }

  if (dump_passes) {
    {
      ExitOnError exit_on_error("Unable to clear the dump passes directory: ");
      // remove dir and recreate
      std::error_code err =
          llvm::sys::fs::remove_directories(g_arg_dump_passes_dir, false);
      exit_on_error(errorCodeToError(err));
    }
    {
      ExitOnError exit_on_error("Unable to create the dump passes directory: ");
      std::error_code err =
          llvm::sys::fs::create_directories(g_arg_dump_passes_dir, false);
      exit_on_error(errorCodeToError(err));
    }
  }

  auto &context = ImmortalcContext::get_instance();
  context.init(g_arg_force_output, g_arg_infix, g_arg_project_header_files_dir);

  std::vector<ImmortalcPasses> pass_groups;
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<MacroExpansionPass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<ForceBracesPass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<TernaryDecompositionPass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<ControlStmtPass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<SwitchTransformationPass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(
        std::make_unique<InitializationDecompositionPass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<ExpressionDecompositionPass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<SelfWritePass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<FunctionCopyPass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<ImmortalFunctionPass>(context));
    if (g_optimize == optimization_type::FORWARD_PROGRESS) {
      passes.push_back(
          std::make_unique<NaiveMemWriteCheckpointMacrosInsertionPass>(
              context));
    } else if (g_optimize == optimization_type::EXECUTION_TIME) {
      passes.push_back(
          std::make_unique<
              MemWriteCheckpointMacrosInsertionPassWithBasicBlockOptimization>(
              context));
    }
    passes.push_back(std::make_unique<ImmortalRuntimeApiPass>(context));
    passes.push_back(std::make_unique<ShimApiReplacementPass>(context));
    pass_groups.push_back(std::move(passes));
  }
  {
    ImmortalcPasses passes;
    passes.push_back(std::make_unique<SimpleShimApiReplacementPass>(context));
    pass_groups.push_back(std::move(passes));
  }

  {
    size_t pass_cnt = 0;
    for (auto &passes : pass_groups) {
      ImmortalcContext::ScopedDumpSubContext dump_context(
          context, std::to_string(pass_cnt));
      ClangTool tool(op.getCompilations(), op.getSourcePathList());
      context.map_changed_files(tool);
      tool.run(newPassFrontendActionFactory(context, passes).get());
      pass_cnt++;
    }
  }
  if (g_arg_stdout.getValue()) {
    context.print_changed_files(std::cout);
  } else {
    context.write_changed_files();
  }
}
