#ifndef KALEIDOSCOPE_AST_EXPRAST_H
#define KALEIDOSCOPE_AST_EXPRAST_H

#include "kaleidoscope/AST/AST.h"

namespace kaleidoscope {

/// ExprAST - Base class for all expression nodes.
class ExprAST : public ASTNode {
  ExprAST() = delete;

protected:
  ExprAST(ASTNodeKind K) : ASTNode(K) {}

public:
  static constexpr ASTNodeKind Kind = ANK_ExprAST;

  static bool classof(const ASTNode *A) noexcept {
    return A->getKind() >= ANK_ExprAST && A->getKind() <= ANK_LastExprAST;
  }

  virtual ~ExprAST() = default;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_EXPRAST_H
