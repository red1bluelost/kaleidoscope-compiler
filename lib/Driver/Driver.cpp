#include "kaleidoscope/Driver/Driver.h"

#include <fmt/core.h>
#include <llvm/Support/Error.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

using namespace kaleidoscope;

static std::unique_ptr<llvm::legacy::FunctionPassManager>
setUpFPM(llvm::Module *Mod) {
  auto FPM = std::make_unique<llvm::legacy::FunctionPassManager>(Mod);

  // Do simple "peephole" optimizations and bit-twiddling optzns.
  FPM->add(llvm::createInstructionCombiningPass());
  // Reassociate expressions.
  FPM->add(llvm::createReassociatePass());
  // Eliminate Common SubExpressions.
  FPM->add(llvm::createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  FPM->add(llvm::createCFGSimplificationPass());

  FPM->doInitialization();

  return FPM;
}

Driver::Driver()
    : Lex(), Parse(Lex), CG(), JIT(ExitOnErr(KaleidoscopeJIT::create())) {
  resetSession();
}

std::unique_ptr<CodeGen::Session> Driver::resetSession() {
  auto LastCGSess = CG.takeSession();
  auto &Mod = CG.getModule();
  Mod.setDataLayout(JIT->getDataLayout());
  FPM = setUpFPM(&Mod);
  return LastCGSess;
}

Driver::VisitRet Driver::visitImpl(const FunctionAST &A) {
  auto *FnIR = CG.visit(A);
  if (!FnIR)
    return VisitRet::Error;

  FPM->run(*FnIR);
  fmt::print(stderr, "Read function definition:\n");
  llvm::errs() << *FnIR;
  fmt::print(stderr, "\n");
  auto CGSess = resetSession();
  ExitOnErr(JIT->addModule(llvm::orc::ThreadSafeModule(
      std::move(CGSess->Module), std::move(CGSess->Context))));
  return VisitRet::Success;
}

Driver::VisitRet Driver::visitImpl(const PrototypeAST &A) {
  auto *FnIR = CG.visit(A);
  if (!FnIR)
    return VisitRet::Error;

  fmt::print(stderr, "Read extern:\n");
  llvm::errs() << *FnIR;
  fmt::print(stderr, "\n");
  CG.addPrototype(std::make_unique<PrototypeAST>(A));
  return VisitRet::Success;
}

Driver::VisitRet Driver::visitImpl(const ExprAST &A) {
  auto *FnIR = CG.handleAnonExpr(A);
  if (!FnIR)
    return VisitRet::Error;

  FPM->run(*FnIR);
  fmt::print(stderr, "Read top-level expression:\n");
  llvm::errs() << *FnIR;
  fmt::print(stderr, "\n");

  // Create a ResourceTracker to track JIT'd memory allocated to our
  // anonymous expression -- that way we can free it after executing.
  auto RT = JIT->getMainJITDylib().createResourceTracker();

  auto CGSess = resetSession();
  auto TSM = llvm::orc::ThreadSafeModule(std::move(CGSess->Module),
                                         std::move(CGSess->Context));
  ExitOnErr(JIT->addModule(std::move(TSM), RT));

  // Search the JIT for the __anon_expr symbol.
  auto ExprSymbol = ExitOnErr(JIT->lookup("__anon_expr"));

  // Get the symbol's address and cast it to the right type (takes no
  // arguments, returns a double) so we can call it as a native function.
  TLEntryPointer FP =
      (TLEntryPointer) static_cast<intptr_t>(ExprSymbol.getAddress());
  fmt::print(stderr, "Evaluated to {}\n", FP());

  // Delete the anonymous expression module from the JIT.
  ExitOnErr(RT->remove());
  return VisitRet::Success;
}

void Driver::mainLoop() {
  while (true) {
    fmt::print(stderr, "ready> ");

    std::unique_ptr<ASTNode> AST = Parse.parse();
    if (!AST) {
      fmt::print(stderr, "parse failed in driver\n");
      continue;
    }

    switch (visit(*AST)) {
    case VisitRet::Success:
      break;
    case VisitRet::Error:
    case VisitRet::EndOfFile:
      llvm::errs() << CG.getModule();
      return;
    }
  }
}