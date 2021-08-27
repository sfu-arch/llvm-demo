

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

  FunctionMemoizer(std::string n) : llvm::ModulePass(ID), functionName(n) { }

  bool runOnModule(llvm::Module& m) override;
  void visitCallInst(llvm::CallInst& c);

    // variables
  llvm::Type *voidTy;
  llvm::Type *i64Ty;
  llvm::Type *i8PtrTy;
  llvm::Type *i32Ty;
};


}  // namespace callcounter


#endif
