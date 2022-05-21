#ifndef KALEIDOSCOPE_AST_EXPRAST_H
#define KALEIDOSCOPE_AST_EXPRAST_H

#include "CodeGen/CodeGen.h"

#include <llvm/IR/Value.h>

namespace kaleidoscope {

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
  virtual ~ExprAST() = default;
  virtual llvm::Value *codegen(CodeGen &CG) = 0;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_EXPRAST_H
