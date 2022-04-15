/**
 * \file control_stmt.cpp
 * \brief Instrument control statements - implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 *
 * This pass applies the following transformations on control statements:
 *
 * * Move the init statement of for loops out from the for loop statement
 * * Move the increment statement of for loops inside the for loop body
 * * Move if statement conditional expressions that require instrumentation
 * out from the if statement. In more details:
 *   * Create a new scope that encloses the if statement
 *   * Create a temporary boolean variable that is initialized with the if
 *     statement's conditional expression.
 *   * Use the temporary boolean variable as conditional expression for the if
 *     statement
 * * For while statements, perform a transformation similar to what is done for
 *   the if statement, but perform assignment of the temporary boolean variable
 *   also as last statement in the while body
 * * For do-while statements, perform a transformation similar to what is done
 *   for the while statement, but don't perform the initialization of temporary
 *   boolean variable with the do-while's condition expression. The do-while's
 *   condition expression is evaluated only at the end of the first iteration.
 */
#include "control_stmt.hpp"

#include "matchers/matchers.hpp"
#include "utils/error.hpp"
#include "utils/immortal.hpp"
#include "utils/location.hpp"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/Lex/Lexer.h"

#include "fmt/core.h"

#include <variant>

using namespace imtc;
using namespace imtc::ast_matchers;
using namespace imtc::utils;

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

/**
 * Variable name that we use for the temporary variable that holds the value of
 * the controlling expression of selection and iteration statements
 */
static constexpr char CONTROL_EXPR_TMP_VAR[] = "condition_tmp";
static constexpr char FOR_STMT_BINDING[] = "for_stmt";
static constexpr char WHILE_STMT_BINDING[] = "while_stmt";
/**
 * Binding name for `for` loop init statements that need instrumentation
 */
static constexpr char FOR_STMT_INIT_STMT[] = "for_stmt_init_stmt";
static constexpr char FOR_STMT_COND_EXPR_WITH_NV_VAR_WRITE[] =
    "for_stmt_nv_var_write_in_cond";
static constexpr char FOR_STMT_INCR_STMT_WITH_NV_VAR_WRITE[] =
    "for_stmt_nv_var_write_in_incr";
static constexpr char IF_STMT_BINDING[] = "if_stmt";
static constexpr char SWITCH_STMT_BINDING[] = "switch_stmt";

/**
 * Handle loop condition expression that must be instrumented.
 * They need to be decomposed into separate statements.
 */
template <typename T>
void ControlStmtPass::handle_loop_condition_to_be_instrumented(
    const T *loop_stmt, const MatchFinder::MatchResult &result) {
  static_assert(std::is_base_of<ForStmt, T>::value ||
                    std::is_base_of<DoStmt, T>::value ||
                    std::is_base_of<WhileStmt, T>::value,
                "T must be a loop statement");

  const Expr *cond_expr = loop_stmt->getCond();
  std::string cond_expr_str = Lexer::getSourceText(
      CharSourceRange::getTokenRange(cond_expr->getSourceRange()),
      result.Context->getSourceManager(), result.Context->getLangOpts());

  auto err =
      this->replace(result.Context->getSourceManager(),
                    CharSourceRange::getTokenRange(cond_expr->getSourceRange()),
                    CONTROL_EXPR_TMP_VAR);
  assert_no_error(err);

  /*************************************************************************
   * create new scope and declare a temporary variable
   *************************************************************************/
  std::string new_loop_start =
      fmt::format("{{\nchar {};\n", CONTROL_EXPR_TMP_VAR, CONTROL_EXPR_TMP_VAR,
                  cond_expr_str);
  if (!std::is_base_of<DoStmt, T>::value) {
    /*
     * For do-while, we don't want to initialize CONDITION_TMP_VAR with
     * cond_expr, since that would cause the side-effects of cond_expr to be
     * occur (i.e. write to non-volatile variables).
     */
    new_loop_start +=
        fmt::format("{} = ({});\n", CONTROL_EXPR_TMP_VAR, cond_expr_str);
  }
  err = this->insert(result.Context->getSourceManager(),
                     loop_stmt->getBeginLoc().getLocWithOffset(-1),
                     new_loop_start, true);
  assert_no_error(err);

  {
    auto after_loop_end_loc = loop_stmt->getEndLoc().getLocWithOffset(1);
    if (std::is_base_of<DoStmt, T>::value) {
      /**
       * Unfortunately, DoStmt->getEndLoc() doesn't include the ';' character of
       * the do-while construct.
       * We need to scan what comes after DoStmt->getEndLoc() until we find the
       * ';'.
       */
      after_loop_end_loc =
          find_symbol_location(loop_stmt->getEndLoc(), ';',
                               result.Context->getSourceManager(),
                               result.Context->getLangOpts())
              .getLocWithOffset(1);
    }
    err = this->insert(result.Context->getSourceManager(), after_loop_end_loc,
                       "\n}", true);
    assert_no_error(err);
  }

  // put the condition expression at the end of the for-loop body
  err = this->insert(
      result.Context->getSourceManager(), loop_stmt->getBody()->getEndLoc(),
      fmt::format("{} = {};\n", CONTROL_EXPR_TMP_VAR, cond_expr_str), false);
  assert_no_error(err);
}

