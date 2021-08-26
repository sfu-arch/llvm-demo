

#ifndef FUNCTIONMEMOIZER_H
#define FUNCTIONMEMOIZER_H


#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include "llvm/IR/InstVisitor.h"

namespace memoizer {


struct FunctionMemoizer : public llvm::ModulePass, llvm::InstVisitor<FunctionMemoizer> {
  static char ID;
  std::string functionName;
  llvm::DenseMap<llvm::Function*, uint64_t> ids;
  llvm::DenseSet<llvm::Function*> internal;

  FunctionMemoizer(std::string n) : llvm::ModulePass(ID), functionName(n) { }

  bool runOnModule(llvm::Module& m) override;

//   void handleCalledFunction(llvm::Function& f, llvm::FunctionCallee counter);
//   void handleInstruction(llvm::CallBase& cb, llvm::FunctionCallee counter);
};


}  // namespace callcounter


#endif
