#ifndef KALEIDOSCOPE_AST_PROTOTYPEAST_H
#define KALEIDOSCOPE_AST_PROTOTYPEAST_H

#include "kaleidoscope/AST/AST.h"

#include <string>
#include <vector>

namespace kaleidoscope {

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST : public ASTNode {
  std::string Name;
  std::vector<std::string> Args;

public:
  static constexpr ASTNodeKind Kind = ANK_PrototypeAST;
  static constexpr std::string_view NodeName = "PrototypeAST";

  PrototypeAST(std::string Name, std::vector<std::string> Args)
      : ASTNode(Kind), Name(std::move(Name)), Args(std::move(Args)) {}
  PrototypeAST(const PrototypeAST &) = default;
  PrototypeAST(PrototypeAST &&) = default;

  static bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] const std::string &getName() const noexcept { return Name; }
  [[nodiscard]] const std::vector<std::string> &getArgs() const noexcept {
    return Args;
  }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_PROTOTYPEAST_H
