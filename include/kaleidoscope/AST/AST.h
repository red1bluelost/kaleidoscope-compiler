#ifndef KALEIDOSCOPE_AST_AST_H
#define KALEIDOSCOPE_AST_AST_H

#include <llvm/Support/Casting.h>

#include <string_view>

namespace kaleidoscope {
class ASTNode {
  ASTNode() = delete;

public:
  enum ASTNodeKind {
    ANK_ExprAST,
    ANK_BinaryExprAST,
    ANK_UnaryExprAST,
    ANK_CallExprAST,
    ANK_ForExprAST,
    ANK_IfExprAST,
    ANK_NumberExprAST,
    ANK_VariableExprAST,
    ANK_LastExprAST,
    ANK_FunctionAST,
    ANK_PrototypeAST,
    ANK_ProtoUnaryAST,
    ANK_ProtoBinaryAST,
    ANK_LastPrototypeAST,
  };

private:
  const ASTNodeKind MyKind;

protected:
  constexpr ASTNode(ASTNodeKind K) noexcept : MyKind(K) {}

public:
  constexpr ASTNodeKind getKind() const noexcept { return MyKind; }

  constexpr virtual ~ASTNode() noexcept = default;
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_AST_H
