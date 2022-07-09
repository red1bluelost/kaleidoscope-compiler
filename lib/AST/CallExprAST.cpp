#include "AST/CallExprAST.h"

#include "CodeGen/CodeGen.h"
#include "Util/Error/Log.h"

#include <llvm/IR/Function.h>

using namespace kaleidoscope;

llvm::Value *CallExprAST::codegen(CodeGen &CG) {
  // Look up the name in the global module table.
  llvm::Function *CalleeF = CG.getFunction(Callee);
  if (!CalleeF)
    return logErrorR<llvm::Value>("Unknown function referenced");

  // If argument mismatch error.
  if (CalleeF->arg_size() != Args.size())
    return logErrorR<llvm::Value>("Incorrect # arguments passed");

  std::vector<llvm::Value *> ArgsV;
  for (auto &Arg : Args) {
    auto *ArgV = Arg->codegen(CG);
    if (!ArgV)
      return logErrorR<llvm::Value>("Could not codegen arg");
    ArgsV.push_back(ArgV);
  }

  return CG.getBuilder().CreateCall(CalleeF, ArgsV, "calltmp");
}
