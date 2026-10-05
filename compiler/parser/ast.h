#pragma once

#include <memory>
#include <string>
#include <vector>

enum class ASTNodeType
{
    Program,
    Module,
    Function,
    CallExpression,
    StringLiteral
};

struct ASTNode
{
    ASTNodeType type;

    std::string value;

    std::vector<std::unique_ptr<ASTNode>> children;

    explicit ASTNode(ASTNodeType type)
        : type(type)
    {
    }

    ASTNode(ASTNodeType type, const std::string &value)
        : type(type),
          value(value)
    {
    }
};