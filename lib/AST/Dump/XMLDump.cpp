#include "kaleidoscope/AST/Dump/XMLDump.h"

#include <fmt/ostream.h>

#include <string>

using namespace kaleidoscope;
using namespace kaleidoscope::ast;

auto XMLDump::open(std::string_view S, const std::size_t MS) -> XMLDump & {
  fmt::print(Out, "{}<{}>\n", padding(MS), S);
  return *this;
}

auto XMLDump::close(std::string_view S, const std::size_t MS) -> XMLDump & {
  fmt::print(Out, "{}</{}>\n", padding(MS), S);
  return *this;
}

template <typename T>
auto XMLDump::printSubItem(const std::string_view Tag, const T &Content,
                           std::size_t MS)
    -> std::enable_if_t<fmt::is_formattable<T>::value, XMLDump &> {
  /// NOTE: if Content is '<', '>', '&', '"', or ''' then it is invalid XML
  fmt::print(Out, "{}<{}>{}</{}>\n", padding(2 + MS), Tag, Content, Tag);
  return *this;
}

XMLDump &XMLDump::printSubAST(std::string_view Tag, const ASTNode &A,
                              std::size_t MS) {
  open(Tag, 2 + MS);
  child(4 + MS).visit(A);
  close(Tag, 2 + MS);
  return *this;
}

auto XMLDump::visitImpl(const BinaryExprAST &A) -> XMLDump & {
  return open(A.NodeName)
      .printSubItem("Op", A.getOp())
      .printSubAST("LHS", A.getLHS())
      .printSubAST("RHS", A.getRHS())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const UnaryExprAST &A) -> XMLDump & {
  return open(A.NodeName)
      .printSubItem("Opcode", A.getOpcode())
      .printSubAST("Operand", A.getOperand())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const CallExprAST &A) -> XMLDump & {
  open(A.NodeName).printSubItem("Callee", A.getCallee()).open("Args", 2);
  for (int Idx = 0; const auto &Arg : A.getArgs())
    printSubAST(fmt::format("Arg[{}]", Idx++), *Arg, 2);
  return close("Args", 2).close(A.NodeName);
}

auto XMLDump::visitImpl(const ForExprAST &A) -> XMLDump & {
  return open(A.NodeName)
      .printSubItem("VarName", A.getVarName())
      .printSubAST("Start", A.getStart())
      .printSubAST("End", A.getEnd())
      .printSubAST("Step", A.getStep())
      .printSubAST("Body", A.getBody())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const IfExprAST &A) -> XMLDump & {
  return open(A.NodeName)
      .printSubAST("Cond", A.getCond())
      .printSubAST("Then", A.getThen())
      .printSubAST("Else", A.getElse())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const NumberExprAST &A) -> XMLDump & {
  return open(A.NodeName).printSubItem("Val", A.getVal()).close(A.NodeName);
}

auto XMLDump::visitImpl(const VariableExprAST &A) -> XMLDump & {
  return open(A.NodeName).printSubItem("Name", A.getName()).close(A.NodeName);
}

auto XMLDump::visitImpl(const FunctionAST &A) -> XMLDump & {
  return open(A.NodeName)
      .printSubAST("Proto", A.getProto())
      .printSubAST("Body", A.getBody())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const PrototypeAST &A) -> XMLDump & {
  open(A.NodeName).printSubItem("Name", A.getName()).open("Args", 2);
  for (int Idx = 0; const auto &Arg : A.getArgs())
    printSubItem(fmt::format("Arg[{}]", Idx++), Arg, 2);
  return close("Args", 2).close(A.NodeName);
}

auto XMLDump::visitImpl(const ProtoBinaryAST &A) -> XMLDump & {
  open(A.NodeName).child(2).visitImpl(llvm::cast<PrototypeAST>(A));
  return printSubItem("Operator", A.getOperator())
      .printSubItem("Precedence", A.getPrecedence())
      .close(A.NodeName);
}

auto XMLDump::visitImpl(const ProtoUnaryAST &A) -> XMLDump & {
  open(A.NodeName).child(2).visitImpl(llvm::cast<PrototypeAST>(A));
  return printSubItem("Operator", A.getOperator()).close(A.NodeName);
}

auto XMLDump::visitImpl(const EndOfFileAST &A) -> XMLDump & {
  return open(A.NodeName).close(A.NodeName);
}