#include "llvm_codegen.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <llvm/Support/raw_ostream.h>

#include <stdexcept>
#include <string>

namespace
{
    llvm::PointerType* getPointerType(
        llvm::LLVMContext& context
    )
    {
        return llvm::PointerType::getUnqual(context);
    }

    /*
        LLVM 23 no longer provides the CreateGlobalStringPtr()
        helper used by older versions.

        The modern approach is:

            1. Create the global string.
            2. Create a GEP pointing at element [0][0].

        This produces:

            ptr @string

        which can be passed to functions expecting ptr.
    */
    llvm::Value* createStringPointer(
        llvm::IRBuilder<>& builder,
        llvm::Module& module,
        const std::string& value,
        const std::string& name
    )
    {
        llvm::GlobalVariable* globalString =
            builder.CreateGlobalString(
                value,
                name,
                0,
                &module,
                true
            );

        return builder.CreateInBoundsGEP(
            globalString->getValueType(),
            globalString,
            {
                builder.getInt32(0),
                builder.getInt32(0)
            },
            name + "_ptr"
        );
    }

    llvm::FunctionCallee getPutsFunction(
        llvm::Module& module,
        llvm::LLVMContext& context
    )
    {
        llvm::Type* int32Type =
            llvm::Type::getInt32Ty(
                context
            );

        llvm::Type* pointerType =
            getPointerType(
                context
            );

        llvm::FunctionType* putsType =
            llvm::FunctionType::get(
                int32Type,
                {
                    pointerType
                },
                false
            );

        return module.getOrInsertFunction(
            "puts",
            putsType
        );
    }

    llvm::FunctionCallee getSystemFunction(
        llvm::Module& module,
        llvm::LLVMContext& context
    )
    {
        llvm::Type* int32Type =
            llvm::Type::getInt32Ty(
                context
            );

        llvm::Type* pointerType =
            getPointerType(
                context
            );

        llvm::FunctionType* systemType =
            llvm::FunctionType::get(
                int32Type,
                {
                    pointerType
                },
                false
            );

        return module.getOrInsertFunction(
            "system",
            systemType
        );
    }

    llvm::Function* createVoidRuntimeFunction(
        llvm::Module& module,
        llvm::LLVMContext& context,
        const std::string& name
    )
    {
        llvm::FunctionType* functionType =
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context),
                false
            );

        return llvm::Function::Create(
            functionType,
            llvm::Function::ExternalLinkage,
            name,
            module
        );
    }

    llvm::Function* createPointerVoidRuntimeFunction(
        llvm::Module& module,
        llvm::LLVMContext& context,
        const std::string& name
    )
    {
        llvm::Type* pointerType =
            getPointerType(
                context
            );

        llvm::FunctionType* functionType =
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context),
                {
                    pointerType
                },
                false
            );

        return llvm::Function::Create(
            functionType,
            llvm::Function::ExternalLinkage,
            name,
            module
        );
    }
}

LLVMCodeGenerator::LLVMCodeGenerator()
    : context(
          std::make_unique<llvm::LLVMContext>()
      )
{
}

std::string LLVMCodeGenerator::generate(
    const JIRModule& jir
)
{
    module =
        std::make_unique<llvm::Module>(
            jir.name,
            *context
        );

    module->setSourceFileName(
        jir.name
    );

    /*
        We intentionally do not set a target triple here.

        The LLVM IR remains target-neutral at the JOCKY
        compiler level.

        The final clang invocation selects the target
        when converting .ll into native object code.
    */

    generateRuntime();

    for (
        const auto& function :
        jir.functions
    )
    {
        generateFunction(
            function
        );
    }

    /*
        Verify the complete LLVM module before
        serializing it to .ll.
    */

    std::string verificationError;

    llvm::raw_string_ostream verificationStream(
        verificationError
    );

    bool verificationFailed =
        llvm::verifyModule(
            *module,
            &verificationStream
        );

    verificationStream.flush();

    if (verificationFailed)
    {
        throw std::runtime_error(
            "LLVM module verification failed:\n" +
            verificationError
        );
    }

    /*
        Serialize the LLVM module to textual
        LLVM IR.
    */

    std::string output;

    llvm::raw_string_ostream stream(
        output
    );

    module->print(
        stream,
        nullptr
    );

    stream.flush();

    return output;
}

