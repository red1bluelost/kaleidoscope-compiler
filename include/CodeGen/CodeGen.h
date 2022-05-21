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
  std::map<std::string, llvm::Value *> NamedValues;

public:
  llvm::LLVMContext Context;
  llvm::IRBuilder<> Builder{Context};
  std::unique_ptr<llvm::Module> Module;

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
