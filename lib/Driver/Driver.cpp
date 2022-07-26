#include "Driver/Driver.h"

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

void Driver::handleDefinition() {
  if (auto FnAST = Parse.parseDefinition()) {
    if (auto *FnIR = CG.visit(*FnAST)) {
      FPM->run(*FnIR);
      fmt::print(stderr, "Read function definition:\n");
      llvm::errs() << *FnIR;
      fmt::print(stderr, "\n");
      auto CGSess = resetSession();
      ExitOnErr(JIT->addModule(llvm::orc::ThreadSafeModule(
          std::move(CGSess->Module), std::move(CGSess->Context))));
    }
  } else
    Parse.getNextToken(); // skip token for error recovery
}

void Driver::handleExtern() {
  if (auto ProtoAST = Parse.parseExtern()) {
    if (auto *FnIR = CG.visit(*ProtoAST)) {
      fmt::print(stderr, "Read extern:\n");
      llvm::errs() << *FnIR;
      fmt::print(stderr, "\n");
      CG.addPrototype(std::move(ProtoAST));
    }
  } else
    Parse.getNextToken(); // skip token for error recovery
}

void Driver::handleTopLevelExpression() {
  // evaluate a top-level expression into an anonymous function
  if (auto FnAST = Parse.parseTopLevelExpr()) {
    if (auto *FnIR = CG.visit(*FnAST)) {
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
    }
  } else
    Parse.getNextToken(); // skip token for error recovery
}

void Driver::mainLoop() {
  fmt::print(stderr, "ready> ");
  Parse.getNextToken();
  while (true) {
    switch (Parse.getCurToken()) {
    case kaleidoscope::Lexer::tok_eof:
      llvm::errs() << CG.getModule();
      return;
    case ';': // ignore top-level semicolons
      Parse.getNextToken();
      break;
    case kaleidoscope::Lexer::tok_def:
      handleDefinition();
      break;
    case kaleidoscope::Lexer::tok_extern:
      handleExtern();
      break;
    default:
      handleTopLevelExpression();
      break;
    }
    fmt::print(stderr, "ready> ");
  }
}
