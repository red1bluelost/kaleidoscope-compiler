#include "kaleidoscope/Parser/Parser.h"

#include "kaleidoscope/AST/AST.h"
#include "kaleidoscope/Lexer/Lexer.h"

#include "TestUtil.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>

using namespace kaleidoscope;

namespace {
using ::testing::ResultOf, ::testing::ElementsAre, ::testing::Truly,
    ::testing::Pair;

template <typename AstType>
auto Isa() { // NOLINT(readability-identifier-naming)
  return Truly([](const auto &A) { return llvm::isa<AstType>(A); });
}

TEST(Parser, BinaryExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("5.0 + x;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('+', C.getOp());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getRHS()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, BinaryExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("func(x > 1) < 10;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('<', C.getOp());
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getRHS()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, BinaryExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("10 * 3 + 2;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('+', C.getOp());
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getRHS()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, BinaryExprAST_3) {
  // Arrange
  Lexer Lex{makeGetCharWithString("52 / func() - x * 3 < 1000;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('<', C.getOp());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getRHS()));
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getLHS()));
  auto &LHS = llvm::cast<BinaryExprAST>(C.getLHS());
  ASSERT_EQ('-', LHS.getOp());
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(LHS.getLHS()));
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(LHS.getRHS()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, Custom_BinaryExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern binary| 5(RHS LHS);\n"
                                  "5.0 | x;")};
  Parser Parse{Lex};

  // Act
  ASSERT_TRUE(llvm::isa<ProtoBinaryAST>(Parse.parse()));
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('|', C.getOp());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getRHS()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, Custom_BinaryExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern binary= 9(RHS RHS);\n"
                                  "52 = func() - x * 3 < 1000;")};
  Parser Parse{Lex};

  // Act
  ASSERT_TRUE(llvm::isa<ProtoBinaryAST>(Parse.parse()));
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('=', C.getOp());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getRHS()));
  auto &RHS = llvm::cast<BinaryExprAST>(C.getRHS());
  ASSERT_EQ('<', RHS.getOp());
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(RHS.getLHS()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(RHS.getRHS()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, Custom_UnaryExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern unary!(v);\n"
                                  "!v;")};
  Parser Parse{Lex};

  // Act
  ASSERT_TRUE(llvm::isa<ProtoUnaryAST>(Parse.parse()));
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<UnaryExprAST>(AST));
  auto &C = llvm::cast<UnaryExprAST>(*AST);
  ASSERT_EQ('!', C.getOpcode());
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getOperand()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, Custom_UnaryExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern unary~(x);\n"
                                  "5 + ~func();")};
  Parser Parse{Lex};

  // Act
  ASSERT_TRUE(llvm::isa<ProtoUnaryAST>(Parse.parse()));
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<UnaryExprAST>(C.getRHS()));
  auto &U = llvm::cast<UnaryExprAST>(C.getRHS());
  ASSERT_EQ('~', U.getOpcode());
  ASSERT_TRUE(llvm::isa<CallExprAST>(U.getOperand()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, Custom_UnaryExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def unary-(v)\n"
                                  "  0-v;\n"
                                  "-5;")};
  Parser Parse{Lex};

  // Act
  ASSERT_TRUE(llvm::isa<FunctionAST>(Parse.parse()));
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<UnaryExprAST>(AST));
  auto &C = llvm::cast<UnaryExprAST>(*AST);
  ASSERT_EQ('-', C.getOpcode());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getOperand()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, Custom_BinaryUnaryExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern binary= 9(RHS LHS);\n"
                                  "extern unary!(v);\n"
                                  "52 = !x;")};
  Parser Parse{Lex};

  // Act
  ASSERT_TRUE(llvm::isa<ProtoBinaryAST>(Parse.parse()));
  ASSERT_TRUE(llvm::isa<ProtoUnaryAST>(Parse.parse()));
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('=', C.getOp());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<UnaryExprAST>(C.getRHS()));
  auto &RHS = llvm::cast<UnaryExprAST>(C.getRHS());
  ASSERT_EQ('!', RHS.getOpcode());
  ASSERT_TRUE(llvm::isa<VariableExprAST>(RHS.getOperand()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, CallExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("nullary_func();")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("nullary_func", C.getCallee());
  ASSERT_EQ(0, C.getArgs().size());
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, CallExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("unary_func(x);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("unary_func", C.getCallee());
  ASSERT_EQ(1, C.getArgs().size());
  ASSERT_THAT(C.getArgs(), ElementsAre(Isa<VariableExprAST>()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, CallExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("binary_func(nullary_func(), 4.20 + x);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("binary_func", C.getCallee());
  ASSERT_EQ(2, C.getArgs().size());
  ASSERT_THAT(C.getArgs(),
              ElementsAre(Isa<CallExprAST>(), Isa<BinaryExprAST>()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, CallExprAST_3) {
  // Arrange
  Lexer Lex{makeGetCharWithString(
      "longfunc(a, b, c, d, e, 1.34, if 1 > 0 then 2 else 4);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("longfunc", C.getCallee());
  ASSERT_EQ(7, C.getArgs().size());
  ASSERT_THAT(C.getArgs(),
              ElementsAre(Isa<VariableExprAST>(), Isa<VariableExprAST>(),
                          Isa<VariableExprAST>(), Isa<VariableExprAST>(),
                          Isa<VariableExprAST>(), Isa<NumberExprAST>(),
                          Isa<IfExprAST>()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, ForExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("for i = 1, i < 10 in func();")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<ForExprAST>(AST));
  auto &C = llvm::cast<ForExprAST>(*AST);
  ASSERT_EQ("i", C.getVarName());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getStart()));
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getEnd()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getStep()));
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getBody()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, ForExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("for idx = start(x), idx < y, step(x) in"
                                  "  100 + func(x, idx);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<ForExprAST>(AST));
  auto &C = llvm::cast<ForExprAST>(*AST);
  ASSERT_EQ("idx", C.getVarName());
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getStart()));
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getEnd()));
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getStep()));
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getBody()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, IfExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if y then func1(y) else func2(y, x);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST));
  auto &C = llvm::cast<IfExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getCond()));
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getThen()));
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getElse()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, IfExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if 32 < h then y else 3;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST));
  auto &C = llvm::cast<IfExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getCond()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getThen()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getElse()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, IfExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if func() then "
                                  "if func() then 100 else 20 else x;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST));
  auto &C = llvm::cast<IfExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getCond()));
  ASSERT_TRUE(llvm::isa<IfExprAST>(C.getThen()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getElse()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, NumberExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("3.1415;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<NumberExprAST>(AST));
  ASSERT_EQ(3.1415, llvm::cast<NumberExprAST>(*AST).getVal());
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, NumberExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("4.20;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<NumberExprAST>(AST));
  ASSERT_EQ(4.20, llvm::cast<NumberExprAST>(*AST).getVal());
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, VariableExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("x;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST));
  ASSERT_EQ("x", llvm::cast<VariableExprAST>(*AST).getName());
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, VariableExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("longname;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST));
  ASSERT_EQ("longname", llvm::cast<VariableExprAST>(*AST).getName());
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, VariableExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("_u_n_d_e_r_;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST));
  ASSERT_EQ("_u_n_d_e_r_", llvm::cast<VariableExprAST>(*AST).getName());
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, VarAssignExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("var x = 1 in x;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VarAssignExprAST>(AST));
  auto &C = llvm::cast<VarAssignExprAST>(*AST);
  ASSERT_THAT(C.getVarAs(), ElementsAre(Pair("x", Isa<NumberExprAST>())));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getExpr()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, VarAssignExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("var name = 3 : 5, y = z, a = 3 * func() in\n"
                                  "  something(a, y, name);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VarAssignExprAST>(AST));
  auto &C = llvm::cast<VarAssignExprAST>(*AST);
  ASSERT_THAT(C.getVarAs(), ElementsAre(Pair("name", Isa<BinaryExprAST>()),
                                        Pair("y", Isa<VariableExprAST>()),
                                        Pair("a", Isa<BinaryExprAST>())));
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getExpr()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, VarAssignExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("var x = 1, y = x in\n"
                                  "  x + y;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VarAssignExprAST>(AST));
  auto &C = llvm::cast<VarAssignExprAST>(*AST);
  ASSERT_THAT(C.getVarAs(), ElementsAre(Pair("x", Isa<NumberExprAST>()),
                                        Pair("y", Isa<VariableExprAST>())));
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getExpr()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, Multiple_ExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("v;\n"
                                  "5 + 10 - 3;\n"
                                  "if func() then 1 else 3;")};
  Parser Parse{Lex};

  // Act
  auto AST0 = Parse.parse();
  auto AST1 = Parse.parse();
  auto AST2 = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST0));
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST1));
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST2));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, FunctionAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def func() 3;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_EQ("func", C.getProto().getName());
  ASSERT_EQ(0, C.getProto().getArgs().size());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getBody()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, FunctionAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def i(x) x;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_EQ("i", C.getProto().getName());
  ASSERT_EQ(1, C.getProto().getArgs().size());
  ASSERT_THAT(C.getProto().getArgs(), ElementsAre("x"));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getBody()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, FunctionAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def fib(x)\n"
                                  "  if x < 3 then\n"
                                  "    1\n"
                                  "  else\n"
                                  "    fib(x-1)+fib(x-2);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_EQ("fib", C.getProto().getName());
  ASSERT_EQ(1, C.getProto().getArgs().size());
  ASSERT_THAT(C.getProto().getArgs(), ElementsAre("x"));
  ASSERT_TRUE(llvm::isa<IfExprAST>(C.getBody()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, FuncBinaryAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def binary| 9 (LHS RHS)\n"
                                  "  if LHS then\n"
                                  "    1\n"
                                  "  else \n"
                                  "    if RHS then 1 else 0;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_TRUE(llvm::isa<ProtoBinaryAST>(C.getProto()));
  auto &P = llvm::cast<ProtoBinaryAST>(C.getProto());
  ASSERT_EQ("binary|", P.getName());
  ASSERT_EQ('|', P.getOperator());
  ASSERT_EQ(2, P.getArgs().size());
  ASSERT_THAT(P.getArgs(), ElementsAre("LHS", "RHS"));
  ASSERT_TRUE(llvm::isa<IfExprAST>(C.getBody()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, FuncBinaryAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def binary : 1 (x y) y;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_TRUE(llvm::isa<ProtoBinaryAST>(C.getProto()));
  auto &P = llvm::cast<ProtoBinaryAST>(C.getProto());
  ASSERT_EQ("binary:", P.getName());
  ASSERT_EQ(':', P.getOperator());
  ASSERT_EQ(2, P.getArgs().size());
  ASSERT_THAT(P.getArgs(), ElementsAre("x", "y"));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getBody()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, FuncUnaryAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def unary!(v)\n"
                                  "  if v then 0 else 1;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_TRUE(llvm::isa<ProtoUnaryAST>(C.getProto()));
  auto &P = llvm::cast<ProtoUnaryAST>(C.getProto());
  ASSERT_EQ("unary!", P.getName());
  ASSERT_EQ('!', P.getOperator());
  ASSERT_EQ(1, P.getArgs().size());
  ASSERT_THAT(P.getArgs(), ElementsAre("v"));
  ASSERT_TRUE(llvm::isa<IfExprAST>(C.getBody()));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, PrototypeAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern zero_func();")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<PrototypeAST>(AST));
  auto &C = llvm::cast<PrototypeAST>(*AST);
  ASSERT_EQ("zero_func", C.getName());
  ASSERT_EQ(0, C.getArgs().size());
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, PrototypeAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern one_func(arg);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<PrototypeAST>(AST));
  auto &C = llvm::cast<PrototypeAST>(*AST);
  ASSERT_EQ("one_func", C.getName());
  ASSERT_EQ(1, C.getArgs().size());
  ASSERT_THAT(C.getArgs(), ElementsAre("arg"));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, PrototypeAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern many_func(first s _third f_r);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<PrototypeAST>(AST));
  auto &C = llvm::cast<PrototypeAST>(*AST);
  ASSERT_EQ("many_func", C.getName());
  ASSERT_EQ(4, C.getArgs().size());
  ASSERT_THAT(C.getArgs(), ElementsAre("first", "s", "_third", "f_r"));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, ProtoBinaryAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern binary=9(LHS RHS);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<ProtoBinaryAST>(AST));
  auto &C = llvm::cast<ProtoBinaryAST>(*AST);
  ASSERT_EQ("binary=", C.getName());
  ASSERT_EQ('=', C.getOperator());
  ASSERT_EQ(9, C.getPrecedence());
  ASSERT_EQ(2, C.getArgs().size());
  ASSERT_THAT(C.getArgs(), ElementsAre("LHS", "RHS"));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, ProtoBinaryAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern binary | 30 ( x y ) ;")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<ProtoBinaryAST>(AST));
  auto &C = llvm::cast<ProtoBinaryAST>(*AST);
  ASSERT_EQ("binary|", C.getName());
  ASSERT_EQ('|', C.getOperator());
  ASSERT_EQ(30, C.getPrecedence());
  ASSERT_EQ(2, C.getArgs().size());
  ASSERT_THAT(C.getArgs(), ElementsAre("x", "y"));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, ProtoUnaryAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern unary!(v);")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<ProtoUnaryAST>(AST));
  auto &C = llvm::cast<ProtoUnaryAST>(*AST);
  ASSERT_EQ("unary!", C.getName());
  ASSERT_EQ('!', C.getOperator());
  ASSERT_EQ(1, C.getArgs().size());
  ASSERT_THAT(C.getArgs(), ElementsAre("v"));
  ASSERT_EQ(';', Parse.getCurToken());
}

TEST(Parser, ProtoUnaryAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern unary ~ ( arg );")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<ProtoUnaryAST>(AST));
  auto &C = llvm::cast<ProtoUnaryAST>(*AST);
  ASSERT_EQ("unary~", C.getName());
  ASSERT_EQ('~', C.getOperator());
  ASSERT_EQ(1, C.getArgs().size());
  ASSERT_THAT(C.getArgs(), ElementsAre("arg"));
  ASSERT_EQ(';', Parse.getCurToken());
}
} // namespace