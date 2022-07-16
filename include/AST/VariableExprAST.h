#ifndef KALEIDOSCOPE_AST_VARIABLEEXPRAST_H
#define KALEIDOSCOPE_AST_VARIABLEEXPRAST_H

#include "AST/ExprAST.h"

#include <string>

namespace kaleidoscope {

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  std::string Name;

public:
  static constexpr ExprASTKind Kind = EAK_VariableExprAST;

  VariableExprAST(std::string Name)
      : ExprAST(Kind), Name(std::move(Name)) {}

  static bool classof(const ExprAST *E) noexcept {
    return E->getKind() == Kind;
  }

  llvm::Value *codegen(CodeGen &CG) override;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_VARIABLEEXPRAST_H
