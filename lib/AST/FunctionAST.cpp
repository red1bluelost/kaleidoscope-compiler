#include "AST/FunctionAST.h"

#include "CodeGen/CodeGen.h"
#include "Util/Error/Log.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>

using namespace kaleidoscope;

llvm::Function *FunctionAST::codegen(CodeGen &CG) {
  assert(Proto != nullptr && "codegen already called for function AST");

  // transfer ownership of the prototype to the FunctionProtos map
  auto P = CG.addPrototype(std::move(Proto));
  llvm::Function *TheFunction = CG.getFunction(P.getName());
  if (!TheFunction)
    return nullptr;

  auto &PArgs = P.getArgs();
  if (PArgs.size() != TheFunction->arg_size())
    return logErrorR<llvm::Function>(
        "arg names do not match length in prototype");
  for (unsigned Idx = 0; auto &Arg : TheFunction->args())
    if (Arg.getName() != PArgs[Idx])
      return logErrorR<llvm::Function>("arg names do not match prototype");
  if (!TheFunction->empty())
    return logErrorR<llvm::Function>("Function cannot be redefined");

  // create a new basic block to start insertion into
  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(CG.getContext(), "entry", TheFunction);
  CG.getBuilder().SetInsertPoint(BB);

  // record the function arguments in the NamedValues map
  CG.recordFuncArgs(*TheFunction);

  if (llvm::Value *RetVal = Body->codegen(CG)) {
    CG.getBuilder().CreateRet(RetVal); // Finish off the function
    llvm::verifyFunction(*TheFunction);
    return TheFunction;
  }

  // Error reading body, remove function
  TheFunction->eraseFromParent();
  return logErrorR<llvm::Function>("Failed to codegen function body");
}
