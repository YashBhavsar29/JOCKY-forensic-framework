#include "jir.h"

#include <stdexcept>

JIRGenerator::JIRGenerator()
{
}

JIRModule JIRGenerator::generate(
    const ASTNode* root
)
{
    if (root == nullptr)
    {
        throw std::runtime_error(
            "Cannot generate JIR from empty AST"
        );
    }

    if (root->type != ASTNodeType::Program)
    {
        throw std::runtime_error(
            "JIR generation requires Program AST root"
        );
    }

    module = JIRModule{};

    generateProgram(root);

    return module;
}

void JIRGenerator::generateProgram(
    const ASTNode* node
)
{
    for (const auto& child : node->children)
    {
        switch (child->type)
        {
            case ASTNodeType::Module:

                generateModule(
                    child.get()
                );

                break;

            case ASTNodeType::Function:

                generateFunction(
                    child.get()
                );

                break;

            default:

                throw std::runtime_error(
                    "Invalid AST node during JIR generation"
                );
        }
    }
}

void JIRGenerator::generateModule(
    const ASTNode* node
)
{
    module.name =
        node->value;
}

void JIRGenerator::generateFunction(
    const ASTNode* node
)
{
    JIRFunction function;

    function.name =
        node->value;

    for (const auto& child : node->children)
    {
        if (
            child->type ==
            ASTNodeType::CallExpression
        )
        {
            function.instructions.push_back(
                generateCall(
                    child.get()
                )
            );
        }
    }

    function.instructions.push_back(
        JIRInstruction{
            JIROpcode::Return,
            ""
        }
    );

    module.functions.push_back(
        std::move(function)
    );
}

JIRInstruction JIRGenerator::generateCall(
    const ASTNode* node
)
{
    const std::string& name =
        node->value;

    if (name == "print")
    {
        if (node->children.size() != 1)
        {
            throw std::runtime_error(
                "print requires one argument"
            );
        }

        return JIRInstruction{
            JIROpcode::Print,
            node->children[0]->value
        };
    }

    if (name == "system_info")
    {
        return JIRInstruction{
            JIROpcode::ForensicSystemInfo,
            ""
        };
    }

    if (name == "process_list")
    {
        return JIRInstruction{
            JIROpcode::ForensicProcessList,
            ""
        };
    }

    if (name == "network_info")
    {
        return JIRInstruction{
            JIROpcode::ForensicNetworkInfo,
            ""
        };
    }

    if (name == "file_scan")
    {
        if (node->children.size() != 1)
        {
            throw std::runtime_error(
                "file_scan requires one path argument"
            );
        }

        return JIRInstruction{
            JIROpcode::ForensicFileScan,
            node->children[0]->value
        };
    }

    if (name == "event_log")
    {
        return JIRInstruction{
            JIROpcode::ForensicEventLog,
            ""
        };
    }

    throw std::runtime_error(
        "Unsupported JOCKY function: " +
        name
    );
}