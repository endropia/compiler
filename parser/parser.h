#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <utility>
#include <iostream>

#include "../lexer/lexer.h"
#include "../lexer/lexeme.h"
#include "../visitor.h"

class Visitor;

class SymbolType;

class Node {
public:
    virtual void DrawTree(std::ostream &os, int depth) = 0;

    virtual void Accept(Visitor *visitor) = 0;

    virtual Position GetPos() { return {}; }

    SymbolType *symbol_type = nullptr;
    bool is_lvalue = false;

protected:
    explicit Node() = default;
};

class NodeBinaryOperation : public Node {
public:
    Node *left;
    Node *right;
    Lexeme lexeme;

    NodeBinaryOperation(Lexeme &lexeme, Node *left, Node *right) : Node(), lexeme(lexeme) {
        this->left = left;
        this->right = right;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeUnaryOperation : public Node {
public:
    Lexeme op;
    Node *operand;

    NodeUnaryOperation(Lexeme &op, Node *operand) : Node(), op(op) {
        this->op = op;
        this->operand = operand;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeString : public Node {
public:
    Lexeme lexeme;

    explicit NodeString(Lexeme &lexeme) : Node(), lexeme(lexeme) {};

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeNumber : public Node {
public:
    Lexeme lexeme;

    explicit NodeNumber(Lexeme &lexeme) : Node(), lexeme(lexeme) {};

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeBoolean : public Node {
public:
    Lexeme lexeme;

    explicit NodeBoolean(Lexeme &lexeme) : Node(), lexeme(lexeme) {};

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeVar : public Node {
public:
    Lexeme lexeme;

    explicit NodeVar(Lexeme &lexeme) : Node(), lexeme(lexeme) {};

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;

};


class NodeRecordAccess : public Node {
public:
    Node *rec, *field;

    explicit NodeRecordAccess(Node *rec, Node *field) : Node() {
        this->field = field;
        this->rec = rec;
    };

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeCallAccess : public Node {
public:
    explicit NodeCallAccess(Node *callable, std::vector<Node *> &params) : Node() {
        this->params = params;
        this->callable = callable;
    };

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;

    Node *callable;
    std::vector<Node *> params;
};

class NodeArrayAccess : public Node {
public:
    Node *arr;
    Node *params;

    explicit NodeArrayAccess(Node *arr, Node *params) : Node() {
        this->params = params;
        this->arr = arr;
    };

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};


class NodeType : public Node {
};

class NodeSimpleType : public NodeType {
public:
    Node *type;

    explicit NodeSimpleType(Node *type) : NodeType() {
        this->type = type;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeRange : public Node {
public:
    Node *exp_first;
    Node *exp_second;

    explicit NodeRange(Node *exp_first, Node *exp_second) : Node() {
        this->exp_first = exp_first;
        this->exp_second = exp_second;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeArrayType : public NodeType {
public:
    Node *type;
    std::vector<NodeRange *> ranges;

    explicit NodeArrayType(Node *type, std::vector<NodeRange *> &ranges) : NodeType() {
        this->type = type;
        this->ranges = ranges;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeField : public Node {
public:
    std::vector<Node *> ids;
    Node *type;

    explicit NodeField(std::vector<Node *> &id, Node *type) : Node() {
        this->type = type;
        this->ids = id;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeRecordType : public NodeType {
public:
    std::vector<Node *> fields;

    explicit NodeRecordType(std::vector<Node *> &field) {
        this->fields = field;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeKeyword : public NodeVar {
public:
    explicit NodeKeyword(Lexeme &lexeme) : NodeVar(lexeme) {}

    void Accept(Visitor *visitor) override { visitor->Visit(this); }
};

class NodeStatement : public Node {

};

class NodeCompoundStatement : public NodeStatement {
public:
    std::vector<NodeStatement *> statements;

    explicit NodeCompoundStatement(std::vector<NodeStatement *> &statements) : NodeStatement() {
        this->statements = statements;
    }

    void DrawTree(std::ostream &os, int depth) override;

    void Accept(Visitor *visitor) override { visitor->Visit(this); }
};

class NodeAssignmentStatement : public NodeStatement, public NodeBinaryOperation {
public:
    explicit NodeAssignmentStatement(Lexeme op, Node *var, Node *exp)
            : NodeBinaryOperation(op, var, exp) {}

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override {
        NodeBinaryOperation::DrawTree(os, depth);
    };
};

class NodeCallStatement : public NodeStatement, public NodeCallAccess {
public:
    [[maybe_unused]] NodeCallStatement(Node *rec, std::vector<Node *> &params) :
            NodeCallAccess(rec, params) {}

    explicit NodeCallStatement(NodeCallAccess *call) :
            NodeCallAccess(call->callable, call->params) {}

    void DrawTree(std::ostream &os, int depth) override {
        NodeCallAccess::DrawTree(os, depth);
    };
};

class NodeIOCallStatement : public NodeCallStatement {
public:
    NodeIOCallStatement(Node *callable, std::vector<Node *> &params) :
            NodeCallStatement(callable, params) {};

    std::string GetName();

    bool IsRead();

    void Accept(Visitor *visitor) override { visitor->Visit(this); }
};

class NodeUserCallStatement : public NodeCallStatement {
public:
    NodeUserCallStatement(Node *callable, std::vector<Node *> &params) :
            NodeCallStatement(callable, params) {}

    NodeUserCallStatement(NodeCallAccess *call) : NodeCallStatement(call) {}

    void Accept(Visitor *visitor) override { visitor->Visit(this); }
};


class NodeStructuredStatement : public NodeStatement {

};

class NodeIfStatement : public NodeStructuredStatement {
public:
    Node *exp;
    NodeStatement *statement;
    NodeStatement *else_statement;

    explicit NodeIfStatement(Node *exp,
                             NodeStatement *statement, NodeStatement *else_statement) : NodeStructuredStatement() {
        this->exp = exp;
        this->statement = statement;
        this->else_statement = else_statement;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeWhileStatement : public NodeStructuredStatement {
public:
    Node *exp;
    NodeStatement *statement;

    explicit NodeWhileStatement(Node *exp, NodeStatement *statement) : NodeStructuredStatement() {
        this->exp = exp;
        this->statement = statement;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeForStatement : public NodeStructuredStatement {
public:
    Node *var;
    Node *exp_begin;
    NodeKeyword *direction;
    Node *exp_end;
    NodeStatement *statement;

    explicit NodeForStatement(NodeStatement *statement, Node *var,
                              Node *exp_begin, NodeKeyword *direction,
                              Node *exp_end) : NodeStructuredStatement() {
        this->statement = statement;
        this->var = var;
        this->exp_begin = exp_begin;
        this->direction = direction;
        this->exp_end = exp_end;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;

};

class NodeDecl : public Node {

};

class NodeBlock : public Node {
public:
    std::vector<Node *> decls;
    NodeStatement *comp_stmt;

    explicit NodeBlock(std::vector<Node *> decls,
                       NodeStatement *comp_stmt) : Node() {
        this->decls = decls;
        this->comp_stmt = comp_stmt;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeProgram : public Node {
public:
    Node *name;
    Node *block;

    explicit NodeProgram(Node *name, Node *block) : Node() {
        this->name = name;
        this->block = block;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeTypeDecl : public NodeDecl {
public:
    NodeVar *var;
    Node *type;

    explicit NodeTypeDecl(NodeVar *var, Node *type) : NodeDecl() {
        this->var = var;
        this->type = type;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeVarDecl : public NodeDecl {
public:
    std::vector<NodeVar *> vars;
    Node *type;
    Node *exp; // may be nullptr;
    explicit NodeVarDecl(std::vector<NodeVar *> vars,
                         Node *type, Node *exp) : NodeDecl() {
        this->vars = vars;
        this->type = type;
        this->exp = exp;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeConstDecl : public NodeDecl {
public:
    NodeVar *var;
    Node *type; // may be nullptr;
    Node *exp;

    explicit NodeConstDecl(NodeVar *vars, Node *type, Node *exp) : NodeDecl() {
        this->var = vars;
        this->type = type;
        this->exp = exp;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;

};

class NodeParam : public Node {
public:
    NodeKeyword *modifier; // "var" or "const"
    std::vector<NodeVar *> vars;
    Node *type;

    explicit NodeParam(NodeKeyword *modifier,
                       std::vector<NodeVar *> vars, Node *type) : Node() {
        this->modifier = modifier;
        this->vars = vars;
        this->type = type;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeProcDecl : public NodeDecl {
public:
    Node *var;
    std::vector<Node *> params;
    Node *block;

    explicit NodeProcDecl(Node *var, std::vector<Node *> params,
                          Node *block) : NodeDecl() {
        this->var = var;
        this->params = params;
        this->block = block;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

    void DrawTree(std::ostream &os, int depth) override;
};

class NodeFuncDecl : public NodeProcDecl {
public:
    Node *type;

    explicit NodeFuncDecl(Node *var, std::vector<Node *> params,
                          Node *block, Node *type) : NodeProcDecl(var, params, block) {
        this->type = type;
    }

    void Accept(Visitor *visitor) override { visitor->Visit(this); }

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

