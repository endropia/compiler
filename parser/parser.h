#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <utility>
#include <iostream>

#include "../lexer/lexer.h"
#include "../lexer/lexeme.h"

class Node {
public:
    virtual void DrawTree(std::ostream &os, int depth) = 0;

protected:
    Lexeme lexeme;

    explicit Node(Lexeme &lexeme) : lexeme(lexeme) {}
};

class NodeBinaryOperation : public Node {
    Node *left;
    Node *right;

public:
    NodeBinaryOperation(Lexeme &lexeme, Node *left, Node *right) : Node(lexeme) {
        this->left = left;
        this->right = right;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeNumber : public Node {

public:
    explicit NodeNumber(Lexeme &lexeme) : Node(lexeme) {};

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeVar : public Node {
public:
    explicit NodeVar(Lexeme &lexeme) : Node(lexeme) {};

    void DrawTree(std::ostream &os, int depth) override;

};

class Parser {
    Lexer lexer;
    Lexeme lexeme;

public:
    explicit Parser(Lexer &lexer) : lexer(lexer), lexeme(this->lexer.GetLexeme()) {
    }

    Node *Expression();

    Node *Term();

    Node *Factor();
};

class ParserException : public std::exception {
    std::string message;
public:
    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    }

    ParserException(Position pos, const std::string &message) : std::exception() {
        this->message = "(" + std::to_string(pos.GetLine()) + ", " + std::to_string(pos.GetColumn()) + ") " + message;
    }
};

#endif

