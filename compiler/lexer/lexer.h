#pragma once

#include <string>
#include <vector>

enum class TokenType {
    Identifier,
    String,

    KeywordModule,
    KeywordFn,

    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,

    Semicolon,

    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    explicit Lexer(const std::string& source);

    std::vector<Token> tokenize();

private:
    std::string source;
    size_t position;
    int line;
    int column;

    char currentChar() const;
    void advance();

    void skipWhitespace();

    Token identifierOrKeyword();
    Token stringLiteral();

    Token makeToken(
        TokenType type,
        const std::string& value,
        int startLine,
        int startColumn
    );
};