/**
 * \file expression_decomposition.cpp
 * \brief Expression decomposition handlers - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */
#include "expression_decomposition.hpp"
#include "matchers/matchers.hpp"
#include "utils/ast.hpp"
#include "utils/error.hpp"
#include "utils/expr.hpp"
#include "utils/immortal.hpp"
#include "utils/location.hpp"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Lexer.h"

#include "fmt/core.h"

#include <algorithm>
#include <stack>
#include <unordered_map>

using namespace imtc;
using namespace imtc::utils;
using namespace imtc::ast_matchers;

using namespace clang;
using namespace clang::ast_type_traits;
using namespace clang::ast_matchers;

static constexpr char STMT_ANCESTOR[] = "stmt_ancestor";

struct ScopedIncrement {
  explicit ScopedIncrement(size_t &depth) : depth(depth) { ++depth; }
  ~ScopedIncrement() { --depth; }

private:
  size_t &depth;
};

/**
 * An instance of this class pops the pushed elements once it goes out of scope.
 */
template <typename T> struct ScopedStack {
  explicit ScopedStack(std::stack<T> &stack) : stack_(stack) {}
  void push(T element) {
    this->stack_.push(element);
    ++this->pushed_cnt_;
  }
  ~ScopedStack() {
    for (size_t i = 0; i < pushed_cnt_; ++i) {
      stack_.pop();
    }
  }

private:
  size_t pushed_cnt_ = 0;
  std::stack<T> &stack_;
};

struct SourceText {
  SourceLocation start;
  std::string text;
};

struct Replacement {
  SourceLocation start;
  std::string original_text;
  std::string new_text;
};

bool are_replacements_orthogonal(const std::vector<Replacement> &replacements) {
  std::vector<size_t> sorted_indexes;
  sorted_indexes.reserve(replacements.size());
  for (size_t i = 0; i < replacements.size(); ++i) {
    sorted_indexes[i] = i;
  }
  std::sort(sorted_indexes.begin(), sorted_indexes.end(),
            [replacements](auto lhs, auto rhs) {
              return replacements[lhs].start < replacements[rhs].start;
            });
  for (auto it = sorted_indexes.cbegin(); it != sorted_indexes.cend(); ++it) {
    if (it + 1 != sorted_indexes.cend()) {
      auto &current = replacements[*it];
      auto &next = replacements[*it + 1];
      if (current.start.getLocWithOffset(current.original_text.size()) >=
          next.start) {
        return false;
      }
    }
  }
  return true;
}

bool is_replacement_out_of_range(const SourceText &text,
                                 const Replacement &replacement) {
  return replacement.start < text.start ||
         (replacement.start.getLocWithOffset(replacement.original_text.size()) >
          (text.start.getLocWithOffset(text.text.size())));
}

void filter_out_of_range_replacements(const SourceText &text,
                                      std::vector<Replacement> &replacements) {
  replacements.erase(std::remove_if(replacements.begin(), replacements.end(),
                                    [&text](Replacement &replacement) {
                                      return is_replacement_out_of_range(
                                          text, replacement);
                                    }),
                     replacements.end());
}

/**
 * Given a text in a certain location, apply orthogonal replacements inside the
 * text.
 */
