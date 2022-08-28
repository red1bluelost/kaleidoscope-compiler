#ifndef KALEIDOSCOPE_AST_ASTVISITOR_H
#define KALEIDOSCOPE_AST_ASTVISITOR_H

#include "kaleidoscope/AST/BinaryExprAST.h"
#include "kaleidoscope/AST/CallExprAST.h"
#include "kaleidoscope/AST/ExprAST.h"
#include "kaleidoscope/AST/ForExprAST.h"
#include "kaleidoscope/AST/FunctionAST.h"
#include "kaleidoscope/AST/IfExprAST.h"
#include "kaleidoscope/AST/NumberExprAST.h"
#include "kaleidoscope/AST/ProtoBinaryAST.h"
#include "kaleidoscope/AST/ProtoUnaryAST.h"
#include "kaleidoscope/AST/PrototypeAST.h"
#include "kaleidoscope/AST/UnaryExprAST.h"
#include "kaleidoscope/AST/VariableExprAST.h"

#include <llvm/Support/ErrorHandling.h>

#include <type_traits>

namespace kaleidoscope {

template <typename SubClass, bool DelegateAST = true>
class ASTVisitor {
#define DELEGATE(AST) static_cast<SubClass *>(this)->visitImpl(AST)

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
    if (const auto *E = llvm::dyn_cast<ExprAST>(AP))
      return visit(*E);
    if (const auto *E = llvm::dyn_cast<FunctionAST>(AP))
      return visit(*E);
    if (const auto *E = llvm::dyn_cast<PrototypeAST>(AP))
      return visit(*E);
    llvm_unreachable("invalid ASTNode class type");
  }

  auto visit(const ExprAST &A) {
    if constexpr (DelegateAST) {
      switch (A.getKind()) {
        HANDLE_EXPR_AST(BinaryExprAST);
        HANDLE_EXPR_AST(UnaryExprAST);
        HANDLE_EXPR_AST(CallExprAST);
        HANDLE_EXPR_AST(ForExprAST);
        HANDLE_EXPR_AST(IfExprAST);
        HANDLE_EXPR_AST(NumberExprAST);
        HANDLE_EXPR_AST(VariableExprAST);
      case ASTNode::ANK_ExprAST:
      case ASTNode::ANK_LastExprAST:
      case ASTNode::ANK_FunctionAST:
      case ASTNode::ANK_PrototypeAST:
      case ASTNode::ANK_ProtoUnaryAST:
      case ASTNode::ANK_ProtoBinaryAST:
      case ASTNode::ANK_LastPrototypeAST:
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

  VISIT_AST(FunctionAST)

  auto visit(const PrototypeAST &A) {
    if constexpr (DelegateAST) {
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
      case ASTNode::ANK_LastExprAST:
      case ASTNode::ANK_FunctionAST:
        break;
      }
      llvm_unreachable("Missing an AST type being handled");
    } else {
      return DELEGATE(A);
    }
  }

  VISIT_AST(ProtoBinaryAST)
  VISIT_AST(ProtoUnaryAST)

#undef HANDLE_EXPR_AST
#undef VISIT_AST
#undef DELEGATE
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_ASTVISITOR_H
