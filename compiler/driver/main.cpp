#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../semantic/semantic.h"
#include "../jir/jir.h"
#include "../llvm/llvm_codegen.h"
#include "../../runtime/jocky_runtime.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::string readFile(
    const std::string& filename
)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Could not open source file: " + filename
        );
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}

std::string replaceExtension(
    const std::string& filename,
    const std::string& newExtension
)
{
    std::string output = filename;

    size_t extensionPosition =
        output.find_last_of('.');

    if (
        extensionPosition !=
        std::string::npos
    )
    {
        output =
            output.substr(
                0,
                extensionPosition
            );
    }

    output += newExtension;

    return output;
}

std::string tokenTypeToString(
    TokenType type
)
{
    switch (type)
    {
        case TokenType::Identifier:
            return "Identifier";

        case TokenType::String:
            return "String";

        case TokenType::KeywordModule:
            return "KeywordModule";

        case TokenType::KeywordFn:
            return "KeywordFn";

        case TokenType::LeftParen:
            return "LeftParen";

        case TokenType::RightParen:
            return "RightParen";

        case TokenType::LeftBrace:
            return "LeftBrace";

        case TokenType::RightBrace:
            return "RightBrace";

        case TokenType::Semicolon:
            return "Semicolon";

        case TokenType::EndOfFile:
            return "EndOfFile";

        case TokenType::Unknown:
            return "Unknown";
    }

    return "Unknown";
}

void printSection(
    const std::string& title
)
{
    std::cout << "\n";
    std::cout << title << "\n";

    std::cout
        << std::string(
               title.length(),
               '-'
           )
        << "\n";
}

void printLexerOutput(
    const std::vector<Token>& tokens
)
{
    printSection("Lexer");

    std::cout
        << "[OK] Lexical analysis completed\n";

    std::cout
        << "     Tokens generated: "
        << tokens.size()
        << "\n\n";

    std::cout
        << "     "
        << "ID"
        << "   "
        << "Type"
        << "                       "
        << "Value"
        << "            "
        << "Location"
        << "\n";

    std::cout
        << "     "
        << "---------------------------------------------------------------"
        << "\n";

    for (
        size_t index = 0;
        index < tokens.size();
        ++index
    )
    {
        const Token& token =
            tokens[index];

        std::string value =
            token.value;

        if (
            token.type ==
            TokenType::String
        )
        {
            value =
                "\"" +
                value +
                "\"";
        }

        std::cout
            << "     ";

        std::cout.width(3);
        std::cout
            << index;

        std::cout
            << "   ";

        std::cout.width(25);
        std::cout
            << std::left
            << tokenTypeToString(
                   token.type
               )
            << std::right;

        std::cout
            << " ";

        std::cout.width(32);

        std::cout
            << std::left
            << value
            << std::right;

        std::cout
            << " "
            << token.line
            << ":"
            << token.column
            << "\n";
    }
}

std::string astNodeTypeToString(
    ASTNodeType type
)
{
    switch (type)
    {
        case ASTNodeType::Program:
            return "Program";

        case ASTNodeType::Module:
            return "Module";

        case ASTNodeType::Function:
            return "Function";

        case ASTNodeType::CallExpression:
            return "CallExpression";

        case ASTNodeType::StringLiteral:
            return "StringLiteral";
    }

    return "Unknown";
}

void printAST(
    const ASTNode* node,
    int depth = 0
)
{
    if (node == nullptr)
    {
        return;
    }

    for (
        int index = 0;
        index < depth;
        ++index
    )
    {
        std::cout << "  ";
    }

    std::cout
        << astNodeTypeToString(
               node->type
           );

    if (!node->value.empty())
    {
        std::cout
            << ": "
            << node->value;
    }

    std::cout << "\n";

    for (
        const auto& child :
        node->children
    )
    {
        printAST(
            child.get(),
            depth + 1
        );
    }
}

void printParserOutput(
    const ASTNode* ast
)
{
    printSection("Parser");

    std::cout
        << "[OK] Parsing completed successfully\n\n";

    size_t callCount = 0;

    if (
        ast != nullptr &&
        ast->type == ASTNodeType::Program
    )
    {
        for (
            const auto& child :
            ast->children
        )
        {
            if (
                child->type ==
                ASTNodeType::Module
            )
            {
                std::cout
                    << "     --> Module: "
                    << child->value
                    << "\n";
            }

            else if (
                child->type ==
                ASTNodeType::Function
            )
            {
                std::cout
                    << "     --> Function: "
                    << child->value
                    << "\n";

                for (
                    const auto& functionChild :
                    child->children
                )
                {
                    if (
                        functionChild->type ==
                        ASTNodeType::CallExpression
                    )
                    {
                        callCount++;
                    }
                }
            }
        }
    }

    std::cout
        << "     --> Calls parsed: "
        << callCount
        << "\n";
}

