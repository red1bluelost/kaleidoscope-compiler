#include "AST/ForExprAST.h"

#include "CodeGen/CodeGen.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>

using namespace kaleidoscope;

llvm::Value *ForExprAST::codegen(CodeGen &CG) {
  // Emit the start code first, without 'variable' in scope
  llvm::Value *StartV = Start->codegen(CG);
  if (!Start)
    return nullptr;

  auto &Builder = CG.getBuilder();
  auto &Context = CG.getContext();

  // Make the new basic block for the loop header, inserting after current block
  llvm::Function *Func = Builder.GetInsertBlock()->getParent();
  llvm::BasicBlock *PreheaderBB = Builder.GetInsertBlock(),
                   *LoopBB = llvm::BasicBlock::Create(Context, "loop", Func);
  // Insert an explicit fall through from the current block to the LoopBB
  Builder.CreateBr(LoopBB);
  // Start insertion in LoopBB
  Builder.SetInsertPoint(LoopBB);

  // Start the PHI node with an entry for Start
  llvm::PHINode *Variable =
      Builder.CreatePHI(llvm::Type::getDoubleTy(Context), 2, VarName);
  Variable->addIncoming(StartV, PreheaderBB);

  // Within the loop, the variable is defined equal to the PHI node. If it
  // shadows an existing variable, we have to restore it, so save it now
  llvm::Value *OldV = CG.shadowNamedVal(VarName, Variable);

  // Emit the body of the loop. This can change the current BB. Note that the
  // value computed by the body is ignored but don't allow an error
  if (!Body->codegen(CG))
    return nullptr;

  // Emit the step value (default step is 1.0)
  llvm::Value *StepVal = nullptr;
  if (Step) {
    StepVal = Step->codegen(CG);
    if (!StepVal)
      return nullptr;
  } else
    StepVal = llvm::ConstantFP::get(Context, llvm::APFloat(1.0));

  llvm::Value *NextVar = Builder.CreateFAdd(Variable, StepVal, "nextvar");

  // Compute the end condition
  llvm::Value *EndCond = End->codegen(CG);
  if (!EndCond)
    return nullptr;
  // Convert condition to a bool by comparing non-equal to 0.0
  EndCond = Builder.CreateFCmpONE(
      EndCond, llvm::ConstantFP::get(Context, llvm::APFloat(0.0)), "loopcond");

  // Create the "after loop" block and insert it
  llvm::BasicBlock *LoopEndBB = Builder.GetInsertBlock(),
                   *AfterBB =
                       llvm::BasicBlock::Create(Context, "afterloop", Func);
  // Insert the conditional branch into the end of LoopEndBB
  Builder.CreateCondBr(EndCond, LoopBB, AfterBB);
  // Any new code will be inserted in AfterBB
  Builder.SetInsertPoint(AfterBB);

  // Add a new entry to the PHI node for the backedge
  Variable->addIncoming(NextVar, LoopEndBB);
  // Restore the unshadowed variable
  CG.setNamedValue(VarName, OldV);
  // for expr always returns 0.0
  return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(Context));
}
