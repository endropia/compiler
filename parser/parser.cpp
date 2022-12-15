#include "parser.h"
#include <magic_enum.hpp>

void DrawIndent(std::ostream &os, int depth) {
    for (int i = 0; i < depth; ++i) {
        os << "   ";
    }
}

template<typename T>
void copy_elements(std::vector<Node *> &vec, const std::vector<T> &other_vec) {
    for (auto &item: other_vec) {
        vec.push_back(item);
    }
}

Node *Parser::Program() {
    Node *name = nullptr;
    if (lexeme == AllKeywords::PROGRAM) {
        lexeme = lexer.GetLexeme();
        if (lexeme != LexemeType::Identifier) {
            throw ParserException(lexeme.GetPos(), "Identifier expected");
        }
        name = new NodeVar(lexeme);
        lexeme = lexer.GetLexeme();
        if (lexeme != Separators::SEMICOLON) {
            throw ParserException(lexeme.GetPos(), "';' expected");
        }
        lexeme = lexer.GetLexeme();
    }
    auto block = Block(true);
    if (lexeme != Separators::PERIOD) {
        throw ParserException(lexeme.GetPos(), "'.' expected");
    }
    return new NodeProgram(name, block);
};

Node *Parser::Block(bool parse_functions) {
    std::vector<Node *> decls;
    while (true) {
        if (lexeme == AllKeywords::CONST) {
            lexeme = lexer.GetLexeme();
            copy_elements(decls, ConstDeclPart());
        } else if (lexeme == AllKeywords::VAR) {
            lexeme = lexer.GetLexeme();
            copy_elements(decls, VarDeclPart());
        } else if (lexeme == AllKeywords::TYPE) {
            lexeme = lexer.GetLexeme();
            copy_elements(decls, TypeDeclPart());
        } else if (lexeme == AllKeywords::FUNCTION) {
            lexeme = lexer.GetLexeme();
            decls.push_back(Function());
        } else if (lexeme == AllKeywords::PROCEDURE) {
            lexeme = lexer.GetLexeme();
            decls.push_back(Procedure());
        } else {
            break;
        }
    }

    if (lexeme != AllKeywords::BEGIN) {
        throw ParserException(lexeme.GetPos(), "'begin' expected");
    }
    lexeme = lexer.GetLexeme();
    auto stmts = CompoundStatement();
    return new NodeBlock(decls, stmts);
}

Node *Parser::Procedure() {
    if (lexeme != LexemeType::Identifier) {
        throw ParserException(lexeme.GetPos(), "Identifier expected");
    }
    auto id = new NodeVar(lexeme);
    lexeme = lexer.GetLexeme();
    if (lexeme != Separators::LPARENTHESIS) {
        throw ParserException(lexeme.GetPos(), "'(' expected");
    }
    lexeme = lexer.GetLexeme();
    auto params = FunctionParams(false);
    if (lexeme != Separators::RPARENTHESIS) {
        throw ParserException(lexeme.GetPos(), "')' expected");
    }
    lexeme = lexer.GetLexeme();
    if (lexeme != Separators::SEMICOLON) {
        throw ParserException(lexeme.GetPos(), "';' expected");
    }
    lexeme = lexer.GetLexeme();
    auto block = Block(false);
    if (lexeme != Separators::SEMICOLON) {
        throw ParserException(lexeme.GetPos(), "';' expected");
    }
    lexeme = lexer.GetLexeme();
    return new NodeProcDecl(id, params, block);
}

