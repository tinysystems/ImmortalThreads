/**
 * \file ternary_decomposition.cpp
 * \brief Ternary decomposition handlers - implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#include "ternary_decomposition.hpp"

#include "matchers/matchers.hpp"
#include "utils/error.hpp"
#include "utils/immortal.hpp"
#include "utils/location.hpp"

#include "fmt/core.h"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"

using namespace imtc::ast_matchers;
using namespace imtc::utils;

using namespace clang;
using namespace clang::ast_matchers;

static constexpr char STMT_ANCESTOR[] = "stmt_ancestor";
static constexpr char TERNARY_EXPR_TO_BE_DECOMPOSED[] = "ternary_expr";

/**
 * A set of nested ternaries expressions naturally form a binary tree.
 * Thus we use it to store the hierarchy of the ternary expressions in a
 * statement. Such tree is then used to decompose the ternary expressions into
 * standalone statements.
 */
struct TernaryExprBinaryTree {
  explicit TernaryExprBinaryTree(const ConditionalOperator &current)
      : current(current) {}

  const ConditionalOperator &current;
  const TernaryExprBinaryTree *parent = nullptr;
  std::unique_ptr<TernaryExprBinaryTree> true_branch;
  std::unique_ptr<TernaryExprBinaryTree> false_branch;

  /**
   * Get decomposed text
   */
  std::string getDecomposedText(const clang::SourceManager &manager,
                                const clang::LangOptions &opts,
                                const std::string *tmp_var) {
    std::string condition_text =
        clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(
                                        current.getCond()->getSourceRange()),
                                    manager, opts);

    std::string true_text;
    if (this->true_branch) {
      true_text = this->true_branch->getDecomposedText(manager, opts, tmp_var);
    } else {
      true_text = clang::Lexer::getSourceText(
          clang::CharSourceRange::getTokenRange(
              current.getTrueExpr()->getSourceRange()),
          manager, opts);
      true_text += ";";
      if (tmp_var) {
        true_text = *tmp_var + " = " + true_text;
      }
    }

    std::string false_text;
    if (this->false_branch) {
      false_text =
          this->false_branch->getDecomposedText(manager, opts, tmp_var);
    } else {
      false_text = clang::Lexer::getSourceText(
          clang::CharSourceRange::getTokenRange(
              current.getFalseExpr()->getSourceRange()),
          manager, opts);
      false_text += ";";
      if (tmp_var) {
        false_text = *tmp_var + " = " + false_text;
      }
    }

    return fmt::format("if ({}) {{ {} }} else {{ {} }}", condition_text,
                       true_text, false_text);
  }
};

/**
 * A RecursiveASTVisitor that we use to recursively decompose nested ternary
 * expressions.
 */
class TernaryExprVisitor : public RecursiveASTVisitor<TernaryExprVisitor> {

  typedef RecursiveASTVisitor<TernaryExprVisitor> VisitorBase;

public:
  bool TraverseStmt(Stmt *stmt) {
    bool continue_traversal = true;

    if (stmt) {
      if (stmt->getStmtClass() == clang::Stmt::ConditionalOperatorClass) {
        NewNode *node = this->current_new_nodes_.top();
        node->node = new TernaryExprBinaryTree(
            *llvm::dyn_cast<ConditionalOperator>(stmt));

        auto found_decompose_expr =
            std::find(this->ternary_expressions_to_be_decomposed_.begin(),
                      this->ternary_expressions_to_be_decomposed_.end(), stmt);
        if (found_decompose_expr !=
            this->ternary_expressions_to_be_decomposed_.end()) {
          node->contains_expr_to_be_decomposed = true;

          ++this->met_expressions_cnt_;

          if (this->met_expressions_cnt_ ==
              this->ternary_expressions_to_be_decomposed_.size()) {
            /// Already found all the ternary_expressions to be decomposed.
            /// Can stop the traversal.
            continue_traversal = false;
          }
        }
        {
          ///
          /// Traverse a branch and process the result of the traversal
          ///
          auto traverse_branch = [&](bool which_branch) {
            if (continue_traversal) {
              NewNode branch;
              this->current_new_nodes_.push(&branch);
              if (which_branch) {
                continue_traversal = VisitorBase::TraverseStmt(
                    llvm::dyn_cast<ConditionalOperator>(stmt)->getTrueExpr());
              } else {
                continue_traversal = VisitorBase::TraverseStmt(
                    llvm::dyn_cast<ConditionalOperator>(stmt)->getFalseExpr());
              }

              // propagate this information to the upper levels
              node->contains_expr_to_be_decomposed =
                  node->contains_expr_to_be_decomposed ||
                  branch.contains_expr_to_be_decomposed;

              // Branches that don't have expressions to be decomposed don't
              // need to be decomposed into if/else statements We can avoid
              // adding them to the ternary expression binary tree.
              if (branch.node && branch.contains_expr_to_be_decomposed) {
                if (which_branch) {
                  node->node->true_branch.reset(branch.node);
                } else {
                  node->node->false_branch.reset(branch.node);
                }
                branch.node->parent = node->node;
              }
              this->current_new_nodes_.pop();
            }
          };
          traverse_branch(true);
          traverse_branch(false);
        }
      } else {
        continue_traversal = VisitorBase::TraverseStmt(stmt);
      }
    }

    return continue_traversal;
  }

