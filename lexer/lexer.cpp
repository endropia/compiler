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

Lexer::~Lexer() {
    if (file.is_open() || file.eof()) {
        file.close();
    }
}

void Lexer::OpenFile(const std::string &filename) {
    file.open(filename);
    position = {1, 1};
}

std::vector<Lexeme> Lexer::Scan(std::string filename) {
    position.Set(1, 1);
    std::vector<Lexeme> lexemes;
    file.open(filename);

    auto lexeme = GetLexeme();
    lexemes.push_back(lexeme);
    while (lexeme.GetType() != LexemeType::eof) {
        lexeme = GetLexeme();
        lexemes.push_back(lexeme);
    }
    file.close();
    return lexemes;
}

char Lexer::Peek() {
    return (char) file.peek();
}

char Lexer::Get() {
    char c = (char) file.get();
    if (c == '\n') {
        column_after_new_line = position.GetColumn();
        position.Set(position.GetLine() + 1, 1);
    } else {
        position.Set(position.GetLine(), position.GetColumn() + 1);
    }
    // std::cout << "GET " << position.GetLine() << "\t" << position.GetColumn() << "\n";
    return c;
}

bool Lexer::UnGet() {
    auto result = (bool) file.unget();
    if (file.peek() == '\n') {
        position.Set(position.GetLine() - 1, column_after_new_line);
    } else {
        position.Set(position.GetLine(), position.GetColumn() - 1);
    }
    // std::cout << "UNGET " << position.GetLine() << "\t" << position.GetColumn() << "\n";
    return result;
}

Lexeme Lexer::GetLexeme() {
    char c = Get();

    while (c == ' ' or c == '\t' or c == '\n') {
        c = Get();
    }

    switch (c) {
        case EOF: // end of file
            return PrepareLexeme(LexemeType::eof, "", "");
        case '=':
            return PrepareLexeme(LexemeType::Operator, "Equal", "=");
        case '<':
            c = Get();
            if (c == '>') {
                return PrepareLexeme(LexemeType::Operator, "Unequal", "<>");
            } else if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, "LTEOE", "<=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, "Less", "<");
            }
        case '>':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, "MOE", ">=");
            } else if (c == '<') {
                return PrepareLexeme(LexemeType::Operator, "SD", "><");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, "More", ">");
            }
        case '+':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, "AE", "+=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, "Addition", "+");
            }
        case '-':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, "SE", "-=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, "Subtraction", "-");
            }
        case '*':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, "ME", "*=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, "Multiplication", "*");
            }
        case '/':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, "DE", "/=");
            } else if (c == '/') {
                UnGet();
                ScanSingleLineComment();
                return GetLexeme();
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, "Division", "/");
            }
        case '.':
            c = Get();
            if (c == '.') {
                return PrepareLexeme(LexemeType::Separator, "DPeriod", "..");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Separator, "Period", ".");
            }
        case ',':
            return PrepareLexeme(LexemeType::Separator, "Comma", ",");
        case ';':
            return PrepareLexeme(LexemeType::Separator, "Semicolon", ";");
        case ':':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, "Assign", ":=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, "Colon", ":");
            }
        case '(':
            c = Peek();
            if (c == '*') {
                ScanMultilineComment_();
                return GetLexeme();
            } else {
                return PrepareLexeme(LexemeType::Separator, "Lparenthesis", "(");
            }
        case ')':
            return PrepareLexeme(LexemeType::Separator, "Rparenthesis", ")");
        case '[':
            return PrepareLexeme(LexemeType::Separator, "RSbracket", "[");
        case ']':
            return PrepareLexeme(LexemeType::Separator, "LSbracket", "]");
        case '{':
            UnGet();
            ScanMultilineComment();
            return GetLexeme();
        case '@':
            return PrepareLexeme(LexemeType::Operator, "At", "@");
        case '#':
            return PrepareLexeme(LexemeType::Separator, "Lattice ", "#");
        case '^':
            return PrepareLexeme(LexemeType::Operator, "Circumflex ", "^");
        case '\'':
            return ScanString();
        case '$':
            c = Peek();
            if ('0' <= c && c <= '9' || 'A' <= c && c <= 'F' || 'a' <= c && c <= 'f') {
                UnGet();
                return ScanNumber(16);
            }
            break;
        case '&':
            c = Peek();
            if ('0' <= c && c <= '7') {
                UnGet();
                return ScanNumber(8);
            }
            break;
        case '%':
            c = Peek();
            if (c == '0' || c == '1') {
                UnGet();
                return ScanNumber(2);
            }
            break;
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
    throw std::runtime_error("Unexpected character");
    //return {position.GetLine(), position.GetColumn(), LexemeType::Invalid, value, ""};
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

    if (system != 10) {
        lex.push_back(Get());
    }

    while (cur_state != finish) {
        if (cur_state == begin) {
            if (system == 10) cur_state = decimal_number;
            if (system == 2) cur_state = binary_number;
            if (system == 8) cur_state = octa_number;
            if (system == 16) cur_state = hex_number;
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
    if (lexemeType == LexemeType::Integer) value = ToDecimal(system, lex);
    return PrepareLexeme(lexemeType, value, lex);
    //return {position.GetLine(), position.GetColumn(), lexemeType, value, lex};
}


std::string Lexer::ToDecimal(int system, const std::string &number) {
    long long int decimal = 0;
    int num;
    for (int i = (system != 10); i < number.size(); ++i) {
        if ('a' <= tolower(number[i]) && tolower(number[i]) <= 'f') {
            num = tolower(number[i]) - 'a' + 10;
        } else {
            num = number[i] - '0';
        }
        decimal = decimal * system + num;
        if (decimal > 2147483647)
            throw std::runtime_error("Overflow integer");
    }
    return std::to_string(decimal);
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
    return PrepareLexeme(type, value, lex);
    //return {position.GetLine(), position.GetColumn(), type, value, lex};
}

void Lexer::ScanSingleLineComment() {
    char c = Peek();
    while (c != '\n' && c != EOF) {
        c = Get();
    }
}

void Lexer::ScanMultilineComment_() {
    char c;
    while (true) {
        c = Get();
        if (c == EOF) {
            throw std::runtime_error("Unterminated multiline comment");
        }
        if (c == '*') {
            if (Peek() == ')') {
                Get();
                break;
            }
        }
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
        if (c == EOF || c == '\n') {
            throw std::runtime_error("Unterminated String");
        }
        c = Get();
        lex += c;
    }

    return PrepareLexeme(LexemeType::String, lex, lex);
}

Lexeme Lexer::PrepareLexeme(LexemeType type, const std::string &value, const std::string &raw) {
    if (type == LexemeType::eof) {
        return Lexeme(position.GetLine(), position.GetColumn() - 1, type, value, raw);
    }
    return Lexeme(position.GetLine(), position.GetColumn() - raw.size(), type, value, raw);
}