Node *Parser::Function() {
    if (lexeme != LexemeType::Identifier) {
        throw ParserException(lexeme.GetPos(), "Identifier expected");
    }
    auto id = new NodeVar(lexeme);
    lexeme = lexer.GetLexeme();
    if (lexeme != Separators::LPARENTHESIS) {
        throw ParserException(lexeme.GetPos(), "'(' expected");
    }
    lexeme = lexer.GetLexeme();
    auto params = FunctionParams(false);
    if (lexeme != Separators::RPARENTHESIS) {
        throw ParserException(lexeme.GetPos(), "')' expected");
    }
    lexeme = lexer.GetLexeme();
    if (lexeme != Separators::COLON) {
        throw ParserException(lexeme.GetPos(), "':' expected");
    }
    lexeme = lexer.GetLexeme();
    auto type = Type();
    if (lexeme != Separators::SEMICOLON) {
        throw ParserException(lexeme.GetPos(), "';' expected");
    }
    lexeme = lexer.GetLexeme();
    auto block = Block(false);
    if (lexeme != Separators::SEMICOLON) {
        throw ParserException(lexeme.GetPos(), "';' expected");;
    }
    lexeme = lexer.GetLexeme();
    return new NodeFuncDecl(id, params, block, type);
}

std::vector<Node *> Parser::FunctionParams(bool required) {
    std::vector<Node *> result;
    if (required or (lexeme != Separators::RPARENTHESIS)) {
        result.push_back(FunctionParam());
        while (lexeme == Separators::SEMICOLON) {
            lexeme = lexer.GetLexeme();
            result.push_back(FunctionParam());
        }
    }
    if (required and result.empty()) {
        throw ParserException(lexeme.GetPos(), "Index expected");
    }
    return result;
}

Node *Parser::FunctionParam() {
    NodeKeyword *mod = nullptr;
    if (lexeme == AllKeywords::CONST or lexeme == AllKeywords::VAR) {
        mod = new NodeKeyword(lexeme);
        lexeme = lexer.GetLexeme();
    }
    std::vector<NodeVar *> vars;
    if (lexeme != LexemeType::Identifier) {
        throw ParserException(lexeme.GetPos(), "Identifier expected");
    }
    vars.push_back(new NodeVar(lexeme));
    lexeme = lexer.GetLexeme();
    while (lexeme == Separators::COMMA) {
        if (lexeme != LexemeType::Identifier) {
            throw ParserException(lexeme.GetPos(), "Identifier expected");
        }
        vars.push_back(new NodeVar(lexeme));
        lexeme = lexer.GetLexeme();
    }
    if (lexeme != Separators::COLON) {
        throw ParserException(lexeme.GetPos(), "':' expected");
    }
    lexeme = lexer.GetLexeme();
    return new NodeParam(mod, vars, Type());
}

Node *Parser::Expression() {
    auto left = SimpleExpression();
    auto lex = lexeme;
    while (lex == Operators::EQUAL or
           lex == Operators::UNEQUAL or
           lex == Operators::GREATER or
           lex == Operators::GREATEREQUAL or
           lex == Operators::LESS or
           lex == Operators::LESSEQUAL or
           lex == AllKeywords::IN) {
        lexeme = lexer.GetLexeme();
        left = new NodeBinaryOperation(lex, left, SimpleExpression());
        lex = lexeme;
    }
    return left;
}

Node *Parser::SimpleExpression() {
    auto left = Term();
    auto lex = lexeme;
    while (lex == Operators::ADD or
           lex == Operators::SUBSTRACT or
           lex == AllKeywords::OR or
           lex == AllKeywords::XOR) {
        lexeme = lexer.GetLexeme();
        left = new NodeBinaryOperation(lex, left, Term());
        lex = lexeme;
    }
    return left;
}

Node *Parser::Term() {
    auto left = SimpleTerm();
    auto lex = lexeme;
    while (lex == Operators::MULTIPLY or
           lex == Operators::DIVISION or
           lex == AllKeywords::DIV or
           lex == AllKeywords::MOD or
           lex == AllKeywords::AND or
           lex == AllKeywords::SHR or
           lex == AllKeywords::SHL) {
        lexeme = lexer.GetLexeme();
        left = new NodeBinaryOperation(lex, left, SimpleTerm());
        lex = lexeme;
    }
    return left;
}

Node *Parser::SimpleTerm() {
    if (lexeme == Operators::ADD or
        lexeme == Operators::SUBSTRACT or
        lexeme == AllKeywords::NOT
            ) {
        auto op = lexeme;
        lexeme = lexer.GetLexeme();
        return new NodeUnaryOperation(op, SimpleTerm());
    }
    return Factor();
}

