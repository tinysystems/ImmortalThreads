/**
 * \file basic_block.cpp
 * \brief Instrument write accesses a bit more smarty - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * This pass omits some checkpoints. The ideas are the following:
 *
 * * _WR is necessary only if the variable being written is previously read.
 *
 * To keeps things simple, we limit the application of these heuristics to
 * basic blocks. More serious optimization is left as future work.
 * Anyway, we place the first _WR or _WR_SELF unconditionally, so we don't
 * have to worry about what happened in the previous basic block.
 */
#include "basic_block.hpp"

#include "matchers/matchers.hpp"
#include "utils/error.hpp"
#include "utils/expr.hpp"
#include "utils/immortal.hpp"

#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Analysis/CFGStmtMap.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"

#include <variant>

using namespace imtc::ast_matchers;
using namespace imtc::utils;

using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;

static constexpr char IMMORTAL_FUNCTION_DEFINITION[] = "immortal_function_body";
static constexpr char ASSIGNMENT_OPERATOR_BINDING[] = "assignment_binding";
static constexpr char READ_LVALUE[] = "read_lvalue_binding";
static constexpr char DEREFERENCED_LOCAL_VARIBLE[] =
    "referenced_local_variable";

namespace imtc {

using ThisPass =
    MemWriteCheckpointMacrosInsertionPassWithBasicBlockOptimization;

/**
 * Metadata of a matched write statement (i.e. a statement that contains an
 * assignment)
 */
struct WriteStatement {
  /**
   * The write statement node.
   */
  const Stmt *stmt;
  /**
   * The assignment operator node contained in the write statement.
   * May be the write statement itself.
   */
  const BinaryOperator *assignment;
  /**
   * Information about L-values that have been read in this write statement.
   *
   * At this stage, the L-values that we match can be:
   *
   * * Simple variable access
   * * Struct member or array member access or a combination of these two
   * * Pointer dereference
   *
   * For a complete list, see
   * https://en.cppreference.com/w/c/language/value_category
   */
  SmallVector<const Expr *, 5> read_lvalues;
  /**
   * Index of the statement in its basic block. Used to determine whether
   * statements are consecutive.
   */
  size_t basic_block_index;
};

struct ThisPass::PassImpl {
  ThisPass *pass;
  PassImpl(ThisPass *pass) : pass(pass) {}
  std::unique_ptr<clang::AnalysisDeclContextManager> analysis_context_manager;
  /**
   * Map immortal function to its analysis context
   */
  llvm::DenseMap<const clang::FunctionDecl *,
                 std::unique_ptr<clang::AnalysisDeclContext>>
      function_analysis_context_map;
  /**
   * Map of immortal function and all the memory writes statement metadata
   * inside it
   * write_stmt_to_metadata_map owns the WriteStatement.
   */
  std::unordered_map<const clang::FunctionDecl *, std::vector<WriteStatement *>>
      immortal_function_memory_writes;
  /**
   * Map a write statement's assignment to the metadata of the write statement
   */
  std::unordered_map<const BinaryOperator *, WriteStatement>
      write_stmt_to_metadata_map;
  /**
   * Set of all local variables that have been dereferenced
   */
  DenseSet<const VarDecl *> dereferenced_local_var_;

  struct MemberAccessInfo {
    std::string member_name;
  };
  struct ArrayAccessInfo {
    const Expr *subscript_expr;
    std::string subscript_expr_str;
    bool is_constant;
    std::size_t constant_value;
  };
  struct DeclRefInfo {
    const DeclRefExpr *decl_ref_expr;
  };
  using AccessInfo =
      std::variant<MemberAccessInfo, ArrayAccessInfo, DeclRefInfo>;
  using AccessPath = std::vector<AccessInfo>;

