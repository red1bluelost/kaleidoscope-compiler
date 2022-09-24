#include "kaleidoscope/Driver/ReplDriver.h"

#include <llvm/Support/TargetSelect.h>

#include <fmt/core.h>

#ifdef _WIN32
# define DLLEXPORT __declspec(dllexport)
#else
# define DLLEXPORT
#endif

/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT [[maybe_unused]] auto putchard(double X) -> double {
  fmt::print(stderr, "{}", static_cast<char>(X));
  return X;
}

/// printd - print that takes a double prints it with newline, returning 0.
extern "C" DLLEXPORT [[maybe_unused]] auto printd(double X) -> double {
  fmt::print(stderr, "{}", X);
  return X;
}

auto main() -> int {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  kaleidoscope::ReplDriver().mainLoop();
}
