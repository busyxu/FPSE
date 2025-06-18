// This file is part of SymCC.
//
// SymCC is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// SymCC is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// SymCC. If not, see <https://www.gnu.org/licenses/>.

#include "llvm/Analysis/LoopPass.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/CFG.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <set>
using namespace llvm;

namespace {
struct ResetLoopPass : public LoopPass {
  static char ID;
    ResetLoopPass() : LoopPass(ID) {}
  int globalVarID = 0;

  bool runOnLoop(Loop *L, LPPassManager &LPM) {
    BasicBlock * loopHeader = L->getHeader();
    Instruction *brInst = &loopHeader->back();
    if (brInst->getOpcode() != Instruction::Br)
      return false;
    BranchInst *bi = cast<BranchInst>(brInst);
    if (bi->isUnconditional())
      return false;

    // get the BB before Loop to set alloca
    BasicBlock * bbPre = L->getLoopPredecessor();
    std::string varName = "limit" + std::to_string(globalVarID);
    globalVarID ++;

    IRBuilder<> IRBPre(bbPre->getFirstNonPHI());
    std::string allocaName = varName + "Alloca";
    auto limitAlloca = IRBPre.CreateAlloca(
            IRBPre.getInt32Ty(), nullptr,allocaName);
    IRBPre.CreateStore(IRBPre.getInt32(0),limitAlloca);

    IRBuilder<> IRBHead(loopHeader->getFirstNonPHI());
    std::string loadName = varName + "Load";
    std::string incName = varName + "Inc";
    std::string cmpName = varName + "Cmp";
    auto limitVal = IRBHead.CreateLoad(
            IRBHead.getInt32Ty(), limitAlloca,loadName);
    auto limitInc = IRBHead.CreateAdd(
            limitVal,IRBHead.getInt32(1),incName);
    IRBHead.CreateStore(limitInc,limitAlloca);
    auto limitCmp = IRBHead.CreateICmpSLT(
            limitInc,IRBHead.getInt32(4),cmpName);
    auto originCmp = bi->getCondition();

    IRBuilder<> IRBCond(brInst);
    std::string andName = varName + "And";
    auto newCmp = IRBCond.CreateAnd(originCmp,limitCmp,andName);
    bi->setCondition(newCmp);

    return true;
  }
};
}

char ResetLoopPass::ID = 0;
static RegisterPass<ResetLoopPass> X("resetLoop", "Reset Loop Pass");
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new ResetLoopPass()); });

