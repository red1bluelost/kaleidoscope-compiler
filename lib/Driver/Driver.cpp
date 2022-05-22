#include "Driver/Driver.h"

#include <fmt/core.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

using namespace kaleidoscope;

Driver::Driver()
    : Lex(), Parse(Parser(Lex)
                       .addBinopPrec('<', 10)
                       .addBinopPrec('+', 20)
                       .addBinopPrec('-', 20)
                       .addBinopPrec('*', 40)),
      CG(), FPM(&CG.Module) {
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  FPM.add(llvm::createInstructionCombiningPass());
  // Reassociate expressions.
  FPM.add(llvm::createReassociatePass());
  // Eliminate Common SubExpressions.
  FPM.add(llvm::createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  FPM.add(llvm::createCFGSimplificationPass());

  FPM.doInitialization();
}

void Driver::handleDefinition() {
  if (auto FnAST = Parse.parseDefinition()) {
    if (auto *FnIR = FnAST->codegen(CG)) {
      FPM.run(*FnIR);
      fmt::print(stderr, "Read function definition:\n");
      llvm::errs() << *FnIR;
      fmt::print(stderr, "\n");
    }
  } else
    Parse.getNextToken(); // skip token for error recovery
}

void Driver::handleExtern() {
  if (auto ProtoAST = Parse.parseExtern()) {
    if (auto *FnIR = ProtoAST->codegen(CG)) {
      fmt::print(stderr, "Read extern:\n");
      llvm::errs() << *FnIR;
      fmt::print(stderr, "\n");
    }
  } else
    Parse.getNextToken(); // skip token for error recovery
}

void Driver::handleTopLevelExpression() {
  // evaluate a top-level expression into an anonymous function
  if (auto FnAST = Parse.parseTopLevelExpr()) {
    if (auto *FnIR = FnAST->codegen(CG)) {
      FPM.run(*FnIR);
      fmt::print(stderr, "Read top-level expression:\n");
      llvm::errs() << *FnIR;
      fmt::print(stderr, "\n");
      FnIR->eraseFromParent(); // remove the anonymous expression
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
      llvm::errs() << CG.Module;
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
