#ifndef KALEIDOSCOPE_CODEGEN_CODEGEN_H
#define KALEIDOSCOPE_CODEGEN_CODEGEN_H

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

  void recordFuncArgs(llvm::Function &Func) {
    NamedValues.clear();
    for (auto &Arg : Func.args())
      NamedValues[(std::string)Arg.getName()] = &Arg;
  }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_CODEGEN_CODEGEN_H
