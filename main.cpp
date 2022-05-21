#include "CodeGen/CodeGen.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

namespace {
class Driver {
  kaleidoscope::Lexer Lex;
  kaleidoscope::Parser Parse = kaleidoscope::Parser(Lex)
                                   .addBinopPrec('<', 10)
                                   .addBinopPrec('+', 20)
                                   .addBinopPrec('-', 20)
                                   .addBinopPrec('*', 40);
  kaleidoscope::CodeGen CG;

  void handleDefinition();
  void handleExtern();
  void handleTopLevelExpression();

public:
  /// top ::= definition | external | expression | ';'
  void mainLoop();
};
} // namespace

void Driver::handleDefinition() {
  if (auto FnAST = Parse.parseDefinition()) {
    if (auto *FnIR = FnAST->codegen(CG)) {
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

int main() { Driver().mainLoop(); }
