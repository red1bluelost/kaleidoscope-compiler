#include "kaleidoscope/CodeGen/CodeGen.h"

#include "kaleidoscope/Util/Error/Log.h"

#include <llvm/IR/Verifier.h>

#include <fmt/compile.h>
#include <fmt/core.h>

using namespace kaleidoscope;

auto CodeGen::genAssignment(const BinaryExprAST &A) -> llvm::Value * {
  auto *AV = llvm::dyn_cast<VariableExprAST>(&A.getLHS());
  if (!AV)
    return logError("for assignment the lhs must be a variable");
  auto *R = visit(A.getRHS());
  if (!R)
    return logError("failed to codegen RHS");
  auto *L = NamedValues[AV->getName()];
  if (!L)
    return logError("unknown variable on LHS of assignment");
  CGS->Builder.CreateStore(R, L);
  return R;
}

auto CodeGen::visitImpl(const BinaryExprAST &A) -> llvm::Value * {
  // Special case with assignment since we don't want to CodeGen the LHS
  if (A.getOp() == '=')
    return genAssignment(A);

  auto *L = visit(A.getLHS()), *R = visit(A.getRHS());
  if (!L || !R)
    return logError("missing rhs and/or lhs");

  auto &Builder = CGS->Builder;
  switch (A.getOp()) {
  case ':':
    return R;
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
  default:
    break; // Handle a non-builtin operator
  }

  llvm::Function *BinFun =
      getFunction(fmt::format(FMT_COMPILE("binary{}"), A.getOp()));
  if (!BinFun)
    return logError("Unknown binary operator referenced");

  return CGS->Builder.CreateCall(BinFun, {L, R}, "binoptmp");
}

auto CodeGen::visitImpl(const UnaryExprAST &A) -> llvm::Value * {
  auto *V = visit(A.getOperand());
  if (!V)
    return logError("failed to codegen operand");

  llvm::Function *UnFun =
      getFunction(fmt::format(FMT_COMPILE("unary{}"), A.getOpcode()));
  if (!UnFun)
    return logError("Unknown binary operator referenced");

  return CGS->Builder.CreateCall(UnFun, {V}, "unoptmp");
}

auto CodeGen::visitImpl(const CallExprAST &A) -> llvm::Value * {
  // Look up the name in the global module table.
  llvm::Function *CalleeF = getFunction(A.getCallee());
  if (!CalleeF)
    return logError("Unknown function referenced");

  // If argument mismatch error.
  auto &Args = A.getArgs();
  if (CalleeF->arg_size() != Args.size())
    return logError("Incorrect # arguments passed");

  std::vector<llvm::Value *> ArgsV;
  for (auto &Arg : Args) {
    auto *ArgV = visit(*Arg);
    if (!ArgV)
      return logError("Could not codegen arg");
    ArgsV.push_back(ArgV);
  }

  return CGS->Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

auto CodeGen::visitImpl(const ForExprAST &A) -> llvm::Value * {
  auto &Builder = CGS->Builder;
  auto &Context = *CGS->Context;
  auto &VarName = A.getVarName();

  // Make the new basic block for the loop header, inserting after current block
  llvm::Function *Func = Builder.GetInsertBlock()->getParent();

  // Create an alloca for the variable in the entry block.
  llvm::AllocaInst *VarAlloca = createEntryBlockAlloca(Func, VarName);
  // Emit the start code first, without 'variable' in scope
  llvm::Value *StartV = visit(A.getStart());
  if (!StartV)
    return nullptr;
  // Store the value into the alloca.
  Builder.CreateStore(StartV, VarAlloca);

  llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(Context, "loop", Func);
  // Insert an explicit fall through from the current block to the LoopBB
  Builder.CreateBr(LoopBB);
  // Start insertion in LoopBB
  Builder.SetInsertPoint(LoopBB);

  // Within the loop, the variable is defined equal to the PHI node. If it
  // shadows an existing variable, we have to restore it, so save it now
  auto *OldV = std::exchange(NamedValues[VarName], VarAlloca);

  // Emit the body of the loop. This can change the current BB. Note that the
  // value computed by the body is ignored but don't allow an error
  if (!visit(A.getBody()))
    return nullptr;

  // Emit the step value
  llvm::Value *StepVal = visit(A.getStep());
  if (!StepVal)
    return nullptr;

  // Compute the end condition
  llvm::Value *EndCond = visit(A.getEnd());
  if (!EndCond)
    return nullptr;

  // Reload, increment, and restore the alloca.  This handles the case where
  // the body of the loop mutates the variable.
  llvm::Value *CurVar =
      Builder.CreateLoad(VarAlloca->getAllocatedType(), VarAlloca, VarName);
  llvm::Value *NextVar = Builder.CreateFAdd(CurVar, StepVal, "nextvar");
  Builder.CreateStore(NextVar, VarAlloca);

  // Convert condition to a bool by comparing non-equal to 0.0
  EndCond = Builder.CreateFCmpONE(
      EndCond, llvm::ConstantFP::get(Context, llvm::APFloat(0.0)), "loopcond");

  // Create the "after loop" block and insert it
  llvm::BasicBlock *AfterBB =
      llvm::BasicBlock::Create(Context, "afterloop", Func);
  // Insert the conditional branch into the end of LoopEndBB
  Builder.CreateCondBr(EndCond, LoopBB, AfterBB);
  // Any new code will be inserted in AfterBB
  Builder.SetInsertPoint(AfterBB);

  NamedValues[VarName] = OldV;

  // for expr always returns 0.0
  return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(Context));
}

