#ifndef KALEIDOSCOPE_AST_BINARYEXPRAST_H
#define KALEIDOSCOPE_AST_BINARYEXPRAST_H

#include "AST/ExprAST.h"

#include <memory>

namespace kaleidoscope {

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_BINARYEXPRAST_H
