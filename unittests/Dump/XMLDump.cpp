#include "kaleidoscope/AST/Dump/XMLDump.h"

#include "kaleidoscope/Lexer/Lexer.h"
#include "kaleidoscope/Parser/Parser.h"

#include "../TestUtil.h"

#include <gtest/gtest.h>

using namespace kaleidoscope;
using namespace std::string_view_literals;

auto convertAST(std::string_view S) -> std::unique_ptr<ASTNode> {
  Lexer Lex{makeGetCharWithString(std::string(S))};
  Parser Parse{Lex};
  return Parse.parse();
}

using namespace kaleidoscope;

TEST(XMLDumpTest, BinaryExprAST_0) {
  // Arrange
  auto AST = convertAST("5.0 + x;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<BinaryExprAST>\n"
            "  <Op>+</Op>\n"
            "  <LHS>\n"
            "    <NumberExprAST>\n"
            "      <Val>5</Val>\n"
            "    </NumberExprAST>\n"
            "  </LHS>\n"
            "  <RHS>\n"
            "    <VariableExprAST>\n"
            "      <Name>x</Name>\n"
            "    </VariableExprAST>\n"
            "  </RHS>\n"
            "</BinaryExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, BinaryExprAST_1) {
  // Arrange
  auto AST = convertAST("func(x > 1) < 10;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<BinaryExprAST>\n"
            "  <Op><</Op>\n"
            "  <LHS>\n"
            "    <CallExprAST>\n"
            "      <Callee>func</Callee>\n"
            "      <Args>\n"
            "        <Arg[0]>\n"
            "          <BinaryExprAST>\n"
            "            <Op>></Op>\n"
            "            <LHS>\n"
            "              <VariableExprAST>\n"
            "                <Name>x</Name>\n"
            "              </VariableExprAST>\n"
            "            </LHS>\n"
            "            <RHS>\n"
            "              <NumberExprAST>\n"
            "                <Val>1</Val>\n"
            "              </NumberExprAST>\n"
            "            </RHS>\n"
            "          </BinaryExprAST>\n"
            "        </Arg[0]>\n"
            "      </Args>\n"
            "    </CallExprAST>\n"
            "  </LHS>\n"
            "  <RHS>\n"
            "    <NumberExprAST>\n"
            "      <Val>10</Val>\n"
            "    </NumberExprAST>\n"
            "  </RHS>\n"
            "</BinaryExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, BinaryExprAST_2) {
  // Arrange
  auto AST = convertAST("10 * 3 + 2;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<BinaryExprAST>\n"
            "  <Op>+</Op>\n"
            "  <LHS>\n"
            "    <BinaryExprAST>\n"
            "      <Op>*</Op>\n"
            "      <LHS>\n"
            "        <NumberExprAST>\n"
            "          <Val>10</Val>\n"
            "        </NumberExprAST>\n"
            "      </LHS>\n"
            "      <RHS>\n"
            "        <NumberExprAST>\n"
            "          <Val>3</Val>\n"
            "        </NumberExprAST>\n"
            "      </RHS>\n"
            "    </BinaryExprAST>\n"
            "  </LHS>\n"
            "  <RHS>\n"
            "    <NumberExprAST>\n"
            "      <Val>2</Val>\n"
            "    </NumberExprAST>\n"
            "  </RHS>\n"
            "</BinaryExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, BinaryExprAST_3) {
  // Arrange
  auto AST = convertAST("52 / func() - x * 3 < 1000;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<BinaryExprAST>\n"
            "  <Op><</Op>\n"
            "  <LHS>\n"
            "    <BinaryExprAST>\n"
            "      <Op>-</Op>\n"
            "      <LHS>\n"
            "        <BinaryExprAST>\n"
            "          <Op>/</Op>\n"
            "          <LHS>\n"
            "            <NumberExprAST>\n"
            "              <Val>52</Val>\n"
            "            </NumberExprAST>\n"
            "          </LHS>\n"
            "          <RHS>\n"
            "            <CallExprAST>\n"
            "              <Callee>func</Callee>\n"
            "              <Args>\n"
            "              </Args>\n"
            "            </CallExprAST>\n"
            "          </RHS>\n"
            "        </BinaryExprAST>\n"
            "      </LHS>\n"
            "      <RHS>\n"
            "        <BinaryExprAST>\n"
            "          <Op>*</Op>\n"
            "          <LHS>\n"
            "            <VariableExprAST>\n"
            "              <Name>x</Name>\n"
            "            </VariableExprAST>\n"
            "          </LHS>\n"
            "          <RHS>\n"
            "            <NumberExprAST>\n"
            "              <Val>3</Val>\n"
            "            </NumberExprAST>\n"
            "          </RHS>\n"
            "        </BinaryExprAST>\n"
            "      </RHS>\n"
            "    </BinaryExprAST>\n"
            "  </LHS>\n"
            "  <RHS>\n"
            "    <NumberExprAST>\n"
            "      <Val>1000</Val>\n"
            "    </NumberExprAST>\n"
            "  </RHS>\n"
            "</BinaryExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, CallExprAST_0) {
  // Arrange
  auto AST = convertAST("nullary_func();");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<CallExprAST>\n"
            "  <Callee>nullary_func</Callee>\n"
            "  <Args>\n"
            "  </Args>\n"
            "</CallExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, CallExprAST_1) {
  // Arrange
  auto AST = convertAST("unary_func(x);");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<CallExprAST>\n"
            "  <Callee>unary_func</Callee>\n"
            "  <Args>\n"
            "    <Arg[0]>\n"
            "      <VariableExprAST>\n"
            "        <Name>x</Name>\n"
            "      </VariableExprAST>\n"
            "    </Arg[0]>\n"
            "  </Args>\n"
            "</CallExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, CallExprAST_2) {
  // Arrange
  auto AST = convertAST("binary_func(nullary_func(), 4.20 + x);");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<CallExprAST>\n"
            "  <Callee>binary_func</Callee>\n"
            "  <Args>\n"
            "    <Arg[0]>\n"
            "      <CallExprAST>\n"
            "        <Callee>nullary_func</Callee>\n"
            "        <Args>\n"
            "        </Args>\n"
            "      </CallExprAST>\n"
            "    </Arg[0]>\n"
            "    <Arg[1]>\n"
            "      <BinaryExprAST>\n"
            "        <Op>+</Op>\n"
            "        <LHS>\n"
            "          <NumberExprAST>\n"
            "            <Val>4.2</Val>\n"
            "          </NumberExprAST>\n"
            "        </LHS>\n"
            "        <RHS>\n"
            "          <VariableExprAST>\n"
            "            <Name>x</Name>\n"
            "          </VariableExprAST>\n"
            "        </RHS>\n"
            "      </BinaryExprAST>\n"
            "    </Arg[1]>\n"
            "  </Args>\n"
            "</CallExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, CallExprAST_3) {
  // Arrange
  auto AST =
      convertAST("longfunc(a, b, c, d, e, 1.34, if 1 > 0 then 2 else 4);");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<CallExprAST>\n"
            "  <Callee>longfunc</Callee>\n"
            "  <Args>\n"
            "    <Arg[0]>\n"
            "      <VariableExprAST>\n"
            "        <Name>a</Name>\n"
            "      </VariableExprAST>\n"
            "    </Arg[0]>\n"
            "    <Arg[1]>\n"
            "      <VariableExprAST>\n"
            "        <Name>b</Name>\n"
            "      </VariableExprAST>\n"
            "    </Arg[1]>\n"
            "    <Arg[2]>\n"
            "      <VariableExprAST>\n"
            "        <Name>c</Name>\n"
            "      </VariableExprAST>\n"
            "    </Arg[2]>\n"
            "    <Arg[3]>\n"
            "      <VariableExprAST>\n"
            "        <Name>d</Name>\n"
            "      </VariableExprAST>\n"
            "    </Arg[3]>\n"
            "    <Arg[4]>\n"
            "      <VariableExprAST>\n"
            "        <Name>e</Name>\n"
            "      </VariableExprAST>\n"
            "    </Arg[4]>\n"
            "    <Arg[5]>\n"
            "      <NumberExprAST>\n"
            "        <Val>1.34</Val>\n"
            "      </NumberExprAST>\n"
            "    </Arg[5]>\n"
            "    <Arg[6]>\n"
            "      <IfExprAST>\n"
            "        <Cond>\n"
            "          <BinaryExprAST>\n"
            "            <Op>></Op>\n"
            "            <LHS>\n"
            "              <NumberExprAST>\n"
            "                <Val>1</Val>\n"
            "              </NumberExprAST>\n"
            "            </LHS>\n"
            "            <RHS>\n"
            "              <NumberExprAST>\n"
            "                <Val>0</Val>\n"
            "              </NumberExprAST>\n"
            "            </RHS>\n"
            "          </BinaryExprAST>\n"
            "        </Cond>\n"
            "        <Then>\n"
            "          <NumberExprAST>\n"
            "            <Val>2</Val>\n"
            "          </NumberExprAST>\n"
            "        </Then>\n"
            "        <Else>\n"
            "          <NumberExprAST>\n"
            "            <Val>4</Val>\n"
            "          </NumberExprAST>\n"
            "        </Else>\n"
            "      </IfExprAST>\n"
            "    </Arg[6]>\n"
            "  </Args>\n"
            "</CallExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, ForExprAST_0) {
  // Arrange
  auto AST = convertAST("for i = 1, i < 10 in func();");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<ForExprAST>\n"
            "  <VarName>i</VarName>\n"
            "  <Start>\n"
            "    <NumberExprAST>\n"
            "      <Val>1</Val>\n"
            "    </NumberExprAST>\n"
            "  </Start>\n"
            "  <End>\n"
            "    <BinaryExprAST>\n"
            "      <Op><</Op>\n"
            "      <LHS>\n"
            "        <VariableExprAST>\n"
            "          <Name>i</Name>\n"
            "        </VariableExprAST>\n"
            "      </LHS>\n"
            "      <RHS>\n"
            "        <NumberExprAST>\n"
            "          <Val>10</Val>\n"
            "        </NumberExprAST>\n"
            "      </RHS>\n"
            "    </BinaryExprAST>\n"
            "  </End>\n"
            "  <Step>\n"
            "    <NumberExprAST>\n"
            "      <Val>1</Val>\n"
            "    </NumberExprAST>\n"
            "  </Step>\n"
            "  <Body>\n"
            "    <CallExprAST>\n"
            "      <Callee>func</Callee>\n"
            "      <Args>\n"
            "      </Args>\n"
            "    </CallExprAST>\n"
            "  </Body>\n"
            "</ForExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, ForExprAST_1) {
  // Arrange
  auto AST = convertAST("for idx = start(x), idx < y, step(x) in"
                        "  100 + func(x, idx);");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<ForExprAST>\n"
            "  <VarName>idx</VarName>\n"
            "  <Start>\n"
            "    <CallExprAST>\n"
            "      <Callee>start</Callee>\n"
            "      <Args>\n"
            "        <Arg[0]>\n"
            "          <VariableExprAST>\n"
            "            <Name>x</Name>\n"
            "          </VariableExprAST>\n"
            "        </Arg[0]>\n"
            "      </Args>\n"
            "    </CallExprAST>\n"
            "  </Start>\n"
            "  <End>\n"
            "    <BinaryExprAST>\n"
            "      <Op><</Op>\n"
            "      <LHS>\n"
            "        <VariableExprAST>\n"
            "          <Name>idx</Name>\n"
            "        </VariableExprAST>\n"
            "      </LHS>\n"
            "      <RHS>\n"
            "        <VariableExprAST>\n"
            "          <Name>y</Name>\n"
            "        </VariableExprAST>\n"
            "      </RHS>\n"
            "    </BinaryExprAST>\n"
            "  </End>\n"
            "  <Step>\n"
            "    <CallExprAST>\n"
            "      <Callee>step</Callee>\n"
            "      <Args>\n"
            "        <Arg[0]>\n"
            "          <VariableExprAST>\n"
            "            <Name>x</Name>\n"
            "          </VariableExprAST>\n"
            "        </Arg[0]>\n"
            "      </Args>\n"
            "    </CallExprAST>\n"
            "  </Step>\n"
            "  <Body>\n"
            "    <BinaryExprAST>\n"
            "      <Op>+</Op>\n"
            "      <LHS>\n"
            "        <NumberExprAST>\n"
            "          <Val>100</Val>\n"
            "        </NumberExprAST>\n"
            "      </LHS>\n"
            "      <RHS>\n"
            "        <CallExprAST>\n"
            "          <Callee>func</Callee>\n"
            "          <Args>\n"
            "            <Arg[0]>\n"
            "              <VariableExprAST>\n"
            "                <Name>x</Name>\n"
            "              </VariableExprAST>\n"
            "            </Arg[0]>\n"
            "            <Arg[1]>\n"
            "              <VariableExprAST>\n"
            "                <Name>idx</Name>\n"
            "              </VariableExprAST>\n"
            "            </Arg[1]>\n"
            "          </Args>\n"
            "        </CallExprAST>\n"
            "      </RHS>\n"
            "    </BinaryExprAST>\n"
            "  </Body>\n"
            "</ForExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, IfExprAST_0) {
  // Arrange
  auto AST = convertAST("if y then func1(y) else func2(y, x);");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<IfExprAST>\n"
            "  <Cond>\n"
            "    <VariableExprAST>\n"
            "      <Name>y</Name>\n"
            "    </VariableExprAST>\n"
            "  </Cond>\n"
            "  <Then>\n"
            "    <CallExprAST>\n"
            "      <Callee>func1</Callee>\n"
            "      <Args>\n"
            "        <Arg[0]>\n"
            "          <VariableExprAST>\n"
            "            <Name>y</Name>\n"
            "          </VariableExprAST>\n"
            "        </Arg[0]>\n"
            "      </Args>\n"
            "    </CallExprAST>\n"
            "  </Then>\n"
            "  <Else>\n"
            "    <CallExprAST>\n"
            "      <Callee>func2</Callee>\n"
            "      <Args>\n"
            "        <Arg[0]>\n"
            "          <VariableExprAST>\n"
            "            <Name>y</Name>\n"
            "          </VariableExprAST>\n"
            "        </Arg[0]>\n"
            "        <Arg[1]>\n"
            "          <VariableExprAST>\n"
            "            <Name>x</Name>\n"
            "          </VariableExprAST>\n"
            "        </Arg[1]>\n"
            "      </Args>\n"
            "    </CallExprAST>\n"
            "  </Else>\n"
            "</IfExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, IfExprAST_1) {
  // Arrange
  auto AST = convertAST("if 32 < h then y else 3;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<IfExprAST>\n"
            "  <Cond>\n"
            "    <BinaryExprAST>\n"
            "      <Op><</Op>\n"
            "      <LHS>\n"
            "        <NumberExprAST>\n"
            "          <Val>32</Val>\n"
            "        </NumberExprAST>\n"
            "      </LHS>\n"
            "      <RHS>\n"
            "        <VariableExprAST>\n"
            "          <Name>h</Name>\n"
            "        </VariableExprAST>\n"
            "      </RHS>\n"
            "    </BinaryExprAST>\n"
            "  </Cond>\n"
            "  <Then>\n"
            "    <VariableExprAST>\n"
            "      <Name>y</Name>\n"
            "    </VariableExprAST>\n"
            "  </Then>\n"
            "  <Else>\n"
            "    <NumberExprAST>\n"
            "      <Val>3</Val>\n"
            "    </NumberExprAST>\n"
            "  </Else>\n"
            "</IfExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, IfExprAST_2) {
  // Arrange
  auto AST = convertAST("if func() then "
                        "if func() then 100 else 20 else x;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<IfExprAST>\n"
            "  <Cond>\n"
            "    <CallExprAST>\n"
            "      <Callee>func</Callee>\n"
            "      <Args>\n"
            "      </Args>\n"
            "    </CallExprAST>\n"
            "  </Cond>\n"
            "  <Then>\n"
            "    <IfExprAST>\n"
            "      <Cond>\n"
            "        <CallExprAST>\n"
            "          <Callee>func</Callee>\n"
            "          <Args>\n"
            "          </Args>\n"
            "        </CallExprAST>\n"
            "      </Cond>\n"
            "      <Then>\n"
            "        <NumberExprAST>\n"
            "          <Val>100</Val>\n"
            "        </NumberExprAST>\n"
            "      </Then>\n"
            "      <Else>\n"
            "        <NumberExprAST>\n"
            "          <Val>20</Val>\n"
            "        </NumberExprAST>\n"
            "      </Else>\n"
            "    </IfExprAST>\n"
            "  </Then>\n"
            "  <Else>\n"
            "    <VariableExprAST>\n"
            "      <Name>x</Name>\n"
            "    </VariableExprAST>\n"
            "  </Else>\n"
            "</IfExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, NumberExprAST_0) {
  // Arrange
  auto AST = convertAST("3.14;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<NumberExprAST>\n"
            "  <Val>3.14</Val>\n"
            "</NumberExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, NumberExprAST_1) {
  // Arrange
  auto AST = convertAST("42.0;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<NumberExprAST>\n"
            "  <Val>42</Val>\n"
            "</NumberExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, VariableExprAST_0) {
  // Arrange
  auto AST = convertAST("x;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<VariableExprAST>\n"
            "  <Name>x</Name>\n"
            "</VariableExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, VariableExprAST_1) {
  // Arrange
  auto AST = convertAST("temp;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<VariableExprAST>\n"
            "  <Name>temp</Name>\n"
            "</VariableExprAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, FunctionExprAST_0) {
  // Arrange
  auto AST = convertAST("def func() 3;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<FunctionAST>\n"
            "  <Proto>\n"
            "    <PrototypeAST>\n"
            "      <Name>func</Name>\n"
            "      <Args>\n"
            "      </Args>\n"
            "    </PrototypeAST>\n"
            "  </Proto>\n"
            "  <Body>\n"
            "    <NumberExprAST>\n"
            "      <Val>3</Val>\n"
            "    </NumberExprAST>\n"
            "  </Body>\n"
            "</FunctionAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, FunctionExprAST_1) {
  // Arrange
  auto AST = convertAST("def i(x) x;");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<FunctionAST>\n"
            "  <Proto>\n"
            "    <PrototypeAST>\n"
            "      <Name>i</Name>\n"
            "      <Args>\n"
            "        <Arg[0]>x</Arg[0]>\n"
            "      </Args>\n"
            "    </PrototypeAST>\n"
            "  </Proto>\n"
            "  <Body>\n"
            "    <VariableExprAST>\n"
            "      <Name>x</Name>\n"
            "    </VariableExprAST>\n"
            "  </Body>\n"
            "</FunctionAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, FunctionExprAST_2) {
  // Arrange
  auto AST = convertAST("def fib(x)\n"
                        "  if x < 3 then\n"
                        "    1\n"
                        "  else\n"
                        "    fib(x-1)+fib(x-2);");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<FunctionAST>\n"
            "  <Proto>\n"
            "    <PrototypeAST>\n"
            "      <Name>fib</Name>\n"
            "      <Args>\n"
            "        <Arg[0]>x</Arg[0]>\n"
            "      </Args>\n"
            "    </PrototypeAST>\n"
            "  </Proto>\n"
            "  <Body>\n"
            "    <IfExprAST>\n"
            "      <Cond>\n"
            "        <BinaryExprAST>\n"
            "          <Op><</Op>\n"
            "          <LHS>\n"
            "            <VariableExprAST>\n"
            "              <Name>x</Name>\n"
            "            </VariableExprAST>\n"
            "          </LHS>\n"
            "          <RHS>\n"
            "            <NumberExprAST>\n"
            "              <Val>3</Val>\n"
            "            </NumberExprAST>\n"
            "          </RHS>\n"
            "        </BinaryExprAST>\n"
            "      </Cond>\n"
            "      <Then>\n"
            "        <NumberExprAST>\n"
            "          <Val>1</Val>\n"
            "        </NumberExprAST>\n"
            "      </Then>\n"
            "      <Else>\n"
            "        <BinaryExprAST>\n"
            "          <Op>+</Op>\n"
            "          <LHS>\n"
            "            <CallExprAST>\n"
            "              <Callee>fib</Callee>\n"
            "              <Args>\n"
            "                <Arg[0]>\n"
            "                  <BinaryExprAST>\n"
            "                    <Op>-</Op>\n"
            "                    <LHS>\n"
            "                      <VariableExprAST>\n"
            "                        <Name>x</Name>\n"
            "                      </VariableExprAST>\n"
            "                    </LHS>\n"
            "                    <RHS>\n"
            "                      <NumberExprAST>\n"
            "                        <Val>1</Val>\n"
            "                      </NumberExprAST>\n"
            "                    </RHS>\n"
            "                  </BinaryExprAST>\n"
            "                </Arg[0]>\n"
            "              </Args>\n"
            "            </CallExprAST>\n"
            "          </LHS>\n"
            "          <RHS>\n"
            "            <CallExprAST>\n"
            "              <Callee>fib</Callee>\n"
            "              <Args>\n"
            "                <Arg[0]>\n"
            "                  <BinaryExprAST>\n"
            "                    <Op>-</Op>\n"
            "                    <LHS>\n"
            "                      <VariableExprAST>\n"
            "                        <Name>x</Name>\n"
            "                      </VariableExprAST>\n"
            "                    </LHS>\n"
            "                    <RHS>\n"
            "                      <NumberExprAST>\n"
            "                        <Val>2</Val>\n"
            "                      </NumberExprAST>\n"
            "                    </RHS>\n"
            "                  </BinaryExprAST>\n"
            "                </Arg[0]>\n"
            "              </Args>\n"
            "            </CallExprAST>\n"
            "          </RHS>\n"
            "        </BinaryExprAST>\n"
            "      </Else>\n"
            "    </IfExprAST>\n"
            "  </Body>\n"
            "</FunctionAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, PrototypeAST_0) {
  // Arrange
  auto AST = convertAST("extern zero_func();");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<PrototypeAST>\n"
            "  <Name>zero_func</Name>\n"
            "  <Args>\n"
            "  </Args>\n"
            "</PrototypeAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, PrototypeAST_1) {
  // Arrange
  auto AST = convertAST("extern one_func(arg);");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<PrototypeAST>\n"
            "  <Name>one_func</Name>\n"
            "  <Args>\n"
            "    <Arg[0]>arg</Arg[0]>\n"
            "  </Args>\n"
            "</PrototypeAST>\n"sv,
            SS.view());
}

TEST(XMLDumpTest, PrototypeAST_2) {
  // Arrange
  auto AST = convertAST("extern many_func(first s _third f_r);");
  std::stringstream SS{};

  // Act
  ast::XMLDump(SS).visit(*AST);

  // Assert
  ASSERT_EQ("<PrototypeAST>\n"
            "  <Name>many_func</Name>\n"
            "  <Args>\n"
            "    <Arg[0]>first</Arg[0]>\n"
            "    <Arg[1]>s</Arg[1]>\n"
            "    <Arg[2]>_third</Arg[2]>\n"
            "    <Arg[3]>f_r</Arg[3]>\n"
            "  </Args>\n"
            "</PrototypeAST>\n"sv,
            SS.view());
}