  /**
   * Traverse the AST starting from \p ancestor_stmt and build a binary
   * tree of the ternary expressions.
   */
  TernaryExprBinaryTree
  buildTernaryExprBinaryTree(Stmt *ancestor_stmt,
                             const std::vector<const ConditionalOperator *>
                                 &ternary_expressions_to_be_decomposed) {
    this->ternary_expressions_to_be_decomposed_ =
        ternary_expressions_to_be_decomposed;
    this->met_expressions_cnt_ = 0;

    NewNode new_node;
    this->current_new_nodes_ = {};
    this->current_new_nodes_.push(&new_node);

    this->TraverseStmt(ancestor_stmt);

    assert(new_node.node != nullptr);
    TernaryExprBinaryTree tree = std::move(*new_node.node);

    // prune tree
    return std::move(tree);
  }

private:
  std::vector<const ConditionalOperator *>
      ternary_expressions_to_be_decomposed_;
  /**
   * Counter of how many ternary expressions to be decomposed have been met so
   * far
   */
  size_t met_expressions_cnt_ = 0;
  struct NewNode {
    TernaryExprBinaryTree *node = nullptr;
    bool contains_expr_to_be_decomposed = false;
  };
  std::stack<NewNode *> current_new_nodes_;
};

static const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<
    Stmt, ValueStmt>
    valueStmt;

namespace clang::ast_matchers {
namespace internal {

/// AFAIK it is not documented, but the difference between has() and forEach()
/// is not only match the first vs match all. has() traverses in AsIs mode
/// whereas forEach() traverses in IgnoreImplicitCastsAndParentheses mode.
/// But we need forEach to traverse in AsIs mode, to catch parenExpr().
//
/// Thus we copied the ForEachMatcher code from llvm-project and replaced the
/// traversal mode to AsIs.
template <typename T, typename ChildT>
class AsIsForEachMatcher
    : public clang::ast_matchers::internal::WrapperMatcherInterface<T> {
  static_assert(IsBaseType<ChildT>::value,
                "for each only accepts base type matcher");

public:
  explicit AsIsForEachMatcher(const Matcher<ChildT> &ChildMatcher)
      : AsIsForEachMatcher::WrapperMatcherInterface(ChildMatcher) {}

  bool matches(const T &Node, ASTMatchFinder *Finder,
               BoundNodesTreeBuilder *Builder) const override {
    return Finder->matchesChildOf(Node, this->InnerMatcher, Builder,
                                  ast_type_traits::TraversalKind::TK_AsIs,
                                  ASTMatchFinder::BK_All);
  }
};
} // namespace internal

const internal::ArgumentAdaptingMatcherFunc<internal::AsIsForEachMatcher>
    asIsforEach = {};
} // namespace clang::ast_matchers

namespace imtc {
void TernaryDecompositionPass::register_matchers(MatchFinder &finder) {
  // We need to decompose ternary operators within immortal functions that
  // contain expressions that in their turn need to be decomposed, i.e. that are
  // direct children of a compound statement (i.e. blocks enclosed by braces)
  // and that have as their descendant some expression that needs to be
  // instrumented.
  //
  // Each of these descendant expressions need to be decomposed into separate
  // statements.
  auto statement_to_be_decomposed =
      immortal_function_definition(forEachDescendant(compoundStmt(asIsforEach(
          valueStmt(
              findAll(
                  conditionalOperator(
                      anyOf(
                          hasCondition(expr_requiring_instrumentation()),
                          hasTrueExpression(expr_requiring_instrumentation()),
                          hasFalseExpression(expr_requiring_instrumentation())))
                      .bind(TERNARY_EXPR_TO_BE_DECOMPOSED)))
              .bind(STMT_ANCESTOR)))));
  finder.addMatcher(statement_to_be_decomposed, this);
}

void TernaryDecompositionPass::run(const MatchFinder::MatchResult &result) {
  auto ternary = result.Nodes.getNodeAs<ConditionalOperator>(
      TERNARY_EXPR_TO_BE_DECOMPOSED);
  assert(ternary);
  auto stmt_ancestor = result.Nodes.getNodeAs<Stmt>(STMT_ANCESTOR);
  assert(stmt_ancestor);
  this->ternary_expressions[stmt_ancestor].push_back(ternary);
}

void TernaryDecompositionPass::onEndOfTranslationUnit() {
  TernaryExprVisitor visitor;
  auto &compiler_instance = this->context_.get_current_CI();
  for (auto &stmt : this->ternary_expressions) {
    auto tree = visitor.buildTernaryExprBinaryTree(
        const_cast<Stmt *>(stmt.first), stmt.second);
    bool requires_tmp = true;
    if (stmt.first == &tree.current) {
      // ternary operator is the root of the statement
      requires_tmp = false;
    }

    if (std::string tmp_type;
        requires_tmp &&
        (tmp_type = tree.current.getType().getAsString()) != "void") {
      std::string tmp_name = "_ternary_tmp_var";
      auto err = this->insert(
          stmt.first->getBeginLoc().getLocWithOffset(-1),
          fmt::format("{{\n{} {};\n{}", tmp_type, tmp_name,
                      tree.getDecomposedText(
                          compiler_instance.getSourceManager(),
                          compiler_instance.getLangOpts(), &tmp_name)),
          false);
      assert_no_error(err);
      err = this->replace(
          CharSourceRange::getTokenRange(tree.current.getSourceRange()),
          tmp_name);
      assert_no_error(err);
      err = this->insert(
          find_symbol_location(stmt.first->getEndLoc(), ';',
                               compiler_instance.getSourceManager(),
                               compiler_instance.getLangOpts())
              .getLocWithOffset(1),
          "\n}", true);
      assert_no_error(err);
    } else {
      auto err = this->replace(
          CharSourceRange::getTokenRange(stmt.first->getSourceRange()),
          tree.getDecomposedText(compiler_instance.getSourceManager(),
                                 compiler_instance.getLangOpts(), nullptr));
      assert_no_error(err);
    }
  }
}

} // namespace imtc
