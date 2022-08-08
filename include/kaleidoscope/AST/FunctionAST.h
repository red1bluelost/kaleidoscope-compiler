#ifndef KALEIDOSCOPE_AST_FUNCTIONAST_H
#define KALEIDOSCOPE_AST_FUNCTIONAST_H

#include "kaleidoscope/AST/AST.h"
#include "kaleidoscope/AST/ExprAST.h"
#include "kaleidoscope/AST/PrototypeAST.h"

#include <memory>

namespace kaleidoscope {

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public ASTNode {
  const std::unique_ptr<PrototypeAST> Proto;
  const std::unique_ptr<ExprAST> Body;

public:
  static constexpr ASTNodeKind Kind = ANK_FunctionAST;
  static constexpr std::string_view NodeName = "FunctionAST";

  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body) noexcept
      : ASTNode(Kind), Proto(std::move(Proto)), Body(std::move(Body)) {}

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] const PrototypeAST &getProto() const noexcept { return *Proto; }
  [[nodiscard]] const ExprAST &getBody() const noexcept { return *Body; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_FUNCTIONAST_H
