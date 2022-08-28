#include "kaleidoscope/Parser/Parser.h"

#include "kaleidoscope/AST/BinaryExprAST.h"
#include "kaleidoscope/AST/CallExprAST.h"
#include "kaleidoscope/AST/ForExprAST.h"
#include "kaleidoscope/AST/IfExprAST.h"
#include "kaleidoscope/AST/NumberExprAST.h"
#include "kaleidoscope/AST/ProtoBinaryAST.h"
#include "kaleidoscope/AST/ProtoUnaryAST.h"
#include "kaleidoscope/AST/UnaryExprAST.h"
#include "kaleidoscope/AST/VariableExprAST.h"
#include "kaleidoscope/Util/Error/Log.h"

using namespace kaleidoscope;

static const std::unordered_map<char, int> DefaultBinOpPrec{
    {'<', 10}, {'>', 10}, {'+', 20}, {'-', 20}, {'*', 40}, {'/', 40}};

int Parser::getTokPrecedence(int Tok) const {
  if (!isascii(Tok))
    return -1;
  char C = static_cast<char>(Tok);
  if (UserBinOpPrec.contains(C))
    return UserBinOpPrec.at(C);
  if (DefaultBinOpPrec.contains(C))
    return DefaultBinOpPrec.at(C);
  return -1;
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
    return logError("expected ')'");
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
      return logError("Expected ')' or ',' in argument list");
    getNextToken();
  }
  getNextToken(); // eat )
  return std::make_unique<CallExprAST>(std::move(IdName), std::move(Args));
}

std::unique_ptr<ExprAST> Parser::parseUnaryExpr() {
  if (!isascii(CurTok) || !UserUnaryOps.contains(static_cast<char>(CurTok)))
    return logError("Unknown unary expression.");

  char Opcode = static_cast<char>(CurTok);
  getNextToken(); // Eat the unary operator

  if (auto A = parseExpression())
    return std::make_unique<UnaryExprAST>(Opcode, std::move(A));

  return logError("Failed to parse operand expression for unary op");
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
  switch (CurTok) {
  default:
    return parseUnaryExpr();
  case Lexer::tok_identifier:
    return parseIdentifierExpr();
  case Lexer::tok_number:
    return parseNumberExpr();
  case '(':
    return parseParenExpr();
  case Lexer::tok_if:
    return parseIfExpr();
  case Lexer::tok_for:
    return parseForExpr();
  }
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int ExprPrec,
                                               std::unique_ptr<ExprAST> LHS) {
  // if this is a binop, find its precedence
  while (CurTok != Lexer::tok_eof && CurTok != ';') {
    int TokPrec = getTokPrecedence(CurTok);

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
    int NextPrec = getTokPrecedence(CurTok);
    if (TokPrec < NextPrec &&
        (!(RHS = parseBinOpRHS(TokPrec + 1, std::move(RHS)))))
      return nullptr;

    // merge LHS/RHS
    LHS =
        std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
  } // loop around to top of the while loop
  return LHS;
}

std::unique_ptr<ExprAST> Parser::parseIfExpr() {
  getNextToken(); // eat the "if"

  auto Cond = parseExpression();
  if (!Cond)
    return nullptr;

  if (CurTok != Lexer::tok_then)
    return logError("expected \"then\" token");
  getNextToken(); // eat the "then"

  auto Then = parseExpression();
  if (!Then)
    return nullptr;

  if (CurTok != Lexer::tok_else)
    return logError("expected \"else\" token");
  getNextToken(); // eat the "else"

  auto Else = parseExpression();
  if (!Else)
    return nullptr;

  return std::make_unique<IfExprAST>(std::move(Cond), std::move(Then),
                                     std::move(Else));
}

