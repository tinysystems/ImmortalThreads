/**
 * \file matchers.hpp
 * \brief Common utility matchers - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#ifndef IMMORTALC_MATCHERS_HPP_
#define IMMORTALC_MATCHERS_HPP_

#include "../context.hpp"
#include "../utils/immortal.hpp"
#include "../utils/location.hpp"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Lex/Lexer.h"
#include "llvm/ADT/BitmaskEnum.h"

#include <functional>
#include <string>
#include <vector>

namespace imtc::ast_matchers {

/**
 * Matches any argument of a call expression.
 *
 * As opposed to 'hasAnyArgument', 'forEachArgument' will cause a match for
 * each result that matches instead of only on the first one.
 */
template <typename... InnerMatcher>
inline auto forEachArgument(InnerMatcher... inner_matcher) {
  using namespace clang::ast_matchers;
  return clang::ast_matchers::forEachArgumentWithParam(inner_matcher...,
                                                       anything());
}

/// Matches if the node or any descendant matches.
/// Similar to findAll, but stops at the first match
template <typename T>
clang::ast_matchers::internal::Matcher<T>
findAny(const clang::ast_matchers::internal::Matcher<T> &Matcher) {
  using namespace clang::ast_matchers;
  return anyOf(Matcher, hasDescendant(Matcher));
}

/**
 * Shorthand matcher for the assignment operator ("=")
 */
template <typename... Args> inline auto assignment_operator(Args... args) {
  using namespace clang::ast_matchers;
  return binaryOperator(hasOperatorName("="), args...);
}

/**
 * Given a Decl node get its next sibling
 */
clang::Decl *getNeighbor(const clang::Decl &Node, clang::ASTContext &Context);

/**
 * Matches a sequence of sibling Decl nodes, each with its own matcher
 *
 * Credits: https://stackoverflow.com/a/60448045
 */
AST_MATCHER_P(clang::Decl, neighbors,
              std::vector<clang::ast_matchers::internal::Matcher<clang::Decl>>,
              inner_matchers) {
  // Node is the current tested node
  const clang::Decl *current_node = &Node;

  /*
   * Our goal is to iterate over the given matchers and match the current node
   * with the first matcher.
   *
   * Further on, we check whether the next matcher matches the neighbor/sibling
   * of the previous node.
   */
  for (auto inner_matcher : inner_matchers) {
    /**
     * The match fails if
     * * current_node is nullptr, which means that there are not enough siblings
     * to form the sequence that should be matched
     * * the inner matcher of the ith sibling doesn't match
     */
    if (current_node == nullptr or
        !inner_matcher.matches(*current_node, Finder, Builder)) {
      return false;
    }

    current_node = getNeighbor(*current_node, Finder->getASTContext());
  }

  return true;
}

AST_POLYMORPHIC_MATCHER(is_located_in_header_file,
                        AST_POLYMORPHIC_SUPPORTED_TYPES(clang::VarDecl,
                                                        clang::FunctionDecl)) {
  return utils::is_location_in_header_file(
      Node.getBeginLoc(), Finder->getASTContext().getSourceManager());
}

AST_POLYMORPHIC_MATCHER(is_declared_in_non_instrumentable_headers,
                        AST_POLYMORPHIC_SUPPORTED_TYPES(clang::VarDecl,
                                                        clang::FunctionDecl)) {
  return !ImmortalcContext::get_instance()
              .is_declared_in_non_instrumentable_headers(Node);
}

bool has_attribute_section_from_immortal_threads_fn(
    const clang::Decl &Node,
    clang::ast_matchers::internal::ASTMatchFinder *Finder,
    clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder);
AST_MATCHER(clang::Decl, has_attribute_section_from_immortal_threads) {
  return has_attribute_section_from_immortal_threads_fn(Node, Finder, Builder);
}

AST_MATCHER_P(clang::Decl, has_annotation,
              std::function<bool(const std::string &)>, annotation_matcher) {
  for (const auto *Attr : Node.attrs()) {
    if (Attr->getKind() == clang::attr::Annotate) {
      std::string str;
      llvm::raw_string_ostream out(str);
      clang::PrintingPolicy policy(
          ImmortalcContext::get_instance().get_current_CI().getLangOpts());
      Attr->printPretty(out, policy);
      if (annotation_matcher(out.str())) {
        return true;
      }
    }
  }
  return false;
}

