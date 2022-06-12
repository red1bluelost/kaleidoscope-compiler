#ifndef KALEIDOSCOPE_AST_IFEXPRAST_H
#define KALEIDOSCOPE_AST_IFEXPRAST_H

#include "AST/ExprAST.h"

#include <memory>

namespace kaleidoscope {

/// IfExprAST - Expression class for if/then/else.
class IfExprAST : public ExprAST {
  std::unique_ptr<ExprAST> Cond, Then, Else;

public:
  IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
            std::unique_ptr<ExprAST> Else)
      : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

  llvm::Value *codegen(CodeGen &CG) override;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_IFEXPRAST_H
