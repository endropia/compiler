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
    explicit Node() = default;
};

class NodeBinaryOperation : public Node {
    Node *left;
    Node *right;
    Lexeme lexeme;

public:
    NodeBinaryOperation(Lexeme &lexeme, Node *left, Node *right) : Node(), lexeme(lexeme) {
        this->left = left;
        this->right = right;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeUnaryOperation : public Node {
    Lexeme op;
    Node *operand;

public:
    NodeUnaryOperation(Lexeme &op, Node *operand) : Node(), op(op) {
        this->op = op;
        this->operand = operand;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeString : public Node {
    Lexeme lexeme;

public:
    explicit NodeString(Lexeme &lexeme) : Node(), lexeme(lexeme) {};

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeNumber : public Node {
    Lexeme lexeme;

public:
    explicit NodeNumber(Lexeme &lexeme) : Node(), lexeme(lexeme) {};

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeVar : public Node {
    Lexeme lexeme;
public:
    explicit NodeVar(Lexeme &lexeme) : Node(), lexeme(lexeme) {};

    void DrawTree(std::ostream &os, int depth) override;

};


class NodeRecordAccess : public Node {
    Node *rec, *field;
public:
    explicit NodeRecordAccess(Node *rec, Node *field) : Node() {
        this->field = field;
        this->rec = rec;
    };

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeCallAccess : public Node {
public:
    explicit NodeCallAccess(Node *rec, std::vector<Node *> &params) : Node() {
        this->params = params;
        this->rec = rec;
    };

    void DrawTree(std::ostream &os, int depth) override;

    Node *rec;
    std::vector<Node *> params;
};

class NodeArrayAccess : public Node {
    Node *rec;
    std::vector<Node *> params;
public:
    explicit NodeArrayAccess(Node *rec, std::vector<Node *> &params) : Node() {
        this->params = params;
        this->rec = rec;
    };

    void DrawTree(std::ostream &os, int depth) override;
};


class NodeType : public Node {

};

class NodeSimpleType : public NodeType {
    Node *type;
public:
    explicit NodeSimpleType(Node *type) : NodeType() {
        this->type = type;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeRange : public NodeType {
    Node *exp_first;
    Node *exp_second;
public:
    explicit NodeRange(Node *exp_first, Node *exp_second) : NodeType() {
        this->exp_first = exp_first;
        this->exp_second = exp_second;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeArrayType : public NodeType {
    Node *type;
    std::vector<NodeRange *> ranges;
public:
    explicit NodeArrayType(Node *type, std::vector<NodeRange *> &ranges) : NodeType() {
        this->type = type;
        this->ranges = ranges;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeField : public NodeType {
    std::vector<Node *> id;
    Node *type;
public:
    explicit NodeField(std::vector<Node *> &id, Node *type) : NodeType() {
        this->type = type;
        this->id = id;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeRecordType : public NodeType {
    std::vector<Node *> field;
public:
    explicit NodeRecordType(std::vector<Node *> &field) {
        this->field = field;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeKeyword : public NodeVar {
public:
    explicit NodeKeyword(Lexeme &lexeme) : NodeVar(lexeme) {}
};

class NodeStatement : public Node {

};

class NodeCompoundStatement : public NodeStatement {
    std::vector<NodeStatement *> statements;
public:
    explicit NodeCompoundStatement(std::vector<NodeStatement *> &statements) : NodeStatement() {
        this->statements = statements;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeAssignmentStatement : public NodeStatement, public NodeBinaryOperation {
public:
    explicit NodeAssignmentStatement(Lexeme op, Node *var, Node *exp)
            : NodeBinaryOperation(op, var, exp) {}

    void DrawTree(std::ostream &os, int depth) override {
        NodeBinaryOperation::DrawTree(os, depth);
    };
};

class NodeCallStatement : public NodeStatement, public NodeCallAccess {
public:
    [[maybe_unused]] NodeCallStatement(Node *rec, std::vector<Node *> &params) :
            NodeCallAccess(rec, params) {}

    explicit NodeCallStatement(NodeCallAccess *call) :
            NodeCallAccess(call->rec, call->params) {}

    void DrawTree(std::ostream &os, int depth) override {
        NodeCallAccess::DrawTree(os, depth);
    };
};

class NodeStructuredStatement : public NodeStatement {

};

class NodeIfStatement : public NodeStructuredStatement {
    Node *exp;
    NodeStatement *statement;
    NodeStatement *else_statement;
public:
    explicit NodeIfStatement(Node *exp,
                             NodeStatement *statement, NodeStatement *else_statement) : NodeStructuredStatement() {
        this->exp = exp;
        this->statement = statement;
        this->else_statement = else_statement;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeWhileStatement : public NodeStructuredStatement {
    Node *exp;
    NodeStatement *statement;
public:
    explicit NodeWhileStatement(Node *exp, NodeStatement *statement) : NodeStructuredStatement() {
        this->exp = exp;
        this->statement = statement;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeForStatement : public NodeStructuredStatement {
    NodeVar *var;
    Node *exp_begin;
    NodeKeyword *direction;
    Node *exp_end;
    NodeStatement *statement;
public:
    explicit NodeForStatement(NodeStatement *statement, NodeVar *var,
                              Node *exp_begin, NodeKeyword *direction,
                              Node *exp_end) : NodeStructuredStatement() {
        this->statement = statement;
        this->var = var;
        this->exp_begin = exp_begin;
        this->direction = direction;
        this->exp_end = exp_end;
    }

    void DrawTree(std::ostream &os, int depth) override;

};

class NodeDecl : public Node {

};

class NodeBlock : public Node {
    std::vector<Node *> decls;
    NodeStatement *comp_stmt;
public:
    explicit NodeBlock(std::vector<Node *> decls,
                       NodeStatement *comp_stmt) : Node() {
        this->decls = decls;
        this->comp_stmt = comp_stmt;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeProgram : public Node {
    Node *name;
    Node *block;
public:
    explicit NodeProgram(Node *name, Node *block) : Node() {
        this->name = name;
        this->block = block;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeTypeDecl : public NodeDecl {
    NodeVar *var;
    Node *type;
public:
    explicit NodeTypeDecl(NodeVar *var, Node *type) : NodeDecl() {
        this->var = var;
        this->type = type;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeVarDecl : public NodeDecl {
    std::vector<NodeVar *> vars;
    Node *type;
    Node *exp; // may be nullptr;
public:
    explicit NodeVarDecl(std::vector<NodeVar *> vars,
                         Node *type, Node *exp) : NodeDecl() {
        this->vars = vars;
        this->type = type;
        this->exp = exp;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeConstDecl : public NodeDecl {
    NodeVar *vars;
    Node *type; // may be nullptr;
    Node *exp;
public:
    explicit NodeConstDecl(NodeVar *vars, Node *type, Node *exp) : NodeDecl() {
        this->vars = vars;
        this->type = type;
        this->exp = exp;
    }

    void DrawTree(std::ostream &os, int depth) override;

};

class NodeParam : public Node {
    NodeKeyword *modifier; // "var" or "const"
    std::vector<NodeVar *> vars;
    Node *type;
public:
    explicit NodeParam(NodeKeyword *modifier,
                       std::vector<NodeVar *> vars, Node *type) : Node() {
        this->modifier = modifier;
        this->vars = vars;
        this->type = type;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeProcDecl : public NodeDecl {
protected:
    Node *var;
    std::vector<Node *> params;
    Node *block;
public:
    explicit NodeProcDecl(Node *var, std::vector<Node *> params,
                          Node *block) : NodeDecl() {
        this->var = var;
        this->params = params;
        this->block = block;
    }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeFuncDecl : public NodeProcDecl {
    Node *type;
public:
    explicit NodeFuncDecl(Node *var, std::vector<Node *> params,
                          Node *block, Node *type) : NodeProcDecl(var, params, block) {
        this->type = type;
    }

    void DrawTree(std::ostream &os, int depth) override;

};

class Parser {
    Lexer lexer;
    Lexeme lexeme;

public:
    explicit Parser(Lexer &lexer) : lexer(lexer), lexeme(this->lexer.GetLexeme()) {
    }

    Node *Program();

    Node *Procedure();

    Node *Function();

    Node *FunctionParam();

    std::vector<Node *> FunctionParams(bool required);

    Node *Block(bool parse_functions);

    Node *Expression();

    Node *SimpleExpression();

    Node *Term();

    Node *SimpleTerm();

    Node *Factor();

    NodeRange *IndexRange();

    std::vector<NodeRange *> IndexRanges();

    Node *ArrayType();

    Node *Type();

    Node *Field();

    Node *RecordType();

    NodeStatement *CompoundStatement();

    NodeStatement *Statement();

    NodeStatement *SimpleStatement();

    NodeStatement *IfStatement();

    NodeStatement *WhileStatement();

    NodeStatement *ForStatement();

    std::vector<Node *> ListIdent();

    std::vector<Node *> ListExpressions(bool required);

    std::vector<Node *> Fields();

    std::vector<NodeTypeDecl *> TypeDeclPart();

    NodeTypeDecl *TypeDecl();

    std::vector<NodeConstDecl *> ConstDeclPart();

    NodeConstDecl *ConstDecl();

    NodeVarDecl *VarDecl();

    std::vector<NodeVarDecl *> VarDeclPart();


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