bool is_interrupt_handler_fn(
    const clang::FunctionDecl &fn_decl,
    clang::ast_matchers::internal::ASTMatchFinder *Finder,
    clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder);
AST_MATCHER(clang::FunctionDecl, is_interrupt_handler) {
  return is_interrupt_handler_fn(Node, Finder, Builder);
}

inline auto annotated_with_immortalc_fn_ignore() {
  return has_annotation([](const auto &str) {
    return str.find("immortalc::ignore") != std::string::npos;
  });
}

inline auto annotated_with_immortalc_fn_idempotent() {
  return has_annotation([](const auto &str) {
    return str.find("immortalc::fn_idempotent") != std::string::npos;
  });
}
inline auto annotated_with_immortalc_fn_thread() {
  return has_annotation([](const auto &str) {
    return str.find("immortalc::thread") != std::string::npos;
  });
}

/**
 * Narrow a functionDecl to one that is not an immortal function
 */
inline auto is_not_immortal_function() {
  using namespace clang::ast_matchers;
  /**
   * An immortal function is a function that
   *
   * * is not annotated with custom annotations
   * * is not declare in non-instrumenteable headers.
   * * is not main (the main function cannot be immortal function)
   */
  return anyOf(isMain(), annotated_with_immortalc_fn_idempotent(),
               annotated_with_immortalc_fn_ignore(), is_interrupt_handler(),
               is_declared_in_non_instrumentable_headers());
}

constexpr char NON_IMMORTAL_FUNCTION_DECL_BINDING[] =
    "non_immortal_function_binding";

inline auto non_immortal_function_decl() {
  using namespace clang::ast_matchers;
  return functionDecl(is_not_immortal_function())
      .bind(NON_IMMORTAL_FUNCTION_DECL_BINDING);
}

/**
 * Match an immortal function declaration
 */
template <typename... InnerMatcher>
inline auto immortal_function_declaration(InnerMatcher... inner_matcher) {
  using namespace clang::ast_matchers;
  return functionDecl(unless(is_not_immortal_function()), inner_matcher...);
}

/**
 * Match an immortal function definition (so with the function body)
 */
template <typename... InnerMatcher>
inline auto immortal_function_definition(InnerMatcher... inner_matcher) {
  using namespace clang::ast_matchers;
  /**
   * An immortal function is a function that is not annotated with custom
   * annotations and is not declare in system header
   */
  return functionDecl(unless(is_not_immortal_function()), has(compoundStmt()),
                      inner_matcher...);
}

enum ImmortalFunctionMultiInstanceTypeFlags {
  DATA_COPY = 1,
  WHOLE_BODY_COPY = 1 << 1,
  LLVM_MARK_AS_BITMASK_ENUM(WHOLE_BODY_COPY)
};

AST_MATCHER_P(clang::FunctionDecl, is_multiinstance_immortal_function,
              ImmortalFunctionMultiInstanceTypeFlags, type) {
  auto &ci = ImmortalcContext::get_instance().get_current_CI();
  if (utils::get_immortal_function_instances(
          Node, ImmortalcContext::get_instance().get_current_CI()) > 1) {
    if (type & (ImmortalFunctionMultiInstanceTypeFlags::WHOLE_BODY_COPY |
                ImmortalFunctionMultiInstanceTypeFlags::DATA_COPY)) {
      return true;
    }
    if (type & ImmortalFunctionMultiInstanceTypeFlags::DATA_COPY) {
      return utils::immortal_function_do_whole_body_copy(Node, ci);
    }
    return true;
  }
  return false;
}

template <typename... InnerMatcher>
inline auto for_each_node_inside_immortal_function_definition(
    InnerMatcher... inner_matcher) {
  using namespace clang::ast_matchers;
  /**
   * An immortal function is a function that is not annotated with custom
   * annotations and is not declare in system header
   */
  return functionDecl(unless(is_not_immortal_function()),
                      has(compoundStmt(forEachDescendant(inner_matcher...))));
}

/**
 * Match a call to an immortal function
 */
