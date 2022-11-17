#include "parser.h"
#include <magic_enum.hpp>


Node *Parser::Expression() {
    auto left = Term();
    auto lex = lexeme;
    while (lex.GetType() == LexemeType::Operator and
           (lex.GetValue<Operators>() == Operators::ADD or
            lex.GetValue<Operators>() == Operators::SUBSTRACT)) {
        lexeme = lexer.GetLexeme();
        left = new NodeBinaryOperation(lex, left, Term());
        lex = lexeme;
    }
    return left;
}

Node *Parser::Term() {
    auto left = Factor();
    auto lex = lexeme;
    while (lex.GetType() == LexemeType::Operator and
           (lex.GetValue<Operators>() == Operators::MULTIPLY or
            lex.GetValue<Operators>() == Operators::DIVISION)) {
        lexeme = lexer.GetLexeme();
        left = new NodeBinaryOperation(lex, left, Factor());
        lex = lexeme;
    }
    return left;
}

Node *Parser::Factor() {
    auto lex = lexeme;
    if (lex.GetType() == LexemeType::Integer or
        lex.GetType() == LexemeType::Double) {
        lexeme = lexer.GetLexeme();
        return new NodeNumber(lex);
    }
    if (lex.GetType() == LexemeType::Identifier) {
        lexeme = lexer.GetLexeme();
        return new NodeVar(lex);
    }
    if (lex.GetType() == LexemeType::Separator and
        lex.GetValue<Separators>() == Separators::LPARENTHESIS) {
        lexeme = lexer.GetLexeme();
        auto exp = Expression();
        if (lexeme.GetType() != LexemeType::Separator or
            !(lexeme.GetType() == LexemeType::Separator and
              lexeme.GetValue<Separators>() == Separators::RPARENTHESIS))
            throw ParserException(lexeme.GetPos(), "Expected )");
        lexeme = lexer.GetLexeme();
        return exp;
    }

    throw ParserException(lexeme.GetPos(), "Factor expected");
}

void NodeBinaryOperation::DrawTree(std::ostream &os, int depth) {
    os << lexeme.GetRaw() << "\n";
    for (int i = 0; i < depth; ++i) {
        os << "\t";
    }
    left->DrawTree(os, depth + 1);
    for (int i = 0; i < depth; ++i) {
        os << "\t";
    }
    right->DrawTree(os, depth + 1);
}

void NodeNumber::DrawTree(std::ostream &os, int depth) {
    if (lexeme.GetType() == LexemeType::Double)
        os << lexeme.GetValue<double>() << "\n";
    if (lexeme.GetType() == LexemeType::Integer)
        os << lexeme.GetValue<int>() << "\n";
}

void NodeVar::DrawTree(std::ostream &os, int depth) {
    os << lexeme.GetRaw() << "\n";
}
