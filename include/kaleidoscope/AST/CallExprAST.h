#ifndef KALEIDOSCOPE_AST_CALLEXPRAST_H
#define KALEIDOSCOPE_AST_CALLEXPRAST_H

#include "kaleidoscope/AST/ExprAST.h"

#include <memory>
#include <span>
#include <string>
#include <vector>

namespace kaleidoscope {

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  static constexpr ASTNodeKind Kind = ANK_CallExprAST;
  static constexpr std::string_view NodeName = "CallExprAST";

  CallExprAST(std::string Callee, std::vector<std::unique_ptr<ExprAST>> Args)
      : ExprAST(Kind), Callee(std::move(Callee)), Args(std::move(Args)) {}

  [[maybe_unused]] static bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] const std::string &getCallee() const noexcept { return Callee; }
  [[nodiscard]] std::span<const std::unique_ptr<ExprAST>>
  getArgs() const noexcept {
    return std::span(Args);
  }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_CALLEXPRAST_H