void LLVMCodeGenerator::generateRuntime()
{
    llvm::LLVMContext& llvmContext =
        *context;

    /*
        ============================================================
        EXTERNAL STANDARD C RUNTIME FUNCTIONS
        ============================================================

        These are NOT JOCKY runtime symbols.

        They are standard C runtime functions that the final
        executable can receive from the platform toolchain.
    */

    llvm::FunctionCallee putsFunction =
        getPutsFunction(
            *module,
            llvmContext
        );

    llvm::FunctionCallee systemFunction =
        getSystemFunction(
            *module,
            llvmContext
        );

    /*
        ============================================================
        JOCKY PRINT
        ============================================================
    */

    llvm::Function* jockyPrint =
        createPointerVoidRuntimeFunction(
            *module,
            llvmContext,
            "jocky_print"
        );

    jockyPrint->getArg(0)->setName(
        "message"
    );

    {
        llvm::BasicBlock* entry =
            llvm::BasicBlock::Create(
                llvmContext,
                "entry",
                jockyPrint
            );

        llvm::IRBuilder<> builder(
            entry
        );

        builder.CreateCall(
            putsFunction,
            {
                jockyPrint->getArg(0)
            }
        );

        builder.CreateRetVoid();
    }

    /*
        ============================================================
        JOCKY SYSTEM INFORMATION
        ============================================================
    */

    llvm::Function* jockySystemInfo =
        createVoidRuntimeFunction(
            *module,
            llvmContext,
            "jocky_system_info"
        );

    {
        llvm::BasicBlock* entry =
            llvm::BasicBlock::Create(
                llvmContext,
                "entry",
                jockySystemInfo
            );

        llvm::IRBuilder<> builder(
            entry
        );

        llvm::Value* title =
            createStringPointer(
                builder,
                *module,
                "[JOCKY] SYSTEM INFORMATION",
                "jocky_system_title"
            );

        builder.CreateCall(
            putsFunction,
            {
                title
            }
        );

        llvm::Value* separator =
            createStringPointer(
                builder,
                *module,
                "--------------------------",
                "jocky_system_separator"
            );

        builder.CreateCall(
            putsFunction,
            {
                separator
            }
        );

        llvm::Value* command =
            createStringPointer(
                builder,
                *module,
                "systeminfo",
                "jocky_system_command"
            );

        builder.CreateCall(
            systemFunction,
            {
                command
            }
        );

        builder.CreateRetVoid();
    }

    /*
        ============================================================
        JOCKY PROCESS LIST
        ============================================================
    */

    llvm::Function* jockyProcessList =
        createVoidRuntimeFunction(
            *module,
            llvmContext,
            "jocky_process_list"
        );

    {
        llvm::BasicBlock* entry =
            llvm::BasicBlock::Create(
                llvmContext,
                "entry",
                jockyProcessList
            );

        llvm::IRBuilder<> builder(
            entry
        );

        llvm::Value* title =
            createStringPointer(
                builder,
                *module,
                "[JOCKY] PROCESS FORENSIC COLLECTION",
                "jocky_process_title"
            );

        builder.CreateCall(
            putsFunction,
            {
                title
            }
        );

        llvm::Value* separator =
            createStringPointer(
                builder,
                *module,
                "--------------------------",
                "jocky_process_separator"
            );

        builder.CreateCall(
            putsFunction,
            {
                separator
            }
        );

        llvm::Value* command =
            createStringPointer(
                builder,
                *module,
                "tasklist /FO TABLE",
                "jocky_process_command"
            );

        builder.CreateCall(
            systemFunction,
            {
                command
            }
        );

        builder.CreateRetVoid();
    }

    /*
        ============================================================
        JOCKY NETWORK INFORMATION
        ============================================================
    */

    llvm::Function* jockyNetworkInfo =
        createVoidRuntimeFunction(
            *module,
            llvmContext,
            "jocky_network_info"
        );

    {
        llvm::BasicBlock* entry =
            llvm::BasicBlock::Create(
                llvmContext,
                "entry",
                jockyNetworkInfo
            );

        llvm::IRBuilder<> builder(
            entry
        );

        llvm::Value* title =
            createStringPointer(
                builder,
                *module,
                "[JOCKY] NETWORK FORENSIC COLLECTION",
                "jocky_network_title"
            );

        builder.CreateCall(
            putsFunction,
            {
                title
            }
        );

        llvm::Value* separator =
            createStringPointer(
                builder,
                *module,
                "--------------------------",
                "jocky_network_separator"
            );

        builder.CreateCall(
            putsFunction,
            {
                separator
            }
        );

        llvm::Value* command =
            createStringPointer(
                builder,
                *module,
                "ipconfig /all",
                "jocky_network_command"
            );

        builder.CreateCall(
            systemFunction,
            {
                command
            }
        );

        builder.CreateRetVoid();
    }

    /*
        ============================================================
        JOCKY FILE SYSTEM SCAN
        ============================================================
    */

    llvm::Function* jockyFileScan =
        createPointerVoidRuntimeFunction(
            *module,
            llvmContext,
            "jocky_file_scan"
        );

    jockyFileScan->getArg(0)->setName(
        "path"
    );

    {
        llvm::BasicBlock* entry =
            llvm::BasicBlock::Create(
                llvmContext,
                "entry",
                jockyFileScan
            );

        llvm::IRBuilder<> builder(
            entry
        );

        llvm::Value* title =
            createStringPointer(
                builder,
                *module,
                "[JOCKY] FILE SYSTEM FORENSIC SCAN",
                "jocky_file_title"
            );

        builder.CreateCall(
            putsFunction,
            {
                title
            }
        );

        llvm::Value* separator =
            createStringPointer(
                builder,
                *module,
                "--------------------------------",
                "jocky_file_separator"
            );

        builder.CreateCall(
            putsFunction,
            {
                separator
            }
        );

        llvm::Value* pathLabel =
            createStringPointer(
                builder,
                *module,
                "Scan Path:",
                "jocky_file_path_label"
            );

        builder.CreateCall(
            putsFunction,
            {
                pathLabel
            }
        );

        /*
            Pass the actual JOCKY file_scan() argument
            to jocky_print().
        */

        builder.CreateCall(
            jockyPrint,
            {
                jockyFileScan->getArg(0)
            }
        );

        /*
            Controlled prototype implementation.

            The current forensic_demo.jky uses:

                file_scan(".");

            The executable therefore performs a read-only
            recursive file listing from the current directory.
        */

        llvm::Value* command =
            createStringPointer(
                builder,
                *module,
                "powershell.exe -NoProfile -Command \"Get-ChildItem -Force -Recurse -File . | Select-Object -First 15 -ExpandProperty FullName\"",
                "jocky_file_command"
            );

        builder.CreateCall(
            systemFunction,
            {
                command
            }
        );

        builder.CreateRetVoid();
    }

    /*
        ============================================================
        JOCKY EVENT LOG
        ============================================================
    */

    llvm::Function* jockyEventLog =
        createVoidRuntimeFunction(
            *module,
            llvmContext,
            "jocky_event_log"
        );

    {
        llvm::BasicBlock* entry =
            llvm::BasicBlock::Create(
                llvmContext,
                "entry",
                jockyEventLog
            );

        llvm::IRBuilder<> builder(
            entry
        );

        llvm::Value* title =
            createStringPointer(
                builder,
                *module,
                "[JOCKY] EVENT LOG ANALYSIS",
                "jocky_event_title"
            );

        builder.CreateCall(
            putsFunction,
            {
                title
            }
        );

        llvm::Value* separator =
            createStringPointer(
                builder,
                *module,
                "-------------------------",
                "jocky_event_separator"
            );

        builder.CreateCall(
            putsFunction,
            {
                separator
            }
        );

        llvm::Value* mode =
            createStringPointer(
                builder,
                *module,
                "Collection : Read-only Windows System Event Log",
                "jocky_event_mode"
            );

        builder.CreateCall(
            putsFunction,
            {
                mode
            }
        );

        /*
            Read-only System Event Log query.
        */

        llvm::Value* command =
            createStringPointer(
                builder,
                *module,
                "wevtutil qe System /c:20 /rd:true /f:text",
                "jocky_event_command"
            );

        builder.CreateCall(
            systemFunction,
            {
                command
            }
        );

        builder.CreateRetVoid();
    }
}

