
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "FunctionInstCounter.h"


using namespace llvm;
using memoizer::FunctionInstCounter;


namespace memoizer {

char FunctionInstCounter::ID = 2;

}

bool
FunctionInstCounter::runOnFunction(Function& f) {
    visit(f);

    errs() << "############## FUNCTION: " << f.getName() << " ##############\n";
    errs() << "Input Count: " << f.arg_size() << "\n";
    errs() << "Load Count: " << load_count << "\n";
    errs() << "Store Count: " << store_count << "\n";
    errs() << "Arithmethic Count: " << arithmatic_count << "\n";
    errs() << "Function Call Count: " << call_count << "\n";

    errs() << "Total Instruction Count: " << f.getInstructionCount() << "\n";
    return false;
}
