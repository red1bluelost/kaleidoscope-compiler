#ifndef KALEIDOSCOPE_AST_ASTVISITOR_H
#define KALEIDOSCOPE_AST_ASTVISITOR_H

#include "kaleidoscope/AST/AST.h"

#include <llvm/Support/ErrorHandling.h>

namespace kaleidoscope {

enum class AVDelType {
  None = 0,
  ExprAST = (1 << 0),
  PrototypeAST = (1 << 1),
  All = (1 << 2) - 1,
};

#define BITMASK_TYPE AVDelType
#include "kaleidoscope/Util/BitmaskType.def"

template <typename SubClass, AVDelType Delegate>
class ASTVisitor {
#define DELEGATE(AST) (static_cast<SubClass *>(this)->visitImpl(AST))

#define HANDLE_EXPR_AST(AST_TYPE)                                              \
  case AST_TYPE::Kind:                                                         \
    return DELEGATE(llvm::cast<AST_TYPE>(A))

#define VISIT_AST(AST_TYPE)                                                    \
  auto visit(const AST_TYPE &A) { return DELEGATE(A); }

protected:
  constexpr ASTVisitor() noexcept = default;
  constexpr ~ASTVisitor() noexcept = default;

public:
  auto visit(const ASTNode &A) {
    const ASTNode *AP = &A;
#define VISIT_CAST(_type_)                                                     \
  if (const auto *E = llvm::dyn_cast<_type_>(AP))                              \
  return visit(*E)

    VISIT_CAST(ExprAST);
    VISIT_CAST(FunctionAST);
    VISIT_CAST(PrototypeAST);
    VISIT_CAST(EndOfFileAST);

    llvm_unreachable("invalid ASTNode class type");
#undef VISIT_CAST
  }

  auto visit(const ExprAST &A) {
    if constexpr ((Delegate & AVDelType::ExprAST) != AVDelType::None) {
      switch (A.getKind()) {
        HANDLE_EXPR_AST(BinaryExprAST);
        HANDLE_EXPR_AST(UnaryExprAST);
        HANDLE_EXPR_AST(CallExprAST);
        HANDLE_EXPR_AST(ForExprAST);
        HANDLE_EXPR_AST(IfExprAST);
        HANDLE_EXPR_AST(NumberExprAST);
        HANDLE_EXPR_AST(VariableExprAST);
        HANDLE_EXPR_AST(VarAssignExprAST);
      case ASTNode::ANK_ExprAST:
      case ASTNode::ANK_LastExprAST:
      case ASTNode::ANK_FunctionAST:
      case ASTNode::ANK_PrototypeAST:
      case ASTNode::ANK_ProtoUnaryAST:
      case ASTNode::ANK_ProtoBinaryAST:
      case ASTNode::ANK_LastPrototypeAST:
      case ASTNode::ANK_EndOfFileAST:
        break;
      }
      llvm_unreachable("Missing an AST type being handled");
    } else {
      return DELEGATE(A);
    }
  }

  VISIT_AST(BinaryExprAST)
  VISIT_AST(UnaryExprAST)
  VISIT_AST(CallExprAST)
  VISIT_AST(ForExprAST)
  VISIT_AST(IfExprAST)
  VISIT_AST(NumberExprAST)
  VISIT_AST(VariableExprAST)
  VISIT_AST(VarAssignExprAST)

  auto visit(const PrototypeAST &A) {
    if constexpr ((Delegate & AVDelType::PrototypeAST) != AVDelType::None) {
      switch (A.getKind()) {
        HANDLE_EXPR_AST(PrototypeAST);
        HANDLE_EXPR_AST(ProtoUnaryAST);
        HANDLE_EXPR_AST(ProtoBinaryAST);
      case ASTNode::ANK_LastPrototypeAST:
      case ASTNode::ANK_ExprAST:
      case ASTNode::ANK_BinaryExprAST:
      case ASTNode::ANK_UnaryExprAST:
      case ASTNode::ANK_CallExprAST:
      case ASTNode::ANK_ForExprAST:
      case ASTNode::ANK_IfExprAST:
      case ASTNode::ANK_NumberExprAST:
      case ASTNode::ANK_VariableExprAST:
      case ASTNode::ANK_VarAssignExprAST:
      case ASTNode::ANK_LastExprAST:
      case ASTNode::ANK_FunctionAST:
      case ASTNode::ANK_EndOfFileAST:
        break;
      }
      llvm_unreachable("Missing an AST type being handled");
    } else {
      return DELEGATE(A);
    }
  }

  VISIT_AST(ProtoBinaryAST)
  VISIT_AST(ProtoUnaryAST)

  VISIT_AST(FunctionAST)

  VISIT_AST(EndOfFileAST)

#undef HANDLE_EXPR_AST
#undef VISIT_AST
#undef DELEGATE
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_ASTVISITOR_H
