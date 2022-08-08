#ifndef KALEIDOSCOPE_AST_EXPRAST_H
#define KALEIDOSCOPE_AST_EXPRAST_H

#include "kaleidoscope/AST/AST.h"

namespace kaleidoscope {

/// ExprAST - Base class for all expression nodes.
class ExprAST : public ASTNode {
  ExprAST() = delete;

protected:
  constexpr ExprAST(ASTNodeKind K) noexcept : ASTNode(K) {}

public:
  static constexpr ASTNodeKind Kind = ANK_ExprAST;

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() >= ANK_ExprAST && A->getKind() <= ANK_LastExprAST;
  }

  constexpr virtual ~ExprAST() noexcept = default;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_EXPRAST_H
