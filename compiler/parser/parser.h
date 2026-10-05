#pragma once

#include "../lexer/lexer.h"
#include "ast.h"

#include <vector>
#include <memory>
#include <string>

class Parser
{
public:
    explicit Parser(const std::vector<Token> &tokens);

    std::unique_ptr<ASTNode> parse();

private:
    const std::vector<Token> &tokens;
    size_t current;

    const Token &peek() const;
    const Token &advance();
    bool check(TokenType type) const;
    bool match(TokenType type);

    std::unique_ptr<ASTNode> parseProgram();
    std::unique_ptr<ASTNode> parseModule();
    std::unique_ptr<ASTNode> parseFunction();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseCall();

    std::string expectIdentifier(const std::string &message);
    void expect(TokenType type, const std::string &message);
};