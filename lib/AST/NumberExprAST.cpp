#include "AST/NumberExprAST.h"

using namespace kaleidoscope;

llvm::Value *NumberExprAST::codegen(CodeGen &CG) {
  return llvm::ConstantFP::get(CG.getBuilder().getContext(),
                               llvm::APFloat(Val));
}
