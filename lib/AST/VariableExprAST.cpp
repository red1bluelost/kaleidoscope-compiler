#include "AST/VariableExprAST.h"

#include "Error/Log.h"

using namespace kaleidoscope;

llvm::Value *VariableExprAST::codegen(CodeGen &CG) {
  if (auto *V = CG.getNamedVal(Name))
    return V;
  return logErrorR<llvm::Value>("unknown variable name");
}