void ControlStmtPass::decompose_while(const MatchFinder::MatchResult &result) {
  if (const auto *while_stmt =
          result.Nodes.getNodeAs<WhileStmt>(WHILE_STMT_BINDING)) {
    handle_loop_condition_to_be_instrumented(while_stmt, result);
  }
  if (const auto *do_while_stmt =
          result.Nodes.getNodeAs<DoStmt>(WHILE_STMT_BINDING)) {
    handle_loop_condition_to_be_instrumented(do_while_stmt, result);
  }
}

void ControlStmtPass::decompose_for(const MatchFinder::MatchResult &result) {
  const auto *for_stmt = result.Nodes.getNodeAs<ForStmt>(FOR_STMT_BINDING);

  /**
   * We rely on the fact that
   *
   * > The order of matches is guaranteed to be equivalent to doing a pre-order
   * traversal on the AST, and applying the matchers in the order in which
   * they were added to the MatchFinder.
   *
   * Be careful with the use of InsertAfter flag when calling this->insert()
   */
  if (auto binding_map = result.Nodes.getMap();
      binding_map.find(FOR_STMT_INIT_STMT) != binding_map.end()) {
    /// If this binding exists, then for some reason the init statement needs to
    /// be instrumented. We don't care for what reason. We just bring it out of
    /// the for statement.
    auto init_stmt = for_stmt->getInit();

    /// Depending of what constitutes the init_stmt, its range that we obtain
    /// may or may not contain the semicolon.
    /// For example in `for (int i = 0;;)`, the init statement is a declaration
    /// statement and the semicolon is included in the range
    /// OTOH in `for (i = 0;;)`, the init statement is an expression and the
    /// semicolon is not included in the range
    /// So what we do is simply to search for the semicolon and manually create
    /// a range that we are sure contains the semicolon.
    auto semicolon_loc = find_symbol_location(
        init_stmt->getBeginLoc(), ';', result.Context->getSourceManager(),
        result.Context->getLangOpts());
    auto init_stmt_range =
        CharSourceRange::getTokenRange(init_stmt->getBeginLoc(), semicolon_loc);

    std::string for_init_stmt_str = Lexer::getSourceText(
        init_stmt_range, result.Context->getSourceManager(),
        result.Context->getLangOpts());

    // remove init statement
    auto err =
        this->replace(result.Context->getSourceManager(), init_stmt_range, ";");
    assert_no_error(err);

    /*
     * create new scope and put the for loop init statement at the
     * beginning of the new scope
     */
    err = this->insert(result.Context->getSourceManager(),
                       for_stmt->getBeginLoc().getLocWithOffset(-1),
                       fmt::format("{{\n{}\n", for_init_stmt_str), true);
    assert_no_error(err);
    err = this->insert(result.Context->getSourceManager(),
                       for_stmt->getEndLoc().getLocWithOffset(1),
                       std::string("\n}"), true);
    assert_no_error(err);
  }

  if (result.Nodes.getNodeAs<Expr>(FOR_STMT_COND_EXPR_WITH_NV_VAR_WRITE)) {
    handle_loop_condition_to_be_instrumented(for_stmt, result);
  }

  if (result.Nodes.getNodeAs<Expr>(FOR_STMT_INCR_STMT_WITH_NV_VAR_WRITE)) {
    auto *inc_stmt = for_stmt->getInc();

    std::string inc_stmt_str = Lexer::getSourceText(
        CharSourceRange::getTokenRange(inc_stmt->getSourceRange()),
        result.Context->getSourceManager(), result.Context->getLangOpts());

    // remove the increment statement
    auto err = this->replace(
        result.Context->getSourceManager(),
        CharSourceRange::getTokenRange(inc_stmt->getSourceRange()), "");
    assert_no_error(err);

    // put the increment statement at the end of the for-loop body
    err = this->insert(result.Context->getSourceManager(),
                       for_stmt->getBody()->getEndLoc(), inc_stmt_str + ";\n",
                       false);
    assert_no_error(err);
  }
}

void ControlStmtPass::decompose_if(const MatchFinder::MatchResult &result) {
  auto if_stmt = result.Nodes.getNodeAs<IfStmt>(IF_STMT_BINDING);
  assert(if_stmt);

  std::string cond_expr_str = this->get_ast_node_string(*if_stmt->getCond());

  // create new scope and define the temporary boolean variable
  std::string if_start =
      fmt::format("{{\nchar {} = ({});\n", CONTROL_EXPR_TMP_VAR, cond_expr_str);
  auto err = this->insert(result.Context->getSourceManager(),
                          if_stmt->getBeginLoc(), if_start, false);
  assert_no_error(err);

  // replace condition expression with temporary boolean variable
  err = this->replace(
      result.Context->getSourceManager(),
      CharSourceRange::getTokenRange(if_stmt->getCond()->getSourceRange()),
      CONTROL_EXPR_TMP_VAR);
  assert_no_error(err);

  // close new scope
  err = this->insert(result.Context->getSourceManager(),
                     if_stmt->getEndLoc().getLocWithOffset(1), "\n}", true);
  assert_no_error(err);
}

