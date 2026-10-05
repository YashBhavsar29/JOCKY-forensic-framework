#include "lexer.h"

#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& source)
    : source(source),
      position(0),
      line(1),
      column(1) {
}

char Lexer::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }

    return source[position];
}

void Lexer::advance() {
    if (position >= source.length()) {
        return;
    }

    if (source[position] == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }

    position++;
}

void Lexer::skipWhitespace() {
    while (std::isspace(static_cast<unsigned char>(currentChar()))) {
        advance();
    }
}

Token Lexer::makeToken(
    TokenType type,
    const std::string& value,
    int startLine,
    int startColumn
) {
    return Token{
        type,
        value,
        startLine,
        startColumn
    };
}

Token Lexer::identifierOrKeyword() {
    int startLine = line;
    int startColumn = column;

    std::string value;

    while (
        std::isalnum(static_cast<unsigned char>(currentChar())) ||
        currentChar() == '_'
    ) {
        value += currentChar();
        advance();
    }

    if (value == "module") {
        return makeToken(
            TokenType::KeywordModule,
            value,
            startLine,
            startColumn
        );
    }

    if (value == "fn") {
        return makeToken(
            TokenType::KeywordFn,
            value,
            startLine,
            startColumn
        );
    }

    return makeToken(
        TokenType::Identifier,
        value,
        startLine,
        startColumn
    );
}

Token Lexer::stringLiteral() {
    int startLine = line;
    int startColumn = column;

    advance(); // Skip opening "

    std::string value;

    while (currentChar() != '"' && currentChar() != '\0') {
        value += currentChar();
        advance();
    }

    if (currentChar() == '\0') {
        throw std::runtime_error(
            "Unterminated string literal"
        );
    }

    advance(); // Skip closing "

    return makeToken(
        TokenType::String,
        value,
        startLine,
        startColumn
    );
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (currentChar() != '\0') {

        skipWhitespace();

        if (currentChar() == '\0') {
            break;
        }

        int startLine = line;
        int startColumn = column;

        char c = currentChar();

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(identifierOrKeyword());
            continue;
        }

        if (c == '"') {
            tokens.push_back(stringLiteral());
            continue;
        }

        switch (c) {

            case '(':
                tokens.push_back(
                    makeToken(
                        TokenType::LeftParen,
                        "(",
                        startLine,
                        startColumn
                    )
                );
                advance();
                break;

            case ')':
                tokens.push_back(
                    makeToken(
                        TokenType::RightParen,
                        ")",
                        startLine,
                        startColumn
                    )
                );
                advance();
                break;

            case '{':
                tokens.push_back(
                    makeToken(
                        TokenType::LeftBrace,
                        "{",
                        startLine,
                        startColumn
                    )
                );
                advance();
                break;

            case '}':
                tokens.push_back(
                    makeToken(
                        TokenType::RightBrace,
                        "}",
                        startLine,
                        startColumn
                    )
                );
                advance();
                break;

            case ';':
                tokens.push_back(
                    makeToken(
                        TokenType::Semicolon,
                        ";",
                        startLine,
                        startColumn
                    )
                );
                advance();
                break;

            default:
                tokens.push_back(
                    makeToken(
                        TokenType::Unknown,
                        std::string(1, c),
                        startLine,
                        startColumn
                    )
                );
                advance();
                break;
        }
    }

    tokens.push_back(
        makeToken(
            TokenType::EndOfFile,
            "",
            line,
            column
        )
    );

    return tokens;
}