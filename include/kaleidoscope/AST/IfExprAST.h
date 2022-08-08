#ifndef KALEIDOSCOPE_AST_IFEXPRAST_H
#define KALEIDOSCOPE_AST_IFEXPRAST_H

#include "kaleidoscope/AST/ExprAST.h"

#include <memory>

namespace kaleidoscope {

/// IfExprAST - Expression class for if/then/else.
class IfExprAST : public ExprAST {
  const std::unique_ptr<ExprAST> Cond, Then, Else;

public:
  static constexpr ASTNodeKind Kind = ANK_IfExprAST;
  static constexpr std::string_view NodeName = "IfExprAST";

  IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
            std::unique_ptr<ExprAST> Else) noexcept
      : ExprAST(Kind), Cond(std::move(Cond)), Then(std::move(Then)),
        Else(std::move(Else)) {}

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] const ExprAST &getCond() const noexcept { return *Cond; }
  [[nodiscard]] const ExprAST &getThen() const noexcept { return *Then; }
  [[nodiscard]] const ExprAST &getElse() const noexcept { return *Else; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_IFEXPRAST_H
