#ifndef KALEIDOSCOPE_LEXER_LEXER_H
#define KALEIDOSCOPE_LEXER_LEXER_H

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

    // tok_err
    tok_err = -6,
  };

private:
  std::string IdentifierStr;
  double NumVal;

  int LastChar = ' ';

  // Identifiers: [a-zA-Z][a-zA-Z0-9]*
  int handleIdentifier();

  // Number: [0-9.]+
  int handleNumber();

  // Comment
  int handleComment();

public:
  const std::string &getIdentifierStr() { return IdentifierStr; }
  double getNumVal() { return NumVal; }

  /// gettok - Return the next token from standard input.
  int gettok();
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_LEXER_LEXER_H
