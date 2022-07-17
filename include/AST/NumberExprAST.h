#ifndef KALEIDOSCOPE_AST_NUMBEREXPRAST_H
#define KALEIDOSCOPE_AST_NUMBEREXPRAST_H

#include "AST/ExprAST.h"

namespace kaleidoscope {

class NumberExprAST : public ExprAST {
  double Val;

public:
  static constexpr ExprASTKind Kind = EAK_NumberExprAST;

  NumberExprAST(double Val) : ExprAST(Kind), Val(Val) {}

  static bool classof(const ExprAST *E) noexcept {
    return E->getKind() == Kind;
  }

  [[nodiscard]] double getVal() const noexcept { return Val; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_NUMBEREXPRAST_H
