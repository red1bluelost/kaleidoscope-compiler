#ifndef KALEIDOSCOPE_AST_FUNCTIONAST_H
#define KALEIDOSCOPE_AST_FUNCTIONAST_H

#include "AST/ExprAST.h"
#include "AST/PrototypeAST.h"

#include <memory>

namespace kaleidoscope {

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
      : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_FUNCTIONAST_H
