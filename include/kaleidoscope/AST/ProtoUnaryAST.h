#ifndef KALEIDOSCOPE_AST_PROTOUNARYAST_H
#define KALEIDOSCOPE_AST_PROTOUNARYAST_H

#include "kaleidoscope/AST/PrototypeAST.h"

#include <fmt/compile.h>
#include <fmt/format.h>

#include <array>
#include <iterator>
#include <string>

namespace kaleidoscope {

/// ProtoUnaryAST - This class represents the "prototype" for a unary operator,
/// which captures its symbol, and its argument name.
class ProtoUnaryAST : public PrototypeAST {
public:
  static constexpr ASTNodeKind Kind = ANK_ProtoUnaryAST;
  static constexpr std::string_view NodeName = "ProtoUnaryAST";

  ProtoUnaryAST(char Op, std::array<std::string, 1> Args) noexcept
      : PrototypeAST(Kind, fmt::format(FMT_COMPILE("unary{}"), Op),
                     {std::make_move_iterator(Args.begin()),
                      std::make_move_iterator(Args.end())}) {}
  ProtoUnaryAST(const ProtoUnaryAST &) noexcept = default;
  ProtoUnaryAST(ProtoUnaryAST &&) noexcept = default;

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] char getOperator() const noexcept { return getName().back(); }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_PROTOUNARYAST_H
