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

class CodeGen : public ASTVisitor<CodeGen, AVDelType::ExprAST> {
  using Parent = ASTVisitor<CodeGen, AVDelType::ExprAST>;
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
  std::unordered_map<std::string, llvm::AllocaInst*> NamedValues{};
  std::unique_ptr<Session>                           CGS{};
  std::unordered_map<std::string, std::unique_ptr<PrototypeAST>>
                                  FunctionProtos{};
  std::unordered_set<std::string> CompiledFunctions{};

  auto genAssignment(const BinaryExprAST& A) -> llvm::Value*;

  auto visitImpl(const BinaryExprAST& A) -> llvm::Value*;
  auto visitImpl(const UnaryExprAST& A) -> llvm::Value*;
  auto visitImpl(const CallExprAST& A) -> llvm::Value*;
  auto visitImpl(const ForExprAST& A) -> llvm::Value*;
  auto visitImpl(const IfExprAST& A) -> llvm::Value*;
  auto visitImpl(const NumberExprAST& A) const -> llvm::Value*;
  auto visitImpl(const VariableExprAST& A) -> llvm::Value*;
  auto visitImpl(const VarAssignExprAST& A) -> llvm::Value*;

  auto visitImpl(const FunctionAST& A) -> llvm::Function*;

  auto visitImpl(const PrototypeAST& A) const -> llvm::Function*;

  auto getFunction(llvm::StringRef Name) const -> llvm::Function*;

  auto createEntryBlockAlloca(
      llvm::Function* TheFunction, const llvm::Twine& VarName
  ) -> llvm::AllocaInst*;

 public:
  auto getModule() noexcept -> llvm::Module& { return *CGS->Module; }

  auto takeSession() -> std::unique_ptr<Session> {
    return std::exchange(CGS, std::make_unique<Session>());
  }

  auto addPrototype(std::unique_ptr<PrototypeAST> P) -> const PrototypeAST& {
    return *(FunctionProtos[P->getName()] = std::move(P));
  }

  auto handleAnonExpr(const ExprAST& A) -> llvm::Function*;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_CODEGEN_CODEGEN_H
