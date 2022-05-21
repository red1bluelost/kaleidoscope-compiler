#include "AST/PrototypeAST.h"

#include "CodeGen/CodeGen.h"

#include <llvm/IR/Type.h>

using namespace kaleidoscope;

llvm::Function *PrototypeAST::codegen(CodeGen &CG) {
  std::vector<llvm::Type *> Doubles(Args.size(),
                                    llvm::Type::getDoubleTy(CG.Context));
  llvm::FunctionType *FT = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(CG.Context), Doubles, false);
  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, Name, CG.Module.get());

  for (unsigned Idx = 0; auto &Arg : F->args())
    Arg.setName(Args[Idx++]);

  return F;
}
