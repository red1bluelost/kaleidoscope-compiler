#include "kaleidoscope/Driver/Driver.h"

#include <fmt/core.h>
#include <llvm/Support/TargetSelect.h>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT double putchard(double X) {
  char C = (char)X;
  fmt::print(stderr, "{}", C);
  return 0;
}

/// printd - print that takes a double prints it with newline, returning 0.
extern "C" DLLEXPORT double printd(double X) {
  fmt::print(stderr, "{}\n", X);
  return 0;
}

int main() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  kaleidoscope::Driver().mainLoop();
}