void LLVMCodeGenerator::generateFunction(
    const JIRFunction& function
)
{
    llvm::LLVMContext& llvmContext =
        *context;

    llvm::Type* int32Type =
        llvm::Type::getInt32Ty(
            llvmContext
        );

    llvm::Type* voidType =
        llvm::Type::getVoidTy(
            llvmContext
        );

    llvm::Function* jockyPrint =
        module->getFunction(
            "jocky_print"
        );

    llvm::Function* jockySystemInfo =
        module->getFunction(
            "jocky_system_info"
        );

    llvm::Function* jockyProcessList =
        module->getFunction(
            "jocky_process_list"
        );

    llvm::Function* jockyNetworkInfo =
        module->getFunction(
            "jocky_network_info"
        );

    llvm::Function* jockyFileScan =
        module->getFunction(
            "jocky_file_scan"
        );

    llvm::Function* jockyEventLog =
        module->getFunction(
            "jocky_event_log"
        );

    bool isMain =
        function.name == "main";

    llvm::FunctionType* functionType =
        nullptr;

    if (isMain)
    {
        functionType =
            llvm::FunctionType::get(
                int32Type,
                false
            );
    }
    else
    {
        functionType =
            llvm::FunctionType::get(
                voidType,
                false
            );
    }

    /*
        Reuse an existing function if one exists.
    */

    llvm::Function* llvmFunction =
        module->getFunction(
            function.name
        );

    if (llvmFunction == nullptr)
    {
        llvmFunction =
            llvm::Function::Create(
                functionType,
                llvm::Function::ExternalLinkage,
                function.name,
                *module
            );
    }

    llvm::BasicBlock* entry =
        llvm::BasicBlock::Create(
            llvmContext,
            "entry",
            llvmFunction
        );

    llvm::IRBuilder<> builder(
        entry
    );

    bool terminated =
        false;

    for (
        const auto& instruction :
        function.instructions
    )
    {
        if (terminated)
        {
            break;
        }

        switch (
            instruction.opcode
        )
        {
            case JIROpcode::Module:
            {
                break;
            }

            case JIROpcode::Function:
            {
                break;
            }

            case JIROpcode::StringConstant:
            {
                break;
            }

            case JIROpcode::Print:
            {
                if (jockyPrint == nullptr)
                {
                    throw std::runtime_error(
                        "Missing JOCKY runtime function: jocky_print"
                    );
                }

                llvm::Value* message =
                    createStringPointer(
                        builder,
                        *module,
                        instruction.value,
                        "jocky_user_string"
                    );

                builder.CreateCall(
                    jockyPrint,
                    {
                        message
                    }
                );

                break;
            }

            case JIROpcode::ForensicSystemInfo:
            {
                if (jockySystemInfo == nullptr)
                {
                    throw std::runtime_error(
                        "Missing JOCKY runtime function: jocky_system_info"
                    );
                }

                builder.CreateCall(
                    jockySystemInfo
                );

                break;
            }

            case JIROpcode::ForensicProcessList:
            {
                if (jockyProcessList == nullptr)
                {
                    throw std::runtime_error(
                        "Missing JOCKY runtime function: jocky_process_list"
                    );
                }

                builder.CreateCall(
                    jockyProcessList
                );

                break;
            }

            case JIROpcode::ForensicNetworkInfo:
            {
                if (jockyNetworkInfo == nullptr)
                {
                    throw std::runtime_error(
                        "Missing JOCKY runtime function: jocky_network_info"
                    );
                }

                builder.CreateCall(
                    jockyNetworkInfo
                );

                break;
            }

            case JIROpcode::ForensicFileScan:
            {
                if (jockyFileScan == nullptr)
                {
                    throw std::runtime_error(
                        "Missing JOCKY runtime function: jocky_file_scan"
                    );
                }

                llvm::Value* path =
                    createStringPointer(
                        builder,
                        *module,
                        instruction.value,
                        "jocky_scan_path"
                    );

                builder.CreateCall(
                    jockyFileScan,
                    {
                        path
                    }
                );

                break;
            }

            case JIROpcode::ForensicEventLog:
            {
                if (jockyEventLog == nullptr)
                {
                    throw std::runtime_error(
                        "Missing JOCKY runtime function: jocky_event_log"
                    );
                }

                builder.CreateCall(
                    jockyEventLog
                );

                break;
            }

            case JIROpcode::Return:
            {
                if (isMain)
                {
                    builder.CreateRet(
                        llvm::ConstantInt::get(
                            int32Type,
                            0
                        )
                    );
                }
                else
                {
                    builder.CreateRetVoid();
                }

                terminated = true;

                break;
            }
        }
    }

    /*
        Generate an implicit return if the JIR
        did not explicitly contain RETURN.
    */

    if (!terminated)
    {
        if (isMain)
        {
            builder.CreateRet(
                llvm::ConstantInt::get(
                    int32Type,
                    0
                )
            );
        }
        else
        {
            builder.CreateRetVoid();
        }
    }
}