#ifndef KALEIDOSCOPE_AST_AST_H
#define KALEIDOSCOPE_AST_AST_H

#include <llvm/Support/Casting.h>

#include <fmt/compile.h>
#include <fmt/format.h>

#include <array>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace kaleidoscope {

#define LLVM_CLASS_OF(_id_name_)                                               \
  [[maybe_unused]] static constexpr auto classof(                              \
      const ASTNode *_id_name_) noexcept -> bool

/// ----------------------------------------------------------------------------
/// Abstract Syntax Tree
/// ----------------------------------------------------------------------------

class ASTNode {
public:
  enum ASTNodeKind {
    ANK_ExprAST,
    ANK_BinaryExprAST,
    ANK_UnaryExprAST,
    ANK_CallExprAST,
    ANK_ForExprAST,
    ANK_IfExprAST,
    ANK_NumberExprAST,
    ANK_VariableExprAST,
    ANK_VarAssignExprAST,
    ANK_LastExprAST,
    ANK_PrototypeAST,
    ANK_ProtoUnaryAST,
    ANK_ProtoBinaryAST,
    ANK_LastPrototypeAST,
    ANK_FunctionAST,
    ANK_EndOfFileAST,
  };

private:
  const ASTNodeKind MyKind;

protected:
  constexpr ASTNode(ASTNodeKind K) noexcept : MyKind(K) {}

public:
  ASTNode() = delete;
  constexpr virtual ~ASTNode() noexcept = default;

  [[nodiscard]] constexpr auto getKind() const noexcept -> ASTNodeKind {
    return MyKind;
  }
};

/// ----------------------------------------------------------------------------
/// Expression ASTs - Nodes that can occur inside the body of functions
/// ----------------------------------------------------------------------------

/// ExprAST - Base class for all expression nodes.
class ExprAST : public ASTNode {
public:
  static constexpr ASTNodeKind Kind = ANK_ExprAST;

protected:
  constexpr ExprAST(ASTNodeKind K) noexcept : ASTNode(K) {}

public:
  ExprAST() = delete;
  constexpr ~ExprAST() noexcept override = default;

  LLVM_CLASS_OF(A) {
    return A->getKind() >= ANK_ExprAST && A->getKind() <= ANK_LastExprAST;
  }
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  const char Op;
  const std::unique_ptr<ExprAST> LHS, RHS;

public:
  static constexpr ASTNodeKind Kind = ANK_BinaryExprAST;
  static constexpr std::string_view NodeName = "BinaryExprAST";

  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS) noexcept
      : ExprAST(Kind), Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getOp() const noexcept -> char { return Op; }
  [[nodiscard]] auto getLHS() const noexcept -> const ExprAST & { return *LHS; }
  [[nodiscard]] auto getRHS() const noexcept -> const ExprAST & { return *RHS; }
};

/// UnaryExprAST - Expression class for a unary operator.
class UnaryExprAST : public ExprAST {
  const char Opcode;
  const std::unique_ptr<ExprAST> Operand;

public:
  static constexpr ASTNodeKind Kind = ANK_UnaryExprAST;
  static constexpr std::string_view NodeName = "UnaryExprAST";

  UnaryExprAST(char Op, std::unique_ptr<ExprAST> Operand) noexcept
      : ExprAST(Kind), Opcode(Op), Operand(std::move(Operand)) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getOpcode() const noexcept -> char { return Opcode; }
  [[nodiscard]] auto getOperand() const noexcept -> const ExprAST & {
    return *Operand;
  }
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  const std::string Callee;
  const std::vector<std::unique_ptr<ExprAST>> Args;

public:
  static constexpr ASTNodeKind Kind = ANK_CallExprAST;
  static constexpr std::string_view NodeName = "CallExprAST";

  CallExprAST(std::string Callee,
              std::vector<std::unique_ptr<ExprAST>> Args) noexcept
      : ExprAST(Kind), Callee(std::move(Callee)), Args(std::move(Args)) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getCallee() const noexcept -> const std::string & {
    return Callee;
  }
  [[nodiscard]] auto getArgs() const noexcept
      -> const std::vector<std::unique_ptr<ExprAST>> & {
    return Args;
  }
};

/// ForExprAST - Expression class for for/in.
class ForExprAST : public ExprAST {
  const std::string VarName;
  const std::unique_ptr<ExprAST> Start, End, Step, Body;

public:
  static constexpr ASTNodeKind Kind = ANK_ForExprAST;
  static constexpr std::string_view NodeName = "ForExprAST";

  ForExprAST(std::string VarName, std::unique_ptr<ExprAST> Start,
             std::unique_ptr<ExprAST> End, std::unique_ptr<ExprAST> Step,
             std::unique_ptr<ExprAST> Body) noexcept
      : ExprAST(Kind), VarName(std::move(VarName)), Start(std::move(Start)),
        End(std::move(End)), Step(std::move(Step)), Body(std::move(Body)) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getVarName() const noexcept -> const std::string & {
    return VarName;
  }
  [[nodiscard]] auto getStart() const noexcept -> const ExprAST & {
    return *Start;
  }
  [[nodiscard]] auto getEnd() const noexcept -> const ExprAST & { return *End; }
  [[nodiscard]] auto getStep() const noexcept -> const ExprAST & {
    return *Step;
  }
  [[nodiscard]] auto getBody() const noexcept -> const ExprAST & {
    return *Body;
  }
};

