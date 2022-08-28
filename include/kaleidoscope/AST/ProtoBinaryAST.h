#ifndef KALEIDOSCOPE_AST_PROTOBINARYAST_H
#define KALEIDOSCOPE_AST_PROTOBINARYAST_H

#include "kaleidoscope/AST/PrototypeAST.h"

#include <fmt/compile.h>
#include <fmt/format.h>

#include <array>
#include <iterator>
#include <string>

namespace kaleidoscope {

/// ProtoBinaryAST - This class represents the "prototype" for a binary
/// operator, which captures its symbol, and its argument names.
class ProtoBinaryAST : public PrototypeAST {
  int Precedence;

public:
  static constexpr ASTNodeKind Kind = ANK_ProtoBinaryAST;
  static constexpr std::string_view NodeName = "ProtoBinaryAST";

  ProtoBinaryAST(char Op, std::array<std::string, 2> Args,
                 int Precedence) noexcept
      : PrototypeAST(Kind, fmt::format(FMT_COMPILE("binary{}"), Op),
                     {std::make_move_iterator(Args.begin()),
                      std::make_move_iterator(Args.end())}),
        Precedence(Precedence) {}
  ProtoBinaryAST(const ProtoBinaryAST &) noexcept = default;
  ProtoBinaryAST(ProtoBinaryAST &&) noexcept = default;

  [[maybe_unused]] static constexpr bool classof(const ASTNode *A) noexcept {
    return A->getKind() == Kind;
  }

  [[nodiscard]] char getOperator() const noexcept { return getName().back(); }
  [[nodiscard]] int getPrecedence() const noexcept { return Precedence; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_PROTOBINARYAST_H
