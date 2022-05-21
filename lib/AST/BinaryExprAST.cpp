#include "AST/BinaryExprAST.h"

#include "CodeGen/CodeGen.h"
#include "Error/Log.h"

#include <llvm/IR/IRBuilder.h>

using namespace kaleidoscope;

llvm::Value *BinaryExprAST::codegen(CodeGen &CG) {
  auto *L = LHS->codegen(CG), *R = RHS->codegen(CG);
  if (!L || !R)
    return logErrorR<llvm::Value>("missing rhs and/or lhs");

  switch (Op) {
  default:
    return logErrorR<llvm::Value>("invalid binary operator");
  case '+':
    return CG.Builder.CreateFAdd(L, R, "addtmp");
  case '-':
    return CG.Builder.CreateFSub(L, R, "subtmp");
  case '*':
    return CG.Builder.CreateFMul(L, R, "multmp");
  case '<':
    // Convert bool 0/1 to double 0.0 or 1.0
    return CG.Builder.CreateUIToFP(
        CG.Builder.CreateFCmpULT(L, R, "cmptmp"),
        llvm::Type::getDoubleTy(CG.Builder.getContext()), "booltmp");
  }
  return nullptr;
}
