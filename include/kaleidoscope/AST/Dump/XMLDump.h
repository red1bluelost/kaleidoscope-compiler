#ifndef KALEIDOSCOPE_AST_DUMP_XMLDUMP_H
#define KALEIDOSCOPE_AST_DUMP_XMLDUMP_H

#include "kaleidoscope/AST/ASTVisitor.h"

#include <fmt/core.h>

#include <ostream>

namespace kaleidoscope::ast {

/// Technically this isn't true XML and just XML inspired
class XMLDump : private ASTVisitor<XMLDump, AVDelegation::All> {
  using Self = XMLDump;
  using Parent = ASTVisitor<XMLDump, AVDelegation::All>;
  friend Parent;

  std::ostream &Out;
  const std::size_t Spaces;

  auto visitImpl(const BinaryExprAST &A) -> XMLDump &;
  auto visitImpl(const UnaryExprAST &A) -> XMLDump & {
    assert(false);
    return *this;
  }
  auto visitImpl(const CallExprAST &A) -> XMLDump &;
  auto visitImpl(const ForExprAST &A) -> XMLDump &;
  auto visitImpl(const IfExprAST &A) -> XMLDump &;
  auto visitImpl(const NumberExprAST &A) -> XMLDump &;
  auto visitImpl(const VariableExprAST &A) -> XMLDump &;

  auto visitImpl(const FunctionAST &A) -> XMLDump &;

  auto visitImpl(const PrototypeAST &A) -> XMLDump &;
  auto visitImpl(const ProtoBinaryAST &A) -> XMLDump & {
    assert(false);
    return *this;
  }
  auto visitImpl(const ProtoUnaryAST &A) -> XMLDump & {
    assert(false);
    return *this;
  }

  auto visitImpl(const EndOfFileAST &A) -> XMLDump & {
    assert(false);
    return *this;
  }

  [[nodiscard]] auto padding(std::size_t MS = 0) -> std::string {
    return std::string(Spaces + MS, ' ');
  }

  auto open(std::string_view S, std::size_t MS = 0) -> XMLDump &;
  auto close(std::string_view S, std::size_t MS = 0) -> XMLDump &;

  template <typename T>
  auto printSubItem(std::string_view Tag, const T &Content, std::size_t MS = 0)
      -> std::enable_if_t<fmt::is_formattable<T>::value, XMLDump &>;

  auto printSubAST(std::string_view Tag, const ASTNode &A, std::size_t MS = 0)
      -> XMLDump &;

  auto child(std::size_t MS) -> XMLDump { return Self(Out, Spaces + MS); }

public:
  XMLDump(std::ostream &Out, std::size_t Spaces = 0) noexcept
      : Out(Out), Spaces(Spaces) {}

  using Parent::visit;
};

} // namespace kaleidoscope::ast

#endif // KALEIDOSCOPE_AST_DUMP_XMLDUMP_H
