

#ifndef FUNCTIONINSTCOUNTER_H
#define FUNCTIONINSTCOUNTER_H


#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include "llvm/IR/InstVisitor.h"

namespace memoizer {


struct FunctionInstCounter : public llvm::FunctionPass, llvm::InstVisitor<FunctionInstCounter> {
  static char ID;
  unsigned int load_count = 0;
  unsigned int store_count = 0;
  unsigned int call_count = 0;
  unsigned int arithmatic_count = 0;

  FunctionInstCounter() : llvm::FunctionPass(ID) { }
  bool runOnFunction(llvm::Function& f) override;

  void visitLoadInst(llvm::LoadInst& l) { ++load_count; }
  void visitStoreInst(llvm::StoreInst& s) { ++store_count; }
  void visitCallInst(llvm::CallInst& c)  { if (!c.getCalledFunction()->getName().contains("llvm.dbg")) ++call_count; }
  void visitBinaryOperator(llvm::BinaryOperator& b) { ++arithmatic_count; }
  void visitUnaryOperator(llvm::UnaryOperator& u) { ++arithmatic_count; }
};
}  // namespace callcounter


#endif
