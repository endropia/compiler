#include "semantic.h"
#include "../parser/parser.h"


SymbolType *Semantic::GetSymType(NodeType *type) {
    auto record_type = dynamic_cast<NodeRecordType *>(type);
    if (record_type != nullptr) {
        auto record_table = new SymbolTable();
        for (auto &field: record_type->fields) {
            auto casted_field = dynamic_cast<NodeField *>(field);
            auto sym_type_field = GetSymType(dynamic_cast<NodeType *>(casted_field->type));
            for (auto &id: casted_field->ids) {
                auto id_field = dynamic_cast<NodeVar *>(id);
                record_table->Push(new SymbolVar(id_field->lexeme.GetValue<std::string>(), sym_type_field));
            }
        }
        return new SymbolRecord(record_table);
    }
    auto array_type = dynamic_cast<NodeArrayType *>(type);
    if (array_type != nullptr) {
        SymbolType *res = GetSymType(dynamic_cast<NodeType *>(array_type->type));
        for (auto it = array_type->ranges.rbegin(); it != array_type->ranges.rend(); it++) {
            auto range = *it;
            res = new SymbolArray(res, range->exp_first, range->exp_second);
        }
        return res;
    }
    auto primitive_type = dynamic_cast<NodeSimpleType *>(type);
    auto name = dynamic_cast<NodeVar *>(primitive_type->type)->lexeme.GetValue<std::string>();
    auto symbol = stack.get(name);
    auto symbol_type = dynamic_cast<SymbolType *>(symbol);
    if (symbol_type == nullptr) {
        throw SemanticException("Type is not found");
    }
    return symbol_type;
}


void Semantic::Visit(NodeBinaryOperation *node) {
    node->left->Accept(this);
    node->right->Accept(this);

    auto lst = node->left->symbol_type;
    auto rst = node->right->symbol_type;

    if (node->lexeme.GetType() == LexemeType::Operator) {
        switch (node->lexeme.GetValue<Operators>()) {
            case Operators::EQUAL:
            case Operators::UNEQUAL:
            case Operators::LESSEQUAL:
            case Operators::LESS:
            case Operators::GREATEREQUAL:
            case Operators::GREATER:
                if (!(
                        (lst->is(rst) && lst->is(SYM_INTEGER)) ||
                        (lst->is(rst) && lst->is(SYM_DOUBLE)) ||
                        (lst->is(rst) && lst->is(SYM_BOOLEAN)) ||
                        (lst->is(rst) && lst->is(SYM_CHAR)) ||
                        (lst->is(rst) && lst->is(SYM_STRING))
                )) {
                    throw SemanticException("It is not binary operation");
                }
                node->symbol_type = SYM_BOOLEAN;
                break;
            case Operators::ADD:
                if (!(
                        (lst->is(rst) && lst->is(SYM_INTEGER)) ||
                        (lst->is(rst) && lst->is(SYM_DOUBLE)) ||
                        (lst->is(rst) && lst->is(SYM_STRING))
                )) {
                    throw SemanticException("It is not binary operation");
                }
                node->symbol_type = lst;
                break;
            case Operators::SUBSTRACT:
            case Operators::MULTIPLY:
                if (!(
                        (lst->is(rst) && lst->is(SYM_INTEGER)) ||
                        (lst->is(rst) && lst->is(SYM_DOUBLE))
                )) {
                    throw SemanticException("It is not binary operation");
                }
                node->symbol_type = lst;
                break;
            case Operators::DIVISION:
                if (!(
                        (lst->is(rst) && lst->is(SYM_INTEGER)) ||
                        (lst->is(rst) && lst->is(SYM_DOUBLE))
                )) {
                    throw SemanticException("It is not binary operation");
                }
                node->symbol_type = SYM_DOUBLE;
        }
    } else {
        switch (node->lexeme.GetValue<AllKeywords>()) {
            case AllKeywords::OR:
            case AllKeywords::XOR:
            case AllKeywords::AND:
                if (!(
                        (lst->is(rst) && lst->is(SYM_INTEGER)) ||
                        (lst->is(rst) && lst->is(SYM_BOOLEAN))
                )) {
                    throw SemanticException("It is not binary operation");
                }
                node->symbol_type = SYM_BOOLEAN;
                break;
            case AllKeywords::DIV:
            case AllKeywords::MOD:
            case AllKeywords::SHR:
            case AllKeywords::SHL:
                if (!(
                        (lst->is(rst) && lst->is(SYM_INTEGER))
                )) {
                    throw SemanticException("It is not binary operation");
                }
                node->symbol_type = lst;
                break;
        }
    }
}


