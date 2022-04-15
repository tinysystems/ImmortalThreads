/**
 * \file matchers.hpp
 * \brief Common utility matchers - implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#include "matchers.hpp"

#include "../context.hpp"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/Lex/Lexer.h"

using namespace imtc::ast_matchers;
using namespace imtc;

using namespace clang;
using namespace clang::ast_matchers;

Decl *imtc::ast_matchers::getNeighbor(const Decl &node, ASTContext &context) {
  auto parents = context.getParents(node);
  if (parents.size() != 1) {
    return nullptr;
  }

  /*
   * As for now we are only interested in cases where Decl is a FunctionDecl (to
   * match an immortal function), which in standard C can only have
   * TranslationUnitDecl as its parent.
   */
  if (auto *parent = parents[0].get<TranslationUnitDecl>()) {
    auto neighbour = std::adjacent_find(
        parent->decls_begin(), parent->decls_end(),
        [&node](const auto *top, const auto *bottom) { return top == &node; });

    if (neighbour != parent->decls_end()) {
      return *std::next(neighbour);
    }
  }

  return nullptr;
}

bool imtc::ast_matchers::has_attribute_section_from_immortal_threads_fn(
    const clang::Decl &var_decl,
    clang::ast_matchers::internal::ASTMatchFinder *Finder,
    clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) {
  static const char *immortal_threads_mem_macros[] = {
      "__fram",
      "__ro_fram",
      "__hifram",
      "__ro_hifram",
  };

  for (const auto *attr : var_decl.attrs()) {
    if (attr->getKind() == attr::Section) {
      auto &sm = Finder->getASTContext().getSourceManager();
      auto &lo = Finder->getASTContext().getLangOpts();

      auto &imt_context = ImmortalcContext::get_instance();
      auto &pp = imt_context.get_current_CI().getPreprocessor();

      auto attribute_range = Lexer::getAsCharRange(attr->getRange(), sm, lo);
      auto attribute_text =
          clang::Lexer::getSourceText(attribute_range, sm, lo);

      for (auto mem_macro : immortal_threads_mem_macros) {
        auto info = pp.getIdentifierInfo(mem_macro);
        auto macro_info = pp.getMacroInfo(info);
        auto range = Lexer::getAsCharRange(
            SourceRange(macro_info->getDefinitionLoc(),
                        macro_info->getDefinitionEndLoc()),
            sm, lo);
        auto macro_text = clang::Lexer::getSourceText(range, sm, lo);
        if (macro_text.find(attribute_text) != std::string::npos) {
          return true;
        }
      }
    }
  }
  return false;
}

bool imtc::ast_matchers::is_interrupt_handler_fn(
    const clang::FunctionDecl &fn_decl,
    clang::ast_matchers::internal::ASTMatchFinder *Finder,
    clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) {

  auto &sm = Finder->getASTContext().getSourceManager();
  auto &lo = Finder->getASTContext().getLangOpts();

#if 0
  // checking the attributes to match interrupt handler doesn't work on MSP430
  // Clang still treats the attribute as X86 and errors out, thus not saving
  // the attribute in the AST, so we won't find the attribute " __attribute__((interrupt("x")))"
  for (const auto *attr : fn_decl.attrs()) {
    auto attribute_range = Lexer::getAsCharRange(attr->getRange(), sm, lo);
    auto attribute_text = clang::Lexer::getSourceText(attribute_range, sm, lo);
    // TODO: figure out more robust way to detect an interrupt handler
    if (attribute_text.find("interrupt") != std::string::npos) {
      return true;
    }
  }
  return false;
#endif

  // quite hacky...
  return clang::Lexer::getSourceText(
             clang::CharSourceRange::getTokenRange(fn_decl.getSourceRange()),
             sm, lo)
             .str()
             .find("__attribute__((interrupt(") != std::string::npos;
}