Node *Parser::Factor() {
    auto lex = lexeme;
    if (lex == LexemeType::Integer or lex == LexemeType::Double) {
        lexeme = lexer.GetLexeme();
        return new NodeNumber(lex);
    }
    if (lex == LexemeType::String) {
        lexeme = lexer.GetLexeme();
        return new NodeString(lex);
    }
    if (lex == LexemeType::Identifier) {
        Node *res = new NodeVar(lex);
        while (true) {
            lexeme = lexer.GetLexeme();
            if (lexeme == Separators::PERIOD) {
                lexeme = lexer.GetLexeme();
                if (lexeme != LexemeType::Identifier) {
                    throw ParserException(lexeme.GetPos(), " Identifier expected");
                }
                res = new NodeRecordAccess(res, new NodeVar(lexeme));
            } else if (lexeme == Separators::LPARENTHESIS) {
                lexeme = lexer.GetLexeme();
                auto params = ListExpressions(false);
                if (lexeme != Separators::RPARENTHESIS) {
                    throw ParserException(lexeme.GetPos(), "')' expected");
                }
                res = new NodeCallAccess(res, params);
            } else if (lexeme == Separators::LSBRACKET) {
                lexeme = lexer.GetLexeme();
                auto params = ListExpressions(true);
                if (lexeme != Separators::RSBRACKET) {
                    throw ParserException(lexeme.GetPos(), "']' expected");
                }
                res = new NodeArrayAccess(res, params);
            } else {
                break;
            }
        }
        return res;
    }

    if (lex == Separators::LPARENTHESIS) {
        lexeme = lexer.GetLexeme();
        auto exp = Expression();
        if (lexeme != Separators::RPARENTHESIS)
            throw ParserException(lexeme.GetPos(), "')' expected");
        lexeme = lexer.GetLexeme();
        return exp;
    }

    throw ParserException(lexeme.GetPos(), "Factor expected");
}

std::vector<Node *> Parser::ListExpressions(bool required) {
    std::vector<Node *> result;
    if (required or (lexeme != Separators::RPARENTHESIS)) {
        result.push_back(Expression());
        while (lexeme == Separators::COMMA) {
            lexeme = lexer.GetLexeme();
            result.push_back(Expression());
        }
    }
    if (required and result.empty()) {
        throw ParserException(lexeme.GetPos(), "Index expected");
    }
    return result;
}

NodeRange *Parser::IndexRange() {
    auto exp_first = Expression();
    if (lexeme != Separators::DOUBLEPERIOD) {
        throw ParserException(lexeme.GetPos(), "Double period expected");
    }
    lexeme = lexer.GetLexeme();
    auto exp_second = Expression();
    return new NodeRange(exp_first, exp_second);
}

std::vector<NodeRange *> Parser::IndexRanges() {
    std::vector<NodeRange *> ranges;
    ranges.push_back(IndexRange());
    while (lexeme == Separators::COMMA) {
        lexeme = lexer.GetLexeme();
        ranges.push_back(IndexRange());
    }
    return ranges;
}

Node *Parser::ArrayType() {
    lexeme = lexer.GetLexeme();
    if (lexeme != Separators::LSBRACKET) {
        throw ParserException(lexeme.GetPos(), "'[' expected");
    }
    lexeme = lexer.GetLexeme();
    auto ranges = IndexRanges();
    if (lexeme != Separators::RSBRACKET) {
        throw ParserException(lexeme.GetPos(), "']' expected");
    }
    lexeme = lexer.GetLexeme();
    if (lexeme != AllKeywords::OF) {
        throw ParserException(lexeme.GetPos(), "'of' expected");
    }
    lexeme = lexer.GetLexeme();
    auto type = Type();
    return new NodeArrayType(type, ranges);
}

