#include "Lexer/Lexer.h"

#include <gtest/gtest.h>

using namespace kaleidoscope;

namespace {

auto makeGetCharWithString(std::string S) -> std::function<int()> {
  return [S, I = 0]() mutable -> int { return static_cast<int>(S[I++]); };
}

TEST(LexerTest, Def) {
  // Arrange
  Lexer Lex{makeGetCharWithString("def")};

  // Act
  int Tok = Lex.gettok();

  // Assert
  ASSERT_EQ(Lexer::tok_def, Tok);
  ASSERT_EQ("def", Lex.getIdentifierStr());
}
} // namespace