#ifndef KALEIDOSCOPE_PARSER_PARSER_H
#define KALEIDOSCOPE_PARSER_PARSER_H

#include "AST/ExprAST.h"
#include "Lexer/Lexer.h"

#include <map>
#include <memory>

namespace kaleidoscope {

class Parser {
  Lexer &Lex;

  /// CurTok/getNextToken - Provide a simple token buffer.
  /// CurTok is the current token the parser is looking at.
  int CurTok;
  /// getNextToken - reads another token from the lexer and updates CurTok with
  /// its results.
  int getNextToken() { return CurTok = Lex.gettok(); }

  /// BinopPrecedence - This holds the precedence for each binary operator that
  /// is defined.
  std::map<char, int> BinopPrecedence{};

  /// getTokPrecedence - Get the precedence of the pending binary operator
  /// token.
  int getTokPrecedence();

  /// numberexpr ::= number
  std::unique_ptr<ExprAST> parseNumberExpr();

  /// parenexpr ::= '(' expression ')'
  std::unique_ptr<ExprAST> parseParenExpr();

  /// identifierexpr
  ///   ::= identifier
  ///   ::= identifier '(' expression* ')'
  std::unique_ptr<ExprAST> parseIdentifierExpr();

  /// primary
  ///   ::= identifierexpr
  ///   ::= numberexpr
  ///   ::= parenexpr
  std::unique_ptr<ExprAST> parsePrimary();

  /// binoprhs
  ///   ::= ('+' primary)*
  std::unique_ptr<ExprAST> parseBinOpRHS(int ExprPrec,
                                         std::unique_ptr<ExprAST> LHS);

  /// expression
  ///   ::= primary binoprhs
  std::unique_ptr<ExprAST> parseExpression();

public:
  Parser(Lexer &Lex) : Lex(Lex) {}

  /// addBinopPrec - installs a binary operator with a precedence
  /// 1 is lowest precedence
  Parser &addBinopPrec(char Op, int Prec) {
    BinopPrecedence[Op] = Prec;
    return *this;
  }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_PARSER_PARSER_H