template <typename InnerMatcher = decltype(clang::ast_matchers::anything())>
inline auto immortal_function_call_expr(
    InnerMatcher call_expr_inner_matcher = clang::ast_matchers::anything()) {
  using namespace clang::ast_matchers;
  return callExpr(callee(immortal_function_declaration()),
                  call_expr_inner_matcher);
}

/**
 * Matches immortal function call expression that are not direct RHS
 * of an assignment.
 *
 * \param [in] binding the binding string used to bind the immortal function
 * call expression
 *
 * When matching nested expressions that require instrumentation, like the
 * following example
 *
 * ```c
 * a = b = foo();
 * ```
 * there are two possibilities:
 *
 * * treat `foo()` as something to be instrumented
 * * treat `b = foo()` as something to be instrumented
 *
 * In general, the more we can instrument in a single shot, the better.
 */
inline auto immortal_function_call_expr_not_rhs_assignment(
    const std::string &call_expr_binding) {
  using namespace clang::ast_matchers;
  /**
   * See
   * https://stackoverflow.com/questions/66080782/clang-ast-matcher-whats-the-best-way-to-match-the-current-matcher-over-hasparen
   */
  return immortal_function_call_expr(
      allOf(immortal_function_call_expr().bind(call_expr_binding),
            unless(hasParent(binaryOperator(
                hasOperatorName("="),
                hasRHS(callExpr(equalsBoundNode(call_expr_binding))))))));
}

/**
 * Default binding id for #non_volatile_var_reference.
 * The non volatile variable declaration node is bound.
 */
constexpr char NON_VOLATILE_VAR_DECL_BINDING[] = "nv_var_decl_binding";
/**
 * Match a varDecl that is the declaration of an immortal variable
 *
 * The following cases are considered immortal value declarations:
 *
 * * Declaration in a immortal function that has not already a section
 * * Declaration in global context that has not already a section and is
 * not declared non a non-instrumentable header file
 * * Declaration already in a section, where the section is the expasion
 * of macros provided by ImmortalThreads
 */
template <typename... InnerMatcher>
inline auto immortal_var_decl(InnerMatcher... inner_matcher) {
  using namespace clang::ast_matchers;
  return varDecl(
      inner_matcher...,
      anyOf(hasAncestor(immortal_function_definition()),
            has_attribute_section_from_immortal_threads(),
            allOf(hasDeclContext(translationUnitDecl()),
                  unless(is_declared_in_non_instrumentable_headers()))));
}
/**
 * Match a reference to a non volatile variable
 *
 * \param [in] var_declaration_binding ID used to match the matched declaration
 * node of the referenced non volatile variable
 */
inline auto
non_volatile_var_reference(const std::string &var_declaration_binding =
                               NON_VOLATILE_VAR_DECL_BINDING) {
  using namespace clang::ast_matchers;
  return declRefExpr(
      to(varDecl(immortal_var_decl()).bind(var_declaration_binding)));
}

/**
 * Match a simple assignment (using the '=' operator) to a non volatile variable
 */
template <typename InnerMatcher = decltype(clang::ast_matchers::anything()),
          typename... Args>
inline auto assignment_to_non_volatile_var(
    InnerMatcher inner_matcher = clang::ast_matchers::anything(),
    Args... args) {
  using namespace clang::ast_matchers;
  return binaryOperator(hasOperatorName("="),
                        hasLHS(findAny(non_volatile_var_reference(args...))),
                        inner_matcher);
}

/**
 * Match a compound assignment to a non volatile variable
 */
template <typename... Args>
inline auto compound_assignment_to_non_volatile_var(Args... args) {
  using namespace clang::ast_matchers;
  return binaryOperator(isAssignmentOperator(), unless(hasOperatorName("=")),
                        hasLHS(findAny(non_volatile_var_reference(args...))));
}

/**
 * Match an increment/decrement to a non volatile variable
 */
template <typename... Args>
inline auto increment_decrement_non_volatile_var(Args... args) {
  using namespace clang::ast_matchers;
  return unaryOperator(anyOf(hasOperatorName("++"), hasOperatorName("--")),
                       hasDescendant(non_volatile_var_reference(args...)));
}

/**
 * Default binding id for #write_non_volatile_var.
 * The expression that performs a write to a non volatile variable is bound.
 */
constexpr char NON_VOLATILE_VAR_WRITE_EXPR_BINDING[] = "nv_var_decl_binding";
/**
 * Match any kind of write to a non volatile variable
 */
