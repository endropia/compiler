#include <sstream>
#include <utility>
#include "lexeme.h"

#include <magic_enum.hpp>


Lexeme::Lexeme(int line, int column, LexemeType type,
               LexemeValue value, const std::string &rawLexeme) {
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
           << magic_enum::enum_name(lexeme.type);
        return os;
    }
    os << lexeme.line << "\t"
       << lexeme.column << "\t"
       << magic_enum::enum_name(lexeme.type) << "\t";

    switch (lexeme.type) {
        case Integer:
            os << std::get<int>(lexeme.value);
            break;
        case Double:
            os << std::get<double>(lexeme.value);
            break;
        case String:
        case Identifier:
            os << std::get<std::string>(lexeme.value);
            break;
        case Keyword:
            os << magic_enum::enum_name(std::get<AllKeywords>(lexeme.value));
            break;
        case Operator:
            os << magic_enum::enum_name(std::get<Operators>(lexeme.value));
            break;
        case Separator:
            os << magic_enum::enum_name(std::get<Separators>(lexeme.value));
            break;
    }
    os << "\t" << lexeme.rawLexeme;
    return os;
}

std::string Lexeme::String() {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}
