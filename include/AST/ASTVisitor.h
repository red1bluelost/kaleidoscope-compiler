#ifndef KALEIDOSCOPE_ASTVISITOR_H
#define KALEIDOSCOPE_ASTVISITOR_H

#include "AST/BinaryExprAST.h"
#include "AST/CallExprAST.h"
#include "AST/ExprAST.h"
#include "AST/ForExprAST.h"
#include "AST/FunctionAST.h"
#include "AST/IfExprAST.h"
#include "AST/NumberExprAST.h"
#include "AST/PrototypeAST.h"
#include "AST/VariableExprAST.h"

#include <llvm/Support/ErrorHandling.h>

#include <type_traits>

namespace kaleidoscope {

template <typename SubClass, bool DelegateExprAST = true> class ASTVisitor {
public:
  auto visit(ASTNode &A) {
    ASTNode *AP = &A;
    if (auto *E = llvm::dyn_cast<ExprAST>(AP))
      return visit(*E);
    if (auto *E = llvm::dyn_cast<FunctionAST>(AP))
      return visit(*E);
    if (auto *E = llvm::dyn_cast<PrototypeAST>(AP))
      return visit(*E);
    llvm_unreachable("invalid ASTNode class type");
  }

#define DELEGATE(AST) static_cast<SubClass *>(this)->visitImpl(AST)
  
  auto visit(ExprAST &A) {
    if constexpr (DelegateExprAST) {
#define HANDLE_EXPR_AST(AST_TYPE)                                              \
  case AST_TYPE::Kind:                                                         \
    return DELEGATE(llvm::cast<AST_TYPE>(A))

      switch (A.getKind()) {
        HANDLE_EXPR_AST(BinaryExprAST);
        HANDLE_EXPR_AST(CallExprAST);
        HANDLE_EXPR_AST(ForExprAST);
        HANDLE_EXPR_AST(IfExprAST);
        HANDLE_EXPR_AST(NumberExprAST);
        HANDLE_EXPR_AST(VariableExprAST);
      case ASTNode::ANK_ExprAST:
      case ASTNode::ANK_LastExprAST:
      case ASTNode::ANK_FunctionAST:
      case ASTNode::ANK_PrototypeAST:
        break;
      }
      llvm_unreachable("Missing an AST type being handled");
#undef HANDLE_EXPR_AST
    } else {
      return DELEGATE(A);
    }
  }

#define VISIT_AST(AST_TYPE)                                                    \
  auto visit(AST_TYPE &A) { return DELEGATE(A); }

  VISIT_AST(BinaryExprAST)
  VISIT_AST(CallExprAST)
  VISIT_AST(ForExprAST)
  VISIT_AST(IfExprAST)
  VISIT_AST(NumberExprAST)
  VISIT_AST(VariableExprAST)

  VISIT_AST(FunctionAST)
  VISIT_AST(PrototypeAST)

#undef VISIT_AST
#undef DELEGATE
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_ASTVISITOR_H
