#ifndef KALEIDOSCOPE_AST_FUNCTIONAST_H
#define KALEIDOSCOPE_AST_FUNCTIONAST_H

#include "AST/AST.h"
#include "AST/ExprAST.h"
#include "AST/PrototypeAST.h"

#include <memory>

namespace kaleidoscope {

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public ASTNode {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  static constexpr ASTNodeKind Kind = ANK_FunctionAST;

  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
      : ASTNode(Kind), Proto(std::move(Proto)), Body(std::move(Body)) {}

  static bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] PrototypeAST &getProto() const noexcept { return *Proto; }
  [[nodiscard]] ExprAST &getBody() const noexcept { return *Body; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_FUNCTIONAST_H
