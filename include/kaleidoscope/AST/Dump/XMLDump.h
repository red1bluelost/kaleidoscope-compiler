#ifndef KALEIDOSCOPE_AST_DUMP_XMLDUMP_H
#define KALEIDOSCOPE_AST_DUMP_XMLDUMP_H

#include "kaleidoscope/AST/ASTVisitor.h"

#include <ostream>

namespace kaleidoscope::ast {

/// Technically this isn't true XML and just XML inspired
class XMLDump : private ASTVisitor<XMLDump, AVDelType::All> {
  using Self = XMLDump;
  using Parent = ASTVisitor<Self, AVDelType::All>;
  friend Parent;

  std::ostream &Out;
  const std::size_t Spaces;

public:
  XMLDump(std::ostream &Out, std::size_t Spaces = 0) noexcept
      : Out(Out), Spaces(Spaces) {}

  using Parent::visit;

private:
  auto child(std::size_t MS) -> Self;

  [[nodiscard]] auto padding(std::size_t MS = 0) -> std::string;

  auto open(std::string_view S, std::size_t MS = 0) -> Self &;
  auto close(std::string_view S, std::size_t MS = 0) -> Self &;

  template <typename T>
  auto printSubItem(std::string_view Tag, const T &Content, std::size_t MS = 0)
      -> std::enable_if_t<fmt::is_formattable<T>::value, Self &>;

  auto printSubAST(std::string_view Tag, const ASTNode &A, std::size_t MS = 0)
      -> Self &;

  auto visitImpl(const BinaryExprAST &A) -> Self &;
  auto visitImpl(const UnaryExprAST &A) -> Self &;
  auto visitImpl(const CallExprAST &A) -> Self &;
  auto visitImpl(const ForExprAST &A) -> Self &;
  auto visitImpl(const IfExprAST &A) -> Self &;
  auto visitImpl(const NumberExprAST &A) -> Self &;
  auto visitImpl(const VariableExprAST &A) -> Self &;

  auto visitImpl(const FunctionAST &A) -> Self &;

  auto visitImpl(const PrototypeAST &A) -> Self &;
  auto visitImpl(const ProtoBinaryAST &A) -> Self &;
  auto visitImpl(const ProtoUnaryAST &A) -> Self &;

  auto visitImpl(const EndOfFileAST &A) -> Self &;
};

} // namespace kaleidoscope::ast

#endif // KALEIDOSCOPE_AST_DUMP_XMLDUMP_H