void printSemanticOutput(
    const ASTNode* ast
)
{
    printSection(
        "Semantic Analysis"
    );

    std::cout
        << "[OK] Semantic analysis passed\n\n";

    std::cout
        << "     Validated operations:\n";

    if (
        ast != nullptr &&
        ast->type == ASTNodeType::Program
    )
    {
        for (
            const auto& child :
            ast->children
        )
        {
            if (
                child->type !=
                ASTNodeType::Function
            )
            {
                continue;
            }

            for (
                const auto& functionChild :
                child->children
            )
            {
                if (
                    functionChild->type !=
                    ASTNodeType::CallExpression
                )
                {
                    continue;
                }

                std::cout
                    << "       --> "
                    << functionChild->value
                    << "\n";
            }
        }
    }
}

void printJIR(
    const JIRModule& module
)
{
    printSection("JIR");

    std::cout
        << "MODULE "
        << module.name
        << "\n\n";

    for (
        const auto& function :
        module.functions
    )
    {
        std::cout
            << "FUNCTION "
            << function.name
            << "\n";

        for (
            const auto& instruction :
            function.instructions
        )
        {
            switch (
                instruction.opcode
            )
            {
                case JIROpcode::Module:
                    std::cout
                        << "    MODULE "
                        << instruction.value
                        << "\n";
                    break;

                case JIROpcode::Function:
                    std::cout
                        << "    FUNCTION "
                        << instruction.value
                        << "\n";
                    break;

                case JIROpcode::StringConstant:
                    std::cout
                        << "    STRING \""
                        << instruction.value
                        << "\"\n";
                    break;

                case JIROpcode::Print:
                    std::cout
                        << "    PRINT \""
                        << instruction.value
                        << "\"\n";
                    break;

                case JIROpcode::ForensicSystemInfo:
                    std::cout
                        << "    FORENSIC_SYSTEM_INFO\n";
                    break;

                case JIROpcode::ForensicProcessList:
                    std::cout
                        << "    FORENSIC_PROCESS_LIST\n";
                    break;

                case JIROpcode::ForensicNetworkInfo:
                    std::cout
                        << "    FORENSIC_NETWORK_INFO\n";
                    break;

                case JIROpcode::ForensicFileScan:
                    std::cout
                        << "    FORENSIC_FILE_SCAN \""
                        << instruction.value
                        << "\"\n";
                    break;

                case JIROpcode::ForensicEventLog:
                    std::cout
                        << "    FORENSIC_EVENT_LOG\n";
                    break;

                case JIROpcode::Return:
                    std::cout
                        << "    RETURN\n";
                    break;
            }
        }

        std::cout
            << "END_FUNCTION\n";
    }
}

void writeJIRFile(
    const JIRModule& module,
    const std::string& filename
)
{
    std::ofstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Could not create JIR file: " +
            filename
        );
    }

    file
        << "; JOCKY Intermediate Representation\n";

    file
        << "; Generated from JOCKY source\n\n";

    file
        << "MODULE "
        << module.name
        << "\n\n";

    for (
        const auto& function :
        module.functions
    )
    {
        file
            << "FUNCTION "
            << function.name
            << "\n";

        for (
            const auto& instruction :
            function.instructions
        )
        {
            switch (
                instruction.opcode
            )
            {
                case JIROpcode::Module:
                    file
                        << "    MODULE "
                        << instruction.value
                        << "\n";
                    break;

                case JIROpcode::Function:
                    file
                        << "    FUNCTION "
                        << instruction.value
                        << "\n";
                    break;

                case JIROpcode::StringConstant:
                    file
                        << "    STRING \""
                        << instruction.value
                        << "\"\n";
                    break;

                case JIROpcode::Print:
                    file
                        << "    PRINT \""
                        << instruction.value
                        << "\"\n";
                    break;

                case JIROpcode::ForensicSystemInfo:
                    file
                        << "    FORENSIC_SYSTEM_INFO\n";
                    break;

                case JIROpcode::ForensicProcessList:
                    file
                        << "    FORENSIC_PROCESS_LIST\n";
                    break;

                case JIROpcode::ForensicNetworkInfo:
                    file
                        << "    FORENSIC_NETWORK_INFO\n";
                    break;

                case JIROpcode::ForensicFileScan:
                    file
                        << "    FORENSIC_FILE_SCAN \""
                        << instruction.value
                        << "\"\n";
                    break;

                case JIROpcode::ForensicEventLog:
                    file
                        << "    FORENSIC_EVENT_LOG\n";
                    break;

                case JIROpcode::Return:
                    file
                        << "    RETURN\n";
                    break;
            }
        }

        file
            << "END_FUNCTION\n\n";
    }

    file.close();
}

void writeLLVMFile(
    const std::string& llvmIR,
    const std::string& filename
)
{
    std::ofstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Could not create LLVM IR file: " +
            filename
        );
    }

    file
        << llvmIR;

    file.close();
}