void Semantic::Visit(NodeUnaryOperation *node) {
    node->operand->Accept(this);
    auto sym_type = node->operand->symbol_type;

    if (node->op == LexemeType::Operator) {
        if (node->op == Operators::ADD || node->op == Operators::SUBSTRACT) {
            if (sym_type->is(SYM_INTEGER)) node->symbol_type = SYM_INTEGER;
            if (sym_type->is(SYM_DOUBLE)) node->symbol_type = SYM_DOUBLE;
        }
    } else if (node->op == AllKeywords::NOT) {
        if (sym_type->is(SYM_BOOLEAN)) node->symbol_type = SYM_BOOLEAN;
        if (sym_type->is(SYM_INTEGER)) node->symbol_type = SYM_INTEGER;
    }
    if (node->symbol_type == nullptr) {
        throw SemanticException("Unary operator does not overloaded");
    }
}


void Semantic::Visit(NodeString *node) {
    node->symbol_type = SYM_STRING;
}


void Semantic::Visit(NodeNumber *node) {
    if (node->lexeme == LexemeType::Double)
        node->symbol_type = SYM_DOUBLE;
    else
        node->symbol_type = SYM_INTEGER;
}


void Semantic::Visit(NodeBoolean *node) {
    node->symbol_type = SYM_BOOLEAN;
}


void Semantic::Visit(NodeVar *node) {
    auto id = stack.get(node->lexeme.GetValue<std::string>());
    auto *id_var_casted = dynamic_cast<SymbolVar *>(id);
    if (id_var_casted != nullptr) {
        node->symbol_type = id_var_casted->type;
        node->is_lvalue = true;
        return;
    }
    auto id_proc_casted = dynamic_cast<SymbolProcedure *>(id);
    if (id_proc_casted != nullptr) {
        node->symbol_type = id_proc_casted;
        return;
    }
    throw SemanticException("It is not not var");
}


void Semantic::Visit(NodeRecordAccess *node) {
    node->rec->Accept(this);
    auto sym_type_of_rec = dynamic_cast<SymbolRecord *>(node->rec->symbol_type->Resolve());
    if (sym_type_of_rec == nullptr) {
        throw SemanticException("It is not record");
    }
    auto sym_field = sym_type_of_rec->fields->Get(
            dynamic_cast<NodeVar *>(node->field)->lexeme.GetValue<std::string>());
    auto sym_field_casted = dynamic_cast<SymbolVar *>(sym_field);
    node->is_lvalue = true;
    node->symbol_type = sym_field_casted->type;
}


void Semantic::Visit(NodeCallAccess *node) {
    node->callable->Accept(this);
    auto sym_casted = dynamic_cast<SymbolProcedure *>(node->callable->symbol_type);
    if (sym_casted == nullptr) {
        throw SemanticException("It is not callable");
    }
    if (node->params.size() != sym_casted->GetCountOfArguments()) {
        throw SemanticException("Do not match count of params");
    }
    for (auto &param: node->params) {
        param->Accept(this);
    }
    for (auto i = 0; i < sym_casted->GetCountOfArguments(); ++i) {
        auto name = sym_casted->locals->ordered[i];
        auto sym_param = dynamic_cast<SymbolParam *>(sym_casted->locals->Get(name));

        if (node->params[i]->symbol_type->is(sym_param->type)) {
            throw SemanticException("Types do not match if call access");
        }
    }
}


void Semantic::Visit(NodeArrayAccess *node) {
    node->is_lvalue = true;
    node->arr->Accept(this);
    node->params->Accept(this);
    if (!node->params->symbol_type->is(SYM_INTEGER)) {
        throw SemanticException("Integer expected");
    }
    auto symbol_type_casted = dynamic_cast<SymbolArray *>(node->arr->symbol_type->Resolve());
    if (symbol_type_casted == nullptr) {
        throw SemanticException("It is not array");
    }
    node->symbol_type = symbol_type_casted->type;
}


void Semantic::Visit(NodeSimpleType *node) {
}


void Semantic::Visit(NodeRange *node) {
    node->exp_first->Accept(this);
    node->exp_second->Accept(this);
    if (!node->exp_first->symbol_type->is(SYM_INTEGER)) {
        throw SemanticException("Integer expected");
    }
    if (!node->exp_second->symbol_type->is(SYM_INTEGER)) {
        throw SemanticException("Integer expected");
    }
}


void Semantic::Visit(NodeArrayType *node) {
    for (auto &range: node->ranges) range->Accept(this);
}


