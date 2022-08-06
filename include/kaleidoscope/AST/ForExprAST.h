#ifndef KALEIDOSCOPE_AST_FOREXPRAST_H
#define KALEIDOSCOPE_AST_FOREXPRAST_H

#include "kaleidoscope/AST/ExprAST.h"

#include <memory>
#include <string>

namespace kaleidoscope {

/// ForExprAST - Expression class for for/in.
class ForExprAST : public ExprAST {
  std::string VarName;
  std::unique_ptr<ExprAST> Start, End, Step, Body;

public:
  static constexpr ASTNodeKind Kind = ANK_ForExprAST;
  static constexpr std::string_view NodeName = "ForExprAST";

  ForExprAST(std::string VarName, std::unique_ptr<ExprAST> Start,
             std::unique_ptr<ExprAST> End, std::unique_ptr<ExprAST> Step,
             std::unique_ptr<ExprAST> Body)
      : ExprAST(Kind), VarName(std::move(VarName)), Start(std::move(Start)),
        End(std::move(End)), Step(std::move(Step)), Body(std::move(Body)) {}

  static bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] const std::string &getVarName() const noexcept {
    return VarName;
  }
  [[nodiscard]] ExprAST &getStart() const noexcept { return *Start; }
  [[nodiscard]] ExprAST &getEnd() const noexcept { return *End; }
  [[nodiscard]] ExprAST &getStep() const noexcept { return *Step; }
  [[nodiscard]] ExprAST &getBody() const noexcept { return *Body; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_FOREXPRAST_H
