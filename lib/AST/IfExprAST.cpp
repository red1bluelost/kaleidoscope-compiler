#include "AST/IfExprAST.h"

#include "CodeGen/CodeGen.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>

using namespace kaleidoscope;

llvm::Value *IfExprAST::codegen(CodeGen &CG) {
  llvm::Value *CondV = Cond->codegen(CG);
  if (!CondV)
    return nullptr;

  auto &Builder = CG.getBuilder();
  auto &Context = CG.getContext();

  // Convert condition to a bool by comparing non-equal to 0.0
  CondV = Builder.CreateFCmpONE(
      CondV, llvm::ConstantFP::get(Context, llvm::APFloat(0.0)), "ifcond");

  llvm::Function *Func = Builder.GetInsertBlock()->getParent();

  // Create blocks for the 'then' and 'else' cases. Insert the 'then' block at
  // the end of the function
  llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(Context, "then", Func),
                   *ElseBB = llvm::BasicBlock ::Create(Context, "else"),
                   *MergeBB = llvm::BasicBlock::Create(Context, "ifcont");

  Builder.CreateCondBr(CondV, ThenBB, ElseBB);

  // Emit 'then' value
  Builder.SetInsertPoint(ThenBB);

  // Codegen of 'Then' can change the current block, update ThenBB for the PHI
  llvm::Value *ThenV = Then->codegen(CG);
  if (!ThenV)
    return nullptr;
  Builder.CreateBr(MergeBB);
  ThenBB = Builder.GetInsertBlock();

  // emit 'else' block
  Func->getBasicBlockList().push_back(ElseBB);
  Builder.SetInsertPoint(ElseBB);

  // codegen of 'Else' can change the current block, update ElseBB for the PHI
  llvm::Value *ElseV = Else->codegen(CG);
  if (!ElseV)
    return nullptr;
  Builder.CreateBr(MergeBB);
  ElseBB = Builder.GetInsertBlock();

  // Emit 'merge' block
  Func->getBasicBlockList().push_back(MergeBB);
  Builder.SetInsertPoint(MergeBB);
  llvm::PHINode *PN =
      Builder.CreatePHI(llvm::Type::getDoubleTy(Context), 2, "iftmp");

  PN->addIncoming(ThenV, ThenBB);
  PN->addIncoming(ElseV, ElseBB);
  return PN;
}
