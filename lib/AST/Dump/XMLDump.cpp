#include "kaleidoscope/AST/Dump/XMLDump.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <string>

using namespace kaleidoscope;
using namespace kaleidoscope::ast;

auto XMLDump::child(std::size_t MS) -> Self { return Self(Out, Spaces + MS); }

auto XMLDump::padding(std::size_t MS) -> std::string {
  return std::string(Spaces + MS, ' ');
}

auto XMLDump::open(std::string_view S, const std::size_t MS) -> Self& {
  fmt::print(Out, "{}<{}>\n", padding(MS), S);
  return *this;
}

auto XMLDump::close(std::string_view S, const std::size_t MS) -> Self& {
  fmt::print(Out, "{}</{}>\n", padding(MS), S);
  return *this;
}

template<typename T>
auto XMLDump::printSubItem(
    const std::string_view Tag, const T& Content, std::size_t MS
) -> std::enable_if_t<fmt::is_formattable<T>::value, Self&> {
  /// NOTE: if Content is '<', '>', '&', '"', or ''' then it is invalid XML
  fmt::print(Out, "{}<{}>{}</{}>\n", padding(2 + MS), Tag, Content, Tag);
  return *this;
}

auto XMLDump::printSubAST(
    std::string_view Tag, const ASTNode& A, std::size_t MS
) -> Self& {
  open(Tag, 2 + MS);
  child(4 + MS).visit(A);
  close(Tag, 2 + MS);
  return *this;
}

auto XMLDump::visitImpl(const BinaryExprAST& A) -> Self& {
  return open(A.NodeName)
      .printSubItem("Op", A.getOp())
      .printSubAST("LHS", A.getLHS())
      .printSubAST("RHS", A.getRHS())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const UnaryExprAST& A) -> Self& {
  return open(A.NodeName)
      .printSubItem("Opcode", A.getOpcode())
      .printSubAST("Operand", A.getOperand())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const CallExprAST& A) -> Self& {
  open(A.NodeName).printSubItem("Callee", A.getCallee()).open("Args", 2);
  for (int Idx = 0; const auto& Arg : A.getArgs())
    printSubAST(fmt::format("Arg[{}]", Idx++), *Arg, 2);
  return close("Args", 2).close(A.NodeName);
}

auto XMLDump::visitImpl(const ForExprAST& A) -> Self& {
  return open(A.NodeName)
      .printSubItem("VarName", A.getVarName())
      .printSubAST("Start", A.getStart())
      .printSubAST("End", A.getEnd())
      .printSubAST("Step", A.getStep())
      .printSubAST("Body", A.getBody())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const IfExprAST& A) -> Self& {
  return open(A.NodeName)
      .printSubAST("Cond", A.getCond())
      .printSubAST("Then", A.getThen())
      .printSubAST("Else", A.getElse())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const NumberExprAST& A) -> Self& {
  return open(A.NodeName).printSubItem("Val", A.getVal()).close(A.NodeName);
}

auto XMLDump::visitImpl(const VariableExprAST& A) -> Self& {
  return open(A.NodeName).printSubItem("Name", A.getName()).close(A.NodeName);
}

auto XMLDump::visitImpl(const VarAssignExprAST& A) -> XMLDump::Self& {
  open(A.NodeName).open("Vars", 2);
  for (const auto& [Name, Init] : A.getVarAs()) printSubAST(Name, *Init, 2);
  return close("Vars", 2).printSubAST("Body", A.getBody()).close(A.NodeName);
}

auto XMLDump::visitImpl(const FunctionAST& A) -> Self& {
  return open(A.NodeName)
      .printSubAST("Proto", A.getProto())
      .printSubAST("Body", A.getBody())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const PrototypeAST& A) -> Self& {
  open(A.NodeName).printSubItem("Name", A.getName()).open("Args", 2);
  for (int Idx = 0; const auto& Arg : A.getArgs())
    printSubItem(fmt::format("Arg[{}]", Idx++), Arg, 2);
  return close("Args", 2).close(A.NodeName);
}

auto XMLDump::visitImpl(const ProtoBinaryAST& A) -> Self& {
  open(A.NodeName).child(2).visitImpl(llvm::cast<PrototypeAST>(A));
  return printSubItem("Operator", A.getOperator())
      .printSubItem("Precedence", A.getPrecedence())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const ProtoUnaryAST& A) -> Self& {
  open(A.NodeName).child(2).visitImpl(llvm::cast<PrototypeAST>(A));
  return printSubItem("Operator", A.getOperator()).close(A.NodeName);
}

auto XMLDump::visitImpl(const EndOfFileAST& A) -> Self& {
  return open(A.NodeName).close(A.NodeName);
}