  void replace_coalesced_write_selfs(
      const ArrayRef<const WriteStatement *> &wr_selfs,
      const clang::FunctionDecl &immortal_function) {

    std::vector<const BinaryOperator *> pending_writes;
    std::transform(wr_selfs.begin(), wr_selfs.end(),
                   std::back_inserter(pending_writes),
                   [](const WriteStatement *write_stmt) {
                     return write_stmt->assignment;
                   });

    auto &sm = this->pass->context_.get_current_CI().getSourceManager();
    auto &lo = this->pass->context_.get_current_CI().getLangOpts();

    std::string new_source = get_immortal_coalesced_write_selfs(
        pending_writes, immortal_function,
        this->pass->context_.get_current_CI());
    auto last_semicolon_loc =
        find_symbol_location(wr_selfs.back()->stmt->getEndLoc(), ';', sm, lo);
    auto err = this->pass->replace(
        CharSourceRange::getTokenRange(wr_selfs.front()->stmt->getBeginLoc(),
                                       last_semicolon_loc),
        new_source);
    assert_no_error(err);
  }

  /**
   * Whether to complex member/array subscript access "paths" may alias
   */
  bool member_access_path_aliases(const AccessPath &lhs,
                                  const AccessPath &rhs) {
    auto lhs_decl_ref = std::get<DeclRefInfo>(lhs.back());
    auto rhs_decl_ref = std::get<DeclRefInfo>(rhs.back());
    if (!lhs_decl_ref.decl_ref_expr->getType()->isPointerType() &&
        !rhs_decl_ref.decl_ref_expr->getType()->isPointerType()) {
      if (lhs_decl_ref.decl_ref_expr->getDecl() !=
          rhs_decl_ref.decl_ref_expr->getDecl()) {
        return false;
      } else if (lhs.size() != rhs.size()) {
        /// they refer to the same "aggregate" variable, but they have different
        /// access path length.
        /// => they are accessing members at different nesting level
        return false;
      }
    }

    for (auto l_it = lhs.begin(), r_it = rhs.begin();
         l_it != lhs.end() && r_it != rhs.end(); ++l_it, ++r_it) {
      if (l_it->index() != r_it->index()) {
        if (std::holds_alternative<DeclRefInfo>(*l_it) ||
            std::holds_alternative<DeclRefInfo>(*r_it)) {
          /// if path "diverged" because one has finished earlier
          /// it could be something like:
          /// struct a {
          ///   struct b {
          ///     int a;
          ///   } a;
          ///   int b;
          /// };
          /// struct a data;
          /// struct b *ptr = &data.a;
          /// data.a.a = ptr->a;
          return true;
        }
        return false;
      }
      if (std::holds_alternative<ArrayAccessInfo>(*l_it)) {
        auto lhs = std::get<ArrayAccessInfo>(*l_it);
        auto rhs = std::get<ArrayAccessInfo>(*r_it);
        if (lhs.is_constant && rhs.is_constant &&
            (lhs.constant_value != rhs.constant_value)) {
          return false;
        }
      } else if (std::holds_alternative<MemberAccessInfo>(*l_it)) {
        auto lhs = std::get<MemberAccessInfo>(*l_it);
        auto rhs = std::get<MemberAccessInfo>(*r_it);
        if (lhs.member_name != rhs.member_name) {
          return false;
        }
      }
    }
    return true;
  };

  /**
   * A RecursiveASTVisitor that we use to recursively decompose nested ternary
   * expressions.
   */
  class AccessPathVisitor : public RecursiveASTVisitor<AccessPathVisitor> {

    typedef RecursiveASTVisitor<AccessPathVisitor> VisitorBase;

  public:
    bool VisitArraySubscriptExpr(ArraySubscriptExpr *expr) {
      ArrayAccessInfo info;
      llvm::APSInt constant_integer;
      info.is_constant = expr->getIdx()->isIntegerConstantExpr(
          constant_integer, this->ci_->getASTContext());
      if (info.is_constant) {
        info.constant_value = constant_integer.getExtValue();
      }
      info.subscript_expr = expr->getIdx();

      auto &sm = this->ci_->getSourceManager();
      auto &lo = this->ci_->getLangOpts();
      info.subscript_expr_str = Lexer::getSourceText(
          CharSourceRange::getTokenRange(expr->getRHS()->getSourceRange()), sm,
          lo);

      this->access_path_->push_back(info);
      return true;
    }
    bool VisitMemberExpr(MemberExpr *expr) {
      MemberAccessInfo info;
      info.member_name = expr->getMemberNameInfo().getAsString();
      this->access_path_->push_back(info);
      return true;
    }
    bool VisitDeclRefExpr(DeclRefExpr *expr) {
      DeclRefInfo info;
      info.decl_ref_expr = expr;
      this->access_path_->push_back(info);
      return true;
    }

