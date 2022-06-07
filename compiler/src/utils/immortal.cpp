/**
 * \file immortal.cpp
 * \brief Utility functions for working with ImmortalThreads's runtime interface
 * - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "immortal.hpp"

#include "location.hpp"

#include "fmt/core.h"

#include "clang/AST/Attr.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/Preprocessor.h"

#include <iostream>
#include <sstream>

using namespace clang;
using namespace imtc::utils;

/*******************************************************************************
 * Private functions declaration
 *******************************************************************************/
static std::optional<std::string> get_arg_list(const CallExpr &call_expr,
                                               const SourceManager &manager,
                                               const LangOptions &opts);
static size_t
get_max_immortal_function_instances(const clang::SourceManager &sm,
                                    const clang::LangOptions &lo,
                                    const clang::Preprocessor &pp);

static size_t get_privatization_buffer_count(const clang::SourceManager &sm,
                                             const clang::LangOptions &lo,
                                             const clang::Preprocessor &pp);

/*******************************************************************************
 * Public interface implementation
 *******************************************************************************/
namespace imtc::utils {
std::string get_immortal_function_declaration(const FunctionDecl &fn,
                                              const SourceManager &manager,
                                              const LangOptions &opts) {
  std::string fn_name = fn.getNameAsString();
  std::ostringstream os;
  if (fn.getReturnType()->isVoidType()) {
    os << "_immortal_function(" << fn_name;
  } else {
    os << "_immortal_function_with_retval(" << fn_name << ","
       << fn.getReturnType().getAsString();
  }
  if (fn.getNumParams() > 0) {
    os << ", "
       << Lexer::getSourceText(
              CharSourceRange::getTokenRange(fn.getParametersSourceRange()),
              manager, opts)
              .str();
  }
  os << ")";
  return os.str();
}

std::string get_immortal_function_metadata_definition(
    const clang::FunctionDecl &fn, size_t max_instances_num,
    const clang::SourceManager &manager, const clang::LangOptions &opts) {
  std::string fn_name = fn.getNameAsString();
  std::ostringstream os;
  os << "_immortal_function_metadata_def(";
  if (fn.getStorageClass() == StorageClass::SC_Static) {
    // if the function is declared as static, so should be it's immortal
    // function data structure
    os << "static,";
  } else {
    os << ",";
  }
  os << fn_name << ", " << max_instances_num << ");\n";
  return os.str();
}

std::string get_immortal_write(const clang::BinaryOperator &assignment,
                               const clang::FunctionDecl &immortal_function,
                               const clang::CompilerInstance &ci) {
  constexpr const char *MACRO = "_WR";

  auto &sm = ci.getSourceManager();
  auto &lo = ci.getLangOpts();

  std::string lhs = Lexer::getSourceText(
      CharSourceRange::getTokenRange(assignment.getLHS()->getSourceRange()), sm,
      lo);
  std::string rhs = Lexer::getSourceText(
      CharSourceRange::getTokenRange(assignment.getRHS()->getSourceRange()), sm,
      lo);

  return std::string(MACRO) + "(" + lhs + ", " + rhs + ")";
}

std::string
get_immortal_write_self(const clang::BinaryOperator &assignment,
                        const clang::FunctionDecl &immortal_function,
                        const clang::CompilerInstance &ci) {
  constexpr const char *MACRO = "_WR_SELF";

  std::string nv_var_type_str = assignment.getLHS()->getType().getAsString();

  auto &sm = ci.getSourceManager();
  auto &lo = ci.getLangOpts();

  std::string lhs = Lexer::getSourceText(
      CharSourceRange::getTokenRange(assignment.getLHS()->getSourceRange()), sm,
      lo);
  std::string rhs = Lexer::getSourceText(
      CharSourceRange::getTokenRange(assignment.getRHS()->getSourceRange()), sm,
      lo);

  return std::string(MACRO) + "(" + nv_var_type_str + ", " + lhs + ", " + rhs +
         ")";
}

/**
 * Copied from https://stackoverflow.com/a/9943807
 */
template <typename Iterator, typename Func, typename Distance>
static void chunks(Iterator begin, Iterator end, Distance k, Func f) {
  Iterator chunk_begin;
  Iterator chunk_end;
  chunk_end = chunk_begin = begin;

  do {
    if (std::distance(chunk_end, end) < k) {
      chunk_end = end;
    } else {
      std::advance(chunk_end, k);
    }
    f(chunk_begin, chunk_end);
    chunk_begin = chunk_end;
  } while (std::distance(chunk_begin, end) > 0);
}
std::string get_immortal_coalesced_write_selfs(
    const std::vector<const clang::BinaryOperator *> assignments,
    const clang::FunctionDecl &immortal_function,
    const clang::CompilerInstance &ci) {
  auto get_macro = [](size_t coalesce_count) {
    assert(coalesce_count > 0);
    if (coalesce_count == 1) {
      return std::string("_WR_SELF");
    }
    return std::string("_WR_SELF_CONTEMPORANEOUS_") +
           std::to_string(coalesce_count);
  };
  size_t buffer_count = get_privatization_buffer_count(
      ci.getSourceManager(), ci.getLangOpts(), ci.getPreprocessor());

  auto &sm = ci.getSourceManager();
  auto &lo = ci.getLangOpts();

  std::ostringstream instrumented_source;
  chunks(assignments.begin(), assignments.end(), buffer_count,
         [&](auto begin, auto end) {
           std::ostringstream arg_list;
           for (auto assignment_it = begin; assignment_it != end;
                ++assignment_it) {
             if (assignment_it != begin) {
               arg_list << ", ";
             }
             std::string nv_var_type_str =
                 (*assignment_it)->getLHS()->getType().getAsString();
             std::string lhs = Lexer::getSourceText(
                 CharSourceRange::getTokenRange(
                     (*assignment_it)->getLHS()->getSourceRange()),
                 sm, lo);
             std::string rhs = Lexer::getSourceText(
                 CharSourceRange::getTokenRange(
                     (*assignment_it)->getRHS()->getSourceRange()),
                 sm, lo);
             arg_list << nv_var_type_str + ", " + lhs + ", " + rhs;
           }
           instrumented_source << get_macro(end - begin) << "("
                               << arg_list.str() << ");";
         });

  return instrumented_source.str();
}

static std::string
get_immortal_call_with_retval(const clang::CallExpr &call_expr,
                              const std::string *retval,
                              const clang::FunctionDecl &immortal_function,
                              const clang::CompilerInstance &ci) {
  constexpr const char *MACRO = "_call";
  constexpr const char *MACRO_MULTI_FN = "_call_multi_fn";

  const auto callee =
      call_expr.getCalleeDecl()->getAsFunction()->getCanonicalDecl();

  const char *macro = nullptr;
  {
    size_t count = get_immortal_function_instances(*callee, ci);
    if (count > 1) {
      macro = MACRO_MULTI_FN;
    } else {
      macro = MACRO;
    }
  }

  const auto &sm = ci.getSourceManager();
  const auto &lo = ci.getLangOpts();

  std::string args = callee->getNameAsString();

  if (!callee->getReturnType()->isVoidType()) {
    if (retval) {
      args += fmt::format(", &{}", *retval);
    } else {
      args += ", 0";
    }
  }

  if (auto arg_list = get_arg_list(call_expr, sm, lo)) {
    args += ", " + arg_list.value();
  }
  return fmt::format("{}({})", macro, args);
}

std::string
get_immortal_call_with_retval(const clang::BinaryOperator &assignment,
                              const clang::FunctionDecl &immortal_function,
                              const clang::CompilerInstance &ci) {
  auto &sm = ci.getSourceManager();
  auto &lo = ci.getLangOpts();

  auto call_expr = dyn_cast<CallExpr>(assignment.getRHS());
  std::string lhs = Lexer::getSourceText(
      CharSourceRange::getTokenRange(assignment.getLHS()->getSourceRange()), sm,
      lo);
  return get_immortal_call_with_retval(*call_expr, &lhs, immortal_function, ci);
}

std::string get_immortal_call(const clang::CallExpr &call_expr,
                              const clang::FunctionDecl &immortal_function,
                              const clang::CompilerInstance &ci) {
  return get_immortal_call_with_retval(call_expr, nullptr, immortal_function,
                                       ci);
}

std::vector<std::string> get_runtime_macros() {
  return {"_gdef",    "_def", "_begin",   "_end",  "_return",
          "_WR_CALL", "_WR",  "_WR_SELF", "_call", "__COUNTER__"};
}

std::string
get_immortal_function_return_stmt(const clang::ReturnStmt &ret_stmt,
                                  const clang::FunctionDecl &immortal_function,
                                  const clang::CompilerInstance &ci) {
  const auto &sm = ci.getSourceManager();
  const auto &lo = ci.getLangOpts();

  if (ret_stmt.getRetValue()) {
    std::string ret_expr =
        Lexer::getSourceText(CharSourceRange::getTokenRange(
                                 ret_stmt.getRetValue()->getSourceRange()),
                             sm, lo);
    return fmt::format("{}({}, {})", "_return",
                       immortal_function.getNameAsString(), ret_expr);
  }
  return fmt::format("{}({})", "_return_void",
                     immortal_function.getNameAsString());
}

std::string get_true_imt_thread_init(
    const clang::CallExpr &imt_thread_init_call, bool is_multi_instance_thread,
    const clang::SourceManager &manager, const clang::LangOptions &opts) {
  auto arg_list = get_arg_list(imt_thread_init_call, manager, opts);
  assert(arg_list);
  if (is_multi_instance_thread) {
    return fmt::format("_imt_thread_init_multi_macro({})", arg_list.value());
  } else {
    return fmt::format("_imt_thread_init_macro({})", arg_list.value());
  }
}

size_t get_immortal_function_instances(const clang::FunctionDecl &fn_decl,
                                       const clang::CompilerInstance &ci) {

  auto &sm = ci.getSourceManager();
  auto &lo = ci.getLangOpts();
  auto &pp = ci.getPreprocessor();
  static const std::string annotation_prefix = "immortalc::fn_instances::";
  for (const auto *attr : fn_decl.attrs()) {
    if (attr->getKind() == attr::Annotate) {
      std::string str;
      llvm::raw_string_ostream out(str);
      clang::PrintingPolicy policy(lo);
      attr->printPretty(out, policy);
      std::string attribute_text(out.str());

      if (auto pos = attribute_text.find(annotation_prefix);
          pos != std::string::npos) {
        size_t number_start = pos + annotation_prefix.size();
        try {
          size_t count = std::stoul(attribute_text.substr(number_start));
          return count;
        } catch (const std::exception &e) {
          std::cerr << "Unable to parse number of instances for function "
                    << fn_decl.getNameAsString() << "\n";
          exit(1);
        }
      }
    }
  }
  return get_max_immortal_function_instances(sm, lo, pp);
}
bool immortal_function_do_whole_body_copy(const clang::FunctionDecl &fn_decl,
                                          const clang::CompilerInstance &ci) {
  auto &sm = ci.getSourceManager();
  auto &lo = ci.getLangOpts();
  for (const auto *attr : fn_decl.attrs()) {
    if (attr->getKind() == attr::Annotate) {
      auto attribute_range = Lexer::getAsCharRange(attr->getRange(), sm, lo);
      auto attribute_text =
          clang::Lexer::getSourceText(attribute_range, sm, lo);
      if (attribute_text.find("immortalc::fn_whole_body_copy") !=
          std::string::npos) {
        return true;
      }
    }
  }
  return false;
}

std::string get_immortal_function_begin_macro_call(
    const clang::FunctionDecl &immortal_function,
    const clang::CompilerInstance &ci) {
  if (get_immortal_function_instances(immortal_function, ci) > 1) {
    return fmt::format("_begin_multi({}, _id);",
                       immortal_function.getNameAsString());
  } else {
    return fmt::format("_begin({});", immortal_function.getNameAsString());
  }
}

std::string get_immortal_function_end_macro_call(
    const clang::FunctionDecl &immortal_function,
    const clang::CompilerInstance &ci) {
  if (get_immortal_function_instances(immortal_function, ci) > 1) {
    return fmt::format("_end_multi({});", immortal_function.getNameAsString());
  } else {
    return fmt::format("_end({});", immortal_function.getNameAsString());
  }
}
} // namespace imtc::utils

