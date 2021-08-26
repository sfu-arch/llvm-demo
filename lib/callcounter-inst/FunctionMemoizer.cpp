

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "FunctionMemoizer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "FunctionInstCounter.h"


using namespace llvm;
using memoizer::FunctionMemoizer;
using llvm::legacy::PassManager;


namespace memoizer {


char FunctionMemoizer::ID = 0;

}

bool
FunctionMemoizer::runOnModule(Module& m) {
    FunctionInstCounter fic;
    for (auto& f : m)
        if (f.getName() == this->functionName) {
            fic.runOnFunction(f);
        }
    return false;
}