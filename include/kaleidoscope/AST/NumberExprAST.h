#ifndef KALEIDOSCOPE_AST_NUMBEREXPRAST_H
#define KALEIDOSCOPE_AST_NUMBEREXPRAST_H

#include "kaleidoscope/AST/ExprAST.h"

namespace kaleidoscope {

class NumberExprAST : public ExprAST {
  const double Val;

public:
  static constexpr ASTNodeKind Kind = ANK_NumberExprAST;
  static constexpr std::string_view NodeName = "NumberExprAST";

  constexpr NumberExprAST(double Val) noexcept : ExprAST(Kind), Val(Val) {}

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] double getVal() const noexcept { return Val; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_NUMBEREXPRAST_H
