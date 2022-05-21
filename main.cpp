#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

int main() {
  kaleidoscope::Lexer Lex;
  kaleidoscope::Parser Parse = kaleidoscope::Parser(Lex)
                                   .addBinopPrec('<', 10)
                                   .addBinopPrec('+', 20)
                                   .addBinopPrec('-', 20)
                                   .addBinopPrec('*', 40);
}
