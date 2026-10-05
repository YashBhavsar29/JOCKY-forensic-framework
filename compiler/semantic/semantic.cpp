#include "semantic.h"

#include <stdexcept>

SemanticAnalyzer::SemanticAnalyzer()
    : hasModule(false),
      hasMainFunction(false)
{
}

[[noreturn]] void SemanticAnalyzer::error(
    const std::string &message)
{
    throw std::runtime_error(
        "Semantic Error: " +
        message);
}

void SemanticAnalyzer::analyze(
    const ASTNode *root)
{
    if (root == nullptr)
    {
        error("AST is empty");
    }

    if (root->type != ASTNodeType::Program)
    {
        error(
            "Root node must be a Program");
    }

    analyzeProgram(root);

    if (!hasModule)
    {
        error(
            "Program must contain a module declaration");
    }

    if (!hasMainFunction)
    {
        error(
            "Program must contain a main function");
    }
}

void SemanticAnalyzer::analyzeProgram(
    const ASTNode *node)
{
    for (const auto &child : node->children)
    {
        switch (child->type)
        {
        case ASTNodeType::Module:

            analyzeModule(
                child.get());

            break;

        case ASTNodeType::Function:

            analyzeFunction(
                child.get());

            break;

        default:

            error(
                "Invalid top-level construct");
        }
    }
}

void SemanticAnalyzer::analyzeModule(
    const ASTNode *node)
{
    if (hasModule)
    {
        error(
            "Multiple module declarations are not allowed");
    }

    if (node->value.empty())
    {
        error(
            "Module name cannot be empty");
    }

    hasModule = true;
}

void SemanticAnalyzer::analyzeFunction(
    const ASTNode *node)
{
    if (node->value.empty())
    {
        error(
            "Function name cannot be empty");
    }

    if (node->value == "main")
    {
        if (hasMainFunction)
        {
            error(
                "Multiple main functions are not allowed");
        }

        hasMainFunction = true;
    }

    for (const auto &child : node->children)
    {
        if (
            child->type ==
            ASTNodeType::CallExpression)
        {
            analyzeCall(
                child.get());
        }
        else
        {
            error(
                "Invalid statement inside function '" +
                node->value +
                "'");
        }
    }
}

void SemanticAnalyzer::analyzeCall(
    const ASTNode *node)
{
    const std::string &name =
        node->value;

    if (name == "print")
    {
        if (node->children.size() != 1)
        {
            error(
                "print expects exactly one argument");
        }

        if (
            node->children[0]->type !=
            ASTNodeType::StringLiteral)
        {
            error(
                "print expects a string argument");
        }

        return;
    }

    if (
        name == "system_info" ||
        name == "process_list" ||
        name == "network_info" ||
        name == "event_log")
    {
        if (!node->children.empty())
        {
            error(
                name +
                " does not accept arguments");
        }

        return;
    }

    if (name == "file_scan")
    {
        if (node->children.size() != 1)
        {
            error(
                "file_scan expects exactly one path argument");
        }

        if (
            node->children[0]->type !=
            ASTNodeType::StringLiteral)
        {
            error(
                "file_scan expects a string path");
        }

        return;
    }

    error(
        "Unknown JOCKY function '" +
        name +
        "'");
}