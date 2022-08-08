#ifndef KALEIDOSCOPE_PARSER_PARSER_H
#define KALEIDOSCOPE_PARSER_PARSER_H

#include "kaleidoscope/AST/ExprAST.h"
#include "kaleidoscope/AST/FunctionAST.h"
#include "kaleidoscope/AST/PrototypeAST.h"
#include "kaleidoscope/Lexer/Lexer.h"

#include <memory>
#include <unordered_map>

namespace kaleidoscope {

class Parser {
  Lexer &Lex;

  /// CurTok/getNextToken - Provide a simple token buffer.
  /// CurTok is the current token the parser is looking at.
  int CurTok;

  /// UserBinOpPrec - This holds the precedence for each binary operator that
  /// is defined.
  std::unordered_map<char, int> UserBinOpPrec{};

  /// getTokPrecedence - Get the precedence of the pending binary operator
  /// token.
  int getTokPrecedence(int Tok) const;

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

  /// ifexpr
  ///   ::= 'if' expression 'then' expression 'else' expression
  std::unique_ptr<ExprAST> parseIfExpr();

  /// forexpr
  ///   ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
  std::unique_ptr<ExprAST> parseForExpr();

  /// expression
  ///   ::= primary binoprhs
  std::unique_ptr<ExprAST> parseExpression();

  /// prototype
  ///   ::= id '(' id* ')'
  std::unique_ptr<PrototypeAST> parsePrototype();

  /// definition ::= 'def' prototype expression
  std::unique_ptr<FunctionAST> parseDefinition();

  /// external ::= 'extern' prototype
  std::unique_ptr<PrototypeAST> parseExtern();

public:
  Parser(Lexer &Lex) : Lex(Lex) {}
  ~Parser() = default;

  Parser() = delete;
  Parser(const Parser &) = delete;
  Parser(Parser &&) = delete;

  /// getNextToken - reads another token from the lexer and updates CurTok with
  /// its results.
  int getNextToken() { return CurTok = Lex.gettok(); }
  [[nodiscard]] int getCurToken() const noexcept { return CurTok; }

  /// addBinopPrec - installs a binary operator with a precedence 1 is lowest
  /// precedence
  void addBinOpPrec(char Op, int Prec) { UserBinOpPrec[Op] = Prec; }

  /// astnode ::= expression | external | definition
  std::unique_ptr<ASTNode> parse();
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_PARSER_PARSER_H
