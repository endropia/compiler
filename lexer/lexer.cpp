#include <iostream>
#include "lexer.h"
#include "lexeme.h"

Position::Position(int line, int column) {
    this->line = line;
    this->column = column;
}

int Position::GetLine() const { return line; }

int Position::GetColumn() const { return column; }

void Position::Set(int line, int column) {
    this->line = line;
    this->column = column;
}

Lexer::Lexer() {

}

std::vector<Lexeme> Lexer::Scan(std::string filename) {
    std::vector<Lexeme> lexemes;
    file.open(filename);

    auto lexeme = GetLexeme();
    lexemes.push_back(lexeme);
    while (lexeme.GetType() != LexemeType::eof) {
        lexeme = GetLexeme();
        lexemes.push_back(lexeme);
    }

    return lexemes;
}

char Lexer::Peek() {
    return (char) file.peek();
}

char Lexer::Get() {
    char c = (char) file.get();
    if (c == ' ' or c == '\t') {
        position.Set(position.GetLine(), position.GetColumn() + 1);
    } else if (c == '\n') {
        position.Set(position.GetLine() + 1, 1);
    }
    return c;
}

bool Lexer::UnGet() {
    auto result = (bool) file.unget();
    return result;
}

std::string Lexer::ToDecimal(int system, const std::string &number) {
    int decimal = 0;
    for (char i: number) {
        decimal = decimal * system + (int) i;
        if ('a' <= tolower(i) && tolower(i) <= 'f') {
            decimal += 10;
        }
    }
    return std::to_string(decimal);
}

Lexeme Lexer::GetLexeme() {
    char c = Get();

    while (c == ' ' or c == '\t' or c == '\n') {
        c = Get();
    }

    switch (c) {
        case EOF: // end of file
            return {position.GetLine(), position.GetColumn(), LexemeType::eof, "", ""};
        case '=':
            return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "Equal", "="};
        case '<':
            c = Get();
            if (c == '>') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "Unequal", "<>"};
            } else if (c == '=') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "LTEOE", "<="};
            } else {
                UnGet();
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "Less", "<"};
            }
        case '>':
            c = Get();
            if (c == '=') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "MOE", ">="};
            } else if (c == '<') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "SD", "><"};
            } else {
                UnGet();
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "More", ">"};
            }
        case '+':
            c = Get();
            if (c == '=') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "AE", "+="};
            } else {
                UnGet();
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "Addition", "+"};
            }
        case '-':
            c = Get();
            if (c == '=') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "SE", "-="};
            } else {
                UnGet();
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "Subtraction", "-"};
            }
        case '*':
            c = Get();
            if (c == '=') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "ME", "*="};
            } else {
                UnGet();
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "Multiplication", "*"};
            }
        case '/':
            c = Get();
            if (c == '=') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "DE", "/="};
            } else if (c == '/') {
                UnGet();
                ScanSingleLineComment();
                return GetLexeme();
            } else {
                UnGet();
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "Division", "/"};
            }
        case '.':
            c = Get();
            if (c == '.') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "DPeriod", ".."};
            } else {
                UnGet();
                return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "Period", "."};
            }
        case ',':
            return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "Comma", ","};
        case ';':
            return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "Semicolon", ";"};
        case ':':
            c = Get();
            if (c == '=') {
                return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "DPeriod", ".."};
            } else {
                UnGet();
                return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "Assign", ":="};
            }
        case '(':
            return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "Lparenthesis", "("};
        case ')':
            return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "Rparenthesis", ")"};
        case '[':
            return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "RSbracket", "["};
        case ']':
            return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "LSbracket", "]"};
        case '{':
            UnGet();
            ScanMultilineComment();
            return GetLexeme();
        case '@':
            return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "At", "@"};
        case '#':
            return {position.GetLine(), position.GetColumn(), LexemeType::Separator, "Lattice ", "#"};
        case '^':
            return {position.GetLine(), position.GetColumn(), LexemeType::Operator, "Circumflex ", "^"};
        case '\'':
            return ScanString();
        case '$':
            return ScanNumber(16);
        case '&':
            return ScanNumber(8);
        case '%':
            return ScanNumber(2);
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            UnGet(); // return character to scan number
            return ScanNumber(10);
        default:
            if ('A' <= c && c <= 'Z'
                || 'a' <= c && c <= 'z' || c == '_') {
                UnGet();
                return ScanIdentifier();
            }
    }

    std::string value;
    value.clear();
    value.push_back(c);
    return {position.GetLine(), position.GetColumn(), LexemeType::Invalid, value, ""};
}

