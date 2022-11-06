#ifndef COMPILER_LEXER_HEADER
#define COMPILER_LEXER_HEADER

#include <vector>
#include <fstream>

#include "lexeme.h"

class Position {
    int line;
    int column;

public:
    Position(int line, int column);

    int GetLine() const;

    int GetColumn() const;

    void Set(int line, int column);
};

class Lexer {
    std::ifstream file;
    Position position = {1, 1};
    int column_after_new_line = 1;

    char Get();

    bool UnGet();

    char Peek();

    static std::string ToDecimal(int system, const std::string &number);

    static bool SearchKeyword(const std::string &lex);

    static std::string ToUpper(std::string);

    Lexeme PrepareLexeme(LexemeType type, LexemeValue value, const std::string &raw);

public:
    Lexer();

    ~Lexer();

    std::vector<Lexeme> Scan(std::string filename);

    void OpenFile(const std::string &filename);

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
    [[nodiscard]] char *what() const noexcept override {
        return const_cast<char *>(message.c_str());
    }

    LexerException(Position pos, const std::string &message) : std::exception() {
        this->message = "(" + std::to_string(pos.GetLine()) + ", " + std::to_string(pos.GetColumn()) + ") " + message;
    }
};


#endif