void ControlStmtPass::decompose_switch(const MatchFinder::MatchResult &result) {
  auto switch_stmt = result.Nodes.getNodeAs<SwitchStmt>(SWITCH_STMT_BINDING);
  assert(switch_stmt);

  std::string cond_expr_str =
      this->get_ast_node_string(*switch_stmt->getCond());
  std::string cond_expr_type = switch_stmt->getCond()->getType().getAsString();

  // create new scope and define the temporary boolean variable
  std::string switch_start = fmt::format("{{\n{} {} = ({});\n", cond_expr_type,
                                         CONTROL_EXPR_TMP_VAR, cond_expr_str);
  auto err = this->insert(result.Context->getSourceManager(),
                          switch_stmt->getBeginLoc(), switch_start, false);
  assert_no_error(err);

  // replace condition expression with temporary boolean variable
  err = this->replace(
      result.Context->getSourceManager(),
      CharSourceRange::getTokenRange(switch_stmt->getCond()->getSourceRange()),
      CONTROL_EXPR_TMP_VAR);
  assert_no_error(err);

  // close new scope
  err = this->insert(result.Context->getSourceManager(),
                     switch_stmt->getEndLoc().getLocWithOffset(1), "\n}", true);
  assert_no_error(err);
}

void ControlStmtPass::register_matchers(MatchFinder &finder) {
  /**
   * For statement that require instrumentation
   *
   * A `for` statement requires instrumentation if any of the following
   * conditions is true:
   *
   * * the init statement:
   *   * contains a write to a non-volatile variable
   *   * contains a declaration (_def makes variable become static, static
   * variables cannot be declared in for loop's init statement)
   * * the condition expression contains writes to a non volatile variable
   * * the increment expression contains writes to a non volatile variable
   */
  auto require_instrumentation_for_stmt =
      for_each_node_inside_immortal_function_definition(
          forStmt(eachOf(hasLoopInit(anyOf(declStmt().bind(FOR_STMT_INIT_STMT),
                                           any_expr_requiring_instrumentation(
                                               FOR_STMT_INIT_STMT))),
                         hasCondition(any_expr_requiring_instrumentation(
                             FOR_STMT_COND_EXPR_WITH_NV_VAR_WRITE)),
                         hasIncrement(any_expr_requiring_instrumentation(
                             FOR_STMT_INCR_STMT_WITH_NV_VAR_WRITE))))
              .bind(FOR_STMT_BINDING));
  finder.addMatcher(require_instrumentation_for_stmt, this);

  /**
   * While statement that require instrumentation.
   */
  auto require_instrumentation_while_stmt =
      for_each_node_inside_immortal_function_definition(
          whileStmt(hasCondition(any_expr_requiring_instrumentation()))
              .bind(WHILE_STMT_BINDING));
  finder.addMatcher(require_instrumentation_while_stmt, this);

  /**
   * Do-while statement that require instrumentation.
   */
  auto require_instrumentation_do_while_stmt =
      for_each_node_inside_immortal_function_definition(
          doStmt(hasCondition(any_expr_requiring_instrumentation()))
              .bind(WHILE_STMT_BINDING));
  finder.addMatcher(require_instrumentation_do_while_stmt, this);

  /**
   * If statement that has a controlling expression that requires
   * instrumentation.
   */
  auto require_instrumentation_if_stmt =
      for_each_node_inside_immortal_function_definition(
          ifStmt(hasCondition(any_expr_requiring_instrumentation()))
              .bind(IF_STMT_BINDING));
  finder.addMatcher(require_instrumentation_if_stmt, this);

  /**
   * Switch statement that has a controlling expression that requires
   * instrumentation.
   */
  auto require_instrumentation_switch_stmt =
      for_each_node_inside_immortal_function_definition(
          switchStmt(hasCondition(any_expr_requiring_instrumentation()))
              .bind(SWITCH_STMT_BINDING));
  finder.addMatcher(require_instrumentation_switch_stmt, this);
}

void ControlStmtPass::run(const MatchFinder::MatchResult &result) {
  auto bound_map = result.Nodes.getMap();
  if (bound_map.find(IF_STMT_BINDING) != bound_map.end()) {
    this->decompose_if(result);
  }
  if (bound_map.find(SWITCH_STMT_BINDING) != bound_map.end()) {
    this->decompose_switch(result);
  }
  if (bound_map.find(FOR_STMT_BINDING) != bound_map.end()) {
    this->decompose_for(result);
  }
  if (bound_map.find(WHILE_STMT_BINDING) != bound_map.end()) {
    this->decompose_while(result);
  }
}
