#ifndef KALEIDOSCOPE_UNITTESTS_TESTUTIL_H
#define KALEIDOSCOPE_UNITTESTS_TESTUTIL_H

#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#include <functional>
#include <string>

inline auto makeGetCharWithString(std::string S) -> std::function<int()> {
  S += static_cast<char>(EOF);
  return [S, I = static_cast<std::size_t>(0)]() mutable -> int {
    return static_cast<int>(S[I++]);
  };
}

inline kaleidoscope::Parser makeParser(kaleidoscope::Lexer &Lex) {
  return std::move(kaleidoscope::Parser(Lex)
                       .addBinopPrec('<', 10)
                       .addBinopPrec('>', 10)
                       .addBinopPrec('+', 20)
                       .addBinopPrec('-', 20)
                       .addBinopPrec('*', 40)
                       .addBinopPrec('/', 40));
}

#endif // KALEIDOSCOPE_UNITTESTS_TESTUTIL_H
