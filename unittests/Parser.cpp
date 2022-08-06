#include "kaleidoscope/Parser/Parser.h"

#include "kaleidoscope/AST/BinaryExprAST.h"
#include "kaleidoscope/AST/CallExprAST.h"
#include "kaleidoscope/AST/ForExprAST.h"
#include "kaleidoscope/AST/FunctionAST.h"
#include "kaleidoscope/AST/IfExprAST.h"
#include "kaleidoscope/AST/NumberExprAST.h"
#include "kaleidoscope/AST/PrototypeAST.h"
#include "kaleidoscope/AST/VariableExprAST.h"
#include "kaleidoscope/Lexer/Lexer.h"

#include "TestUtil.h"

#include <gtest/gtest.h>

using namespace kaleidoscope;

namespace {
TEST(Parser, BinaryExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("5.0 + x")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('+', C.getOp());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getRHS()));
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, BinaryExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("func(x > 1) < 10")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('<', C.getOp());
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getRHS()));
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, BinaryExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("10 * 3 + 2")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(AST));
  auto &C = llvm::cast<BinaryExprAST>(*AST);
  ASSERT_EQ('+', C.getOp());
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getLHS()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getRHS()));
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, BinaryExprAST_3) {
  // Arrange
  Lexer Lex{makeGetCharWithString("52 / func() - x * 3 < 1000")};
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
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, CallExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("nullary_func()")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("nullary_func", C.getCallee());
  ASSERT_EQ(0, C.getArgs().size());
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, CallExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("unary_func(x)")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("unary_func", C.getCallee());
  ASSERT_EQ(1, C.getArgs().size());
  for (const auto &S : C.getArgs())
    ASSERT_NE(nullptr, S);
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, CallExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("binary_func(nullary_func(), 4.20 + x)")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("binary_func", C.getCallee());
  ASSERT_EQ(2, C.getArgs().size());
  for (const auto &S : C.getArgs())
    ASSERT_NE(nullptr, S);
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, CallExprAST_3) {
  // Arrange
  Lexer Lex{makeGetCharWithString(
      "longfunc(a, b, c, d, e, 1.34, if 1 > 0 then 2 else 4)")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<CallExprAST>(AST));
  auto &C = llvm::cast<CallExprAST>(*AST);
  ASSERT_EQ("longfunc", C.getCallee());
  ASSERT_EQ(7, C.getArgs().size());
  for (const auto &S : C.getArgs())
    ASSERT_NE(nullptr, S);
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, ForExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("for i = 1, i < 10 in func()")};
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
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, ForExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("for idx = start(x), idx < y, step(x) in"
                                  "  100 + func(x, idx)")};
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
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, IfExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if y then func1(y) else func2(y, x)")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST));
  auto &C = llvm::cast<IfExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getCond()));
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getThen()));
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getElse()));
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, IfExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if 32 < h then y else 3")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST));
  auto &C = llvm::cast<IfExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<BinaryExprAST>(C.getCond()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getThen()));
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getElse()));
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, IfExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if func() then "
                                  "if func() then 100 else 20 else x")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<IfExprAST>(AST));
  auto &C = llvm::cast<IfExprAST>(*AST);
  ASSERT_TRUE(llvm::isa<CallExprAST>(C.getCond()));
  ASSERT_TRUE(llvm::isa<IfExprAST>(C.getThen()));
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getElse()));
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, NumberExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("3.1415")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<NumberExprAST>(AST));
  ASSERT_EQ(3.1415, llvm::cast<NumberExprAST>(*AST).getVal());
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, NumberExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("4.20")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<NumberExprAST>(AST));
  ASSERT_EQ(4.20, llvm::cast<NumberExprAST>(*AST).getVal());
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, VariableExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("x")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST));
  ASSERT_EQ("x", llvm::cast<VariableExprAST>(*AST).getName());
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, VariableExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("longname")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST));
  ASSERT_EQ("longname", llvm::cast<VariableExprAST>(*AST).getName());
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, VariableExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("_u_n_d_e_r_")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<VariableExprAST>(AST));
  ASSERT_EQ("_u_n_d_e_r_", llvm::cast<VariableExprAST>(*AST).getName());
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, FunctionExprAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def func() 3")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_EQ("func", C.getProto().getName());
  ASSERT_EQ(0, C.getProto().getArgs().size());
  ASSERT_TRUE(llvm::isa<NumberExprAST>(C.getBody()));
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, FunctionExprAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def i(x) x")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_EQ("i", C.getProto().getName());
  ASSERT_EQ(1, C.getProto().getArgs().size());
  ASSERT_EQ(std::vector<std::string>{"x"}, C.getProto().getArgs());
  ASSERT_TRUE(llvm::isa<VariableExprAST>(C.getBody()));
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, FunctionExprAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def fib(x)\n"
                                  "  if x < 3 then\n"
                                  "    1\n"
                                  "  else\n"
                                  "    fib(x-1)+fib(x-2)")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<FunctionAST>(AST));
  auto &C = llvm::cast<FunctionAST>(*AST);
  ASSERT_EQ("fib", C.getProto().getName());
  ASSERT_EQ(1, C.getProto().getArgs().size());
  ASSERT_EQ(std::vector<std::string>{"x"}, C.getProto().getArgs());
  ASSERT_TRUE(llvm::isa<IfExprAST>(C.getBody()));
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, PrototypeAST_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern zero_func()")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<PrototypeAST>(AST));
  auto &C = llvm::cast<PrototypeAST>(*AST);
  ASSERT_EQ("zero_func", C.getName());
  ASSERT_EQ(0, C.getArgs().size());
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, PrototypeAST_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern one_func(arg)")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<PrototypeAST>(AST));
  auto &C = llvm::cast<PrototypeAST>(*AST);
  ASSERT_EQ("one_func", C.getName());
  ASSERT_EQ(1, C.getArgs().size());
  ASSERT_EQ(std::vector<std::string>{"arg"}, C.getArgs());
  ASSERT_EQ(EOF, Parse.getCurToken());
}

TEST(Parser, PrototypeAST_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern many_func(first s _third f_r)")};
  Parser Parse{Lex};

  // Act
  auto AST = Parse.parse();

  // Assert
  ASSERT_TRUE(llvm::isa<PrototypeAST>(AST));
  auto &C = llvm::cast<PrototypeAST>(*AST);
  ASSERT_EQ("many_func", C.getName());
  ASSERT_EQ(4, C.getArgs().size());
  ASSERT_EQ((std::vector<std::string>{"first", "s", "_third", "f_r"}),
            C.getArgs());
  ASSERT_EQ(EOF, Parse.getCurToken());
}
} // namespace