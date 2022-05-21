#ifndef KALEIDOSCOPE_AST_VARIABLEEXPRAST_H
#define KALEIDOSCOPE_AST_VARIABLEEXPRAST_H

#include "AST/ExprAST.h"

#include <string>

namespace kaleidoscope {

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  std::string Name;

public:
  VariableExprAST(std::string Name) : Name(std::move(Name)) {}
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_VARIABLEEXPRAST_H