Node *Parser::Type() {
    if (lexeme == Identifier) {
        auto id = lexeme;
        lexeme = lexer.GetLexeme();
        return new NodeSimpleType(new NodeVar(id));
    }
    if (lexeme == AllKeywords::STRING) {
        auto keyword = lexeme;
        lexeme = lexer.GetLexeme();
        return new NodeSimpleType(new NodeKeyword(keyword));
    }
    if (lexeme == AllKeywords::ARRAY) {
        return ArrayType();
    }
    if (lexeme == AllKeywords::RECORD) {
        return RecordType();
    }
    throw ParserException(lexeme.GetPos(), "Illegal type");
}

std::vector<Node *> Parser::ListIdent() {
    std::vector<Node *> list;
    do {
        if (lexeme != Identifier) {
            throw ParserException(lexeme.GetPos(), "id expected");
        }
        list.push_back(new NodeVar(lexeme));
        lexeme = lexer.GetLexeme();
        if (lexeme != Separators::COMMA) {
            break;
        }
        lexeme = lexer.GetLexeme();
    } while (true);
    return list;
}

Node *Parser::Field() {
    auto ident_list = ListIdent();
    if (lexeme != Separators::COLON) {
        throw ParserException(lexeme.GetPos(), "':' expected");
    }
    lexeme = lexer.GetLexeme();
    return new NodeField(ident_list, Type());
}

std::vector<Node *> Parser::Fields() {
    std::vector<Node *> fields;
    do {
        if (lexeme == AllKeywords::END) {
            lexeme = lexer.GetLexeme();
            return fields;
        }
        fields.push_back(Field());
        if (lexeme != Separators::SEMICOLON) {
            break;
        }
        lexeme = lexer.GetLexeme();
    } while (true);
    return fields;
}

Node *Parser::RecordType() {
    lexeme = lexer.GetLexeme();
    auto fields = Fields();
    return new NodeRecordType(fields);
}

NodeStatement *Parser::SimpleStatement() {
    auto exp1 = Expression();
    if (dynamic_cast<NodeCallAccess *>(exp1) != nullptr) {
        return new NodeCallStatement(dynamic_cast<NodeCallAccess *>(exp1));
    }
    if (lexeme != Operators::ASSIGN and
        lexeme != Operators::ADDASSIGN and
        lexeme != Operators::SUBSTRACTASSIGN and
        lexeme != Operators::MULTIPLYASSIGN and
        lexeme != Operators::DIVISIONASSIGN) {
        throw ParserException(lexeme.GetPos(), "Assignment symbol expected");
    }
    auto op = lexeme;
    lexeme = lexer.GetLexeme();
    auto exp2 = Expression();
    return new NodeAssignmentStatement(op, exp1, exp2);
}

NodeStatement *Parser::CompoundStatement() {
    std::vector<NodeStatement *> statements;
    while (true) {
        bool a = false;
        while (lexeme == Separators::SEMICOLON) {
            lexeme = lexer.GetLexeme();
            a = true;
        }
        if (lexeme == AllKeywords::END) {
            lexeme = lexer.GetLexeme();
            break;
        }
        if (statements.size() != 0 and !a) {
            throw ParserException(lexeme.GetPos(), "Expected ';'");
        }
        statements.push_back(Statement());
    }
    return new NodeCompoundStatement(statements);
}

NodeStatement *Parser::Statement() {
    if (lexeme == AllKeywords::BEGIN) {
        lexeme = lexer.GetLexeme();
        return CompoundStatement();
    }
    if (lexeme == AllKeywords::IF) {
        lexeme = lexer.GetLexeme();
        return IfStatement();
    }
    if (lexeme == AllKeywords::FOR) {
        return ForStatement();
    }
    if (lexeme == AllKeywords::WHILE) {
        return WhileStatement();
    }
    return SimpleStatement();
}

NodeStatement *Parser::IfStatement() {
    auto exp = Expression();
    if (lexeme != AllKeywords::THEN) {
        throw ParserException(lexeme.GetPos(), "'then' expected");
    }
    lexeme = lexer.GetLexeme();
    auto stmt = Statement();
    NodeStatement *else_stmt = nullptr;
    if (lexeme == AllKeywords::ELSE) {
        lexeme = lexer.GetLexeme();
        else_stmt = Statement();
    }
    return new NodeIfStatement(exp, stmt, else_stmt);
}

