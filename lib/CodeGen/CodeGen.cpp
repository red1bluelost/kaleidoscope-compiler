#include "CodeGen/CodeGen.h"

#include "Util/Error/Log.h"

#include <llvm/IR/Verifier.h>

using namespace kaleidoscope;

llvm::Value *CodeGen::visitImpl(BinaryExprAST &A) {
  auto *L = visit(A.getLHS()), *R = visit(A.getRHS());
  if (!L || !R)
    return logErrorR<llvm::Value>("missing rhs and/or lhs");

  auto &Builder = CGS->Builder;
  switch (A.getOp()) {
  default:
    return logErrorR<llvm::Value>("invalid binary operator");
  case '+':
    return Builder.CreateFAdd(L, R, "addtmp");
  case '-':
    return Builder.CreateFSub(L, R, "subtmp");
  case '*':
    return Builder.CreateFMul(L, R, "multmp");
  case '/':
    return Builder.CreateFDiv(L, R, "divtmp");
  case '<':
    // Convert bool 0/1 to double 0.0 or 1.0
    return Builder.CreateUIToFP(Builder.CreateFCmpULT(L, R, "cmptmp"),
                                llvm::Type::getDoubleTy(Builder.getContext()),
                                "booltmp");
  case '>':
    // Convert bool 0/1 to double 0.0 or 1.0
    return Builder.CreateUIToFP(Builder.CreateFCmpUGT(L, R, "cmptmp"),
                                llvm::Type::getDoubleTy(Builder.getContext()),
                                "booltmp");
  }
}

llvm::Value *CodeGen::visitImpl(CallExprAST &A) {
  // Look up the name in the global module table.
  llvm::Function *CalleeF = getFunction(A.getCallee());
  if (!CalleeF)
    return logErrorR<llvm::Value>("Unknown function referenced");

  // If argument mismatch error.
  auto Args = A.getArgs();
  if (CalleeF->arg_size() != Args.size())
    return logErrorR<llvm::Value>("Incorrect # arguments passed");

  std::vector<llvm::Value *> ArgsV;
  for (auto &Arg : Args) {
    auto *ArgV = visit(*Arg);
    if (!ArgV)
      return logErrorR<llvm::Value>("Could not codegen arg");
    ArgsV.push_back(ArgV);
  }

  return CGS->Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

llvm::Value *CodeGen::visitImpl(ForExprAST &A) {
  // Emit the start code first, without 'variable' in scope
  llvm::Value *StartV = visit(A.getStart());
  if (!StartV)
    return nullptr;

  auto &Builder = CGS->Builder;
  auto &Context = *CGS->Context;

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
      Builder.CreatePHI(llvm::Type::getDoubleTy(Context), 2, A.getVarName());
  Variable->addIncoming(StartV, PreheaderBB);

  // Within the loop, the variable is defined equal to the PHI node. If it
  // shadows an existing variable, we have to restore it, so save it now
  llvm::Value *OldV = shadowNamedVal(A.getVarName(), Variable);

  // Emit the body of the loop. This can change the current BB. Note that the
  // value computed by the body is ignored but don't allow an error
  if (visit(A.getBody()))
    return nullptr;

  // Emit the step value
  llvm::Value *StepVal = visit(A.getStep());
  if (!StepVal)
    return nullptr;

  llvm::Value *NextVar = Builder.CreateFAdd(Variable, StepVal, "nextvar");

  // Compute the end condition
  llvm::Value *EndCond = visit(A.getEnd());
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
  setNamedValue(A.getVarName(), OldV);
  // for expr always returns 0.0
  return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(Context));
}

llvm::Value *CodeGen::visitImpl(IfExprAST &A) {
  llvm::Value *CondV = visit(A.getCond());
  if (!CondV)
    return nullptr;

  auto &Builder = CGS->Builder;
  auto &Context = *CGS->Context;

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
  llvm::Value *ThenV = visit(A.getThen());
  if (!ThenV)
    return nullptr;
  Builder.CreateBr(MergeBB);
  ThenBB = Builder.GetInsertBlock();

  // emit 'else' block
  Func->getBasicBlockList().push_back(ElseBB);
  Builder.SetInsertPoint(ElseBB);

  // codegen of 'Else' can change the current block, update ElseBB for the PHI
  llvm::Value *ElseV = visit(A.getElse());
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

llvm::Value *CodeGen::visitImpl(NumberExprAST &A) {
  return llvm::ConstantFP::get(*CGS->Context, llvm::APFloat(A.getVal()));
}

llvm::Value *CodeGen::visitImpl(VariableExprAST &A) {
  if (auto *V = getNamedVal(A.getName()))
    return V;
  return logErrorR<llvm::Value>("unknown variable name");
}

llvm::Function *CodeGen::visitImpl(FunctionAST &A) {
  assert(!CompiledFunctions.contains(A.getProto().getName()) &&
         "function with name has already been compiled");

  // transfer ownership of the prototype to the FunctionProtos map
  auto P = addPrototype(std::make_unique<PrototypeAST>(A.getProto()));
  llvm::Function *TheFunction = getFunction(P.getName());
  if (!TheFunction)
    return nullptr;

  auto &PArgs = P.getArgs();
  if (PArgs.size() != TheFunction->arg_size())
    return logErrorR<llvm::Function>(
        "arg names do not match length in prototype");
  for (unsigned Idx = 0; auto &Arg : TheFunction->args())
    if (Arg.getName() != PArgs[Idx])
      return logErrorR<llvm::Function>("arg names do not match prototype");
  if (!TheFunction->empty())
    return logErrorR<llvm::Function>("Function cannot be redefined");

  // create a new basic block to start insertion into
  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(*CGS->Context, "entry", TheFunction);
  CGS->Builder.SetInsertPoint(BB);

  // record the function arguments in the NamedValues map
  recordFuncArgs(*TheFunction);

  if (llvm::Value *RetVal = visit(A.getBody())) {
    if (auto S = P.getName(); S != "__anon_expr")
      CompiledFunctions.insert(S);
    CGS->Builder.CreateRet(RetVal); // Finish off the function
    llvm::verifyFunction(*TheFunction);
    return TheFunction;
  }

  // Error reading body, remove function
  TheFunction->eraseFromParent();
  return logErrorR<llvm::Function>("Failed to codegen function body");
}

llvm::Function *CodeGen::visitImpl(PrototypeAST &A) {
  auto &Args = A.getArgs();
  std::vector<llvm::Type *> Doubles(Args.size(),
                                    llvm::Type::getDoubleTy(*CGS->Context));
  llvm::FunctionType *FT = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(*CGS->Context), Doubles, false);
  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, A.getName(), CGS->Module.get());

  for (unsigned Idx = 0; auto &Arg : F->args())
    Arg.setName(Args[Idx++]);

  return F;
}
