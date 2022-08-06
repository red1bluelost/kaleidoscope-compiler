#ifndef KALEIDOSCOPE_AST_DUMP_TREEDUMP_H
#define KALEIDOSCOPE_AST_DUMP_TREEDUMP_H

#include "kaleidoscope/AST/ASTVisitor.h"

#include <fmt/core.h>

#include <ostream>

namespace kaleidoscope::ast {

/// Technically this isn't true XML and just XML inspired
class XMLDump : private ASTVisitor<XMLDump> {
  using Self = XMLDump;
  using Parent = ASTVisitor<Self>;
  friend Parent;

  std::ostream &Out;
  const std::size_t Spaces;

  auto visitImpl(BinaryExprAST &A) -> XMLDump &;
  auto visitImpl(CallExprAST &A) -> XMLDump &;
  auto visitImpl(ForExprAST &A) -> XMLDump &;
  auto visitImpl(IfExprAST &A) -> XMLDump &;
  auto visitImpl(NumberExprAST &A) -> XMLDump &;
  auto visitImpl(VariableExprAST &A) -> XMLDump &;

  auto visitImpl(FunctionAST &A) -> XMLDump &;
  auto visitImpl(PrototypeAST &A) -> XMLDump &;

  [[nodiscard]] auto padding(std::size_t MS = 0) -> std::string {
    return std::string(Spaces + MS, ' ');
  }

  auto open(std::string_view S, std::size_t MS = 0) -> XMLDump &;
  auto close(std::string_view S, std::size_t MS = 0) -> XMLDump &;

  template <typename T>
  auto printSubItem(std::string_view Tag, const T &Content, std::size_t MS = 0)
      -> std::enable_if_t<fmt::is_formattable<T>::value, XMLDump &>;

  auto printSubAST(std::string_view Tag, ASTNode &A, std::size_t MS = 0)
      -> XMLDump &;

  auto child(std::size_t MS) -> XMLDump { return Self(Out, Spaces + MS); }

public:
  XMLDump(std::ostream &Out, std::size_t Spaces = 0) noexcept
      : Out(Out), Spaces(Spaces) {}

  using Parent::visit;
};

} // namespace kaleidoscope::ast

#endif // KALEIDOSCOPE_AST_DUMP_TREEDUMP_H