Lexeme Lexer::ScanNumber(int system) {
    enum state {
        begin,
        decimal_number,
        dot,
        decimal_number_with_dot,
        e,
        decimal_number_with_e,
        binary_number,
        octa_number,
        hex_number,
        finish
    };
    state cur_state = begin;
    std::string lex;
    LexemeType lexemeType = Integer;

    while (cur_state != finish) {
        if (cur_state == begin) {
            if (system == 10) cur_state = decimal_number;
            if (system == 2) cur_state = binary_number;
            if (system == 8) cur_state = octa_number;
            if (system == 16) cur_state = hex_number;
            //           continue;
        }
        if (cur_state == decimal_number) {
            char c = Get();
            if ('0' <= c && c <= '9') {
                cur_state = decimal_number;
                lex += c;
            } else if (c == 'e' || c == 'E') {
                lex += c;
                cur_state = e;
            } else if (c == '.') {
                lex += c;
                cur_state = dot;
            } else {
                UnGet();
                cur_state = finish;
            }
        }
        if (cur_state == binary_number) {
            char c = Get();
            if ('0' == c || c == '1') {
                cur_state = binary_number;
                lex += c;
            } else {
                UnGet();
                cur_state = finish;
            }
        }
        if (cur_state == octa_number) {
            char c = Get();
            if ('0' <= c && c <= '7') {
                cur_state = octa_number;
                lex += c;
            } else {
                UnGet();
                cur_state = finish;
            }
        }
        if (cur_state == hex_number) {
            char c = Get();
            if ('0' <= c && c <= '9' || 'A' <= c && c <= 'F' || 'a' <= c && c <= 'f') {
                cur_state = hex_number;
                lex += c;
            } else {
                UnGet();
                cur_state = finish;
            }
        }
        if (cur_state == dot) {
            char c = Peek();
            if ('0' <= c && c <= '9') {
                cur_state = decimal_number_with_dot;
                Get();
                lex += c;
            } else {
                lex.pop_back();
                UnGet();
                cur_state = finish;
            }
        }
        if (cur_state == decimal_number_with_dot) {
            lexemeType = Real;
            char c = Get();
            if ('0' <= c && c <= '9') {
                cur_state = decimal_number_with_dot;
                lex += c;
            } else if (c == 'e' || c == 'E') {
                cur_state = e;
                lex += c;
            } else {
                UnGet();
                cur_state = finish;
            }
        }
        if (cur_state == e) {
            char c = Peek();
            if ('0' <= c && c <= '9') {
                cur_state = decimal_number_with_e;
                Get();
                lex += c;
            } else {
                UnGet();
                lex.pop_back();
                cur_state = finish;
            }
        }
        if (cur_state == decimal_number_with_e) {
            lexemeType = Real;
            char c = Get();
            if ('0' <= c && c <= '9') {
                cur_state = decimal_number_with_e;
                lex += c;
            } else {
                UnGet();
                cur_state = finish;
            }
        }
    }
    std::string value = lex;
    if (system != 10) value = ToDecimal(system, lex);
    return {position.GetLine(), position.GetColumn(), lexemeType, value, lex};
}

Lexeme Lexer::ScanIdentifier() {
    char c = Peek();
    std::string lex;
    while (true) {
        c = Peek();
        if ('0' <= c && c <= '9' || 'A' <= c && c <= 'Z'
            || 'a' <= c && c <= 'z' || c == '_')
            c = Get();
        else break;
        lex += c;
    }
    LexemeType type = Identifier;
    std::string value = lex;
    if (SearchKeyword(lex)) {
        type = Keyword;
        value = ToLower(value);
    }
    return {position.GetLine(), position.GetColumn(), type, value, lex};
}

void Lexer::ScanSingleLineComment() {
    char c = Peek();
    while (c != '\n' && c != EOF) {
        c = Get();
    }
}

void Lexer::ScanMultilineComment() {
    char c = Peek();
    while (c != '}') {
        if (c == EOF) {
            throw std::runtime_error("Unterminated multiline comment");
        }
        c = Get();
    }
}

std::string Lexer::ToLower(std::string s) {
    for (char &i: s) {
        i = tolower(i);
    }
    return s;
}

bool Lexer::SearchKeyword(const std::string &lex) {
    for (int i = 0; i <= allKeywords.size(); i++) {
        if (allKeywords[i] == lex || allKeywords[i] == Lexer::ToLower(lex)) {
            return true;
        }
    }
    return false;
}

Lexeme Lexer::ScanString() {
    char c = Peek();
    std::string lex;
    lex.push_back('\'');

    while (c != '\'') {
        if (c == EOF || c =='\n') {
            throw std::runtime_error("Unterminated string");
        }
        c = Get();
        lex += c;
    }

    return Lexeme(position.GetLine(), position.GetColumn(), LexemeType::String, lex, lex);
}



