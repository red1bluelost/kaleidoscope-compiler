#ifndef KALEIDOSCOPE_AST_AST_H
#define KALEIDOSCOPE_AST_AST_H

#include <llvm/Support/Casting.h>

namespace kaleidoscope {
class ASTNode {
  ASTNode() = delete;

public:
  enum ASTNodeKind {
    ANK_ExprAST,
    ANK_BinaryExprAST,
    ANK_CallExprAST,
    ANK_ForExprAST,
    ANK_IfExprAST,
    ANK_NumberExprAST,
    ANK_VariableExprAST,
    ANK_LastExprAST,
    ANK_FunctionAST,
    ANK_PrototypeAST,
  };

private:
  const ASTNodeKind Kind;

protected:
  ASTNode(ASTNodeKind K) : Kind(K) {}

public:
  ASTNodeKind getKind() const { return Kind; }

  virtual ~ASTNode() = default;
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_AST_H