NodeStatement *Parser::WhileStatement() {
    lexeme = lexer.GetLexeme();
    auto exp = Expression();
    if (lexeme != AllKeywords::DO) {
        throw ParserException(lexeme.GetPos(), "'do' expected");
    }
    lexeme = lexer.GetLexeme();
    auto stmt = Statement();

    return new NodeWhileStatement(exp, stmt);
}

NodeStatement *Parser::ForStatement() {
    lexeme = lexer.GetLexeme();
    if (lexeme != Identifier) {
        throw ParserException(lexeme.GetPos(), "Identifier expected");
    }
    auto var = new NodeVar(lexeme);
    lexeme = lexer.GetLexeme();
    if (lexeme != Operators::ASSIGN) {
        throw ParserException(lexeme.GetPos(), "Assign expected");
    }
    lexeme = lexer.GetLexeme();
    auto exp_begin = Expression();
    if (lexeme != AllKeywords::TO and
        lexeme != AllKeywords::DOWNTO) {
        throw ParserException(lexeme.GetPos(), "'to' or 'downto' expected");
    }
    auto dir = new NodeKeyword(lexeme);
    lexeme = lexer.GetLexeme();
    auto exp_end = Expression();
    if (lexeme != AllKeywords::DO) {
        throw ParserException(lexeme.GetPos(), "Expected 'do'");
    }
    lexeme = lexer.GetLexeme();
    auto stmt = Statement();
    return new NodeForStatement(stmt, var, exp_begin, dir, exp_end);

}

std::vector<NodeTypeDecl *> Parser::TypeDeclPart() {
    std::vector<NodeTypeDecl *> type_declarations;
    type_declarations.push_back(TypeDecl());
    while (lexeme == LexemeType::Identifier) {
        type_declarations.push_back(TypeDecl());
    }
    return type_declarations;
}

NodeTypeDecl *Parser::TypeDecl() {
    if (lexeme != LexemeType::Identifier) {
        throw ParserException(lexeme.GetPos(), "Identifier expected");
    }
    auto id = new NodeVar(lexeme);
    lexeme = lexer.GetLexeme();
    if (lexeme != Operators::EQUAL) {
        throw ParserException(lexeme.GetPos(), "'=' expected");
    }
    lexeme = lexer.GetLexeme();
    auto type = Type();
    if (lexeme != Separators::SEMICOLON) {
        throw ParserException(lexeme.GetPos(), "';' expected");
    }
    lexeme = lexer.GetLexeme();
    return new NodeTypeDecl(id, type);
}

NodeConstDecl *Parser::ConstDecl() {
    if (lexeme != LexemeType::Identifier) {
        throw ParserException(lexeme.GetPos(), "Identifier expected");
    }
    auto var = new NodeVar(lexeme);
    lexeme = lexer.GetLexeme();
    Node *type = nullptr;
    if (lexeme == Separators::COLON) {
        lexeme = lexer.GetLexeme();
        type = Type();
    }
    if (lexeme != Operators::EQUAL) {
        throw ParserException(lexeme.GetPos(), "'=' expected");
    }
    lexeme = lexer.GetLexeme();
    auto exp = Expression();
    if (lexeme != Separators::SEMICOLON) {
        throw ParserException(lexeme.GetPos(), "';' expected");
    }
    lexeme = lexer.GetLexeme();
    return new NodeConstDecl(var, type, exp);
}

std::vector<NodeConstDecl *> Parser::ConstDeclPart() {
    std::vector<NodeConstDecl *> const_declarations;
    const_declarations.push_back(ConstDecl());
    while (lexeme == LexemeType::Identifier) {
        const_declarations.push_back(ConstDecl());
    }
    return const_declarations;
}

