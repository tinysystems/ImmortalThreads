/**
 * \file force_braces.cpp
 * \brief Force braces around control statements - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * Copied from ClangTidy BracesAroundStatementsCheck
 */

#include "force_braces.hpp"

#include "utils/error.hpp"

using namespace clang;
using namespace clang::ast_matchers;
using namespace imtc::utils;

namespace {

tok::TokenKind getTokenKind(SourceLocation Loc, const SourceManager &SM,
                            const ASTContext *Context) {
  Token Tok;
  SourceLocation Beginning =
      Lexer::GetBeginningOfToken(Loc, SM, Context->getLangOpts());
  const bool Invalid =
      Lexer::getRawToken(Beginning, Tok, SM, Context->getLangOpts());
  assert(!Invalid && "Expected a valid token.");

  if (Invalid)
    return tok::NUM_TOKENS;

  return Tok.getKind();
}

SourceLocation forwardSkipWhitespaceAndComments(SourceLocation Loc,
                                                const SourceManager &SM,
                                                const ASTContext *Context) {
  assert(Loc.isValid());
  for (;;) {
    while (isWhitespace(*SM.getCharacterData(Loc)))
      Loc = Loc.getLocWithOffset(1);

    tok::TokenKind TokKind = getTokenKind(Loc, SM, Context);
    if (TokKind == tok::NUM_TOKENS || TokKind != tok::comment)
      return Loc;

    // Fast-forward current token.
    Loc = Lexer::getLocForEndOfToken(Loc, 0, SM, Context->getLangOpts());
  }
}

SourceLocation findEndLocation(SourceLocation LastTokenLoc,
                               const SourceManager &SM,
                               const ASTContext *Context) {
  SourceLocation Loc =
      Lexer::GetBeginningOfToken(LastTokenLoc, SM, Context->getLangOpts());
  // Loc points to the beginning of the last (non-comment non-ws) token
  // before end or ';'.
  assert(Loc.isValid());
  bool SkipEndWhitespaceAndComments = true;
  tok::TokenKind TokKind = getTokenKind(Loc, SM, Context);
  if (TokKind == tok::NUM_TOKENS || TokKind == tok::semi ||
      TokKind == tok::r_brace) {
    // If we are at ";" or "}", we found the last token. We could use as well
    // `if (isa<NullStmt>(S))`, but it wouldn't work for nested statements.
    SkipEndWhitespaceAndComments = false;
  }

  Loc = Lexer::getLocForEndOfToken(Loc, 0, SM, Context->getLangOpts());
  // Loc points past the last token before end or after ';'.
  if (SkipEndWhitespaceAndComments) {
    Loc = forwardSkipWhitespaceAndComments(Loc, SM, Context);
    tok::TokenKind TokKind = getTokenKind(Loc, SM, Context);
    if (TokKind == tok::semi)
      Loc = Lexer::getLocForEndOfToken(Loc, 0, SM, Context->getLangOpts());
  }

  for (;;) {
    assert(Loc.isValid());
    while (isHorizontalWhitespace(*SM.getCharacterData(Loc))) {
      Loc = Loc.getLocWithOffset(1);
    }

    if (isVerticalWhitespace(*SM.getCharacterData(Loc))) {
      // EOL, insert brace before.
      break;
    }
    tok::TokenKind TokKind = getTokenKind(Loc, SM, Context);
    if (TokKind != tok::comment) {
      // Non-comment token, insert brace before.
      break;
    }

    SourceLocation TokEndLoc =
        Lexer::getLocForEndOfToken(Loc, 0, SM, Context->getLangOpts());
    SourceRange TokRange(Loc, TokEndLoc);
    StringRef Comment = Lexer::getSourceText(
        CharSourceRange::getTokenRange(TokRange), SM, Context->getLangOpts());
    if (Comment.startswith("/*") && Comment.find('\n') != StringRef::npos) {
      // Multi-line block comment, insert brace before.
      break;
    }
    // else: Trailing comment, insert brace after the newline.

    // Fast-forward current token.
    Loc = TokEndLoc;
  }
  return Loc;
}

} // namespace

