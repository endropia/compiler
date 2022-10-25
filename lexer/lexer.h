#ifndef COMPILER_LEXER_HEADER
#define COMPILER_LEXER_HEADER

#include <vector>
#include <fstream>

#include "lexeme.h"

class Position{
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

    char Get();
    bool UnGet();
    char Peek();
    static std::string ToDecimal(int system, const std::string& number);
    static bool SearchKeyword(const std::string &lex);
public:
    Lexer ();
    std::vector<Lexeme> Scan(std::string filename);
    Lexeme GetLexeme();
    Lexeme ScanString();
    Lexeme ScanNumber(int system);
    Lexeme ScanIdentifier();
    void ScanSingleLineComment();
    void ScanMultilineComment();
    static std::string ToLower(std::string);
};


#endif