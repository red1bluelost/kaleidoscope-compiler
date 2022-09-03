#ifndef KALEIDOSCOPE_PARSER_PARSER_H
#define KALEIDOSCOPE_PARSER_PARSER_H

#include "kaleidoscope/AST/AST.h"
#include "kaleidoscope/Lexer/Lexer.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace kaleidoscope {

class Parser {
  Lexer &Lex;

  /// CurTok/getNextToken - Provide a simple token buffer.
  /// CurTok is the current token the parser is looking at.
  int CurTok;

  /// UserBinOpPrec - This holds the precedence for each binary operator that
  /// is defined.
  std::unordered_map<char, int> UserBinOpPrec{};

  /// UserUnaryOps - This holds each unary operator that is defined.
  std::unordered_set<char> UserUnaryOps{};

  /// getTokPrecedence - Get the precedence of the pending binary operator
  /// token.
  auto getTokPrecedence(int Tok) const -> int;

  /// numberexpr ::= number
  auto parseNumberExpr() -> std::unique_ptr<NumberExprAST>;

  /// parenexpr ::= '(' expression ')'
  auto parseParenExpr() -> std::unique_ptr<ExprAST>;

  /// identifierexpr
  ///   ::= identifier
  ///   ::= identifier '(' expression* ')'
  auto parseIdentifierOrCallExpr() -> std::unique_ptr<ExprAST>;

  /// unaryexpr
  ///   ::= unaryop expression
  auto parseUnaryExpr() -> std::unique_ptr<UnaryExprAST>;

  /// primary
  ///   ::= identifierexpr
  ///   ::= numberexpr
  ///   ::= parenexpr
  ///   ::= unaryexpr
  ///   ::= ifexpr
  ///   ::= forexpr
  ///   ::= varassignexpr
  auto parsePrimary() -> std::unique_ptr<ExprAST>;

  /// binoprhs
  ///   ::= ('+' primary)*
  auto parseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS)
      -> std::unique_ptr<ExprAST>;

  /// ifexpr
  ///   ::= 'if' expression 'then' expression 'else' expression
  auto parseIfExpr() -> std::unique_ptr<IfExprAST>;

  /// forexpr
  ///   ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
  auto parseForExpr() -> std::unique_ptr<ForExprAST>;

  /// varassignexpr
  ///   ::= 'var' identifier '=' expr (',' identifier '=' expr)* 'in' expression
  auto parseVarAssignExpr() -> std::unique_ptr<VarAssignExprAST>;

  /// expression
  ///   ::= primary binoprhs
  auto parseExpression() -> std::unique_ptr<ExprAST>;

  /// protobinary
  ///   ::= binary char number (id, id)
  auto parseProtoBinary() -> std::unique_ptr<ProtoBinaryAST>;

  /// protounary
  ///   ::= unary char (id)
  auto parseProtoUnary() -> std::unique_ptr<ProtoUnaryAST>;

  /// prototype
  ///   ::= id '(' id* ')'
  ///   ::= protobinary
  ///   ::= protounary
  auto parsePrototype() -> std::unique_ptr<PrototypeAST>;

  /// definition ::= 'def' prototype expression
  auto parseDefinition() -> std::unique_ptr<FunctionAST>;

  /// external ::= 'extern' prototype
  auto parseExtern() -> std::unique_ptr<PrototypeAST>;

public:
  Parser(Lexer &Lex) : Lex(Lex) {}
  ~Parser() = default;

  Parser() = delete;
  Parser(const Parser &) = delete;
  Parser(Parser &&) = delete;

  /// getNextToken - reads another token from the lexer and updates CurTok with
  /// its results.
  auto getNextToken() -> int { return CurTok = Lex.gettok(); }
  [[nodiscard]] auto getCurToken() const noexcept -> int { return CurTok; }

  /// astnode ::= expression | external | definition
  auto parse() -> std::unique_ptr<ASTNode>;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_PARSER_PARSER_H
