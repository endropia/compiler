#include <iostream>
#include "lexer.h"
#include "lexeme.h"

#include <magic_enum.hpp>
#include <cmath>

Lexer::Lexer(std::ifstream &file) : file(file) {
    position.Set(1, 1);
}

Lexer::~Lexer() {
    if (file.is_open() || file.eof()) {
        file.close();
    }
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
    return c;
}

bool Lexer::UnGet() {
    auto result = (bool) file.unget();
    if (file.peek() == '\n') {
        position.Set(position.GetLine() - 1, column_after_new_line);
    } else {
        position.Set(position.GetLine(), position.GetColumn() - 1);
    }
    return result;
}

Lexeme Lexer::GetLexeme() {
    SaveBeginPosition();
    char c = Get();

    while (c == ' ' or c == '\t' or c == '\n' or c == '\r'
           or c == '{' or c == '(' or c == '/') {
        if (c == '(') {
            if (Peek() == '*') {
                Get();
                ScanMultilineComment_();
                SaveBeginPosition();
                c = Get();
            } else {
                break;
            }
        } else if (c == '{') {
            ScanMultilineComment();
            c = Get();
        } else if (c == '/') {
            if (Peek() == '/') {
                Get();
                ScanSingleLineComment();
                SaveBeginPosition();
                c = Get();
            } else {
                break;
            }
        } else {
            SaveBeginPosition();
            c = Get();
        }
    }

    switch (c) {
        case EOF:
            return PrepareLexeme(LexemeType::eof, "", "");
        case '=':
            return PrepareLexeme(LexemeType::Operator, Operators::EQUAL, "=");
        case '<':
            c = Get();
            if (c == '>') {
                return PrepareLexeme(LexemeType::Operator, Operators::UNEQUAL, "<>");
            } else if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, Operators::LESSEQUAL, "<=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, Operators::LESS, "<");
            }
        case '>':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, Operators::GREATEREQUAL, ">=");
            } else if (c == '<') {
                return PrepareLexeme(LexemeType::Operator, Operators::SYMDIFF, "><");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, Operators::GREATER, ">");
            }
        case '+':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, Operators::ADDASSIGN, "+=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, Operators::ADD, "+");
            }
        case '-':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, Operators::SUBSTRACTASSIGN, "-=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, Operators::SUBSTRACT, "-");
            }
        case '*':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, Operators::MULTIPLYASSIGN, "*=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, Operators::MULTIPLY, "*");
            }
        case '/':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, Operators::DIVISIONASSIGN, "/=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Operator, Operators::DIVISION, "/");
            }
        case '.':
            c = Get();
            if (c == '.') {
                return PrepareLexeme(LexemeType::Separator, Separators::DOUBLEPERIOD, "..");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Separator, Separators::PERIOD, ".");
            }
        case ',':
            return PrepareLexeme(LexemeType::Separator, Separators::COMMA, ",");
        case ';':
            return PrepareLexeme(LexemeType::Separator, Separators::SEMICOLON, ";");
        case ':':
            c = Get();
            if (c == '=') {
                return PrepareLexeme(LexemeType::Operator, Operators::ASSIGN, ":=");
            } else {
                UnGet();
                return PrepareLexeme(LexemeType::Separator, Separators::COLON, ":");
            }
        case '(':
            return PrepareLexeme(LexemeType::Separator, Separators::LPARENTHESIS, "(");
        case ')':
            return PrepareLexeme(LexemeType::Separator, Separators::RPARENTHESIS, ")");
        case '[':
            return PrepareLexeme(LexemeType::Separator, Separators::LSBRACKET, "[");
        case ']':
            return PrepareLexeme(LexemeType::Separator, Separators::RSBRACKET, "]");
        case '@':
            return PrepareLexeme(LexemeType::Operator, Operators::AT, "@");
        case '^':
            return PrepareLexeme(LexemeType::Operator, Operators::CIRCUMFLEX, "^");
        case '\'':
            UnGet();
            return ScanString();
        case '#':
            UnGet();
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
    throw LexerException(position, "Unexpected character");
}