NodeVarDecl *Parser::VarDecl() {
    std::vector<NodeVar *> vars;
    while (true) {
        if (lexeme != LexemeType::Identifier) {
            throw ParserException(lexeme.GetPos(), "Identifier expected");
        }
        vars.push_back(new NodeVar(lexeme));
        lexeme = lexer.GetLexeme();
        if (lexeme == Separators::COMMA) {
            lexeme = lexer.GetLexeme();
        } else {
            break;
        }
    }
    if (lexeme != Separators::COLON) {
        throw ParserException(lexeme.GetPos(), "':' expected");
    }
    lexeme = lexer.GetLexeme();
    auto type = Type();
    Node *exp = nullptr;
    if (lexeme == Operators::EQUAL) {
        if (vars.size() == 1) {
            lexeme = lexer.GetLexeme();
            exp = Expression();
        } else {
            throw ParserException(lexeme.GetPos(), "Only ine variable can be initialized");
        }
    }
    if (lexeme != Separators::SEMICOLON) {
        throw ParserException(lexeme.GetPos(), "';' expected");
    }
    lexeme = lexer.GetLexeme();
    return new NodeVarDecl(vars, type, exp);
}

std::vector<NodeVarDecl *> Parser::VarDeclPart() {
    std::vector<NodeVarDecl *> var_declarations;
    var_declarations.push_back(VarDecl());
    while (lexeme == LexemeType::Identifier) {
        var_declarations.push_back(VarDecl());
    }
    return var_declarations;
}


void NodeBinaryOperation::DrawTree(std::ostream &os, int depth) {
    os << lexeme.GetRaw() << "\n";
    DrawIndent(os, depth + 1);
    left->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 1);
    right->DrawTree(os, depth + 1);
}

void NodeString::DrawTree(std::ostream &os, int depth) {
    os << lexeme.GetValue<std::string>() << "\n";
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

void NodeUnaryOperation::DrawTree(std::ostream &os, int depth) {
    os << op.GetRaw() << "\n";
    DrawIndent(os, depth);
    operand->DrawTree(os, depth + 1);
}

void NodeRecordAccess::DrawTree(std::ostream &os, int depth) {
    rec->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 1);
    field->DrawTree(os, depth + 1);
}

void NodeCallAccess::DrawTree(std::ostream &os, int depth) {
    os << "call" << "\n";
    DrawIndent(os, depth + 1);
    rec->DrawTree(os, depth + 1);
    for (int i = 0; i < params.size(); i++) {
        DrawIndent(os, depth + 2);
        params[i]->DrawTree(os, depth + 2);
    }
}

void NodeArrayAccess::DrawTree(std::ostream &os, int depth) {
    os << "array" << "\n";
    DrawIndent(os, depth + 1);
    rec->DrawTree(os, depth + 1);
    for (int i = 0; i < params.size(); i++) {
        DrawIndent(os, depth + 1);
        params[i]->DrawTree(os, depth + 1);
    }
}

void NodeSimpleType::DrawTree(std::ostream &os, int depth) {
    os << "type: ";
    type->DrawTree(os, depth + 1);
}

void NodeRange::DrawTree(std::ostream &os, int depth) {
    os << "range" << "\n";
    DrawIndent(os, depth + 1);
    exp_first->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 1);
    exp_second->DrawTree(os, depth + 1);
}

void NodeArrayType::DrawTree(std::ostream &os, int depth) {
    os << "array" << "\n";
    DrawIndent(os, depth);
    type->DrawTree(os, depth + 1);
    for (int i = 0; i < ranges.size(); i++) {
        DrawIndent(os, depth);
        ranges[i]->DrawTree(os, depth);
    }

}

void NodeField::DrawTree(std::ostream &os, int depth) {
    for (auto &i: id) {
        DrawIndent(os, depth);
        i->DrawTree(os, depth);
        DrawIndent(os, depth + 1);
        type->DrawTree(os, depth + 1);
    }
}

void NodeRecordType::DrawTree(std::ostream &os, int depth) {
    os << "record\n";
    for (auto &i: field) {
        i->DrawTree(os, depth);
    }
}

