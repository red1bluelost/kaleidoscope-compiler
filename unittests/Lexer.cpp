#include "kaleidoscope/Lexer/Lexer.h"

#include "TestUtil.h"

#include <gtest/gtest.h>

using namespace kaleidoscope;

namespace {

TEST(LexerTest, Def) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_def, Tok);
  ASSERT_EQ("def", Lex.getIdentifierStr());
}

TEST(LexerTest, Extern) {
  // Arrange
  Lexer Lex{makeGetCharWithString("extern")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_extern, Tok);
  ASSERT_EQ("extern", Lex.getIdentifierStr());
}

TEST(LexerTest, If) {
  // Arrange
  Lexer Lex{makeGetCharWithString("if")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_if, Tok);
  ASSERT_EQ("if", Lex.getIdentifierStr());
}

TEST(LexerTest, Then) {
  // Arrange
  Lexer Lex{makeGetCharWithString("then")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_then, Tok);
  ASSERT_EQ("then", Lex.getIdentifierStr());
}

TEST(LexerTest, Else) {
  // Arrange
  Lexer Lex{makeGetCharWithString("else")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_else, Tok);
  ASSERT_EQ("else", Lex.getIdentifierStr());
}

TEST(LexerTest, In) {
  // Arrange
  Lexer Lex{makeGetCharWithString("in")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_in, Tok);
  ASSERT_EQ("in", Lex.getIdentifierStr());
}

TEST(LexerTest, Binary) {
  // Arrange
  Lexer Lex{makeGetCharWithString("binary")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_binary, Tok);
  ASSERT_EQ("binary", Lex.getIdentifierStr());
}

TEST(LexerTest, Unary) {
  // Arrange
  Lexer Lex{makeGetCharWithString("unary")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_unary, Tok);
  ASSERT_EQ("unary", Lex.getIdentifierStr());
}

TEST(LexerTest, Identifier_variable) {
  // Arrange
  Lexer Lex{makeGetCharWithString("variable")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_identifier, Tok);
  ASSERT_EQ("variable", Lex.getIdentifierStr());
}

TEST(LexerTest, Number_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("0.0")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_number, Tok);
  ASSERT_EQ(0.0, Lex.getNumVal());
}

TEST(LexerTest, Number_1) {
  // Arrange
  Lexer Lex{makeGetCharWithString("3.1415")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_number, Tok);
  ASSERT_EQ(3.1415, Lex.getNumVal());
}

TEST(LexerTest, Number_2) {
  // Arrange
  Lexer Lex{makeGetCharWithString("6.9")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_number, Tok);
  ASSERT_EQ(6.9, Lex.getNumVal());
}

TEST(LexerTest, Char_LT) {
  // Arrange
  Lexer Lex{makeGetCharWithString("<")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ('<', Tok);
}

TEST(LexerTest, Char_EQ) {
  // Arrange
  Lexer Lex{makeGetCharWithString("=")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ('=', Tok);
}

TEST(LexerTest, Char_OpenParen) {
  // Arrange
  Lexer Lex{makeGetCharWithString("(")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ('(', Tok);
}

TEST(LexerTest, EndOfFile) {
  // Arrange
  Lexer Lex{makeGetCharWithString(
      std::string(1, static_cast<char>(std::char_traits<char>::eof())))};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_eof, Tok);
}

TEST(LexerTest, MultipleTokens_0) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def identity(x) x")};

  // Act Assert
  ASSERT_EQ(Lexer::tok_def, Lex.gettok());
  ASSERT_EQ("def", Lex.getIdentifierStr());

  ASSERT_EQ(Lexer::tok_identifier, Lex.gettok());
  ASSERT_EQ("identity", Lex.getIdentifierStr());

  ASSERT_EQ('(', Lex.gettok());

  ASSERT_EQ(Lexer::tok_identifier, Lex.gettok());
  ASSERT_EQ("x", Lex.getIdentifierStr());

  ASSERT_EQ(')', Lex.gettok());

  ASSERT_EQ(Lexer::tok_identifier, Lex.gettok());
  ASSERT_EQ("x", Lex.getIdentifierStr());
}
} // namespace