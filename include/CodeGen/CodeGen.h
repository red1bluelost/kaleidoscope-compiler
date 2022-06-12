#ifndef KALEIDOSCOPE_CODEGEN_CODEGEN_H
#define KALEIDOSCOPE_CODEGEN_CODEGEN_H

#include "AST/PrototypeAST.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include <map>
#include <memory>
#include <string>

namespace kaleidoscope {

class CodeGen {
public:
  struct Session {
    std::unique_ptr<llvm::LLVMContext> Context =
        std::make_unique<llvm::LLVMContext>();
    std::unique_ptr<llvm::Module> Module =
        std::make_unique<llvm::Module>("default codegen", *Context);
    llvm::IRBuilder<> Builder{*Context};
  };

private:
  std::map<std::string, llvm::Value *> NamedValues{};
  std::unique_ptr<Session> CGS = std::make_unique<Session>();
  std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos{};

public:
  llvm::LLVMContext &getContext() { return *CGS->Context; }
  llvm::Module &getModule() { return *CGS->Module; }
  llvm::IRBuilder<> &getBuilder() { return CGS->Builder; }

  std::unique_ptr<Session> takeSession() {
    return std::exchange(CGS, std::make_unique<Session>());
  }

  llvm::Value *getNamedVal(const std::string &Name) {
    if (NamedValues.contains(Name))
      return NamedValues[Name];
    return nullptr;
  }

  llvm::Value *setNamedValue(const std::string &Name, llvm::Value *Val) {
    return NamedValues[Name] = Val;
  }

  llvm::Value *shadowNamedVal(const std::string &Name, llvm::Value *Val) {
    return std::exchange(NamedValues[Name], Val);
  }

  void recordFuncArgs(llvm::Function &Func) {
    NamedValues.clear();
    for (auto &Arg : Func.args())
      NamedValues[(std::string)Arg.getName()] = &Arg;
  }

  PrototypeAST &addPrototype(std::unique_ptr<PrototypeAST> P) {
    return *(FunctionProtos[P->getName()] = std::move(P));
  }

  llvm::Function *getFunction(llvm::StringRef Name) {
    // first, see if the function has already been added to the current module
    if (auto *F = CGS->Module->getFunction(Name))
      return F;

    // if not, check whether we can codegen the declaration from some existing
    // prototype
    if (auto FI = FunctionProtos.find(Name.str()); FI != FunctionProtos.end())
      return FI->second->codegen(*this);

    // if no existing prototype exists, return null
    return nullptr;
  }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_CODEGEN_CODEGEN_H