void NodeCompoundStatement::DrawTree(std::ostream &os, int depth) {
    os << "stmts:\n";
    if (statements.empty()) {
        DrawIndent(os, depth + 1);
        os << "empty";
    }
    for (auto &statement: statements) {
        DrawIndent(os, depth + 1);
        statement->DrawTree(os, depth + 1);
    }
}

void NodeIfStatement::DrawTree(std::ostream &os, int depth) {
    os << "if\n";
    DrawIndent(os, depth + 1);
    exp->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 1);
    statement->DrawTree(os, depth + 1);
    if (else_statement) {
        DrawIndent(os, depth + 1);
        os << "else\n";
        DrawIndent(os, depth + 2);
        statement->DrawTree(os, depth + 2);
    }
}

void NodeWhileStatement::DrawTree(std::ostream &os, int depth) {
    os << "while\n";
    DrawIndent(os, depth + 1);
    exp->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 2);
    statement->DrawTree(os, depth + 2);
}

void NodeForStatement::DrawTree(std::ostream &os, int depth) {
    os << "for\n";
    DrawIndent(os, depth + 1);
    var->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 1);
    direction->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 2);
    exp_begin->DrawTree(os, depth + 2);
    DrawIndent(os, depth + 2);
    exp_end->DrawTree(os, depth + 2);
    DrawIndent(os, depth + 3);
    statement->DrawTree(os, depth + 3);
}

void NodeBlock::DrawTree(std::ostream &os, int depth) {
    for (auto &decl: decls) {
        DrawIndent(os, depth);
        decl->DrawTree(os, depth);
    }
    DrawIndent(os, depth);
    comp_stmt->DrawTree(os, depth);
}

void NodeProgram::DrawTree(std::ostream &os, int depth) {
    os << "program : ";
    if (name != nullptr) {
        name->DrawTree(os, depth);
    } else {
        os << "Unnamed program\n";
    }
    block->DrawTree(os, depth);
}

void NodeTypeDecl::DrawTree(std::ostream &os, int depth) {
    os << "alias\n";
    DrawIndent(os, depth + 1);
    type->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 1);
    var->DrawTree(os, depth + 1);
}

void NodeVarDecl::DrawTree(std::ostream &os, int depth) {
    os << "var: \n";
    for (auto &var: vars) {
        DrawIndent(os, depth + 1);
        var->DrawTree(os, depth + 1);
    }
    DrawIndent(os, depth + 1);
    type->DrawTree(os, depth + 1);
    if (exp) {
        DrawIndent(os, depth + 1);
        exp->DrawTree(os, depth + 1);
    }

}

void NodeConstDecl::DrawTree(std::ostream &os, int depth) {
    os << "const:\n";
    DrawIndent(os, depth + 1);
    vars->DrawTree(os, depth + 1);
    if (type) {
        DrawIndent(os, depth + 1);
        type->DrawTree(os, depth + 1);
    }
    DrawIndent(os, depth + 1);
    exp->DrawTree(os, depth + 1);
}

void NodeParam::DrawTree(std::ostream &os, int depth) {
    type->DrawTree(os, depth);
    if (modifier) {
        DrawIndent(os, depth);
        modifier->DrawTree(os, depth);
    }

    for (auto &var: vars) {
        DrawIndent(os, depth);
        var->DrawTree(os, depth);
    }
}

void NodeProcDecl::DrawTree(std::ostream &os, int depth) {
    os << "procedure:\n";
    DrawIndent(os, depth + 1);
    var->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 1);
    os << "parameters: \n";
    for (auto &param: params) {
        DrawIndent(os, depth + 2);
        param->DrawTree(os, depth + 2);
    }
    block->DrawTree(os, depth + 1);
}

void NodeFuncDecl::DrawTree(std::ostream &os, int depth) {
    os << "function:\n";
    DrawIndent(os, depth + 1);
    var->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 1);
    type->DrawTree(os, depth + 1);
    DrawIndent(os, depth + 1);
    os << "parameters: \n";
    for (auto &param: params) {
        DrawIndent(os, depth + 2);
        param->DrawTree(os, depth + 2);
    }
    block->DrawTree(os, depth + 1);

}