std::string
apply_orthogonal_replacements(const SourceText &a_text,
                              std::vector<Replacement> &a_replacements) {
  assert(are_replacements_orthogonal(a_replacements));
  for (auto &replacement : a_replacements) {
    assert(!is_replacement_out_of_range(a_text, replacement));
  }
  /**
   * For efficiency, here we break the encapsulation of the SourceLocation
   * class.
   * We assume that the raw encoding of SourceLocation are effectively simple
   * offsets
   */
  struct SourceTextExplicitOffset {
    size_t start;
    std::string text;
  };
  struct ReplacementWithExplicitOffset {
    size_t start;
    std::string original_text;
    std::string new_text;
  };

  SourceTextExplicitOffset text;
  text.start = a_text.start.getRawEncoding();
  text.text = a_text.text;

  std::vector<ReplacementWithExplicitOffset> replacements;
  replacements.reserve(a_replacements.size());
  std::transform(a_replacements.cbegin(), a_replacements.cend(),
                 std::back_inserter(replacements), [](const Replacement &r) {
                   ReplacementWithExplicitOffset change;
                   change.start = r.start.getRawEncoding();
                   change.new_text = r.new_text;
                   change.original_text = r.original_text;
                   return change;
                 });

  std::sort(replacements.begin(), replacements.end(),
            [](const auto &lhs, const auto &rhs) -> bool {
              return lhs.start < rhs.start;
            });

  {
    int32_t offset_change = 0;
    for (auto it = replacements.begin(); it != replacements.end(); ++it) {
      int32_t offset = it->start - text.start + offset_change;
      text.text.erase(offset, it->original_text.size());
      text.text.insert(offset, it->new_text);
      offset_change += it->new_text.size() - it->original_text.size();
    }
  }
  return text.text;
}

/**
 * Information about a decomposed expression
 */
struct DecomposedExpression {
  /**
   * The expression node in the AST
   */
  const Expr *expression = NULL;
  /**
   * Expression that is ancestor of the current expression in the AST
   * and that in turn need also to be decomposed
   */
  DecomposedExpression *parent = NULL;
  /**
   * Expressions that are descendants of the current expression in the AST
   * and that in turn need also to be decomposed
   */
  std::vector<DecomposedExpression *> children;
  /**
   * This expression is the ith expression that has been met during the
   * post-order traversal
   */
  size_t traversal_ith = 0;
  /**
   * The relative depth of the expression in the AST
   */
  size_t depth = 0;
  /**
   * The text that should be used in the parent expression in place of the
   * expression that is being decomposed
   */
  std::string placeholder;
  /**
   * The content of the expression
   */
  std::string content;
  /**
   * Whether the decomposition of the expression causes the creation of
   * temporary variables
   */
  bool creates_tmp_var = false;
};

/**
 * A RecursiveASTVisitor that we use to recursively decompose nested expressions
 * that must be instrumented into statements.
 */
