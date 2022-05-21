#ifndef KALEIDOSCOPE_AST_NUMBEREXPRAST_H
#define KALEIDOSCOPE_AST_NUMBEREXPRAST_H

#include "AST/ExprAST.h"

namespace kaleidoscope {

class NumberExprAST : public ExprAST {
  double Val;

public:
  NumberExprAST(double Val) : Val(Val) {}
  llvm::Value *codegen(CodeGen &CG) override;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_NUMBEREXPRAST_H
