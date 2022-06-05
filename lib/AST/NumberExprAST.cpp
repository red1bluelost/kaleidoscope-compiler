#include "AST/NumberExprAST.h"

#include "CodeGen/CodeGen.h"

#include <llvm/IR/Constants.h>

using namespace kaleidoscope;

llvm::Value *NumberExprAST::codegen(CodeGen &CG) {
  return llvm::ConstantFP::get(CG.getContext(), llvm::APFloat(Val));
}
