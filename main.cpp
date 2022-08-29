#include "kaleidoscope/Driver/Driver.h"

#include <llvm/Support/TargetSelect.h>

#include <fmt/core.h>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT [[maybe_unused]] double putchard(double X) {
  char C = (char)X;
  fmt::print(stderr, "{}", C);
  return 0;
}

/// printd - print that takes a double prints it with newline, returning 0.
extern "C" DLLEXPORT [[maybe_unused]] double printd(double X) {
  fmt::print(stderr, "{}", X);
  return 0;
}

int main() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  kaleidoscope::Driver().mainLoop();
}
