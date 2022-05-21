#include "Lexer/Lexer.h"

#include <cstdio>
#include <utility>

using namespace kaleidoscope;

int Lexer::handleIdentifier() {
  IdentifierStr = LastChar;
  while (std::isalnum((LastChar = std::getchar())))
    IdentifierStr += LastChar;

  if (IdentifierStr == "def")
    return tok_def;
  if (IdentifierStr == "extern")
    return tok_extern;
  return tok_identifier;
}

int Lexer::handleNumber() {
  std::string NumStr;
  do {
    NumStr += LastChar;
    LastChar = std::getchar();
  } while (std::isdigit(LastChar) || LastChar == '.');

  std::size_t Len;
  NumVal = std::stod(NumStr, &Len);
  if (Len != NumStr.length())
    return tok_err;

  return tok_number;
}

int Lexer::handleComment() {
  // Comment until end of line.
  LastChar = std::getchar();
  while (LastChar != EOF && LastChar != '\n' && LastChar != '\r')
    LastChar = std::getchar();

  if (LastChar == EOF)
    return tok_eof;
  return gettok();
}

int Lexer::gettok() {
  // Skip any whitespace.
  while (std::isspace(LastChar))
    LastChar = std::getchar();

  if (std::isalpha(LastChar))
    return handleIdentifier();
  if (std::isdigit(LastChar) || LastChar == '.')
    return handleNumber();
  if (LastChar == '#')
    return handleComment();
  if (LastChar == EOF) // Check for end of file. Don't eat the EOF.
    return tok_eof;

  // Otherwise, just return the character as its ascii value.
  return std::exchange(LastChar, getchar());
}
