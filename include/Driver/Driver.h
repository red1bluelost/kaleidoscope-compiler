#ifndef KALEIDOSCOPE_DRIVER_DRIVER_H
#define KALEIDOSCOPE_DRIVER_DRIVER_H

#include "CodeGen/CodeGen.h"
#include "JIT/KaleidoscopeJIT.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/Error.h>

namespace kaleidoscope {

class Driver {
  using TLEntryPointer = double(*)();

  llvm::ExitOnError ExitOnErr{};

  Lexer Lex;
  Parser Parse;
  CodeGen CG;
  std::unique_ptr<KaleidoscopeJIT> JIT;

  std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;

  std::unique_ptr<CodeGen::Session> resetSession();

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
