#include "AST/FunctionAST.h"

#include "CodeGen/CodeGen.h"
#include "Error/Log.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>

using namespace kaleidoscope;

llvm::Function *FunctionAST::codegen(CodeGen &CG) {
  // check for an existing function from a previous 'extern' declaration
  llvm::Function *TheFunction = CG.Module.getFunction(Proto->getName());

  if (TheFunction) {
    for (unsigned Idx = 0; auto &Arg : TheFunction->args())
      if (Arg.getName() != Proto->getArgs()[Idx])
        return logErrorR<llvm::Function>("arg names do not match prototype");
  } else
    TheFunction = Proto->codegen(CG);
  if (!TheFunction)
    return nullptr;
  if (!TheFunction->empty())
    return logErrorR<llvm::Function>("Function cannot be redefined");

  // create a new basic block to start insertion into
  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(CG.Context, "entry", TheFunction);
  CG.Builder.SetInsertPoint(BB);

  // record the function arguments in the NamedValues map
  CG.recordFuncArgs(*TheFunction);

  if (llvm::Value *RetVal = Body->codegen(CG)) {
    CG.Builder.CreateRet(RetVal); // Finish off the function
    llvm::verifyFunction(*TheFunction);
    return TheFunction;
  }
  // Error reading body, remove function
  TheFunction->eraseFromParent();
  return logErrorR<llvm::Function>("Failed to codegen function body");
}
