#ifndef KALEIDOSCOPE_AST_CALLEXPRAST_H
#define KALEIDOSCOPE_AST_CALLEXPRAST_H

#include "AST/ExprAST.h"

#include <memory>
#include <string>
#include <vector>

namespace kaleidoscope {

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(std::string Callee, std::vector<std::unique_ptr<ExprAST>> Args)
      : Callee(std::move(Callee)), Args(std::move(Args)) {}
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_CALLEXPRAST_H