    /**
     * Traverse the AST starting from \p expr and build a member/array subscript
     * "access path"
     */
    AccessPath build_path(Expr &expr, const CompilerInstance &ci) {
      AccessPath path;
      this->access_path_ = &path;
      this->ci_ = &ci;

      auto &sm = ci.getSourceManager();
      auto &lo = ci.getLangOpts();
      auto src = Lexer::getSourceText(
          CharSourceRange::getTokenRange(expr.getSourceRange()), sm, lo);
      this->TraverseStmt(&expr);

      return std::move(path);
    }

  private:
    AccessPath *access_path_;
    const CompilerInstance *ci_;
  };

  bool has_any_alias(const ArrayRef<const Expr *> &lhs_lvalues,
                     const ArrayRef<const Expr *> &rhs_lvalues,
                     const CompilerInstance &instance) {
    for (const auto &lvalue : lhs_lvalues) {
      bool found = std::find_if(rhs_lvalues.begin(), rhs_lvalues.end(),
                                [&](const Expr *read_lvalue) {
                                  return this->lvalues_do_alias(
                                      lvalue, read_lvalue, instance);
                                }) != rhs_lvalues.end();
      if (found) {
        return true;
      }
    }
    return false;
  }

  /**
   * Given an lvalue expression, determine whether it aliases with any of the
   * lvalues expressions providd in the given array.
   */
  bool has_any_alias(const Expr *lhs_lvalue,
                     const ArrayRef<const Expr *> &rhs_lvalues,
                     const CompilerInstance &instance) {
    assert(lhs_lvalue);
    return std::find_if(rhs_lvalues.begin(), rhs_lvalues.end(),
                        [&](const Expr *read_lvalue) {
                          return this->lvalues_do_alias(lhs_lvalue, read_lvalue,
                                                        instance);
                        }) != rhs_lvalues.end();
  }

