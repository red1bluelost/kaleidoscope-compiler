#ifndef KALEIDOSCOPE_AST_PROTOTYPEAST_H
#define KALEIDOSCOPE_AST_PROTOTYPEAST_H

#include <string>
#include <vector>

namespace kaleidoscope {

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;

public:
  PrototypeAST(std::string Name, std::vector<std::string> Args)
      : Name(std::move(Name)), Args(std::move(Args)) {}

  const std::string &getName() const { return Name; }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_AST_PROTOTYPEAST_H
