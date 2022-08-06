#ifndef KALEIDOSCOPE_DRIVER_DRIVER_H
#define KALEIDOSCOPE_DRIVER_DRIVER_H

#include "kaleidoscope/AST/ASTVisitor.h"
#include "kaleidoscope/CodeGen/CodeGen.h"
#include "kaleidoscope/JIT/KaleidoscopeJIT.h"
#include "kaleidoscope/Lexer/Lexer.h"
#include "kaleidoscope/Parser/Parser.h"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/Error.h>

namespace kaleidoscope {

class Driver : public ASTVisitor<Driver, /*DelegateExprAST=*/false> {
  using Parent = ASTVisitor<Driver, false>;
  friend Parent;

  using TLEntryPointer = double (*)();

  enum class VisitRet { Success, Error, EndOfFile };

  llvm::ExitOnError ExitOnErr{};

  Lexer Lex;
  Parser Parse;
  CodeGen CG;
  std::unique_ptr<KaleidoscopeJIT> JIT;

  std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;

  std::unique_ptr<CodeGen::Session> resetSession();

  VisitRet visitImpl(ExprAST &A);
  VisitRet visitImpl(FunctionAST &A);
  VisitRet visitImpl(PrototypeAST &A);

public:
  Driver();
  /// top ::= definition | external | expression | ';'
  void mainLoop();
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_DRIVER_DRIVER_H
