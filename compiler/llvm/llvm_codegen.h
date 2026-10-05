#pragma once

#include "../jir/jir.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <string>

class LLVMCodeGenerator {
public:
    LLVMCodeGenerator();

    std::string generate(
        const JIRModule& jir
    );

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;

    void generateRuntime();

    void generateFunction(
        const JIRFunction& function
    );
};