

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
            fic.runOnFunction(f);
        }
    std::vector<Instruction*> inst_vec;
    for (auto& f : m)
        for (auto& bb : f)
            for (auto& i : bb)
                inst_vec.push_back(&i);
    for (auto& i : inst_vec) 
        visit(i);
    
    auto *print_status_type = FunctionType::get(voidTy, {}, false);
    auto print_status_func = m.getOrInsertFunction("MEMOIZER_printStats", print_status_type);
    llvm::appendToGlobalDtors(m, (Function*) print_status_func.getCallee(), 0);

    return true;
}

void FunctionMemoizer::visitCallInst(CallInst& c) {
    if (c.getCalledFunction()->getName() != functionName) 
        return;
    auto &m = *c.getModule();

    // store arg[0] and call runtime functions with it
    auto argVal = c.getArgOperand(0);
    auto argI = new AllocaInst(argVal->getType(), Type::getInt8PtrTy(c.getContext())->getPointerAddressSpace(), "", &c);
    auto sI = new StoreInst(argVal, argI, &c);
    auto stored = sI->getPointerOperand();
    auto bc = new BitCastInst(stored, i8PtrTy, "", sI);


    // Check whether the input is already in the table
    auto *exist_arg_types = FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
    auto on_exist = m.getOrInsertFunction("MEMOIZER_existEntry", exist_arg_types);
    auto DL = m.getDataLayout();
    auto Sz = DL.getTypeStoreSize(argVal->getType());
    Value *exist_call_args[] = {bc, ConstantInt::get(i32Ty, GetTypeEnum(argVal))};
    CallInst *exist_call = CallInst::Create(on_exist, exist_call_args, "", &c);
    // std::vector<Value*> args = {argVal, exist_call};
    // callPrintf(exist_call, "exists res: %lf, %d\n", args);

    auto *zero = Constant::getNullValue(i32Ty);
    ICmpInst* if_cond = new ICmpInst(&c, ICmpInst::ICMP_EQ, zero, exist_call);
    Instruction *then_inst, *else_inst;
    SplitBlockAndInsertIfThenElse(if_cond, &c, &then_inst, &else_inst, nullptr);

    // Call get Entry function
    auto *getEntry_arg_types = FunctionType::get(i8PtrTy, {i8PtrTy, i32Ty}, false);
    auto on_get = m.getOrInsertFunction("MEMOIZER_getEntry", getEntry_arg_types);
    CallInst *get_call = CallInst::Create(on_get, exist_call_args, "", else_inst);
    // result of the getEntry() is a void*. We need to cast it to the desired type
    auto *cast_result = new BitCastInst(get_call, c.getType()->getPointerTo(), "", else_inst);
    auto *load_entry = new LoadInst(c.getType(), cast_result, "", else_inst);
    
    // Insert PHI node after if-else statement
    auto *phi = PHINode::Create(c.getType(), 2, "memoized_phi", &c);
    c.replaceAllUsesWith(phi);
    phi->addIncoming(load_entry, else_inst->getParent());
    phi->addIncoming(&c, then_inst->getParent());
    c.moveBefore(then_inst);

    // Call insert function
    auto *insert_arg_types = FunctionType::get(voidTy, {i8PtrTy, i32Ty, i8PtrTy, i32Ty}, false);
    auto on_insert = m.getOrInsertFunction("MEMOIZER_insertEntry", insert_arg_types);
        // store return value of c and call runtime functions with it
    auto c_result = new AllocaInst(c.getType(), Type::getInt8PtrTy(c.getContext())->getPointerAddressSpace(), "", &c);
    auto c_result_store_inst = new StoreInst(&c, c_result, then_inst);
    auto c_stored = c_result_store_inst->getPointerOperand();
    auto c_result_bc = new BitCastInst(c_stored, i8PtrTy, "", c_result_store_inst);
    
    Value *insert_call_args[] = {bc, ConstantInt::get(i32Ty, GetTypeEnum(argVal)), c_result_bc, ConstantInt::get(i32Ty, GetTypeEnum(&c))};
    CallInst *insert_call = CallInst::Create(on_insert, insert_call_args, "", then_inst);

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

int 
FunctionMemoizer::GetTypeEnum(Value *v)
{
    if (v->getType()->isFloatTy())
        return FLT;
    else if (v->getType()->isDoubleTy())
        return DBL;
    else if (v->getType()->isIntegerTy(1))
        return BOL;
    else if (v->getType()->isIntegerTy(8))
        return CHR;
    else if (v->getType()->isIntegerTy(16))
        return HLF;
    else if (v->getType()->isIntegerTy(32))
        return WHL;
    else if (v->getType()->isIntegerTy(64))
        return LNG;
    else if (v->getType()->isPointerTy())
        return PTR;

    return INVALID;
} //gettypeenum
