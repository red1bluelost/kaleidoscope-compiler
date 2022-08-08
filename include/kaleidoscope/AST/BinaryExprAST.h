#ifndef KALEIDOSCOPE_AST_BINARYEXPRAST_H
#define KALEIDOSCOPE_AST_BINARYEXPRAST_H

#include "kaleidoscope/AST/ExprAST.h"

#include <memory>

namespace kaleidoscope {

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  const char Op;
  const std::unique_ptr<ExprAST> LHS, RHS;

public:
  static constexpr ASTNodeKind Kind = ANK_BinaryExprAST;
  static constexpr std::string_view NodeName = "BinaryExprAST";

  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS) noexcept
      : ExprAST(Kind), Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] char getOp() const noexcept { return Op; }
  [[nodiscard]] const ExprAST &getLHS() const noexcept { return *LHS; }
  [[nodiscard]] const ExprAST &getRHS() const noexcept { return *RHS; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_BINARYEXPRAST_H
