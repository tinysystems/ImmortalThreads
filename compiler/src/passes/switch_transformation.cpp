/**
 * \file switch_transformation.cpp
 * \brief Transform switch statement info if/else - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */
#include "switch_transformation.hpp"

#include "matchers/matchers.hpp"
#include "utils/error.hpp"
#include "utils/immortal.hpp"
#include "utils/location.hpp"

#include "fmt/core.h"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Lexer.h"

#include <cctype>
#include <sstream>

using namespace imtc::ast_matchers;
using namespace imtc::utils;

using namespace clang;
using namespace clang::ast_matchers;

static constexpr char SWITCH_STMT_BINDING[] = "switch_stmt";
static constexpr char CASE_STMT_BINDING[] = "case_stmt";
static constexpr char BREAK_STMT_BINDING[] = "break_stmt";
static constexpr char DEFAULT_STMT_BINDING[] = "default_stmt";

namespace imtc {

/**
 * A RecursiveASTVisitor that we use to recursively to visit each switch
 * statement, because we need to determine the ordering of the queries case and
 * break statements.
 * I was not able to find a way to express a matcher query that gives me the
 * elements in correct order...
 */
class SwitchElementsSortVisitor
    : public RecursiveASTVisitor<SwitchElementsSortVisitor> {

  typedef RecursiveASTVisitor<SwitchElementsSortVisitor> VisitorBase;

public:
  SwitchElementsSortVisitor() {}

  bool TraverseStmt(Stmt *stmt) {
    if (auto it = std::find(this->context_.remaining.begin(),
                            this->context_.remaining.end(), stmt);
        it != this->context_.remaining.end()) {
      this->context_.sorted.push_back(*it);
      this->context_.remaining.erase(it);
      if (this->context_.remaining.size() == 0) {
        // Nothing remaining. Sorted everything. Stop traversal.
        return false;
      }
    }
    bool res = VisitorBase::TraverseStmt(stmt);
    return res;
  }

  /**
   * Traverse the AST starting from \p ancestor_stmt and decompose the
   * expressions \p nested_expressions_to_be_decomposed.
   */
  SwitchTransformationPass::SwitchStmtInfo::second_type
  order_switch_elements(const SwitchTransformationPass::SwitchStmtInfo &info) {
    this->context_.remaining.clear();
    this->context_.sorted.clear();
    std::copy(info.second.begin(), info.second.end(),
              std::back_inserter(this->context_.remaining));
    this->TraverseStmt(const_cast<SwitchStmt *>(info.first));
    assert(info.second.size() == this->context_.sorted.size());
    return this->context_.sorted;
  }

private:
  /**
   * Traversal context data
   */
  struct {
    std::list<const Stmt *> remaining;
    std::vector<const Stmt *> sorted;
  } context_;
};

void SwitchTransformationPass::register_matchers(MatchFinder &finder) {
  /**
   * Matches any variable declaration that is not alraedy declared as immortal
   * value
   */
  auto switch_statement_matcher =
      for_each_node_inside_immortal_function_definition(
          switchStmt(forEachDescendant(stmt(
                         eachOf(caseStmt().bind(CASE_STMT_BINDING),
                                breakStmt().bind(BREAK_STMT_BINDING),
                                defaultStmt().bind(DEFAULT_STMT_BINDING)))))
              .bind(SWITCH_STMT_BINDING));

  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               switch_statement_matcher),
      this);
}

void SwitchTransformationPass::run(const MatchFinder::MatchResult &result) {
  const auto *switch_stmt =
      result.Nodes.getNodeAs<SwitchStmt>(SWITCH_STMT_BINDING);
  /// The switch should always be bound, since the match handler is called
  /// either when a switch statement is matched or when a child of a switch
  /// statement is matched (in which case the parent switch statement should
  /// still be boundh)
  assert(switch_stmt);

  if (const auto *case_stmt =
          result.Nodes.getNodeAs<CaseStmt>(CASE_STMT_BINDING)) {
    this->switch_stmts[switch_stmt].push_back(case_stmt);
  }

  if (const auto *break_stmt =
          result.Nodes.getNodeAs<BreakStmt>(BREAK_STMT_BINDING)) {
    this->switch_stmts[switch_stmt].push_back(break_stmt);
  }

  if (const auto *default_stmt =
          result.Nodes.getNodeAs<DefaultStmt>(DEFAULT_STMT_BINDING)) {
    this->switch_stmts[switch_stmt].push_back(default_stmt);
  }
}

