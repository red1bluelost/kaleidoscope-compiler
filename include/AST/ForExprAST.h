#ifndef KALEIDOSCOPE_AST_FOREXPRAST_H
#define KALEIDOSCOPE_AST_FOREXPRAST_H

#include "AST/ExprAST.h"

#include <memory>
#include <string>

namespace kaleidoscope {

/// ForExprAST - Expression class for for/in.
class ForExprAST : public ExprAST {
  std::string VarName;
  std::unique_ptr<ExprAST> Start, End, Step, Body;

public:
  static constexpr ExprASTKind Kind = EAK_ForExprAST;

  ForExprAST(std::string VarName, std::unique_ptr<ExprAST> Start,
             std::unique_ptr<ExprAST> End, std::unique_ptr<ExprAST> Step,
             std::unique_ptr<ExprAST> Body)
      : ExprAST(Kind), VarName(std::move(VarName)), Start(std::move(Start)),
        End(std::move(End)), Step(std::move(Step)), Body(std::move(Body)) {}

  static bool classof(const ExprAST *E) noexcept {
    return E->getKind() == Kind;
  }

  llvm::Value *codegen(CodeGen &CG) override;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_FOREXPRAST_H