inline auto any_write_non_volatile_var(
    const std::string &non_volatile_var_write_expr_binding =
        NON_VOLATILE_VAR_WRITE_EXPR_BINDING) {
  using namespace clang::ast_matchers;
  return anyOf(hasDescendant(increment_decrement_non_volatile_var().bind(
                   non_volatile_var_write_expr_binding)),
               increment_decrement_non_volatile_var().bind(
                   non_volatile_var_write_expr_binding),
               hasDescendant(assignment_to_non_volatile_var().bind(
                   non_volatile_var_write_expr_binding)),
               assignment_to_non_volatile_var().bind(
                   non_volatile_var_write_expr_binding),
               hasDescendant(compound_assignment_to_non_volatile_var().bind(
                   non_volatile_var_write_expr_binding)),
               compound_assignment_to_non_volatile_var().bind(
                   non_volatile_var_write_expr_binding));
}

/**
 * Same as #any_write_non, but this function will generate
 * a match result for each matched expression
 */
inline auto each_write_non_volatile_var(
    const std::string &non_volatile_var_write_expr_binding =
        NON_VOLATILE_VAR_WRITE_EXPR_BINDING) {
  using namespace clang::ast_matchers;
  return eachOf(
      forEachDescendant(increment_decrement_non_volatile_var().bind(
          non_volatile_var_write_expr_binding)),
      increment_decrement_non_volatile_var().bind(
          non_volatile_var_write_expr_binding),
      forEachDescendant(assignment_to_non_volatile_var().bind(
          non_volatile_var_write_expr_binding)),
      assignment_to_non_volatile_var().bind(
          non_volatile_var_write_expr_binding),
      forEachDescendant(compound_assignment_to_non_volatile_var().bind(
          non_volatile_var_write_expr_binding)),
      compound_assignment_to_non_volatile_var().bind(
          non_volatile_var_write_expr_binding));
}

/**
 * Default binding id for #require_instrumentation_expr.
 * The expression that requires instrumentation is bound.
 */
constexpr char EXPR_REQUIRING_INSTRUMENTATION[] =
    "require_instrumentation_expr_binding";

/**
 * Match a expression that must be instrumented (e.g. contains assignments,
 * increment/decrement, function calls, etc.)
 *
 * \param [in] expr_binding ID used to bind to the matched expression that
 * requires instrumentation. The bound node can then be retrieved as type Expr.
 * Possible subtypes are CallExpr, BinaryOperator, UnaryOperator.
 */
inline auto any_expr_requiring_instrumentation(
    const std::string &expr_binding = EXPR_REQUIRING_INSTRUMENTATION) {
  using namespace clang::ast_matchers;
  /**
   * What kind of expressions need to be instrumented?
   *
   * * A function call needs to be instrumented.
   * * A write to a non volatile variable needs to be instrumented.
   */
  return anyOf(immortal_function_call_expr_not_rhs_assignment(expr_binding),
               hasDescendant(immortal_function_call_expr_not_rhs_assignment(
                   expr_binding)),
               any_write_non_volatile_var(expr_binding));
}

inline auto expr_requiring_instrumentation(
    const std::string &expr_binding = EXPR_REQUIRING_INSTRUMENTATION) {
  using namespace clang::ast_matchers;
  /**
   * What kind of expressions need to be instrumented?
   *
   * * A function call needs to be instrumented.
   * * A write to a non volatile variable needs to be instrumented.
   */
  return expr(
      anyOf(immortal_function_call_expr_not_rhs_assignment(expr_binding),
            any_write_non_volatile_var(expr_binding)));
}

/**
 * Same as #any_expr_requiring_instrumentation, but this function will generate
 * a match result for each matched expression
 */
inline auto each_expr_requiring_instrumentation(
    const std::string &expr_binding = EXPR_REQUIRING_INSTRUMENTATION) {
  using namespace clang::ast_matchers;
  return eachOf(
      immortal_function_call_expr_not_rhs_assignment(expr_binding),
      forEachDescendant(
          immortal_function_call_expr_not_rhs_assignment(expr_binding)),
      each_write_non_volatile_var(expr_binding));
}

} // namespace imtc::ast_matchers

#endif /* ifndef IMMORTALC_MATCHERS_HPP_ */
