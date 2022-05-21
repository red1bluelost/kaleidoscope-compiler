#include "Parser/Parser.h"

#include "AST/BinaryExprAST.h"
#include "AST/CallExprAST.h"
#include "AST/NumberExprAST.h"
#include "AST/VariableExprAST.h"

#include <fmt/core.h>

#include <string_view>

using namespace kaleidoscope;

/// logError<AST> - These are little helper functions for error handling.
template <typename T> std::unique_ptr<T> logError(std::string_view Str) {
  fmt::print(stderr, "LogError: {}\n", Str);
  return nullptr;
}

int Parser::getTokPrecedence() {
  if (!isascii(CurTok))
    return -1;
  if (!BinopPrecedence.contains(CurTok))
    return -1;
  return BinopPrecedence[CurTok];
}

std::unique_ptr<ExprAST> Parser::parseNumberExpr() {
  auto Res = std::make_unique<NumberExprAST>(Lex.getNumVal());
  getNextToken();
  return Res;
}

std::unique_ptr<ExprAST> Parser::parseParenExpr() {
  getNextToken(); // eat (
  auto V = parseExpression();
  if (!V)
    return nullptr;
  if (CurTok != ')')
    return logError<ExprAST>("expected ')'");
  getNextToken(); // eat )
  return V;
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpr() {
  std::string IdName = Lex.getIdentifierStr();
  getNextToken(); // eat identifier

  if (CurTok != '(') // simple variable ref
    return std::make_unique<VariableExprAST>(std::move(IdName));

  // call
  getNextToken(); // eat (
  if (CurTok == ')') {
    getNextToken(); // eat )
    return std::make_unique<CallExprAST>(
        std::move(IdName), std::vector<std::unique_ptr<ExprAST>>());
  }

  std::vector<std::unique_ptr<ExprAST>> Args;
  while (true) {
    if (auto Arg = parseExpression())
      Args.push_back(std::move(Arg));
    else
      return nullptr;

    if (CurTok == ')')
      break;

    if (CurTok != ',')
      return logError<ExprAST>("Expected ')' or ',' in argument list");
    getNextToken();
  }
  getNextToken(); // eat )
  return std::make_unique<CallExprAST>(std::move(IdName), std::move(Args));
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
  switch (CurTok) {
  default:
    return logError<ExprAST>("fuck");
  case Lexer::tok_identifier:
    return parseIdentifierExpr();
  case Lexer::tok_number:
    return parseNumberExpr();
  case ')':
    return parseParenExpr();
  }
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int ExprPrec,
                                               std::unique_ptr<ExprAST> LHS) {
  // if this is a binop, find its precedence
  while (CurTok != Lexer::tok_eof) {
    int TokPrec = getTokPrecedence();

    // if this is a binop that binds at least as tightly as the current binop,
    // consume it, otherwise we are done
    if (TokPrec < ExprPrec)
      return LHS;

    // we know this is a binop
    int BinOp = CurTok;
    getNextToken(); // eat binop

    // parse the primary expression after the binary operator
    auto RHS = parsePrimary();
    if (!RHS)
      return nullptr;

    // if BinOp binds less tightly with RHS than the operator after RHS, let the
    // pending operator take RHS as its LHS
    int NextPrec = getTokPrecedence();
    if (TokPrec < NextPrec &&
        (!(RHS = parseBinOpRHS(TokPrec + 1, std::move(RHS)))))
      return nullptr;

    // merge LHS/RHS
    LHS =
        std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
  } // loop around to top of the while loop
  return logError<ExprAST>("ran into EOF too early");
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
  auto LHS = parsePrimary();
  if (!LHS)
    return nullptr;
  return parseBinOpRHS(0, std::move(LHS));
}