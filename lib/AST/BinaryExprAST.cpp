#include "AST/BinaryExprAST.h"

#include "CodeGen/CodeGen.h"
#include "Util/Error/Log.h"

#include <llvm/IR/IRBuilder.h>

using namespace kaleidoscope;

llvm::Value *BinaryExprAST::codegen(CodeGen &CG) {
  auto *L = LHS->codegen(CG), *R = RHS->codegen(CG);
  if (!L || !R)
    return logErrorR<llvm::Value>("missing rhs and/or lhs");

  auto &Builder = CG.getBuilder();
  switch (Op) {
  default:
    return logErrorR<llvm::Value>("invalid binary operator");
  case '+':
    return Builder.CreateFAdd(L, R, "addtmp");
  case '-':
    return Builder.CreateFSub(L, R, "subtmp");
  case '*':
    return Builder.CreateFMul(L, R, "multmp");
  case '/':
    return Builder.CreateFDiv(L, R, "divtmp");
  case '<':
    // Convert bool 0/1 to double 0.0 or 1.0
    return Builder.CreateUIToFP(
        Builder.CreateFCmpULT(L, R, "cmptmp"),
        llvm::Type::getDoubleTy(CG.getBuilder().getContext()), "booltmp");
  case '>':
    // Convert bool 0/1 to double 0.0 or 1.0
    return Builder.CreateUIToFP(
        Builder.CreateFCmpUGT(L, R, "cmptmp"),
        llvm::Type::getDoubleTy(CG.getBuilder().getContext()), "booltmp");
  }
}