std::unique_ptr<ExprAST> Parser::parseForExpr() {
  getNextToken(); // eat "for"

  if (CurTok != Lexer::tok_identifier)
    return logError("expected identifier after \"for\"");
  std::string IdName = Lex.getIdentifierStr();
  getNextToken(); // eat identifier

  if (CurTok != '=')
    return logError("expected '=' after \"for\"");
  getNextToken(); // eat '='

  auto Start = parseExpression();
  if (!Start)
    return nullptr;
  if (CurTok != ',')
    return logError("expected ',' after for-loop start value");
  getNextToken(); // eat ','

  auto End = parseExpression();
  if (!End)
    return nullptr;

  // The step value is optional.
  std::unique_ptr<ExprAST> Step;
  if (CurTok == ',') {
    getNextToken(); // eat ','
    Step = parseExpression();
    if (!Step)
      return nullptr;
  } else
    Step = std::make_unique<NumberExprAST>(1.0); // default to 1.0

  if (CurTok != Lexer::tok_in)
    return logError("expected 'in' after for");
  getNextToken(); // eat "in"

  auto Body = parseExpression();
  if (!Body)
    return nullptr;

  return std::make_unique<ForExprAST>(IdName, std::move(Start), std::move(End),
                                      std::move(Step), std::move(Body));
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
  auto LHS = parsePrimary();
  if (!LHS)
    return nullptr;
  return parseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<PrototypeAST> Parser::parseProtoBinary() {
  if (!isascii(getNextToken()))
    return logError("Expected binary operator");
  char Op = static_cast<char>(CurTok);

  if (getNextToken() != Lexer::tok_number)
    return logError("Binary prototype requires precedence number");

  if (double V = Lex.getNumVal(); V < 1 || V > 100)
    return logError("Precedence should be in range [1,100]");
  int Prec = static_cast<int>(Lex.getNumVal());

  if (getNextToken() != '(')
    return logError("expected '(' in binary prototype");

  if (getNextToken() != Lexer::tok_identifier)
    return logError(
        "Expecting left side identifier in binary operator parameter list");
  std::string LHS = Lex.getIdentifierStr();

  if (getNextToken() != Lexer::tok_identifier)
    return logError(
        "Expecting right side identifier in binary operator parameter list");
  std::string RHS = Lex.getIdentifierStr();

  if (getNextToken() != ')')
    return logError("expected ')' in binary prototype");
  getNextToken(); // eat )

  // Install the new operator once the prototype is successfully parsed
  UserBinOpPrec[Op] = Prec;

  return std::make_unique<ProtoBinaryAST>(
      Op, std::array{std::move(LHS), std::move(RHS)}, Prec);
}

std::unique_ptr<PrototypeAST> Parser::parseProtoUnary() {
  if (!isascii(getNextToken()))
    return logError("Expected unary operator");
  char Op = static_cast<char>(CurTok);

  if (getNextToken() != '(')
    return logError("expected '(' in unary prototype");

  if (getNextToken() != Lexer::tok_identifier)
    return logError(
        "Expecting single identifier in unary operator parameter list");
  std::string Arg = Lex.getIdentifierStr();

  if (getNextToken() != ')')
    return logError("expected ')' in unary prototype");
  getNextToken(); // eat )

  // Install the new operator once the prototype is successfully parsed
  UserUnaryOps.insert(Op);

  return std::make_unique<ProtoUnaryAST>(Op, std::array{std::move(Arg)});
}

std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
  switch (CurTok) {
  default:
    return logError("expected function name or operator in prototype");
  case Lexer::tok_binary:
    return parseProtoBinary();
  case Lexer::tok_unary:
    return parseProtoUnary();
  case Lexer::tok_identifier:
    break; // Keep doing the default behavior
  }

  std::string FnName = Lex.getIdentifierStr();
  getNextToken();

  if (CurTok != '(')
    return logError("expected '(' in prototype");

  // read the list of argument names
  std::vector<std::string> ArgNames;
  while (getNextToken() == Lexer::tok_identifier)
    ArgNames.push_back(Lex.getIdentifierStr());
  if (CurTok != ')')
    return logError("expected ')' in prototype");

  getNextToken(); // eat )
  return std::make_unique<PrototypeAST>(std::move(FnName), std::move(ArgNames));
}

std::unique_ptr<FunctionAST> Parser::parseDefinition() {
  getNextToken(); // eat def
  auto Proto = parsePrototype();
  if (!Proto)
    return nullptr;
  if (auto E = parseExpression())
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::parseExtern() {
  getNextToken(); // eat extern
  return parsePrototype();
}

std::unique_ptr<ASTNode> Parser::parse() {
  switch (getNextToken()) {
  case ';':
    return logError("given semicolon where expression should start");
  case Lexer::tok_eof:
    return nullptr;
  case Lexer::tok_def:
    return parseDefinition();
  case Lexer::tok_extern:
    return parseExtern();
  default:
    return parseExpression();
  }
}
