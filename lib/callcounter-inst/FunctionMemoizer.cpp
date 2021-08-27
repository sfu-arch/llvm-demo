

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "FunctionMemoizer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "FunctionInstCounter.h"
#include "llvm/Option/ArgList.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;
using memoizer::FunctionMemoizer;
using llvm::legacy::PassManager;


namespace memoizer {


char FunctionMemoizer::ID = 0;

}

bool
FunctionMemoizer::runOnModule(Module& m) {
    auto &context = m.getContext();
    voidTy  = Type::getVoidTy(context);
    i64Ty   = Type::getInt64Ty(context);
    i8PtrTy = Type::getInt8PtrTy(context);
    i32Ty   = Type::getInt32Ty(context);
  
    FunctionInstCounter fic;
    for (auto& f : m)
        if (f.getName() == this->functionName) {
            if (f.getReturnType()->isVoidTy()) {
                errs() << "Can't memoize void function\n";
                return false;
            }
            // Print function stats
            fic.runOnFunction(f);
        }
    
    visit(m);
    return true;
}

void FunctionMemoizer::visitCallInst(CallInst& c) {
    if (c.getCalledFunction()->getName() != functionName) 
        return;

    auto &m = *c.getModule();
    auto &context = m.getContext();

    PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(context));
    FunctionType *PrintfTy = FunctionType::get(IntegerType::getInt32Ty(context), PrintfArgTy, /*IsVarArgs=*/true);
    FunctionCallee Printf = m.getOrInsertFunction("printf", PrintfTy);

    llvm::Constant *ResultFormatStr = llvm::ConstantDataArray::getString(context, "Input: %d, Output: %d\n");

    Constant *ResultFormatStrVar =
        m.getOrInsertGlobal("ResultFormatStrIR", ResultFormatStr->getType());
    dyn_cast<GlobalVariable>(ResultFormatStrVar)->setInitializer(ResultFormatStr);

    Instruction *ResultHeaderStrPtr = CastInst::CreatePointerCast(ResultFormatStrVar, PrintfArgTy, "");
    ResultHeaderStrPtr->insertAfter(&c);
    CallInst * int32_call = CallInst::Create(Printf, {ResultHeaderStrPtr, c.getArgOperand(0), &c}, "", c.getNextNode()->getNextNode());

    // Call insert function
    auto *insert_arg_types = FunctionType::get(voidTy, {i32Ty, i32Ty}, false);

    auto on_insert = m.getOrInsertFunction("MEMOIZER_insertData", insert_arg_types);
    CallInst *insert_call = CallInst::Create(on_insert, {c.getArgOperand(0), &c}, "", int32_call->getNextNode());
}