#ifndef KALEIDOSCOPE_AST_NUMBEREXPRAST_H
#define KALEIDOSCOPE_AST_NUMBEREXPRAST_H

#include "kaleidoscope/AST/ExprAST.h"

namespace kaleidoscope {

class NumberExprAST : public ExprAST {
  double Val;

public:
  static constexpr ASTNodeKind Kind = ANK_NumberExprAST;
  static constexpr std::string_view NodeName = "NumberExprAST";

  NumberExprAST(double Val) : ExprAST(Kind), Val(Val) {}

  static bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] double getVal() const noexcept { return Val; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_NUMBEREXPRAST_H
