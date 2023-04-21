#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/FileSystem.h"
#include <system_error>
#include <string>

using namespace llvm;


namespace {
    // Global variable to name all basic blocks.
    // This is not thread safe.
    int BBCounter = 0;
    // This method implements what the pass does
    void foo(Function &F) {
        // To do: name all basic blocks once one is found.
        std::error_code EC;
        raw_fd_ostream File("output.txt", EC, sys::fs::OF_Append);
        File << "instructions in this function: " << F.getName() << "\n";
        for (BasicBlock &BB : F) {
            BB.setName("BB"+std::to_string(BBCounter));
            ++BBCounter;
            File << "Basic block: " << BB.getName() << "\n";
            for (Instruction &I : BB) {
                // To do: if instruction is "br", get its successor's name
                File << I << "\n";
                if (auto* brInst = dyn_cast<BranchInst>(&I)) {
                    BasicBlock* targetBB = brInst->getSuccessor(0);
                    File << "Points to: " << targetBB->getName() << "\n";
                }
            }
        }
    }

    // New PM implementation
    struct CustomCFGPrinter : PassInfoMixin<CustomCFGPrinter> {
        // Main entry point, takes IR unit to run the pass on (&F) and the
        // corresponding pass manager (to be queried if need be)
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
            foo(F);
            return PreservedAnalyses::all();
        }

        // Without isRequired returning true, this pass will be skipped for functions
        // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
        // all functions with optnone.
        static bool isRequired() { return true; }
    };
}

llvm::PassPluginLibraryInfo getCustomCFGPrinterPluginInfo() {
  return {
        LLVM_PLUGIN_API_VERSION,
        "CustomCFGPrinter",
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](
                    StringRef Name,
                    FunctionPassManager &FPM,
                    ArrayRef<PassBuilder::PipelineElement>
                ) {
                    if (Name == "custom-cfg-printer") {
                        FPM.addPass(CustomCFGPrinter());
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
  return getCustomCFGPrinterPluginInfo();
}
