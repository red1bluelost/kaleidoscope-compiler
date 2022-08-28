

#ifndef KALEIDOSCOPE_AST_UNARYEXPRAST_H
#define KALEIDOSCOPE_AST_UNARYEXPRAST_H

#include "kaleidoscope/AST/ExprAST.h"

#include <memory>

namespace kaleidoscope {

/// UnaryExprAST - Expression class for a unary operator.
class UnaryExprAST : public ExprAST {
  char Opcode;
  std::unique_ptr<ExprAST> Operand;

public:
  static constexpr ASTNodeKind Kind = ANK_UnaryExprAST;
  static constexpr std::string_view NodeName = "UnaryExprAST";

  UnaryExprAST(char Op, std::unique_ptr<ExprAST> Operand) noexcept
      : ExprAST(Kind), Opcode(Op), Operand(std::move(Operand)) {}

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] char getOpcode() const noexcept { return Opcode; }
  [[nodiscard]] const ExprAST &getOperand() const noexcept { return *Operand; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_UNARYEXPRAST_H