Lexeme Lexer::ScanNumber(int system) {
    enum state {
        begin,
        decimal_number,
        dot,
        decimal_number_with_dot,
        not_decimal_number_with_dot,
        e,
        decimal_number_with_e,
        binary_number,
        octal_number,
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
            if (system == 8) cur_state = octal_number;
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
            } else if (c == '.') {
                cur_state = not_decimal_number_with_dot;
                lex += c;
            } else {
                UnGet();
                cur_state = finish;
            }
        }
        if (cur_state == octal_number) {
            char c = Get();
            if ('0' <= c && c <= '7') {
                cur_state = octal_number;
                lex += c;
            } else if (c == '.') {
                cur_state = not_decimal_number_with_dot;
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
            } else if (c == '.') {
                cur_state = not_decimal_number_with_dot;
                lex += c;
            } else {
                UnGet();
                cur_state = finish;
            }
        }
        if (cur_state == dot) {
            lexemeType = Double;
            char c = Peek();
            if ('0' <= c && c <= '9') {
                cur_state = decimal_number_with_dot;
                Get();
                lex += c;
            } else if (c == 'e' or c == 'E') {
                cur_state = e;
                c = Get();
                lex += c;
            } else if (c == '.') {
                UnGet();
                lex.pop_back();
                cur_state = finish;
                lexemeType = Integer;
            } else {
                cur_state = finish;
            }
        }
        if (cur_state == not_decimal_number_with_dot) {
            lexemeType = Double;
            char c = Peek();
            if (c == 'e' or c == 'E') {
                cur_state = e;
                c = Get();
                lex += c;
            } else {
                cur_state = finish;
            }
        }
        if (cur_state == decimal_number_with_dot) {
            lexemeType = Double;
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
            if (c == '+' || c == '-') {
                lex += c;
                Get();
                c = Peek();
            }
            if ('0' <= c && c <= '9') {
                cur_state = decimal_number_with_e;
                Get();
                lex += c;
            } else {
                throw LexerException(position, "Illegal character");
            }
        }
        if (cur_state == decimal_number_with_e) {
            lexemeType = Double;
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
    if (system == 10) {
        if (lexemeType == LexemeType::Double) {
            try {
                return PrepareLexeme(lexemeType, std::stod(lex), lex);
            } catch (...) {
                return PrepareLexeme(lexemeType, INFINITY, lex);
            }
        } else {
            try {
                return PrepareLexeme(lexemeType, std::stoi(lex), lex);
            } catch (...) {
                throw LexerException(position, "Integer overflow");
            }
        }
    } else {
        if (lexemeType == LexemeType::Double) {
            auto pos_of_dot = lex.find('.');
            int integer_part;
            try {
                integer_part = std::stoi(lex.substr(1, lex.size() - pos_of_dot), nullptr, system);
            } catch (...) {
                throw LexerException(position, "Invalid integer expression");
            }
            try {
                return PrepareLexeme(lexemeType,
                                     integer_part * std::stod("1" + lex.substr(pos_of_dot + 1, lex.size() - 1)),
                                     lex);
            } catch (...) {
                return PrepareLexeme(lexemeType, INFINITY, lex);
            }
        } else {
            try {
                return PrepareLexeme(lexemeType, std::stoi(lex.substr(1, lex.size() - 1), nullptr, system), lex);
            } catch (...) {
                throw LexerException(position, "Integer overflow");
            }
        }
    }
}


Lexeme Lexer::ScanIdentifier() {
    std::string lex;
    while (true) {
        char c = Peek();
        if ('0' <= c && c <= '9' || 'A' <= c && c <= 'Z'
            || 'a' <= c && c <= 'z' || c == '_')
            c = Get();
        else break;
        lex += c;
    }

    auto keyword = magic_enum::enum_cast<AllKeywords>(ToUpper(lex));

    if (keyword.has_value()) {
        return PrepareLexeme(LexemeType::Keyword, keyword.value(), lex);
    }

    return PrepareLexeme(LexemeType::Identifier, ToLower(lex), lex);
}

void Lexer::ScanSingleLineComment() {
    char c;
    while (true) {
        c = Get();
        if (c == '\n' || c == EOF) {
            return;
        }
    }
}

void Lexer::ScanMultilineComment_() {
    char c;
    while (true) {
        c = Get();
        if (c == EOF) {
            throw LexerException(position, "Unterminated multiline comment");
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
    char c;
    while (true) {
        c = Get();
        if (c == EOF) {
            throw LexerException(position, "Unterminated multiline comment");
        }
        if (c == '}') {
            return;
        }
    }
}

std::string Lexer::ToUpper(std::string s) {
    for (char &i: s) {
        i = toupper(i);
    }
    return s;
}

std::string Lexer::ToLower(std::string s) {
    for (char &i: s) {
        i = tolower(i);
    }
    return s;
}

Lexeme Lexer::ScanString() {
    enum state {
        begin,
        quoted_string,
        before_hash,
        hash,
        unsigned_integer,
        after_hash,
        finish
    };
    char c;
    std::string num;
    std::string raw;
    std::string value;
    state cur_state = begin;
    while (cur_state != finish) {
        switch (cur_state) {
            case begin:
                c = Peek();
                if (c == '\'') {
                    c = Get();
                    cur_state = quoted_string;
                    raw += c;
                } else if (c == '#') {
                    c = Get();
                    cur_state = hash;
                    raw += c;
                }
                break;
            case quoted_string:
                c = Peek();
                if (c == '\n' or c == EOF) {
                    cur_state = finish;
                    throw LexerException(position, "Unterminated string");
                } else if (c == '\'') {
                    c = Get();
                    cur_state = before_hash;
                    raw += c;
                } else {
                    c = Get();
                    cur_state = quoted_string;
                    raw += c;
                    value += c;
                }
                break;
            case before_hash:
                c = Peek();
                if (c == '#') {
                    c = Get();
                    cur_state = hash;
                    raw += c;
                } else {
                    cur_state = finish;
                }
                break;
            case hash:
                c = Peek();
                if ('0' <= c and c <= '9') {
                    c = Get();
                    cur_state = unsigned_integer;
                    raw += c;
                    num += c;
                } else {
                    throw LexerException(position, "Unexpected character");
                }
                break;
            case unsigned_integer:
                c = Peek();
                if ('0' <= c and c <= '9') {
                    c = Get();
                    raw += c;
                    num += c;
                    if (std::stoi(num) < 256) {
                        cur_state = unsigned_integer;
                    } else {
                        throw LexerException(position, "Illegal number after #");
                    }
                } else {
                    cur_state = after_hash;
                }
                break;
            case after_hash:
                c = Peek();
                value += (char) stoi(num);
                num = "";
                if (c == '\'') {
                    c = Get();
                    raw += c;
                    cur_state = quoted_string;
                } else if (c == '#') {
                    cur_state = before_hash;
                } else {
                    cur_state = finish;
                }
                break;
        }
    }
    return PrepareLexeme(LexemeType::String, value, raw);
}

Lexeme Lexer::PrepareLexeme(LexemeType type, LexemeValue value, const std::string &raw) {
    if (type == LexemeType::eof) {
        return Lexeme(begin_position, type, value, raw);
    }
    return Lexeme(begin_position, type, value, raw);
}

void Lexer::SaveBeginPosition() {
    begin_position.Set(position);
}