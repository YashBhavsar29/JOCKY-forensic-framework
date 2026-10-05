#include "parser.h"

#include <stdexcept>

Parser::Parser(const std::vector<Token> &tokens)
    : tokens(tokens),
      current(0)
{
}

const Token &Parser::peek() const
{
    return tokens[current];
}

const Token &Parser::advance()
{
    if (current < tokens.size())
    {
        current++;
    }

    return tokens[current - 1];
}

bool Parser::check(TokenType type) const
{
    return peek().type == type;
}

bool Parser::match(TokenType type)
{
    if (!check(type))
    {
        return false;
    }

    advance();
    return true;
}

void Parser::expect(
    TokenType type,
    const std::string &message)
{
    if (!check(type))
    {
        throw std::runtime_error(
            message +
            " at line " +
            std::to_string(peek().line) +
            ", column " +
            std::to_string(peek().column));
    }

    advance();
}

std::string Parser::expectIdentifier(
    const std::string &message)
{
    if (!check(TokenType::Identifier))
    {
        throw std::runtime_error(
            message +
            " at line " +
            std::to_string(peek().line) +
            ", column " +
            std::to_string(peek().column));
    }

    return advance().value;
}

std::unique_ptr<ASTNode> Parser::parse()
{
    return parseProgram();
}

std::unique_ptr<ASTNode> Parser::parseProgram()
{

    auto program = std::make_unique<ASTNode>(
        ASTNodeType::Program);

    while (!check(TokenType::EndOfFile))
    {

        if (check(TokenType::KeywordModule))
        {
            program->children.push_back(
                parseModule());
        }
        else if (check(TokenType::KeywordFn))
        {
            program->children.push_back(
                parseFunction());
        }
        else
        {
            throw std::runtime_error(
                "Unexpected token '" +
                peek().value +
                "' at line " +
                std::to_string(peek().line) +
                ", column " +
                std::to_string(peek().column));
        }
    }

    return program;
}

std::unique_ptr<ASTNode> Parser::parseModule()
{

    expect(
        TokenType::KeywordModule,
        "Expected 'module'");

    std::string moduleName =
        expectIdentifier(
            "Expected module name");

    expect(
        TokenType::Semicolon,
        "Expected ';' after module declaration");

    auto module = std::make_unique<ASTNode>(
        ASTNodeType::Module,
        moduleName);

    return module;
}

std::unique_ptr<ASTNode> Parser::parseFunction()
{

    expect(
        TokenType::KeywordFn,
        "Expected 'fn'");

    std::string functionName =
        expectIdentifier(
            "Expected function name");

    expect(
        TokenType::LeftParen,
        "Expected '(' after function name");

    expect(
        TokenType::RightParen,
        "Expected ')' after '('");

    expect(
        TokenType::LeftBrace,
        "Expected '{' before function body");

    auto function = std::make_unique<ASTNode>(
        ASTNodeType::Function,
        functionName);

    while (!check(TokenType::RightBrace))
    {

        if (check(TokenType::EndOfFile))
        {
            throw std::runtime_error(
                "Unexpected end of file inside function");
        }

        function->children.push_back(
            parseStatement());
    }

    expect(
        TokenType::RightBrace,
        "Expected '}' after function body");

    return function;
}

std::unique_ptr<ASTNode> Parser::parseStatement()
{

    if (check(TokenType::Identifier))
    {
        return parseCall();
    }

    throw std::runtime_error(
        "Unexpected token '" +
        peek().value +
        "' inside function at line " +
        std::to_string(peek().line) +
        ", column " +
        std::to_string(peek().column));
}

std::unique_ptr<ASTNode> Parser::parseCall()
{

    std::string functionName =
        expectIdentifier(
            "Expected function name");

    expect(
        TokenType::LeftParen,
        "Expected '(' after function name");

    auto call = std::make_unique<ASTNode>(
        ASTNodeType::CallExpression,
        functionName);

    if (check(TokenType::String))
    {

        std::string value = advance().value;

        call->children.push_back(
            std::make_unique<ASTNode>(
                ASTNodeType::StringLiteral,
                value));
    }

    expect(
        TokenType::RightParen,
        "Expected ')' after function arguments");

    expect(
        TokenType::Semicolon,
        "Expected ';' after function call");

    return call;
}