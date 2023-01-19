#ifndef COMPILER_SEMANTIC_H
#define COMPILER_SEMANTIC_H

#include "../visitor.h"
#include "../symbol/symbol.h"

class NodeType;

class Semantic : public Visitor {
public:
    SymbolType *GetSymType(NodeType *type);

    void Visit(NodeBinaryOperation *node) override;

    void Visit(NodeUnaryOperation *node) override;

    void Visit(NodeString *node) override;

    void Visit(NodeNumber *node) override;

    void Visit(NodeBoolean *node) override;

    void Visit(NodeVar *node) override;

    void Visit(NodeRecordAccess *node) override;

    void Visit(NodeCallAccess *node) override;

    void Visit(NodeIOCallStatement *node) override;

    void Visit(NodeArrayAccess *node) override;

    void Visit(NodeSimpleType *node) override;

    void Visit(NodeRange *node) override;

    void Visit(NodeArrayType *node) override;

    void Visit(NodeField *node) override;

    void Visit(NodeRecordType *node) override;

    void Visit(NodeCompoundStatement *node) override;

    void Visit(NodeAssignmentStatement *node) override;

    void Visit(NodeUserCallStatement *node) override;

    void Visit(NodeIfStatement *node) override;

    void Visit(NodeWhileStatement *node) override;

    void Visit(NodeForStatement *node) override;

    void Visit(NodeBlock *node) override;

    void Visit(NodeProgram *node) override;

    void Visit(NodeTypeDecl *node) override;

    void Visit(NodeVarDecl *node) override;

    void Visit(NodeConstDecl *node) override;

    void Visit(NodeParam *node) override;

    void Visit(NodeProcDecl *node) override;

    void Visit(NodeFuncDecl *node) override;

    SymbolTableStack GetStack();

    SymbolTableStack stack;
};


#endif //COMPILER_SEMANTIC_H
