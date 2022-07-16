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
  static constexpr ExprASTKind Kind = EAK_BinaryExprAST;

  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : ExprAST(Kind), Op(Op), LHS(std::move(LHS)),
        RHS(std::move(RHS)) {}

  static bool classof(const ExprAST *E) noexcept {
    return E->getKind() == Kind;
  }

  llvm::Value *codegen(CodeGen &CG) override;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_BINARYEXPRAST_H
