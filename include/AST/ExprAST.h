#ifndef KALEIDOSCOPE_AST_EXPRAST_H
#define KALEIDOSCOPE_AST_EXPRAST_H

#include <llvm/Support/Casting.h>

namespace llvm {
class Value;
} // namespace llvm

namespace kaleidoscope {

class CodeGen;

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
  enum ExprASTKind {
    EAK_BinaryExprAST,
    EAK_CallExprAST,
    EAK_ForExprAST,
    EAK_IfExprAST,
    EAK_NumberExprAST,
    EAK_VariableExprAST,
  };

private:
  const ExprASTKind Kind;

public:
  ExprASTKind getKind() const { return Kind; }

  ExprAST(ExprASTKind K) : Kind(K) {}

  virtual ~ExprAST() = default;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_EXPRAST_H
