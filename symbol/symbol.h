#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

#include "../lexer/lexeme.h"
#include <map>
#include <string>
#include <vector>

class Symbol {
public:
    explicit Symbol(std::string name) : name(name) {}

    ~Symbol() = default;

    virtual std::string GetName();

    virtual std::string GetClass() { return "symbol"; }

    std::string name;
};

class SymbolTable {
public:
    SymbolTable() = default;

    ~SymbolTable() = default;

    Symbol *Get(std::string name);

    void Push(std::string name, Symbol *symbol);

    void Push(Symbol *symbol);

    void Del(std::string name);

    void Draw(std::ostream &os, int depth);

    [[nodiscard]] bool Contains(std::string name);

    std::map<std::string, Symbol *> data;
    std::vector<std::string> ordered;
};

class SymbolTableStack {
public:
    SymbolTableStack() = default;

    ~SymbolTableStack() = default;

    Symbol *get(std::string name);

    void Push(std::string name, Symbol *symbol);

    void Pop();

    void Push(Symbol *symbol);

    void Push(SymbolTable *table);

    void CreateTable();

    [[nodiscard]] bool ContainsInScope(std::string name);

    void Draw(std::ostream &os);

    std::vector<SymbolTable *> data;
};

class SymbolType : public Symbol {
public:
    explicit SymbolType(std::string name) : Symbol(name) {}

    virtual bool is(SymbolType *b);

    virtual SymbolType *Resolve();

    virtual std::string GetClass() { return "primitive type"; }

    ~SymbolType() = default;
};

class SymbolInteger : public SymbolType {
public:
    SymbolInteger() : SymbolType("integer") {}

    ~SymbolInteger() = default;
};

class SymbolDouble : public SymbolType {
public:
    SymbolDouble() : SymbolType("double") {}

    ~SymbolDouble() = default;
};

class SymbolBoolean : public SymbolType {
public:
    SymbolBoolean() : SymbolType("boolean") {}

    ~SymbolBoolean() = default;
};

class SymbolChar : public SymbolType {
public:
    SymbolChar() : SymbolType("char") {}

    ~SymbolChar() = default;
};

class SymbolString : public SymbolType {
public:
    SymbolString() : SymbolType("string") {}

    ~SymbolString() = default;
};

class SymbolAlias : public SymbolType {
public:
    SymbolAlias(std::string name, SymbolType *original) : SymbolType(name), original(original) {}

    ~SymbolAlias() = default;

    virtual std::string GetClass() { return "alias"; }

    SymbolType *Resolve() override;

    SymbolType *original;
};

class SymbolRecord : public SymbolType {
public:
    explicit SymbolRecord(SymbolTable *fields) : SymbolType("record"), fields(fields) {}

    ~SymbolRecord() = default;

    virtual std::string GetClass() { return "record"; }

    bool is(SymbolType *b) override;

    SymbolTable *fields;
};

class Node;

class SymbolArray : public SymbolType {
public:
    SymbolArray(SymbolType *type, Node *beg, Node *end) : SymbolType("array"), type(type), beg(beg), end(end) {}

    ~SymbolArray() = default;

    virtual std::string GetClass() { return "array"; }

    bool is(SymbolType *b) override;

    SymbolType *type;
    Node *beg;
    Node *end;
};

class SymbolVar : public Symbol {
public:
    SymbolVar(std::string name, SymbolType *type) : Symbol(name), type(type) {}

    ~SymbolVar() = default;

    virtual std::string GetClass() { return "variable"; }

    SymbolType *type;
};

class SymbolParam : public SymbolVar {
public:
    SymbolParam(std::string name, SymbolType *type) : SymbolVar(name, type) {}

    ~SymbolParam() = default;

    virtual std::string GetClass() { return "param"; }
};

class SymbolVarParam : public SymbolParam {
public:
    SymbolVarParam(std::string name, SymbolType *type) : SymbolParam(name, type) {}

    ~SymbolVarParam() = default;

    virtual std::string GetClass() { return "parameter variable reference"; }
};

class SymbolConstParam : public SymbolParam {
public:
    SymbolConstParam(std::string name, SymbolType *type) : SymbolParam(name, type) {}

    ~SymbolConstParam() = default;

    virtual std::string GetClass() { return "const parameter"; }
};

class SymbolConst : public SymbolVar {
public:
    SymbolConst(std::string name, SymbolType *type) : SymbolVar(name, type) {}

    ~SymbolConst() = default;

    virtual std::string GetClass() { return "const"; }
};

class NodeCompoundStatement;

class SymbolProcedure : public SymbolType {
public:
    SymbolProcedure(std::string name, SymbolTable *locals, NodeCompoundStatement *body) : SymbolType(name),
                                                                                          locals(locals),
                                                                                          body(body) {}

    ~SymbolProcedure() = default;

    int GetCountOfArguments();

    virtual std::string GetClass() { return "procedure"; }

    SymbolTable *locals;
    NodeCompoundStatement *body;
};

class SymbolFunction : public SymbolProcedure {
public:
    SymbolFunction(std::string name, SymbolTable *locals, NodeCompoundStatement *body, SymbolType *ret)
            : SymbolProcedure(name,
                              locals,
                              body),
              ret(ret) {}

    ~SymbolFunction() = default;

    virtual std::string GetClass() { return "function"; }

    SymbolType *ret;
};

class SemanticException : public std::exception {
    std::string message;

public:
    [[nodiscard]] const char *what() const

    noexcept override {
        return message.c_str();
    }

    SemanticException(Position pos, const std::string &message)
            : std::exception() {
        this->message = "(" + std::to_string(pos.GetLine()) + ", " +
                        std::to_string(pos.GetColumn()) + ") " + message;
    }

    template<class T>
    SemanticException(T object_with_pos, const std::string &message)
            : SemanticException(object_with_pos->GetPos(), message) {}

    explicit SemanticException(const std::string &message) : std::exception() {
        this->message = message;
    }
};

const auto SYM_INTEGER = new SymbolInteger();
const auto SYM_DOUBLE = new SymbolDouble();
const auto SYM_BOOLEAN = new SymbolBoolean();
const auto SYM_CHAR = new SymbolChar();
const auto SYM_STRING = new SymbolString();


#endif // COMPILER_SYMBOL_H
