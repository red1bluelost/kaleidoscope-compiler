#include "Driver/Driver.h"

#include <llvm/Support/TargetSelect.h>

int main() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  kaleidoscope::Driver().mainLoop();
}