/*******************************************************************************
 * Private functions implementation
 *******************************************************************************/

static std::optional<std::string> get_arg_list(const CallExpr &call_expr,
                                               const SourceManager &manager,
                                               const LangOptions &opts) {
  std::string arg_list = Lexer::getSourceText(
      CharSourceRange::getTokenRange(
          find_symbol_location(call_expr.getBeginLoc(), '(', manager, opts)
              .getLocWithOffset(1),
          call_expr.getRParenLoc().getLocWithOffset(-1)),
      manager, opts);

  if (arg_list.size() && std::find_if_not(arg_list.cbegin(), arg_list.cend(),
                                          isspace) != arg_list.end()) {
    return arg_list;
  }
  return std::nullopt;
}
size_t get_max_immortal_function_instances(const clang::SourceManager &sm,
                                           const clang::LangOptions &lo,
                                           const clang::Preprocessor &pp) {
  static const std::string runtime_config_macro =
      "IMMORTALITY_MAX_THREAD_COUNT";

  auto info = pp.getIdentifierInfo(runtime_config_macro);
  auto macro_info = pp.getMacroInfo(info);
  auto range =
      Lexer::getAsCharRange(SourceRange(macro_info->getDefinitionLoc(),
                                        macro_info->getDefinitionEndLoc()),
                            sm, lo);
  auto macro_text = clang::Lexer::getSourceText(range, sm, lo);
  size_t count = 1;
  try {
    count = std::stoul(macro_text.substr(runtime_config_macro.size()));
  } catch (const std::exception &e) {
    std::cerr << "Unable to obtain value for IMMORTALITY_MAX_THREAD_COUNT";
    exit(1);
  }
  return count;
}

static size_t get_privatization_buffer_count(const clang::SourceManager &sm,
                                             const clang::LangOptions &lo,
                                             const clang::Preprocessor &pp) {
  static const std::string runtime_config_macro =
      "IMMORTALITY_PRIVATIZATION_BUFFER_COUNT";

  auto info = pp.getIdentifierInfo(runtime_config_macro);
  auto macro_info = pp.getMacroInfo(info);
  auto range =
      Lexer::getAsCharRange(SourceRange(macro_info->getDefinitionLoc(),
                                        macro_info->getDefinitionEndLoc()),
                            sm, lo);
  auto macro_text = clang::Lexer::getSourceText(range, sm, lo);
  size_t count = 1;
  try {
    count = std::stoul(macro_text.substr(runtime_config_macro.size()));
  } catch (const std::exception &e) {
    std::cerr << "Unable to obtain value for " << runtime_config_macro
              << std::endl;
    exit(1);
  }
  return count;
}