void SwitchTransformationPass::onEndOfTranslationUnit() {
  SwitchElementsSortVisitor sorter;
  for (auto &switch_info : this->switch_stmts) {
    switch_info.second = sorter.order_switch_elements(switch_info);
    this->transformt_switch_stmt(switch_info);
  }
}

bool SwitchTransformationPass::check_support(const SwitchStmtInfo &info) {
  return true;
}

void SwitchTransformationPass::transformt_switch_stmt(
    const SwitchStmtInfo &info) {
  if (!this->check_support(info)) {
    assert(0);
  }

  const auto *cond_expr = info.first->getCond();
  std::string cond_expr_str = this->get_ast_node_string(*cond_expr);

  ///
  /// This can be refactored into a state machine
  ///
  std::ostringstream os;
  std::string final_str;
  size_t break_stmt_met_so_far = 0;
  const CaseStmt *last_case_stmt_since_last_break = nullptr;

  for (const Stmt *stmt : info.second) {
    if (stmt->getStmtClass() == Stmt::StmtClass::DefaultStmtClass) {
      if (last_case_stmt_since_last_break) {
        /// We reached a default case, so all the condtions from previous cases
        /// are no more important to us
        os.str("");
      }
      if (break_stmt_met_so_far > 0) {
        os << "else ";
      }
      os << "{"
         << this->get_ast_node_string(
                *dyn_cast<DefaultStmt>(stmt)->getSubStmt())
         << ";}\n";
      final_str += os.str();

      // reset
      os.str("");
      last_case_stmt_since_last_break = nullptr;
    }
    if (stmt->getStmtClass() == Stmt::StmtClass::CaseStmtClass) {
      auto case_stmt = dyn_cast<CaseStmt>(stmt);
      if (!last_case_stmt_since_last_break) {
        /// no case statement met yet
        if (break_stmt_met_so_far == 0) {
          os << "if (";
        } else {
          os << "else if (";
        }
      } else {
        os << " || ";
      }
      os << cond_expr_str
         << " == " << this->get_ast_node_string(*case_stmt->getLHS());

      last_case_stmt_since_last_break = case_stmt;
    }
    if (stmt->getStmtClass() == Stmt::StmtClass::BreakStmtClass) {
      if (!last_case_stmt_since_last_break) {
        /// no case statement met yet
        /// ignore this break statement
        continue;
      } else {
        /// close if, else if control expression
        /// start a block and copy the case statement's body
        /// close the block
        os << ") {\n"
           << this->get_ast_node_string(
                  *last_case_stmt_since_last_break->getSubStmt())
           << ";\n}\n";

        final_str += os.str();

        /// reset
        os.str("");
        last_case_stmt_since_last_break = nullptr;
        ++break_stmt_met_so_far;
      }
    }
  }

  bool contains_break_statement = false;
  {
    std::size_t found = final_str.find("break");
    constexpr std::size_t BREAK_STR_LEN = 5;
    if (found != std::string::npos) {
      /// the break substring should not be the final portion in a valid piece
      /// of code
      assert(found + BREAK_STR_LEN < final_str.size());
      char next_char = final_str[found + BREAK_STR_LEN];
      if (next_char == ';' || isspace(next_char)) {
        /// if the break substring is a token, then we can be sure that it's a
        /// break statement
        contains_break_statement = true;
      }
    }
  }

  if (contains_break_statement) {
    /// enclose everything in a do while loop that executes only once.
    /// this way break statements in arbitrary location can be supported
    final_str = fmt::format("do {{ {} }} while(0);", final_str);
  }
  auto err = this->replace(
      CharSourceRange::getTokenRange(info.first->getSourceRange()), final_str);
  assert_no_error(err);
}

} // namespace imtc
