#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/FileSystem.h"
#include <system_error>

using namespace llvm;


namespace {
    // This method implements what the pass does
    void foo(Function &F) {
        // This will print "its alive" once for each function present in the target .ll file
        std::error_code EC;
        raw_fd_ostream File("output.txt", EC, sys::fs::OF_Text);
        File << "instructions in this function:\n";
        for (BasicBlock &BB : F) {
            File << "Basic block: " << BB.getName() << "\n";
            for (Instruction &I : BB) {
                File << I << "\n";
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
