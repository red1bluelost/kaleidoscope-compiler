#ifndef KALEIDOSCOPE_CODEGEN_CODEGEN_H
#define KALEIDOSCOPE_CODEGEN_CODEGEN_H

#include "kaleidoscope/AST/ASTVisitor.h"
#include "kaleidoscope/AST/PrototypeAST.h"

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

  llvm::Value *visitImpl(BinaryExprAST &A);
  llvm::Value *visitImpl(CallExprAST &A);
  llvm::Value *visitImpl(ForExprAST &A);
  llvm::Value *visitImpl(IfExprAST &A);
  llvm::Value *visitImpl(NumberExprAST &A) const;
  llvm::Value *visitImpl(VariableExprAST &A) const;

  llvm::Function *visitImpl(FunctionAST &A);
  llvm::Function *visitImpl(PrototypeAST &A) const;

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

  llvm::Function *getFunction(llvm::StringRef Name) const;

public:
  llvm::Module &getModule() { return *CGS->Module; }

  std::unique_ptr<Session> takeSession() {
    return std::exchange(CGS, std::make_unique<Session>());
  }

  PrototypeAST &addPrototype(std::unique_ptr<PrototypeAST> P) {
    return *(FunctionProtos[P->getName()] = std::move(P));
  }

  llvm::Function *handleAnonExpr(ExprAST &A);
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_CODEGEN_CODEGEN_H