namespace imtc {

void ForceBracesPass::register_matchers(MatchFinder &finder) {
  finder.addMatcher(
      ifStmt(unless(allOf(isConstexpr(), isInTemplateInstantiation())))
          .bind("if"),
      this);
  finder.addMatcher(whileStmt().bind("while"), this);
  finder.addMatcher(doStmt().bind("do"), this);
  finder.addMatcher(forStmt().bind("for"), this);
  finder.addMatcher(cxxForRangeStmt().bind("for-range"), this);
}

void ForceBracesPass::run(const MatchFinder::MatchResult &result) {
  const SourceManager &SM = *result.SourceManager;
  const ASTContext *Context = result.Context;

  // Get location of closing parenthesis or 'do' to insert opening brace.
  if (auto S = result.Nodes.getNodeAs<ForStmt>("for")) {
    checkStmt(result, S->getBody(), S->getRParenLoc());
  } else if (auto S = result.Nodes.getNodeAs<DoStmt>("do")) {
    checkStmt(result, S->getBody(), S->getDoLoc(), S->getWhileLoc());
  } else if (auto S = result.Nodes.getNodeAs<WhileStmt>("while")) {
    SourceLocation StartLoc = findRParenLoc(S, SM, Context);
    if (StartLoc.isInvalid())
      return;
    checkStmt(result, S->getBody(), StartLoc);
  } else if (auto S = result.Nodes.getNodeAs<IfStmt>("if")) {
    SourceLocation StartLoc = findRParenLoc(S, SM, Context);
    if (StartLoc.isInvalid())
      return;
    if (ForceBracesStmts.erase(S))
      ForceBracesStmts.insert(S->getThen());
    bool BracedIf = checkStmt(result, S->getThen(), StartLoc, S->getElseLoc());
    const Stmt *Else = S->getElse();
    if (Else && BracedIf)
      ForceBracesStmts.insert(Else);
    if (Else && !isa<IfStmt>(Else)) {
      // Omit 'else if' statements here, they will be handled directly.
      checkStmt(result, Else, S->getElseLoc());
    }
  } else {
    llvm_unreachable("Invalid match");
  }
}

/// Find location of right parenthesis closing condition.
template <typename IfOrWhileStmt>
SourceLocation ForceBracesPass::findRParenLoc(const IfOrWhileStmt *S,
                                              const SourceManager &SM,
                                              const ASTContext *Context) {
  // Skip macros.
  if (S->getBeginLoc().isMacroID())
    return SourceLocation();

  SourceLocation CondEndLoc = S->getCond()->getEndLoc();
  if (const DeclStmt *CondVar = S->getConditionVariableDeclStmt())
    CondEndLoc = CondVar->getEndLoc();

  if (!CondEndLoc.isValid()) {
    return SourceLocation();
  }

  SourceLocation PastCondEndLoc =
      Lexer::getLocForEndOfToken(CondEndLoc, 0, SM, Context->getLangOpts());
  if (PastCondEndLoc.isInvalid())
    return SourceLocation();
  SourceLocation RParenLoc =
      forwardSkipWhitespaceAndComments(PastCondEndLoc, SM, Context);
  if (RParenLoc.isInvalid())
    return SourceLocation();
  tok::TokenKind TokKind = getTokenKind(RParenLoc, SM, Context);
  if (TokKind != tok::r_paren)
    return SourceLocation();
  return RParenLoc;
}

/// Determine if the statement needs braces around it, and add them if it does.
/// Returns true if braces where added.
bool ForceBracesPass::checkStmt(const MatchFinder::MatchResult &Result,
                                const Stmt *S, SourceLocation InitialLoc,
                                SourceLocation EndLocHint) {
  // 1) If there's a corresponding "else" or "while", the check inserts "} "
  // right before that token.
  // 2) If there's a multi-line block comment starting on the same line after
  // the location we're inserting the closing brace at, or there's a non-comment
  // token, the check inserts "\n}" right before that token.
  // 3) Otherwise the check finds the end of line (possibly after some block or
  // line comments) and inserts "\n}" right before that EOL.
  if (!S || isa<CompoundStmt>(S)) {
    // Already inside braces.
    return false;
  }

  if (!InitialLoc.isValid())
    return false;
  const SourceManager &SM = *Result.SourceManager;
  const ASTContext *Context = Result.Context;

  // Treat macros.
  CharSourceRange FileRange = Lexer::makeFileCharRange(
      CharSourceRange::getTokenRange(S->getSourceRange()), SM,
      Context->getLangOpts());
  if (FileRange.isInvalid())
    return false;

  // Convert InitialLoc to file location, if it's on the same macro expansion
  // level as the start of the statement. We also need file locations for
  // Lexer::getLocForEndOfToken working properly.
  InitialLoc = Lexer::makeFileCharRange(
                   CharSourceRange::getCharRange(InitialLoc, S->getBeginLoc()),
                   SM, Context->getLangOpts())
                   .getBegin();
  if (InitialLoc.isInvalid())
    return false;
  SourceLocation StartLoc =
      Lexer::getLocForEndOfToken(InitialLoc, 0, SM, Context->getLangOpts());

  // StartLoc points at the location of the opening brace to be inserted.
  SourceLocation EndLoc;
  std::string ClosingInsertion;
  if (EndLocHint.isValid()) {
    EndLoc = EndLocHint;
    ClosingInsertion = "} ";
  } else {
    const auto FREnd = FileRange.getEnd().getLocWithOffset(-1);
    EndLoc = findEndLocation(FREnd, SM, Context);
    ClosingInsertion = "\n}";
  }

  assert(StartLoc.isValid());
  assert(EndLoc.isValid());

  auto err = this->insert(SM, StartLoc, " {");
  assert_no_error(err);
  err = this->insert(SM, EndLoc, ClosingInsertion);
  assert_no_error(err);
  return true;
}
} // namespace imtc
