#pragma once

#include "../parser/ast.h"

#include <string>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();

    void analyze(const ASTNode* root);

private:
    bool hasModule;
    bool hasMainFunction;

    void analyzeProgram(const ASTNode* node);
    void analyzeModule(const ASTNode* node);
    void analyzeFunction(const ASTNode* node);
    void analyzeCall(const ASTNode* node);

    [[noreturn]] void error(const std::string& message);
};