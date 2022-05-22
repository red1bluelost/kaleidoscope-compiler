#ifndef KALEIDOSCOPE_DRIVER_DRIVER_H
#define KALEIDOSCOPE_DRIVER_DRIVER_H

#include "CodeGen/CodeGen.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#include <llvm/IR/LegacyPassManager.h>

namespace kaleidoscope {

class Driver {
  Lexer Lex;
  Parser Parse;
  CodeGen CG;

  llvm::legacy::FunctionPassManager FPM;

  void handleDefinition();
  void handleExtern();
  void handleTopLevelExpression();

public:
  Driver();
  /// top ::= definition | external | expression | ';'
  void mainLoop();
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_DRIVER_DRIVER_H