void Semantic::Visit(NodeField *node) {
    auto sym_type = GetSymType(dynamic_cast<NodeType *>(node->type));
    for (auto &id: node->ids) {
        auto id_casted = dynamic_cast<NodeVar *>(id);
        stack.Push(new SymbolVar(id_casted->lexeme.GetValue<std::string>(), sym_type));
    }
}


void Semantic::Visit(NodeRecordType *node) {
    auto table = new SymbolTable();
    stack.Push(table);
    for (auto &field: node->fields) {
        field->Accept(this);
    }
    stack.Pop();
}


void Semantic::Visit(NodeCompoundStatement *node) {
    for (auto &statement: node->statements) statement->Accept(this);
}


void Semantic::Visit(NodeAssignmentStatement *node) {
    node->left->Accept(this);
    node->right->Accept(this);
    if (!node->left->is_lvalue) {
        throw SemanticException("It is not possible to assign rvalue");
    }
    auto lst = node->left->symbol_type;
    auto rst = node->right->symbol_type;
    switch (node->lexeme.GetValue<Operators>()) {
        case Operators::ASSIGN:
            if (!(
                    (lst->is(rst) && lst->is(SYM_INTEGER)) ||
                    (lst->is(rst) && lst->is(SYM_DOUBLE)) ||
                    (lst->is(rst) && lst->is(SYM_BOOLEAN)) ||
                    (lst->is(rst) && lst->is(SYM_CHAR)) ||
                    (lst->is(rst) && lst->is(SYM_STRING))
            )) {
                throw SemanticException("Assigment statement does not overloaded");
            }
            break;
        case Operators::ADDASSIGN:
            if (!(
                    (lst->is(rst) && lst->is(SYM_INTEGER)) ||
                    (lst->is(rst) && lst->is(SYM_DOUBLE)) ||
                    (lst->is(rst) && lst->is(SYM_STRING))
            )) {
                throw SemanticException("Assigment statement does not overloaded");
            }
        case Operators::SUBSTRACTASSIGN:
        case Operators::MULTIPLYASSIGN:
        case Operators::DIVISIONASSIGN:
            if (!(
                    (lst->is(rst) && lst->is(SYM_INTEGER)) ||
                    (lst->is(rst) && lst->is(SYM_DOUBLE))
            )) {
                throw SemanticException("Assigment statement does not overloaded");
            }

    }
}


void Semantic::Visit(NodeUserCallStatement *node) {
    node->callable->Accept(this);

    auto sym_casted = dynamic_cast<SymbolProcedure *>(node->callable->symbol_type);
    if (sym_casted == nullptr) {
        throw SemanticException("It is not callable");
    }
    if (node->params.size() != sym_casted->GetCountOfArguments()) {
        throw SemanticException("Count of params does not match");
    }
    for (auto &param: node->params) {
        param->Accept(this);
    }
    bool is_function = dynamic_cast<SymbolFunction *>(node->callable->symbol_type) != nullptr;
    for (auto i = 0; i < sym_casted->GetCountOfArguments(); ++i) {
        auto name = sym_casted->locals->ordered[i + is_function];
        auto sym_param = dynamic_cast<SymbolParam *>(sym_casted->locals->Get(name));
        if (!node->params[i]->symbol_type->is(sym_param->type)) {
            throw SemanticException("Types do not match in call access");
        }
    }
}


void Semantic::Visit(NodeIOCallStatement *node) {
    if (node->IsRead()) {
        for (auto &param: node->params) {
            param->Accept(this);
            if (!param->is_lvalue) {
                throw SemanticException("Rvalue can not be used in read procedure");
            }
        }
    }
    for (auto &param: node->params) {
        param->Accept(this);
        if ((!param->symbol_type->is(SYM_INTEGER) &&
             !param->symbol_type->is(SYM_DOUBLE) &&
             !param->symbol_type->is(SYM_BOOLEAN) &&
             !param->symbol_type->is(SYM_STRING) &&
             !param->symbol_type->is(SYM_CHAR))) {
            throw SemanticException("It can not be used in io procedures");
        }
    }
}



void Semantic::Visit(NodeIfStatement *node) {
    node->exp->Accept(this);
    if (!node->exp->symbol_type->is(SYM_BOOLEAN)) {
        throw SemanticException("Boolean expected");
    }
    node->statement->Accept(this);
    if (node->else_statement != nullptr) {
        node->else_statement->Accept(this);
    }
}


