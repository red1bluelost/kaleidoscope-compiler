#include "kaleidoscope/Lexer/Lexer.h"

#include <llvm/ADT/StringSwitch.h>

#include <cstdio>
#include <utility>

using namespace kaleidoscope;

auto Lexer::handleIdentifier() -> int {
  IdentifierStr = static_cast<char>(LastChar);
  while (std::isalnum((LastChar = GetChar())) || LastChar == '_')
    IdentifierStr += static_cast<char>(LastChar);

  return llvm::StringSwitch<int>(IdentifierStr)
      .Case("def", tok_def)
      .Case("extern", tok_extern)
      .Case("if", tok_if)
      .Case("then", tok_then)
      .Case("else", tok_else)
      .Case("for", tok_for)
      .Case("in", tok_in)
      .Case("binary", tok_binary)
      .Case("unary", tok_unary)
      .Case("var", tok_var)
      .Default(tok_identifier);
}

auto Lexer::handleNumber() -> int {
  std::string NumStr;
  do {
    NumStr += static_cast<char>(LastChar);
    LastChar = GetChar();
  } while (std::isdigit(LastChar) || LastChar == '.');

  std::size_t Len;
  NumVal = std::stod(NumStr, &Len);
  if (Len != NumStr.length()) return tok_err;

  return tok_number;
}

auto Lexer::handleComment() -> int {
  // Comment until end of line.
  LastChar = GetChar();
  while (LastChar != EOF && LastChar != '\n' && LastChar != '\r')
    LastChar = GetChar();

  if (LastChar == EOF) return tok_eof;
  return gettok();
}

auto Lexer::gettok() -> int {
  // Skip any whitespace.
  while (std::isspace(LastChar)) LastChar = GetChar();

  if (std::isalpha(LastChar) || LastChar == '_') return handleIdentifier();
  if (std::isdigit(LastChar) || LastChar == '.') return handleNumber();
  if (LastChar == '#') return handleComment();
  if (LastChar == EOF) // Check for end of file. Don't eat the EOF.
    return tok_eof;

  // Otherwise, just return the character as its ascii value.
  return std::exchange(LastChar, GetChar());
}
