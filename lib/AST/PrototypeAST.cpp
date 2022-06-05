#include "AST/PrototypeAST.h"

#include "CodeGen/CodeGen.h"

#include <llvm/IR/Type.h>

using namespace kaleidoscope;

llvm::Function *PrototypeAST::codegen(CodeGen &CG) {
  std::vector<llvm::Type *> Doubles(Args.size(),
                                    llvm::Type::getDoubleTy(CG.getContext()));
  llvm::FunctionType *FT = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(CG.getContext()), Doubles, false);
  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, Name, &CG.getModule());

  for (unsigned Idx = 0; auto &Arg : F->args())
    Arg.setName(Args[Idx++]);

  return F;
}
