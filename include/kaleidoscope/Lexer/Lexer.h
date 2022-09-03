#ifndef KALEIDOSCOPE_LEXER_LEXER_H
#define KALEIDOSCOPE_LEXER_LEXER_H

#include <cstdio>
#include <functional>
#include <string>

namespace kaleidoscope {

class Lexer {
public:
  /// The lexer returns tokens [0-255] if it is an unknown character, otherwise
  /// one of these for known things.
  enum Token {
    tok_eof = -1,

    // commands
    tok_def = -2,
    tok_extern = -3,

    // primary
    tok_identifier = -4,
    tok_number = -5,

    // control
    tok_if = -6,
    tok_then = -7,
    tok_else = -8,
    tok_for = -9,
    tok_in = -10,

    // operators
    tok_binary = -11,
    tok_unary = -12,

    // variable assignment
    tok_var = -13,

    // tok_err
    tok_err = -256,
  };

private:
  std::function<int()> GetChar;
  std::string IdentifierStr;
  double NumVal;

  int LastChar = ' ';

  // Identifiers: [_a-zA-Z][_a-zA-Z0-9]*
  auto handleIdentifier() -> int;

  // Number: [0-9.]+
  auto handleNumber() -> int;

  // Comment
  auto handleComment() -> int;

public:
  Lexer(std::function<int()> GetChar = std::getchar) noexcept
      : GetChar(std::move(GetChar)) {}

  [[nodiscard]] auto getIdentifierStr() const noexcept -> const std::string & {
    return IdentifierStr;
  }
  [[nodiscard]] auto getNumVal() const noexcept -> double { return NumVal; }

  /// gettok - Return the next token from standard input.
  auto gettok() -> int;
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_LEXER_LEXER_H
