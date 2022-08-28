#ifndef KALEIDOSCOPE_AST_PROTOTYPEAST_H
#define KALEIDOSCOPE_AST_PROTOTYPEAST_H

#include "kaleidoscope/AST/AST.h"

#include <span>
#include <string>
#include <utility>
#include <vector>

namespace kaleidoscope {

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST : public ASTNode {
  const std::string Name;
  const std::vector<std::string> Args;

protected:
  PrototypeAST(ASTNodeKind K, std::string Name,
               std::vector<std::string> Args) noexcept
      : ASTNode(K), Name(std::move(Name)), Args(std::move(Args)) {}

public:
  static constexpr ASTNodeKind Kind = ANK_PrototypeAST;
  static constexpr std::string_view NodeName = "PrototypeAST";

  PrototypeAST(std::string Name, std::vector<std::string> Args) noexcept
      : PrototypeAST(Kind, std::move(Name), std::move(Args)) {}
  PrototypeAST(const PrototypeAST &) noexcept = default;
  PrototypeAST(PrototypeAST &&) noexcept = default;
  virtual ~PrototypeAST() = default;

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() >= Kind && A->getKind() <= ANK_LastPrototypeAST;
  }

  [[nodiscard]] const std::string &getName() const noexcept { return Name; }
  [[nodiscard]] std::span<const std::string> getArgs() const noexcept {
    return std::span(Args);
  }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_PROTOTYPEAST_H
