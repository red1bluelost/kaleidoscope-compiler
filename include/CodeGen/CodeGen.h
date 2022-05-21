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
  llvm::LLVMContext Context;
  llvm::IRBuilder<> Builder{Context};
  std::unique_ptr<llvm::Module> Module;
  std::map<std::string, llvm::Value *> NamedValues;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_CODEGEN_CODEGEN_H