void executeForensicRuntime(
    const JIRModule& jir
)
{
    printSection(
        "Direct Execution"
    );

    std::cout
        << "[OK] Executing JOCKY program\n";

    std::cout
        << "     Mode: Direct runtime execution\n";

    std::cout
        << "     Executable generation: Not performed\n";

    std::cout << "\n";

    for (
        const auto& function :
        jir.functions
    )
    {
        for (
            const auto& instruction :
            function.instructions
        )
        {
            switch (
                instruction.opcode
            )
            {
                case JIROpcode::Print:
                    jocky_print(
                        instruction.value.c_str()
                    );
                    break;

                case JIROpcode::ForensicSystemInfo:
                    jocky_system_info();
                    break;

                case JIROpcode::ForensicProcessList:
                    jocky_process_list();
                    break;

                case JIROpcode::ForensicNetworkInfo:
                    jocky_network_info();
                    break;

                case JIROpcode::ForensicFileScan:
                    jocky_file_scan(
                        instruction.value.c_str()
                    );
                    break;

                case JIROpcode::ForensicEventLog:
                    jocky_event_log();
                    break;

                case JIROpcode::Return:
                    return;

                case JIROpcode::Module:
                case JIROpcode::Function:
                case JIROpcode::StringConstant:
                    break;
            }
        }
    }
}

int main(
    int argc,
    char* argv[]
)
{
    if (argc < 2)
    {
        std::cerr
            << "JOCKY Error: No source file specified.\n\n";

        std::cerr
            << "Usage:\n";

        std::cerr
            << "  jocky <file.jky>\n";

        std::cerr
            << "  jocky <file.jky> --export-ir\n";

        return 1;
    }

    std::string filename;

    bool exportIR = false;

    for (
        int index = 1;
        index < argc;
        ++index
    )
    {
        std::string argument =
            argv[index];

        if (
            argument ==
            "--export-ir"
        )
        {
            exportIR = true;
        }
        else if (
            filename.empty()
        )
        {
            filename = argument;
        }
        else
        {
            std::cerr
                << "JOCKY Error: Unknown argument: "
                << argument
                << "\n";

            return 1;
        }
    }

    if (filename.empty())
    {
        std::cerr
            << "JOCKY Error: No source file specified.\n";

        return 1;
    }

    try
    {
        std::cout
            << "\n";

        std::cout
            << "JOCKY Compiler\n";

        std::cout
            << "==============\n";

        std::cout
            << "Source: "
            << filename
            << "\n";

        /*
            ========================================================
            LEXER
            ========================================================
        */

        std::string source =
            readFile(filename);

        Lexer lexer(source);

        std::vector<Token> tokens =
            lexer.tokenize();

        printLexerOutput(
            tokens
        );

        /*
            ========================================================
            PARSER
            ========================================================
        */

        Parser parser(tokens);

        std::unique_ptr<ASTNode> ast =
            parser.parse();

        printParserOutput(
            ast.get()
        );

        /*
            ========================================================
            AST
            ========================================================
        */

        printSection("AST");

        printAST(
            ast.get()
        );

        /*
            ========================================================
            SEMANTIC ANALYSIS
            ========================================================
        */

        SemanticAnalyzer semanticAnalyzer;

        semanticAnalyzer.analyze(
            ast.get()
        );

        printSemanticOutput(
            ast.get()
        );

        /*
            ========================================================
            JIR GENERATION
            ========================================================
        */

        JIRGenerator jirGenerator;

        JIRModule jir =
            jirGenerator.generate(
                ast.get()
            );

        printJIR(
            jir
        );

        /*
            ========================================================
            OPTIONAL JIR EXPORT
            ========================================================
        */

        std::string jirFilename =
            replaceExtension(
                filename,
                ".jir"
            );

        if (exportIR)
        {
            writeJIRFile(
                jir,
                jirFilename
            );

            std::cout
                << "\n[OK] JIR file exported:\n"
                << "     "
                << jirFilename
                << "\n";
        }

        /*
            ========================================================
            LLVM CODE GENERATION
            ========================================================
        */

        printSection(
            "LLVM Code Generation"
        );

        LLVMCodeGenerator llvmCodeGenerator;

        std::string llvmIR =
            llvmCodeGenerator.generate(
                jir
            );

        std::cout
            << "[OK] LLVM IR generated successfully\n";

        /*
            ========================================================
            OPTIONAL LLVM EXPORT
            ========================================================
        */

        std::string llvmFilename =
            replaceExtension(
                filename,
                ".ll"
            );

        if (exportIR)
        {
            writeLLVMFile(
                llvmIR,
                llvmFilename
            );

            std::cout
                << "[OK] LLVM IR file exported:\n"
                << "     "
                << llvmFilename
                << "\n";
        }

        /*
            ========================================================
            DIRECT EXECUTION
            ========================================================

            The compiler directly executes the JIR through
            the JOCKY runtime.

            No .exe is created in this mode.
        */

        executeForensicRuntime(
            jir
        );

        /*
            ========================================================
            FINAL STATUS
            ========================================================
        */

        std::cout
            << "\n";

        std::cout
            << "============================================\n";

        std::cout
            << "FORENSIC INVESTIGATION COMPLETE\n";

        std::cout
            << "============================================\n";

        return 0;
    }
    catch (
        const std::exception& error
    )
    {
        std::cerr
            << "\n";

        std::cerr
            << "JOCKY Compilation Error\n";

        std::cerr
            << "-----------------------\n";

        std::cerr
            << error.what()
            << "\n";

        return 1;
    }
}