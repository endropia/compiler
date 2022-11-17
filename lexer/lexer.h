#ifndef COMPILER_LEXER_HEADER
#define COMPILER_LEXER_HEADER

#include <vector>
#include <fstream>

#include "lexeme.h"


class Lexer {
    std::ifstream &file;
    Position position = {1, 1};
    Position begin_position = {1, 1};
    int column_after_new_line = 1;

    char Get();

    bool UnGet();

    char Peek();

    static std::string ToUpper(std::string);

    void SaveBeginPosition();

    Lexeme PrepareLexeme(LexemeType type, LexemeValue value, const std::string &raw);

public:
    Lexer(std::ifstream &file);

    ~Lexer();

    Lexeme GetLexeme();

    Lexeme ScanString();

    Lexeme ScanNumber(int system);

    Lexeme ScanIdentifier();

    void ScanSingleLineComment();

    void ScanMultilineComment();

    void ScanMultilineComment_();
};

class LexerException : public std::exception {
    std::string message;
public:
    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    }

    LexerException(Position pos, const std::string &message) : std::exception() {
        this->message = "(" + std::to_string(pos.GetLine()) + ", " + std::to_string(pos.GetColumn()) + ") " + message;
    }
};


#endif