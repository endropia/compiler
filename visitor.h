#ifndef COMPILER_VISITOR_H
#define COMPILER_VISITOR_H

class NodeBinaryOperation;

class NodeUnaryOperation;

class NodeString;

class NodeNumber;

class NodeBoolean;

class NodeVar;

class NodeRecordAccess;

class NodeCallAccess;

class NodeIOCallStatement;

class NodeArrayAccess;

class NodeSimpleType;

class NodeRange;

class NodeArrayType;

class NodeField;

class NodeRecordType;

class NodeCompoundStatement;

class NodeAssignmentStatement;

class NodeUserCallStatement;

class NodeIfStatement;

class NodeWhileStatement;

class NodeForStatement;

class NodeBlock;

class NodeProgram;

class NodeTypeDecl;

class NodeVarDecl;

class NodeConstDecl;

class NodeParam;

class NodeProcDecl;

class NodeFuncDecl;

class Visitor {
public:
    virtual void Visit(NodeBinaryOperation *node) = 0;

    virtual void Visit(NodeUnaryOperation *node) = 0;

    virtual void Visit(NodeString *node) = 0;

    virtual void Visit(NodeNumber *node) = 0;

    virtual void Visit(NodeBoolean *node) = 0;

    virtual void Visit(NodeVar *node) = 0;

    virtual void Visit(NodeRecordAccess *node) = 0;

    virtual void Visit(NodeCallAccess *node) = 0;

    virtual void Visit(NodeIOCallStatement *node) = 0;

    virtual void Visit(NodeArrayAccess *node) = 0;

    virtual void Visit(NodeSimpleType *node) = 0;

    virtual void Visit(NodeRange *node) = 0;

    virtual void Visit(NodeArrayType *node) = 0;

    virtual void Visit(NodeField *node) = 0;

    virtual void Visit(NodeRecordType *node) = 0;

    virtual void Visit(NodeCompoundStatement *node) = 0;

    virtual void Visit(NodeAssignmentStatement *node) = 0;

    virtual void Visit(NodeUserCallStatement *node) = 0;

    virtual void Visit(NodeIfStatement *node) = 0;

    virtual void Visit(NodeWhileStatement *node) = 0;

    virtual void Visit(NodeForStatement *node) = 0;

    virtual void Visit(NodeBlock *node) = 0;

    virtual void Visit(NodeProgram *node) = 0;

    virtual void Visit(NodeTypeDecl *node) = 0;

    virtual void Visit(NodeVarDecl *node) = 0;

    virtual void Visit(NodeConstDecl *node) = 0;

    virtual void Visit(NodeParam *node) = 0;

    virtual void Visit(NodeProcDecl *node) = 0;

    virtual void Visit(NodeFuncDecl *node) = 0;
};

#endif //COMPILER_VISITOR_H
