#ifndef KALEIDOSCOPE_PARSER_PARSER_H
#define KALEIDOSCOPE_PARSER_PARSER_H

#include "AST/ExprAST.h"
#include "Lexer/Lexer.h"

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

  std::unique_ptr<ExprAST> parseExpression();

public:
  Parser(Lexer &Lex) : Lex(Lex) {}
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_PARSER_PARSER_H
