#ifndef KALEIDOSCOPE_AST_FOREXPRAST_H
#define KALEIDOSCOPE_AST_FOREXPRAST_H

#include "kaleidoscope/AST/ExprAST.h"

#include <memory>
#include <string>

namespace kaleidoscope {

/// ForExprAST - Expression class for for/in.
class ForExprAST : public ExprAST {
  const std::string VarName;
  const std::unique_ptr<ExprAST> Start, End, Step, Body;

public:
  static constexpr ASTNodeKind Kind = ANK_ForExprAST;
  static constexpr std::string_view NodeName = "ForExprAST";

  ForExprAST(std::string VarName, std::unique_ptr<ExprAST> Start,
             std::unique_ptr<ExprAST> End, std::unique_ptr<ExprAST> Step,
             std::unique_ptr<ExprAST> Body) noexcept
      : ExprAST(Kind), VarName(std::move(VarName)), Start(std::move(Start)),
        End(std::move(End)), Step(std::move(Step)), Body(std::move(Body)) {}

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] const std::string &getVarName() const noexcept {
    return VarName;
  }
  [[nodiscard]] const ExprAST &getStart() const noexcept { return *Start; }
  [[nodiscard]] const ExprAST &getEnd() const noexcept { return *End; }
  [[nodiscard]] const ExprAST &getStep() const noexcept { return *Step; }
  [[nodiscard]] const ExprAST &getBody() const noexcept { return *Body; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_FOREXPRAST_H
