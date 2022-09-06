#include "kaleidoscope/Driver/ReplDriver.h"

#include <llvm/ADT/Optional.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/Host.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>

#include <fmt/core.h>

using namespace kaleidoscope;

static auto setUpFPM(llvm::Module *Mod)
    -> std::unique_ptr<llvm::legacy::FunctionPassManager> {
  auto FPM = std::make_unique<llvm::legacy::FunctionPassManager>(Mod);

  // Promote allocas to registers.
  FPM->add(llvm::createPromoteMemoryToRegisterPass());
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

static void generateObjFile(std::string_view FN, llvm::TargetMachine &TM,
                            llvm::Module &Mod) {
  std::error_code EC;
  llvm::raw_fd_ostream Dest(fmt::format("{}.o", FN), EC,
                            llvm::sys::fs::OF_None);
  if (EC) {
    fmt::print(stderr, "Could not open file: {}\n", EC.message());
    std::exit(1);
  }

  llvm::legacy::PassManager Pass;
  if (TM.addPassesToEmitFile(Pass, Dest, nullptr, llvm::CGFT_ObjectFile)) {
    fmt::print(stderr, "TargetMachine can't emit a file of this type\n");
    std::exit(1);
  }

  Mod.setDataLayout(TM.createDataLayout());
  Mod.setTargetTriple(TM.getTargetTriple().str());

  Pass.run(Mod);
}

ReplDriver::ReplDriver()
    : Lex(), Parse(Lex), CG(), JIT(ExitOnErr(KaleidoscopeJIT::create())) {
  {
    std::string TargetTriple = llvm::sys::getDefaultTargetTriple();
    std::string Error;
    auto *Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
    if (!Target) {
      fmt::print(stderr, "{}\n", Error);
      std::exit(1);
    }
    TargetMachine = Target->createTargetMachine(
        TargetTriple, "generic", "", llvm::TargetOptions{}, llvm::None);
  }

  resetSession();
}

auto ReplDriver::resetSession() -> std::unique_ptr<CodeGen::Session> {
  auto LastCGSess = CG.takeSession();
  auto &Mod = CG.getModule();
  Mod.setDataLayout(JIT->getDataLayout());
  FPM = setUpFPM(&Mod);
  return LastCGSess;
}

auto ReplDriver::visitImpl(const FunctionAST &A) -> VisitRet {
  auto *FnIR = CG.visit(A);
  if (!FnIR)
    return VisitRet::Error;

  FPM->run(*FnIR);
  fmt::print(stderr, "Read function definition:\n");
  llvm::errs() << *FnIR;
  fmt::print(stderr, "\n");
  auto CGSess = resetSession();

  generateObjFile(A.getProto().getName(), *TargetMachine, *CGSess->Module);

  ExitOnErr(JIT->addModule(llvm::orc::ThreadSafeModule(
      std::move(CGSess->Module), std::move(CGSess->Context))));
  return VisitRet::Success;
}

auto ReplDriver::visitImpl(const PrototypeAST &A) -> VisitRet {
  auto *FnIR = CG.visit(A);
  if (!FnIR)
    return VisitRet::Error;

  fmt::print(stderr, "Read extern:\n");
  llvm::errs() << *FnIR;
  fmt::print(stderr, "\n");
  CG.addPrototype(std::make_unique<PrototypeAST>(A));
  return VisitRet::Success;
}

auto ReplDriver::visitImpl(const ExprAST &A) -> VisitRet {
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
  auto FP = reinterpret_cast<TLEntryPointer>(
      static_cast<intptr_t>(ExprSymbol.getAddress()));
  fmt::print(stderr, "Evaluated to {}\n", FP());

  // Delete the anonymous expression module from the JIT.
  ExitOnErr(RT->remove());
  return VisitRet::Success;
}

void ReplDriver::mainLoop() {
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