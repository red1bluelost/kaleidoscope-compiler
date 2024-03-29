#ifndef KALEIDOSCOPE_DRIVER_REPLDRIVER_H
#define KALEIDOSCOPE_DRIVER_REPLDRIVER_H

#include "kaleidoscope/AST/ASTVisitor.h"
#include "kaleidoscope/CodeGen/CodeGen.h"
#include "kaleidoscope/JIT/KaleidoscopeJIT.h"
#include "kaleidoscope/Lexer/Lexer.h"
#include "kaleidoscope/Parser/Parser.h"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/Error.h>

namespace kaleidoscope {

class ReplDriver : protected ASTVisitor<ReplDriver, AVDelType::None> {
  using Parent = ASTVisitor<ReplDriver, AVDelType::None>;
  friend Parent;

  using TLEntryPointer = double (*)();

  enum class VisitRet { Success, Error, EndOfFile };

  const llvm::ExitOnError ExitOnErr{};

  Lexer                                  Lex;
  Parser                                 Parse;
  CodeGen                                CG;
  const std::unique_ptr<KaleidoscopeJIT> JIT;

  llvm::TargetMachine* TargetMachine;

  std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;

  auto resetSession() -> std::unique_ptr<CodeGen::Session>;

  auto visitImpl(const ExprAST& A) -> VisitRet;
  auto visitImpl(const FunctionAST& A) -> VisitRet;
  auto visitImpl(const PrototypeAST& A) -> VisitRet;

  auto visitImpl(const EndOfFileAST&) const noexcept -> VisitRet {
    return VisitRet::EndOfFile;
  }

 public:
  explicit ReplDriver();
  /// top ::= definition | external | expression
  void mainLoop();
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_DRIVER_REPLDRIVER_H
