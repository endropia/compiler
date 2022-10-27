#include <sstream>
#include <utility>
#include "lexeme.h"

std::string to_string(LexemeType type)
{
    switch(type) {
        case eof:
            return "eof";
        case Invalid:
            return "Invalid";
        case Identifier:
            return "Identifier";
        case Integer:
            return "Integer";
        case Real:
            return "Real";
        case Keyword:
            return "Keyword";
        case Operator:
            return "Operator";
        case Separator:
            return "Separator";
        case String:
            return "String";
    }
    return "";
}

Lexeme::Lexeme(int line, int column, LexemeType type,
               const std::string& value, const std::string& rawLexeme) {
    this->line = line;
    this->column = column;
    this->type = type;
    this->value = value;
    this->rawLexeme = rawLexeme;
}

LexemeType Lexeme::GetType() {
    return type;
}

std::ostream &operator<<(std::ostream &os, const Lexeme &lexeme) {
    if (lexeme.type == LexemeType::eof) {
        os << lexeme.line << "\t"
           << lexeme.column << "\t"
           << to_string(lexeme.type);
        return os;
    }
    os << lexeme.line << "\t"
       << lexeme.column << "\t"
       << to_string(lexeme.type) << "\t"
       << lexeme.value << "\t"
       << lexeme.rawLexeme;
    return os;
}

std::string Lexeme::String() {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}
