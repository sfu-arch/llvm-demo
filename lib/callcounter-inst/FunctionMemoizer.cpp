

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "FunctionMemoizer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "FunctionInstCounter.h"
#include "llvm/Option/ArgList.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

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
            // fic.runOnFunction(f);
        }
    std::vector<Instruction*> inst_vec;
    for (auto& f : m)
        for (auto& bb : f)
            for (auto& i : bb)
                inst_vec.push_back(&i);
    for (auto& i : inst_vec) 
        visit(i);
    return true;
}

void FunctionMemoizer::visitCallInst(CallInst& c) {
    if (c.getCalledFunction()->getName() != functionName) 
        return;

    auto &m = *c.getModule();

    // Check whether the input is already in the table
    auto *exist_arg_types = FunctionType::get(i32Ty, {i32Ty}, false);
    auto on_exist = m.getOrInsertFunction("MEMOIZER_existEntry", exist_arg_types);
    CallInst *exist_call = CallInst::Create(on_exist, {c.getArgOperand(0)}, "", &c);

    auto *zero = Constant::getNullValue(i32Ty);
    ICmpInst* if_cond = new ICmpInst(&c, ICmpInst::ICMP_EQ, zero, exist_call);
    Instruction *then_inst, *else_inst;
    SplitBlockAndInsertIfThenElse(if_cond, &c, &then_inst, &else_inst, nullptr);

    // Call get Entry function
    auto *getEntry_arg_types = FunctionType::get(i32Ty, {i32Ty}, false);
    auto on_get = m.getOrInsertFunction("MEMOIZER_getEntry", getEntry_arg_types);
    CallInst *get_call = CallInst::Create(on_get, {c.getArgOperand(0)}, "", else_inst);

    // Call insert function
    auto *insert_arg_types = FunctionType::get(voidTy, {i32Ty, i32Ty}, false);
    auto on_insert = m.getOrInsertFunction("MEMOIZER_insertEntry", insert_arg_types);
    CallInst *insert_call = CallInst::Create(on_insert, {c.getArgOperand(0), &c}, "", c.getNextNode());

    // Insert PHI node after if statement
    auto *phi = PHINode::Create(i32Ty, 2, "memoized_phi", &c);
    c.replaceAllUsesWith(phi);
    phi->addIncoming(get_call, else_inst->getParent());
    phi->addIncoming(&c, then_inst->getParent());
    c.moveBefore(then_inst);
}



// -------------------------------------------------------------------------- 
// Helper functions 
// -------------------------------------------------------------------------- 


// This function calls the printf before the given instruction.
// HOW TO USE:
//     std::vector<Value*> args = {arg0, arg1, arg2, ...};
//     callPrintf(call_before, format, args);
// 
void 
FunctionMemoizer::callPrintf(Instruction *I, char *format, std::vector<Value *> args) {
    auto &m = *I->getModule();
    auto &context = m.getContext();

    PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(context));
    FunctionType *PrintfTy = FunctionType::get(IntegerType::getInt32Ty(context), PrintfArgTy, /*IsVarArgs=*/true);
    FunctionCallee Printf = m.getOrInsertFunction("printf", PrintfTy);

    llvm::Constant *ResultFormatStr = llvm::ConstantDataArray::getString(context, format);

    Constant *ResultFormatStrVar =
        m.getOrInsertGlobal("ResultFormatStrIR", ResultFormatStr->getType());
    dyn_cast<GlobalVariable>(ResultFormatStrVar)->setInitializer(ResultFormatStr);

    Instruction *ResultHeaderStrPtr = CastInst::CreatePointerCast(ResultFormatStrVar, PrintfArgTy, "");
    ResultHeaderStrPtr->insertAfter(I);
    std::vector<Value *> print_args = {ResultHeaderStrPtr};
    print_args.insert(print_args.end(), args.begin(), args.end());
    CallInst::Create(Printf, print_args, "", ResultHeaderStrPtr->getNextNode());
}