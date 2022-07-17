#ifndef KALEIDOSCOPE_AST_VARIABLEEXPRAST_H
#define KALEIDOSCOPE_AST_VARIABLEEXPRAST_H

#include "AST/ExprAST.h"

#include <string>

namespace kaleidoscope {

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  std::string Name;

public:
  static constexpr ASTNodeKind Kind = ANK_VariableExprAST;

  VariableExprAST(std::string Name) : ExprAST(Kind), Name(std::move(Name)) {}

  static bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] const std::string &getName() const noexcept { return Name; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_VARIABLEEXPRAST_H