void Semantic::Visit(NodeWhileStatement *node) {
    node->exp->Accept(this);
    if (!node->exp->symbol_type->is(SYM_BOOLEAN)) {
        throw SemanticException("Boolean expected");
    }
    node->statement->Accept(this);
}


void Semantic::Visit(NodeForStatement *node) {
    node->var->Accept(this);
    node->exp_begin->Accept(this);
    node->exp_end->Accept(this);
    if (!node->var->symbol_type->is(SYM_INTEGER)) {
        throw SemanticException("Ordinary expected in for");
    }
    if (!node->exp_begin->symbol_type->is(SYM_INTEGER)) {
        throw SemanticException("Integer expected");
    }
    if (!node->exp_end->symbol_type->is(SYM_INTEGER)) {
        throw SemanticException("Integer expected");
    }
    node->statement->Accept(this);
}


void Semantic::Visit(NodeBlock *node) {
    for (auto &decl: node->decls) decl->Accept(this);
    node->comp_stmt->Accept(this);
}


void Semantic::Visit(NodeProgram *node) {
    stack.CreateTable();
    stack.Push(SYM_INTEGER);
    stack.Push(SYM_DOUBLE);
    stack.Push(SYM_BOOLEAN);
    stack.Push(SYM_CHAR);
    stack.Push(SYM_STRING);
    stack.CreateTable();
    node->block->Accept(this);
}


void Semantic::Visit(NodeTypeDecl *node) {
    auto sym_type = GetSymType(dynamic_cast<NodeType *>(node->type));
    stack.Push(new SymbolAlias(node->var->lexeme.GetValue<std::string>(), sym_type));
}


void Semantic::Visit(NodeVarDecl *node) {
    for (auto &id: node->vars) {
        auto sym_type = GetSymType(dynamic_cast<NodeType *>(node->type));
        if (node->exp != nullptr) {
            node->exp->Accept(this);
            if (sym_type->is(node->exp->symbol_type)) {
                throw SemanticException("Types do not match");
            }
        }
        stack.Push(
                new SymbolVar(id->lexeme.GetValue<std::string>(), sym_type));
    }
}


void Semantic::Visit(NodeConstDecl *node) {
    SymbolType *sym_type;
    node->exp->Accept(this);
    if (node->type != nullptr) {
        sym_type = GetSymType(dynamic_cast<NodeType *>(node->type));
        if (sym_type->is(node->exp->symbol_type)) {
            throw SemanticException("Types do not match");
        }
    } else {
        sym_type = node->exp->symbol_type;
    }
    stack.Push(new SymbolVar(node->var->lexeme.GetValue<std::string>(), sym_type));

}


void Semantic::Visit(NodeParam *node) {
    auto sym_type = GetSymType(dynamic_cast<NodeType *>(node->type));
    for (auto &id: node->vars) {
        if (node->modifier == nullptr) {
            stack.Push(new SymbolParam(id->lexeme.GetValue<std::string>(), sym_type));
        } else if (node->modifier->lexeme == AllKeywords::CONST) {
            stack.Push(new SymbolConstParam(id->lexeme.GetValue<std::string>(), sym_type));
        } else if (node->modifier->lexeme == AllKeywords::VAR) {
            stack.Push(new SymbolVarParam(id->lexeme.GetValue<std::string>(), sym_type));
        }
    }
}


void Semantic::Visit(NodeProcDecl *node) {
    auto local = new SymbolTable();
    auto var_casted = dynamic_cast<NodeVar *>(node->var);
    auto symbol_proc = new SymbolProcedure(
            var_casted->lexeme.GetValue<std::string>(),
            local,
            dynamic_cast<NodeCompoundStatement *>(node->block)
    );
    stack.Push(local);
    for (auto param: node->params) param->Accept(this);
    node->block->Accept(this);
    stack.Pop();
    stack.Push(symbol_proc);
}


void Semantic::Visit(NodeFuncDecl *node) {
    auto local = new SymbolTable();
    auto var_casted = dynamic_cast<NodeVar *>(node->var);
    auto symbol_func = new SymbolFunction(
            var_casted->lexeme.GetValue<std::string>(),
            local,
            dynamic_cast<NodeCompoundStatement *>(node->block),
            nullptr
    );
    local->Push(symbol_func);
    local->Push(new SymbolVar("result", GetSymType(dynamic_cast<NodeType *>(node->type))));
    stack.Push(local);
    for (auto param: node->params) param->Accept(this);
    node->block->Accept(this);
    stack.Pop();
    local->Del(symbol_func->GetName());
    stack.Push(symbol_func);
}

SymbolTableStack Semantic::GetStack() {
    return stack;
}
