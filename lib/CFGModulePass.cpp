#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Pass.h"
#include <system_error>
#include <string>

using namespace llvm;

namespace {
    // Global variable to name all basic blocks.
    // This is not thread safe.
    int BBCounter = 0;

    struct CFGModulePass : public PassInfoMixin<CFGModulePass> {
        PreservedAnalyses run(Module &M,
                                    ModuleAnalysisManager &);
        void nameBBs(Module &M);
        void linkBBs(Module &M);
        static bool isRequired() { return true; }
    };

    void CFGModulePass::nameBBs(Module &M) {
        for (Function &F : M) {
            for (BasicBlock &BB : F) {
                BB.setName("BB"+std::to_string(BBCounter));
                ++BBCounter;
            } 
        }
    }

    void CFGModulePass::linkBBs(Module &M) {
        std::error_code EC;
        raw_fd_ostream File("output.txt", EC, sys::fs::OF_Append);
        for (Function &F : M) {
            File << "Function name: " << F.getName() << "\n";
            for (BasicBlock &BB : F) {
                File << "Basic block: " << BB.getName() << "\n";
                for (Instruction &I : BB) {
                    File << I << "\n";
                    if (auto* brInst = dyn_cast<BranchInst>(&I)) {
                        BasicBlock* targetBB = brInst->getSuccessor(0);
                        File << "Points to: " << targetBB->getName() << "\n";
                    }
                }
            } 
        }
    }

    PreservedAnalyses CFGModulePass::run(Module &M,
                                         ModuleAnalysisManager &) {
        nameBBs(M);
        linkBBs(M);
        return PreservedAnalyses::all();
    }
}

PassPluginLibraryInfo getCFGModulePassPluginInfo() {
  return {
        LLVM_PLUGIN_API_VERSION,
        "CFGModulePass",
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](
                    StringRef Name,
                    ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>
                ) {
                    if (Name == "cfg-module-pass") {
                        MPM.addPass(CFGModulePass());
                        return true;
                    }
                    return false;
                }
            );
        }
    };
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize CustomCFGPrinter when added to the pass pipeline on the
// command line, i.e. via '-passes=custom-cfg-printer'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getCFGModulePassPluginInfo();
}
