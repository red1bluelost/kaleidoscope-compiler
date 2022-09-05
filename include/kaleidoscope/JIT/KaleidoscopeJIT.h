#ifndef KALEIDOSCOPE_JIT_KALEIDOSCOPEJIT_H
#define KALEIDOSCOPE_JIT_KALEIDOSCOPEJIT_H

#include <llvm/ADT/StringRef.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutorProcessControl.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>

#include <memory>

namespace kaleidoscope {

class KaleidoscopeJIT {
private:
  const std::unique_ptr<llvm::orc::ExecutionSession> ExecSess;

  const llvm::DataLayout DataLayout;
  llvm::orc::MangleAndInterner Mangle;

  llvm::orc::RTDyldObjectLinkingLayer ObjectLayer;
  llvm::orc::IRCompileLayer CompileLayer;

  llvm::orc::JITDylib &MainJD;

public:
  KaleidoscopeJIT(std::unique_ptr<llvm::orc::ExecutionSession> ES,
                  llvm::orc::JITTargetMachineBuilder JTMB, llvm::DataLayout DL)
      : ExecSess(std::move(ES)), DataLayout(std::move(DL)),
        Mangle(*ExecSess, DataLayout),
        ObjectLayer(
            *ExecSess,
            []() { return std::make_unique<llvm::SectionMemoryManager>(); }),
        CompileLayer(
            *ExecSess, ObjectLayer,
            std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(JTMB))),
        MainJD(ExecSess->createBareJITDylib("<main>")) {
    MainJD.addGenerator(
        cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
            DataLayout.getGlobalPrefix())));
    if (ExecSess->getExecutorProcessControl()
            .getTargetTriple()
            .isOSBinFormatCOFF()) {
      ObjectLayer.setOverrideObjectFlagsWithResponsibilityFlags(true);
      ObjectLayer.setAutoClaimResponsibilityForObjectSymbols(true);
    }
  }

  ~KaleidoscopeJIT() {
    if (auto Err = ExecSess->endSession())
      ExecSess->reportError(std::move(Err));
  }

  static auto create() -> llvm::Expected<std::unique_ptr<KaleidoscopeJIT>> {
    auto EPC = llvm::orc::SelfExecutorProcessControl::Create();
    if (!EPC)
      return EPC.takeError();

    auto ES = std::make_unique<llvm::orc::ExecutionSession>(std::move(*EPC));

    llvm::orc::JITTargetMachineBuilder JTMB(
        ES->getExecutorProcessControl().getTargetTriple());

    auto DL = JTMB.getDefaultDataLayoutForTarget();
    if (!DL)
      return DL.takeError();

    return std::make_unique<KaleidoscopeJIT>(std::move(ES), std::move(JTMB),
                                             std::move(*DL));
  }

  auto getDataLayout() const -> const llvm::DataLayout & { return DataLayout; }

  auto getMainJITDylib() -> llvm::orc::JITDylib & { return MainJD; }

  auto addModule(llvm::orc::ThreadSafeModule TSM,
                 llvm::orc::ResourceTrackerSP RT = nullptr) -> llvm::Error {
    if (!RT)
      RT = MainJD.getDefaultResourceTracker();
    return CompileLayer.add(RT, std::move(TSM));
  }

  auto lookup(llvm::StringRef Name)
      -> llvm::Expected<llvm::JITEvaluatedSymbol> {
    return ExecSess->lookup({&MainJD}, Mangle(Name.str()));
  }
};

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_JIT_KALEIDOSCOPEJIT_H
