#include "Parser/Parser.h"
#include "AST/BinaryExprAST.h"
#include "AST/CallExprAST.h"
#include "AST/ForExprAST.h"
#include "AST/FunctionAST.h"
#include "AST/IfExprAST.h"
#include "AST/NumberExprAST.h"
#include "AST/PrototypeAST.h"
#include "AST/VariableExprAST.h"
#include "Lexer/Lexer.h"
#include "TestUtil.h"

#include <gtest/gtest.h>

using namespace kaleidoscope;

namespace {
TEST(Parser, BinaryExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("5.0 + x")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('+', C.getOp());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getRHS()));
}

TEST(Parser, BinaryExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("func(x > 1) < 10")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('<', C.getOp());
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getRHS()));
}

TEST(Parser, BinaryExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("10 * 3 + 2")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('+', C.getOp());
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getRHS()));
}

TEST(Parser, BinaryExprAST_3) {
  // Arrange
  Lexer Lex{makeGetCharWithString("52 / func() - x * 3 < 1000")};
  auto Parse = makeParser(Lex);

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
}

TEST(Parser, CallExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("nullary_func()")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("nullary_func", C.getCallee());
  ASSERT_EQ(0, C.getArgs().size());
}

TEST(Parser, CallExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("unary_func(x)")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("unary_func", C.getCallee());
  ASSERT_EQ(1, C.getArgs().size());
  for (const auto &S : C.getArgs())
    ASSERT_NE(nullptr, S);
}

TEST(Parser, CallExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("binary_func(nullary_func(), 4.20 + x)")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("binary_func", C.getCallee());
  ASSERT_EQ(2, C.getArgs().size());
  for (const auto &S : C.getArgs())
    ASSERT_NE(nullptr, S);
}

TEST(Parser, CallExprAST_3) {
  // Arrange
  Lexer Lex{makeGetCharWithString(
      "longfunc(a, b, c, d, e, 1.34, if 1 > 0 then 2 else 4)")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("longfunc", C.getCallee());
  ASSERT_EQ(7, C.getArgs().size());
  for (const auto &S : C.getArgs())
    ASSERT_NE(nullptr, S);
}

TEST(Parser, ForExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("for i = 1, i < 10 in func()")};
  auto Parse = makeParser(Lex);

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
}

TEST(Parser, ForExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("for idx = start(x), idx < y, step(x) in"
                                  "  100 + func(x, idx)")};
  auto Parse = makeParser(Lex);

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
}

TEST(Parser, IfExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if y then func1(y) else func2(y, x)")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST));
  auto &C = llvm::cast<IfExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getCond()));
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getThen()));
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getElse()));
}

TEST(Parser, IfExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if 32 < h then y else 3")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST));
  auto &C = llvm::cast<IfExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getCond()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getThen()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getElse()));
}

TEST(Parser, IfExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if func() then "
                                  "if func() then 100 else 20 else x")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST));
  auto &C = llvm::cast<IfExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getCond()));
  ASSERT_TRUE(llvm::isa<IfExprAST>(C.getThen()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getElse()));
}

TEST(Parser, NumberExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("3.1415")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<NumberExprAST>(AST));
  ASSERT_EQ(3.1415, llvm::cast<NumberExprAST>(*AST).getVal());
}

TEST(Parser, NumberExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("4.20")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<NumberExprAST>(AST));
  ASSERT_EQ(4.20, llvm::cast<NumberExprAST>(*AST).getVal());
}

TEST(Parser, VariableExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("x")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST));
  ASSERT_EQ("x", llvm::cast<VariableExprAST>(*AST).getName());
}

TEST(Parser, VariableExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("longname")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST));
  ASSERT_EQ("longname", llvm::cast<VariableExprAST>(*AST).getName());
}

TEST(Parser, VariableExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("_u_n_d_e_r_")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST));
  ASSERT_EQ("_u_n_d_e_r_", llvm::cast<VariableExprAST>(*AST).getName());
}

TEST(Parser, FunctionExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def func() 3")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_EQ("func", C.getProto().getName());
  ASSERT_EQ(0, C.getProto().getArgs().size());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getBody()));
}

TEST(Parser, FunctionExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def i(x) x")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_EQ("i", C.getProto().getName());
  ASSERT_EQ(1, C.getProto().getArgs().size());
  ASSERT_EQ(std::vector<std::string>{"x"}, C.getProto().getArgs());
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getBody()));
}

TEST(Parser, PrototypeAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern zero_func()")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<PrototypeAST>(AST));
  auto &C = llvm::cast<PrototypeAST>(*AST);
  ASSERT_EQ("zero_func", C.getName());
  ASSERT_EQ(0, C.getArgs().size());
}

TEST(Parser, PrototypeAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern one_func(arg)")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<PrototypeAST>(AST));
  auto &C = llvm::cast<PrototypeAST>(*AST);
  ASSERT_EQ("one_func", C.getName());
  ASSERT_EQ(1, C.getArgs().size());
  ASSERT_EQ(std::vector<std::string>{"arg"}, C.getArgs());
}

TEST(Parser, PrototypeAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern many_func(first s _third f_r)")};
  auto Parse = makeParser(Lex);

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<PrototypeAST>(AST));
  auto &C = llvm::cast<PrototypeAST>(*AST);
  ASSERT_EQ("many_func", C.getName());
  ASSERT_EQ(4, C.getArgs().size());
  ASSERT_EQ((std::vector<std::string>{"first", "s", "_third", "f_r"}),
            C.getArgs());
}
} // namespace