#ifndef KALEIDOSCOPE_AST_EXPRAST_H
#define KALEIDOSCOPE_AST_EXPRAST_H

namespace kaleidoscope {

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
  virtual ~ExprAST() = default;
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_EXPRAST_H
