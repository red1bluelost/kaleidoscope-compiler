#ifndef KALEIDOSCOPE_CODEGEN_CODEGEN_H
#define KALEIDOSCOPE_CODEGEN_CODEGEN_H

#include "kaleidoscope/AST/AST.h"
#include "kaleidoscope/AST/ASTVisitor.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace kaleidoscope {

class CodeGen : public ASTVisitor<CodeGen> {
  using Parent = ASTVisitor<CodeGen>;
  friend Parent;

public:
  struct Session {
    std::unique_ptr<llvm::LLVMContext> Context =
        std::make_unique<llvm::LLVMContext>();
    std::unique_ptr<llvm::Module> Module =
        std::make_unique<llvm::Module>("default codegen", *Context);
    llvm::IRBuilder<> Builder{*Context};
  };

private:
  std::unordered_map<std::string, llvm::Value *> NamedValues{};
  std::unique_ptr<Session> CGS{};
  std::unordered_map<std::string, std::unique_ptr<PrototypeAST>>
      FunctionProtos{};
  std::unordered_set<std::string> CompiledFunctions{};

  llvm::Value *visitImpl(const BinaryExprAST &A);
  llvm::Value *visitImpl(const UnaryExprAST &A) {
    assert(false);
    return nullptr;
  }
  llvm::Value *visitImpl(const CallExprAST &A);
  llvm::Value *visitImpl(const ForExprAST &A);
  llvm::Value *visitImpl(const IfExprAST &A);
  llvm::Value *visitImpl(const NumberExprAST &A) const;
  llvm::Value *visitImpl(const VariableExprAST &A) const;

  llvm::Function *visitImpl(const FunctionAST &A);

  llvm::Function *visitImpl(const PrototypeAST &A) const;
  llvm::Function *visitImpl(const ProtoBinaryAST &A) const {
    assert(false);
    return nullptr;
  }
  llvm::Function *visitImpl(const ProtoUnaryAST &A) const {
    assert(false);
    return nullptr;
  }

  llvm::Function *getFunction(llvm::StringRef Name) const;

public:
  llvm::Module &getModule() noexcept { return *CGS->Module; }

  std::unique_ptr<Session> takeSession() {
    return std::exchange(CGS, std::make_unique<Session>());
  }

  const PrototypeAST &addPrototype(std::unique_ptr<PrototypeAST> P) {
    return *(FunctionProtos[P->getName()] = std::move(P));
  }

  llvm::Function *handleAnonExpr(const ExprAST &A);
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_CODEGEN_CODEGEN_H