  bool lvalues_do_alias(const Expr *lhs, const Expr *rhs,
                        const CompilerInstance &instance) {
    assert(lhs->getValueKind() == ExprValueKind::VK_LValue &&
           rhs->getValueKind() == ExprValueKind::VK_LValue);

    if (lhs->getStmtClass() == clang::Stmt::ParenExprClass) {
      auto expr = dyn_cast<ParenExpr>(lhs);
      assert(expr);
      return lvalues_do_alias(expr->getSubExpr(), rhs, instance);
    }
    if (rhs->getStmtClass() == clang::Stmt::ParenExprClass) {
      auto expr = dyn_cast<ParenExpr>(rhs);
      assert(expr);
      return lvalues_do_alias(lhs, expr->getSubExpr(), instance);
    }

    switch (lhs->getStmtClass()) {
    case clang::Stmt::DeclRefExprClass: {
      auto decl_ref = dyn_cast<DeclRefExpr>(lhs);
      if (is_pointer_dereference_lvalue(rhs)) {
        // if our lhs has been dereferenced, then it may alias with rhs,
        // otherwise no
        return this->dereferenced_local_var_.count(
                   dyn_cast<VarDecl>(decl_ref->getDecl())) == 1;
      }
      if (auto read_ref = dyn_cast<DeclRefExpr>(rhs)) {
        return decl_ref->getDecl()->getID() == read_ref->getDecl()->getID();
      }
      return false;
    }
    case clang::Stmt::MemberExprClass:
    case clang::Stmt::ArraySubscriptExprClass: {
      auto &sm = instance.getSourceManager();
      auto &lo = instance.getLangOpts();
      auto write_lvalue_src = Lexer::getSourceText(
          CharSourceRange::getTokenRange(lhs->getSourceRange()), sm, lo);
      if (is_pointer_dereference_lvalue(rhs)) {
        return true;
      }
      if (lhs->getStmtClass() == rhs->getStmtClass() &&
          lhs->getType() == rhs->getType()) {
        AccessPathVisitor visitor;
        auto lhs_path = visitor.build_path(*const_cast<Expr *>(lhs), instance);
        auto rhs_path = visitor.build_path(*const_cast<Expr *>(rhs), instance);
        return member_access_path_aliases(lhs_path, rhs_path);
      }
      return false;
    }
    case clang::Stmt::UnaryOperatorClass: {
      if (is_pointer_dereference_lvalue(lhs)) {
        if (rhs->isIntegerConstantExpr(
                this->pass->context_.get_current_CI().getASTContext())) {
          return false;
        }
        if (rhs->getStmtClass() == clang::Stmt::DeclRefExprClass) {
          // if our rhs has been dereferenced, then it may alias with lhs,
          // otherwise no
          return this->dereferenced_local_var_.count(dyn_cast<VarDecl>(
                     dyn_cast<DeclRefExpr>(rhs)->getDecl())) == 1;
        }
        if (is_pointer_dereference_lvalue(rhs) &&
            // assuming strict aliasing
            (lhs->getType() == rhs->getType())) {
          return true;
        }
      }
      return false;
    }
    default:
      llvm::errs() << "Unhandled statement class: " << lhs->getStmtClassName()
                   << "\n";
      assert(0 && "Unhandled");
    }
  }
};

ThisPass::MemWriteCheckpointMacrosInsertionPassWithBasicBlockOptimization(
    ImmortalcContext &context)
    : ImmortalcPass(context),
      p_impl_(std::make_unique<ThisPass::PassImpl>(this)) {}
ThisPass::~MemWriteCheckpointMacrosInsertionPassWithBasicBlockOptimization() {}

AST_MATCHER(clang::Expr, is_lvalue) { return Node.isLValue(); }

template <typename... InnerMatcher>
inline auto lvalue_expr(InnerMatcher... inner_matcher) {
  using namespace clang::ast_matchers;
  return expr(is_lvalue(), inner_matcher...);
}

static auto for_each_lvalue_descendant() {
  return forEachDescendant(lvalue_expr().bind(READ_LVALUE));
}

template <typename... InnerMatcher>
static auto assignment_to_non_volatile_var_and_all_the_lvalues(
    InnerMatcher... inner_matcher) {
  return assignment_to_non_volatile_var(binaryOperator(
      hasRHS(for_each_lvalue_descendant()),
      hasLHS(lvalue_expr(for_each_lvalue_descendant())), inner_matcher...));
}

void ThisPass::register_matchers(MatchFinder &finder) {
  auto rhs_is_immortal_fn_call =
      hasRHS(anyOf(immortal_function_call_expr(),
                   hasDescendant(immortal_function_call_expr())));

  auto local_varialbe_dereferences =
      for_each_node_inside_immortal_function_definition(
          unaryOperator(hasOperatorName("&"),
                        hasUnaryOperand(declRefExpr(
                            to(varDecl(hasLocalStorage())
                                   .bind(DEREFERENCED_LOCAL_VARIBLE))))))
          .bind(IMMORTAL_FUNCTION_DEFINITION);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               local_varialbe_dereferences),
      this);

  auto non_volatile_write =
      for_each_node_inside_immortal_function_definition(
          assignment_to_non_volatile_var(
              binaryOperator(
                  eachOf(hasRHS(for_each_lvalue_descendant()),
                         hasLHS(lvalue_expr(for_each_lvalue_descendant())),
                         anything()),
                  unless(rhs_is_immortal_fn_call)))
              .bind(ASSIGNMENT_OPERATOR_BINDING))
          .bind(IMMORTAL_FUNCTION_DEFINITION);
  finder.addMatcher(
      traverse(clang::ast_type_traits::TK_IgnoreUnlessSpelledInSource,
               non_volatile_write),
      this);
}