class NestedExpressionDecompositionVisitor
    : public RecursiveASTVisitor<NestedExpressionDecompositionVisitor> {

  typedef RecursiveASTVisitor<NestedExpressionDecompositionVisitor> VisitorBase;

public:
  NestedExpressionDecompositionVisitor(const Rewriter &rewriter)
      : rewriter_(rewriter) {}

  bool TraverseStmt(Stmt *stmt) {
    ScopedIncrement scoped_depth(this->current_depth_);
    ScopedStack scoped_stack(this->current_ancestors_);
    DecomposedExpression *parent = this->current_ancestors_.empty()
                                       ? NULL
                                       : this->current_ancestors_.top();

    auto found_decompose_expr =
        std::find_if(this->decomposed_expressions_.begin(),
                     this->decomposed_expressions_.end(),
                     [&stmt](auto expr) { return stmt == expr.expression; });
    if (found_decompose_expr != this->decomposed_expressions_.end()) {
      scoped_stack.push(found_decompose_expr.base());
    }

    bool res = VisitorBase::TraverseStmt(stmt);

    if (found_decompose_expr != this->decomposed_expressions_.end()) {
      this->handle_expression(*found_decompose_expr, parent);
    }
    return res;
  }

  /**
   * Traverse the AST starting from \p ancestor_stmt and decompose the
   * expressions in \p nested_expressions_to_be_decomposed.
   */
  std::vector<DecomposedExpression> traverseAndDecompose(
      Stmt *ancestor_stmt,
      std::vector<const Expr *> nested_expressions_to_be_decomposed) {

    this->decomposed_expressions_.clear();
    this->decomposed_expressions_.reserve(
        nested_expressions_to_be_decomposed.size());

    for (auto expr : nested_expressions_to_be_decomposed) {
      DecomposedExpression e;
      e.expression = expr;
      this->decomposed_expressions_.push_back(e);
    }

    this->tmp_var_counter_ = 0;
    this->met_expressions_cnt_ = 0;
    this->current_depth_ = 0;
    this->current_ancestors_ = {};
    this->TraverseStmt(ancestor_stmt);

    return this->decomposed_expressions_;
  }

private:
  /**
   * Handle the decomposition of the expression that is being visited.
   * Note that this method is invoked in a post-order traversal
   */
  void handle_expression(DecomposedExpression &expr,
                         DecomposedExpression *parent) {
    expr.depth = this->current_depth_;
    expr.traversal_ith = this->met_expressions_cnt_++;
    expr.content = Lexer::getSourceText(
        CharSourceRange::getTokenRange(expr.expression->getSourceRange()),
        this->rewriter_.getSourceMgr(), this->rewriter_.getLangOpts());

    /*
     * replace children expressions with their new placeholders
     */
    std::vector<Replacement> children_replacements;
    if (expr.children.size() != 0) {
      children_replacements.reserve(expr.children.size());
      std::transform(expr.children.cbegin(), expr.children.cend(),
                     std::back_inserter(children_replacements),
                     [this](const DecomposedExpression *child) {
                       Replacement change;
                       change.start = child->expression->getBeginLoc();
                       change.original_text = Lexer::getSourceText(
                           CharSourceRange::getTokenRange(
                               child->expression->getSourceRange()),
                           this->rewriter_.getSourceMgr(),
                           this->rewriter_.getLangOpts());
                       change.new_text = child->placeholder;
                       return change;
                     });
      SourceText original;
      original.start = expr.expression->getBeginLoc();
      original.text = expr.content;
      expr.content =
          apply_orthogonal_replacements(original, children_replacements);
    }

    std::string tmp_var;
    if (does_expr_create_tmp_var(expr.expression)) {
      tmp_var = this->get_unique_tmp_var_name();
      std::string type =
          get_type_of_the_expression(expr.expression).getAsString();
      expr.content = expr_tmp_var_assignment(expr.expression, expr.content,
                                             tmp_var, this->rewriter_);
    }

    if (does_expr_have_value(expr.expression)) {
      if (does_expr_create_tmp_var(expr.expression)) {
        assert(tmp_var != "");
        expr.placeholder = tmp_var;
        expr.creates_tmp_var = true;
      } else {
        Expr *value = get_value_of_the_expression(expr.expression);
        std::string value_text = Lexer::getSourceText(
            CharSourceRange::getTokenRange(value->getSourceRange()),
            this->rewriter_.getSourceMgr(), this->rewriter_.getLangOpts());
        expr.placeholder = value_text;
        if (expr.children.size() != 0) {
          SourceText original;
          original.start = value->getBeginLoc();
          original.text = value_text;
          filter_out_of_range_replacements(original, children_replacements);
          expr.placeholder =
              apply_orthogonal_replacements(original, children_replacements);
        }
      }
    }

    if (parent) {
      expr.parent = parent;
      parent->children.push_back(&expr);
    }
  }

  std::string get_unique_tmp_var_name() {
    return std::string("tmp_var_") + std::to_string(this->tmp_var_counter_++);
  }

  const Rewriter &rewriter_;
  std::vector<DecomposedExpression> decomposed_expressions_;
  size_t current_depth_;
  /**
   * Counter of how many expressions to be decomposed have been met so far
   */
  size_t met_expressions_cnt_;
  size_t tmp_var_counter_;
  std::stack<DecomposedExpression *> current_ancestors_;
};

