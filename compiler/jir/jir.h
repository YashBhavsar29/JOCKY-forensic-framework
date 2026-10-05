#pragma once

#include "../parser/ast.h"

#include <string>
#include <vector>

enum class JIROpcode
{
    Module,
    Function,
    StringConstant,

    Print,

    ForensicSystemInfo,
    ForensicProcessList,
    ForensicNetworkInfo,
    ForensicFileScan,
    ForensicEventLog,

    Return
};

struct JIRInstruction
{
    JIROpcode opcode;
    std::string value;
};

struct JIRFunction
{
    std::string name;
    std::vector<JIRInstruction> instructions;
};

struct JIRModule
{
    std::string name;
    std::vector<JIRFunction> functions;
};

class JIRGenerator
{
public:
    JIRGenerator();

    JIRModule generate(
        const ASTNode *root);

private:
    JIRModule module;

    void generateProgram(
        const ASTNode *node);

    void generateModule(
        const ASTNode *node);

    void generateFunction(
        const ASTNode *node);

    JIRInstruction generateCall(
        const ASTNode *node);
};