void ThisPass::run(const MatchFinder::MatchResult &result) {
  if (!this->p_impl_->analysis_context_manager) {
    // create AnalysisDeclContextManager only once
    this->p_impl_->analysis_context_manager =
        std::make_unique<AnalysisDeclContextManager>(*result.Context);
  }
  if (auto *fn =
          result.Nodes.getNodeAs<FunctionDecl>(IMMORTAL_FUNCTION_DEFINITION)) {
    if (this->p_impl_->function_analysis_context_map.count(fn) == 0) {
      // create AnalysisDeclContext for each immmortal function
      this->p_impl_->function_analysis_context_map[fn] =
          std::make_unique<AnalysisDeclContext>(
              this->p_impl_->analysis_context_manager.get(), fn);
    }
  }
  if (auto *var_decl =
          result.Nodes.getNodeAs<VarDecl>(DEREFERENCED_LOCAL_VARIBLE)) {
    this->p_impl_->dereferenced_local_var_.insert(var_decl);
  }
  if (auto *assignment =
          result.Nodes.getNodeAs<BinaryOperator>(ASSIGNMENT_OPERATOR_BINDING);
      assignment &&
      this->p_impl_->write_stmt_to_metadata_map.count(assignment) == 0) {
    /// Create the metadata object WriteStatement for the matched statement

    /// when the ASSIGNMENT_OPERATOR_BINDING binding is present we expect all
    /// the parent node bindings to be also present
    auto *fn =
        result.Nodes.getNodeAs<FunctionDecl>(IMMORTAL_FUNCTION_DEFINITION);
    assert(fn);

    WriteStatement write_stmt;
    write_stmt.assignment = assignment;

    write_stmt.stmt = assignment->getExprStmt();
    assert(write_stmt.stmt);

    this->p_impl_->write_stmt_to_metadata_map[assignment] = write_stmt;
    this->p_impl_->immortal_function_memory_writes[fn].push_back(
        &this->p_impl_->write_stmt_to_metadata_map.at(assignment));
  }
  if (auto *lvalue = result.Nodes.getNodeAs<Expr>(READ_LVALUE)) {
    auto *assignment =
        result.Nodes.getNodeAs<BinaryOperator>(ASSIGNMENT_OPERATOR_BINDING);
    assert(assignment);

    this->p_impl_->write_stmt_to_metadata_map.at(assignment)
        .read_lvalues.push_back(lvalue);
  }
}