namespace imtc {
void ExpressionDecompositionPass::register_matchers(
    clang::ast_matchers::MatchFinder &finder) {

  /**
   * We need to decompose any of the following syntax constructs:
   *
   * * binary operator (typically an assignment, but also the comma operator)
   * * unary operator (typically an increment)
   * * function call expression
   * * return statement
   *
   * that are direct children of a immortal function definition's compuond block
   * and that have as their descendant some expression that needs to be
   * instrumented.
   *
   * Each of these descendant expressions need to be decomposed into separate
   * statements.
   */
  auto statement_to_be_decomposed =
      immortal_function_definition(forEachDescendant(compoundStmt(eachOf(
          forEach(binaryOperator(
                      eachOf(hasLHS(each_expr_requiring_instrumentation()),
                             hasRHS(each_expr_requiring_instrumentation())))
                      .bind(STMT_ANCESTOR)),
          forEach(unaryOperator(
                      hasUnaryOperand(each_expr_requiring_instrumentation()))
                      .bind(STMT_ANCESTOR)),
          forEach(returnStmt(each_expr_requiring_instrumentation())
                      .bind(STMT_ANCESTOR)),
          forEach(
              callExpr(forEachArgument(each_expr_requiring_instrumentation()))
                  .bind(STMT_ANCESTOR))))));
  finder.addMatcher(
      /// traverse in AsIs mode so that we catch also implicit casts
      traverse(clang::ast_type_traits::TK_AsIs, statement_to_be_decomposed),
      this);
}

void ExpressionDecompositionPass::run(
    const clang::ast_matchers::MatchFinder::MatchResult &result) {
  if (!this->rewriter_) {
    this->rewriter_ = std::make_unique<Rewriter>(*result.SourceManager,
                                                 result.Context->getLangOpts());
  }
  auto expr = result.Nodes.getNodeAs<Expr>(EXPR_REQUIRING_INSTRUMENTATION);
  assert(expr);
  auto stmt_ancestor = result.Nodes.getNodeAs<Stmt>(STMT_ANCESTOR);
  assert(stmt_ancestor);
  expressions[stmt_ancestor].push_back(expr);
}

void ExpressionDecompositionPass::onEndOfTranslationUnit() {
  NestedExpressionDecompositionVisitor visitor(*this->rewriter_);
  for (auto &stmt : this->expressions) {
    auto decomposed = visitor.traverseAndDecompose(
        const_cast<Stmt *>(stmt.first), stmt.second);

    // sort by increasing depth. When expressions have the same depth, the
    // order they were visited during the decomposition traversal is used as
    // discriminator. This is particularly relevant for the comma operator.
    //
    // Use stable sort so that expressions at the same depth have always to
    // same order. We do this to avoid false positive in snapshot tests.
    std::stable_sort(
        decomposed.begin(), decomposed.end(),
        [](const DecomposedExpression &lhs, const DecomposedExpression &rhs) {
          if (lhs.depth != rhs.depth) {
            return lhs.depth < rhs.depth;
          }
          return lhs.traversal_ith > rhs.traversal_ith;
        });

    bool new_scope_necessary = false;
    bool rewrite_res = false;

    /**
     * We take the location at offset -1 because ReplaceText
     * see RewriteOptions
     */
    auto stmt_start_loc = stmt.first->getBeginLoc().getLocWithOffset(-1);

    for (auto &expr : decomposed) {
      auto err = this->insert(this->rewriter_->getSourceMgr(), stmt_start_loc,
                              expr.content + ";\n", false);
      assert_no_error(err);
      if (!expr.parent) {
        err = this->replace(
            this->rewriter_->getSourceMgr(),
            CharSourceRange::getTokenRange(expr.expression->getSourceRange()),
            expr.placeholder);
        assert_no_error(err);
      }
      if (expr.creates_tmp_var) {
        new_scope_necessary = true;
      }
    }

    if (new_scope_necessary) {
      auto err = this->insert(this->rewriter_->getSourceMgr(), stmt_start_loc,
                              "{\n", false);
      assert_no_error(err);
      err = this->insert(this->rewriter_->getSourceMgr(),
                         find_symbol_location(stmt.first->getEndLoc(), ';',
                                              this->rewriter_->getSourceMgr(),
                                              this->rewriter_->getLangOpts())
                             .getLocWithOffset(1),
                         "\n}", true);
      assert_no_error(err);
    }
  }
}

} // namespace imtc