auto CodeGen::visitImpl(const IfExprAST &A) -> llvm::Value * {
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

auto CodeGen::visitImpl(const NumberExprAST &A) const -> llvm::Value * {
  return llvm::ConstantFP::get(*CGS->Context, llvm::APFloat(A.getVal()));
}

auto CodeGen::visitImpl(const VariableExprAST &A) const -> llvm::Value * {
  auto S = A.getName();
  if (!NamedValues.contains(S))
    return logError("unknown variable name");
  llvm::AllocaInst *V = NamedValues.at(S);
  return CGS->Builder.CreateLoad(V->getAllocatedType(), V, S);
}

auto CodeGen::visitImpl(const FunctionAST &A) -> llvm::Function * {
  assert(!CompiledFunctions.contains(A.getProto().getName()) &&
         "function with name has already been compiled");

  // transfer ownership of the prototype to the FunctionProtos map
  auto P = addPrototype(std::make_unique<PrototypeAST>(A.getProto()));
  llvm::Function *TheFunction = getFunction(P.getName());
  if (!TheFunction)
    return nullptr;

  auto PArgs = P.getArgs();
  if (PArgs.size() != TheFunction->arg_size())
    return logError("arg names do not match length in prototype");
  for (unsigned Idx = 0; auto &Arg : TheFunction->args())
    if (Arg.getName() != PArgs[Idx++])
      return logError("arg names do not match prototype");
  if (!TheFunction->empty())
    return logError("Function cannot be redefined");

  // create a new basic block to start insertion into
  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(*CGS->Context, "entry", TheFunction);
  CGS->Builder.SetInsertPoint(BB);

  // record the function arguments in the NamedValues map
  NamedValues.clear();
  for (auto &Arg : TheFunction->args()) {
    std::string Name = Arg.getName().str();
    auto *ArgAlloca = createEntryBlockAlloca(TheFunction, Name);
    CGS->Builder.CreateStore(&Arg, ArgAlloca);
    NamedValues[Name] = ArgAlloca;
  }

  if (llvm::Value *RetVal = visit(A.getBody())) {
    CompiledFunctions.insert(P.getName());
    CGS->Builder.CreateRet(RetVal); // Finish off the function
    llvm::verifyFunction(*TheFunction);
    return TheFunction;
  }

  // Error reading body, remove function
  TheFunction->eraseFromParent();
  return logError("Failed to codegen function body");
}

auto CodeGen::visitImpl(const PrototypeAST &A) const -> llvm::Function * {
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

auto CodeGen::getFunction(llvm::StringRef Name) const -> llvm::Function * {
  // first, see if the function has already been added to the current module
  if (auto *F = CGS->Module->getFunction(Name))
    return F;

  // if not, check whether we can codegen the declaration from some existing
  // prototype
  if (auto FI = FunctionProtos.find(Name.str()); FI != FunctionProtos.end())
    return visitImpl(*FI->second);

  // if no existing prototype exists, return null
  return nullptr;
}

/// createEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
auto CodeGen::createEntryBlockAlloca(llvm::Function *Func,
                                     const llvm::Twine &VarName)
    -> llvm::AllocaInst * {
  llvm::IRBuilder<> TmpB(&Func->getEntryBlock(), Func->getEntryBlock().begin());
  return TmpB.CreateAlloca(llvm::Type::getDoubleTy(*CGS->Context), 0, VarName);
}

auto CodeGen::handleAnonExpr(const ExprAST &A) -> llvm::Function * {
  // make an anonymous proto
  PrototypeAST Proto("__anon_expr", std::vector<std::string>());
  llvm::Function *TheFunction = visit(Proto);
  if (!TheFunction)
    return nullptr;

  // create a new basic block to start insertion into
  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(*CGS->Context, "entry", TheFunction);
  CGS->Builder.SetInsertPoint(BB);

  if (llvm::Value *RetVal = visit(A)) {
    CGS->Builder.CreateRet(RetVal); // Finish off the function
    llvm::verifyFunction(*TheFunction);
    return TheFunction;
  }

  // Error reading body, remove function
  TheFunction->eraseFromParent();
  return logError("Failed to codegen function body");
}