/// IfExprAST - Expression class for if/then/else.
class IfExprAST : public ExprAST {
  const std::unique_ptr<ExprAST> Cond, Then, Else;

public:
  static constexpr ASTNodeKind Kind = ANK_IfExprAST;
  static constexpr std::string_view NodeName = "IfExprAST";

  IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
            std::unique_ptr<ExprAST> Else) noexcept
      : ExprAST(Kind), Cond(std::move(Cond)), Then(std::move(Then)),
        Else(std::move(Else)) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getCond() const noexcept -> const ExprAST & {
    return *Cond;
  }
  [[nodiscard]] auto getThen() const noexcept -> const ExprAST & {
    return *Then;
  }
  [[nodiscard]] auto getElse() const noexcept -> const ExprAST & {
    return *Else;
  }
};

/// NumberExprAST - AST node containing a literal number
class NumberExprAST : public ExprAST {
  const double Val;

public:
  static constexpr ASTNodeKind Kind = ANK_NumberExprAST;
  static constexpr std::string_view NodeName = "NumberExprAST";

  constexpr NumberExprAST(double Val) noexcept : ExprAST(Kind), Val(Val) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getVal() const noexcept -> double { return Val; }
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  const std::string Name;

public:
  static constexpr ASTNodeKind Kind = ANK_VariableExprAST;
  static constexpr std::string_view NodeName = "VariableExprAST";

  VariableExprAST(std::string Name) noexcept
      : ExprAST(Kind), Name(std::move(Name)) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getName() const noexcept -> const std::string & {
    return Name;
  }
};

/// VarAssignExprAST - Expression class for referencing a variable, like "a".
class VarAssignExprAST : public ExprAST {
public:
  using VarAssignPair = std::pair<std::string, std::unique_ptr<ExprAST>>;

private:
  std::vector<VarAssignPair> VarAs;
  std::unique_ptr<ExprAST> Body;

public:
  static constexpr ASTNodeKind Kind = ANK_VarAssignExprAST;
  static constexpr std::string_view NodeName = "VarAssignExprAST";

  VarAssignExprAST(std::vector<VarAssignPair> VarAs,
                   std::unique_ptr<ExprAST> Body) noexcept
      : ExprAST(Kind), VarAs(std::move(VarAs)), Body(std::move(Body)) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getVarAs() const noexcept
      -> const std::vector<VarAssignPair> & {
    return VarAs;
  }
  [[nodiscard]] auto getBody() const noexcept -> const ExprAST & {
    return *Body;
  }
};

/// ----------------------------------------------------------------------------
/// Prototype ASTs - Declarations of function and operator signatures
/// ----------------------------------------------------------------------------

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
  ~PrototypeAST() override = default;

  LLVM_CLASS_OF(A) {
    return A->getKind() >= Kind && A->getKind() <= ANK_LastPrototypeAST;
  }

  [[nodiscard]] auto getName() const noexcept -> const std::string & {
    return Name;
  }
  [[nodiscard]] auto getArgs() const noexcept
      -> const std::vector<std::string> & {
    return Args;
  }
};

/// ProtoBinaryAST - This class represents the "prototype" for a binary
/// operator, which captures its symbol, and its argument names.
class ProtoBinaryAST : public PrototypeAST {
  const int Precedence;

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

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getOperator() const noexcept -> char {
    return getName().back();
  }
  [[nodiscard]] auto getPrecedence() const noexcept -> int {
    return Precedence;
  }
};

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

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getOperator() const noexcept -> char {
    return getName().back();
  }
};

/// ----------------------------------------------------------------------------
/// Function AST - Definitions of functions and operators
/// ----------------------------------------------------------------------------

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public ASTNode {
  const std::unique_ptr<PrototypeAST> Proto;
  const std::unique_ptr<ExprAST> Body;

public:
  static constexpr ASTNodeKind Kind = ANK_FunctionAST;
  static constexpr std::string_view NodeName = "FunctionAST";

  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body) noexcept
      : ASTNode(Kind), Proto(std::move(Proto)), Body(std::move(Body)) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }

  [[nodiscard]] auto getProto() const noexcept -> const PrototypeAST & {
    return *Proto;
  }
  [[nodiscard]] auto getBody() const noexcept -> const ExprAST & {
    return *Body;
  }
};

/// ExprAST - Base class for all expression nodes.
class EndOfFileAST : public ASTNode {
public:
  static constexpr ASTNodeKind Kind = ANK_EndOfFileAST;
  static constexpr std::string_view NodeName = "EndOfFileAST";

  constexpr EndOfFileAST() noexcept : ASTNode(Kind) {}

  LLVM_CLASS_OF(A) { return A->getKind() == Kind; }
};

#undef LLVM_CLASS_OF

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_AST_H
