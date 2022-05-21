#ifndef KALEIDOSCOPE_AST_FUNCTIONAST_H
#define KALEIDOSCOPE_AST_FUNCTIONAST_H

#include "AST/ExprAST.h"
#include "AST/PrototypeAST.h"

#include <memory>

namespace llvm {
class Function;
} // namespace llvm

namespace kaleidoscope {
class CodeGen;

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
      : Proto(std::move(Proto)), Body(std::move(Body)) {}
  llvm::Function *codegen(CodeGen &CG);
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_FUNCTIONAST_H
