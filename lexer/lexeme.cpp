#include "lexeme.h"
#include <sstream>

#include <magic_enum.hpp>

Position::Position() {
    this->line = line;
    this->column = column;
}

int Position::GetLine() const { return line; }

int Position::GetColumn() const { return column; }

void Position::Set(int line, int column) {
    this->line = line;
    this->column = column;
}

void Position::Set(const Position &pos) {
    Set(pos.line, pos.column);
}

Lexeme::Lexeme(const Position &position, LexemeType type, LexemeValue &value,
               const std::string &rawLexeme)
        : position(position) {
    this->type = type;
    this->value = value;
    this->rawLexeme = rawLexeme;
}

LexemeType Lexeme::GetType() { return type; }

std::ostream &operator<<(std::ostream &os, const Lexeme &lexeme) {
    if (lexeme.type == LexemeType::eof) {
        os << lexeme.position.GetLine() << "\t" << lexeme.position.GetColumn()
           << "\t" << magic_enum::enum_name(lexeme.type);
        return os;
    }
    os << lexeme.position.GetLine() << "\t" << lexeme.position.GetColumn() << "\t"
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

bool operator==(Lexeme &lex, LexemeType type) {
    return lex.GetType() == type;
}

bool operator==(Lexeme &lex, Operators op) {
    if (lex.type != LexemeType::Operator) return false;
    return lex.GetValue<Operators>() == op;
}

bool operator==(Lexeme &lex, Separators sep) {
    if (lex.type != LexemeType::Separator) return false;
    return lex.GetValue<Separators>() == sep;
}

bool operator==(Lexeme &lex, AllKeywords keyword) {
    if (lex.type != LexemeType::Keyword) return false;
    return lex.GetValue<AllKeywords>() == keyword;
}

void Lexeme::ConvertToId() {
    if (*this == LexemeType::Keyword) {
        auto _value = std::string(magic_enum::enum_name(this->GetValue<AllKeywords>()));
        for (auto &item: _value) {
            item = (char) tolower(item);
        }
        this->value = _value;
    }
    type = LexemeType::Identifier;
}