void ThisPass::onEndOfTranslationUnit() {
  for (const auto &[fn, fn_analysis_context] :
       this->p_impl_->function_analysis_context_map) {
    auto stmt_to_block = fn_analysis_context->getCFGStmtMap();

    /// build a mapping between each basic block and the write statements inside
    /// it
    DenseMap<const CFGBlock *, SmallVector<WriteStatement *, 10>>
        block_to_stmts;
    for (auto &fn_wr_stmt :
         this->p_impl_->immortal_function_memory_writes[fn]) {
      block_to_stmts[stmt_to_block->getBlock(fn_wr_stmt->stmt)].push_back(
          fn_wr_stmt);
    }

    for (auto &[block, wr_stmts] : block_to_stmts) {
      {
        auto wr_stmt_it = wr_stmts.begin();
        auto block_element_it = block->begin();
        size_t i = 0;
        while (wr_stmt_it != wr_stmts.end() &&
               block_element_it != block->end()) {
          if (block_element_it->getKind() == CFGElement::Kind::Statement) {
            if ((*wr_stmt_it)->stmt ==
                (block_element_it->castAs<CFGStmt>()).getStmt()) {
              (*wr_stmt_it)->basic_block_index = i;
              ++wr_stmt_it;
            }
          }
          ++block_element_it;
          ++i;
        }
        // All the write statements have been found in the basic block
        assert(wr_stmt_it == wr_stmts.end());
      }
    }

    for (auto &[block, wr_stmts] : block_to_stmts) {
      size_t count = 0;
      SmallVector<const Expr *, 20> lvalues_read_since_last_ckpt;
      struct WrSelfQueue {
        WrSelfQueue(PassImpl *p_impl, const FunctionDecl &fn)
            : p_impl_(*p_impl), fn_(fn) {}
        void push(const WriteStatement *wr_stmt) {
          this->wr_stmts.push_back(wr_stmt);
          this->pending_lhs.push_back(wr_stmt->assignment->getLHS());
        }
        void apply_and_flush() {
          assert(this->size() != 0);
          this->p_impl_.replace_coalesced_write_selfs(this->wr_stmts,
                                                      this->fn_);
          this->wr_stmts.clear();
          this->pending_lhs.clear();
        }
        size_t size() const {
          assert(this->wr_stmts.size() == this->pending_lhs.size());
          return this->wr_stmts.size();
        }
        SmallVector<const WriteStatement *, 5> wr_stmts;
        /**
         * LHS lvalues that are pending for memory updates
         */
        SmallVector<const Expr *, 5> pending_lhs;

      private:
        PassImpl &p_impl_;
        const FunctionDecl &fn_;
      } pending_contiguous_wr_selfs(p_impl_.get(), *fn);

      for (auto &wr_stmt : wr_stmts) {
        if (bool write_self = this->p_impl_->has_any_alias(
                wr_stmt->assignment->getLHS(), wr_stmt->read_lvalues,
                this->context_.get_current_CI());
            // first write
            count == 0 ||
            // or write self
            write_self ||
            // or this write writes to previously read variables
            this->p_impl_->has_any_alias(wr_stmt->assignment->getLHS(),
                                         lvalues_read_since_last_ckpt,
                                         this->context_.get_current_CI())) {

          if (write_self) {
            if (pending_contiguous_wr_selfs.size() == 0) {
              // queue the _WR_SELF
              pending_contiguous_wr_selfs.push(wr_stmt);
            } else {
              if ((wr_stmt->basic_block_index - 1) !=
                  pending_contiguous_wr_selfs.wr_stmts.back()
                      ->basic_block_index) {
                // determine whether this write statement comes right after the
                // previous one
                // if not consecutive, then first apply the pending _WR_SELFs
                pending_contiguous_wr_selfs.apply_and_flush();
              } else if (this->p_impl_->has_any_alias(
                             wr_stmt->read_lvalues,
                             pending_contiguous_wr_selfs.pending_lhs,
                             this->context_.get_current_CI())) {
                // Determine whether this write statement contains in its RHS
                // operand a previously queued LHS of a _WR_SELF.
                // If yes, we can't coalesce this write statement.
                // This write statement depends on the memory update of the
                // queued _WR_SELF write statement.
                pending_contiguous_wr_selfs.apply_and_flush();
              }
              pending_contiguous_wr_selfs.push(wr_stmt);
            }
          } else {
            // Need to use _WR. Flush the previously queued _WR_SELFs in
            // coalesced form.
            if (pending_contiguous_wr_selfs.size() != 0) {
              pending_contiguous_wr_selfs.apply_and_flush();
            }
            std::string new_source = get_immortal_write(
                *wr_stmt->assignment, *fn, this->context_.get_current_CI());
            auto err = this->replace(CharSourceRange::getTokenRange(
                                         wr_stmt->assignment->getBeginLoc(),
                                         wr_stmt->assignment->getEndLoc()),
                                     new_source);
            assert_no_error(err);
          }

          // we put a new checkpoint, so we can clear this
          lvalues_read_since_last_ckpt.clear();
        }

        lvalues_read_since_last_ckpt.insert(lvalues_read_since_last_ckpt.end(),
                                            wr_stmt->read_lvalues.begin(),
                                            wr_stmt->read_lvalues.end());

        ++count;
      }

      // If any queued _WR_SELFs has not been processed yet, do it at the end of
      // the basic block
      if (pending_contiguous_wr_selfs.size() != 0) {
        pending_contiguous_wr_selfs.apply_and_flush();
      }
    }
  }
}
} // namespace imtc
