#include "symbol.h"

#include <iomanip>

std::string Symbol::GetName() { return name; }

Symbol *SymbolTable::Get(std::string name) {
    if (!data.contains(name)) {
        throw SemanticException("Id is undeclared");
    }
    return data[name];
}

void SymbolTable::Push(std::string name, Symbol *symbol) {
    if (data.contains(name)) {
        throw SemanticException("It is already declared");
    }
    ordered.push_back(name);
    data[name] = symbol;
}

void SymbolTable::Push(Symbol *symbol) {
    if (data.contains(symbol->GetName())) {
        throw SemanticException("Id is already declared");
    }
    ordered.push_back(symbol->GetName());
    data[symbol->GetName()] = symbol;
}

void SymbolTable::Del(std::string name) {
    if (!data.contains(name)) {
        throw SemanticException("Id is not declared");
    }
    ordered.erase(std::find(ordered.begin(), ordered.end(), name));
    data.erase(name);
}

void SymbolTable::Draw(std::ostream &os, int depth) {
    if (depth <= 0) {
        os << std::setw(10) << std::left << "scope"
           << std::setw(30) << std::left << "name"
           << std::setw(20) << std::left << "class"
           << "\n"
           << std::string(60, '-') << "\n";
    }
    for (auto &name: ordered) {
        auto sym = data[name];
        os << std::setw(10) << std::left << depth
           << std::setw(30) << std::left << sym->GetName()
           << std::setw(20) << std::left << sym->GetClass() << "\n";
        if (auto proc = dynamic_cast<SymbolProcedure *>(sym)) {
            proc->locals->Draw(os, depth + 1);
        }
    }
}

bool SymbolTable::Contains(std::string name) { return data.contains(name); }

Symbol *SymbolTableStack::get(std::string name) {
    for (auto &it: data) {
        if (it->Contains(name)) {
            return it->Get(name);
        }
    }
    throw SemanticException("Id is not declared");
}

void SymbolTableStack::Push(std::string name, Symbol *symbol) {
    if (ContainsInScope(name)) {
        throw SemanticException("Id is already declared in scope");
    }
    data.back()->Push(name, symbol);
}

void SymbolTableStack::Push(Symbol *symbol) {
    if (ContainsInScope(symbol->GetName())) {
        throw SemanticException("Id is already declared in scope");
    }
    data.back()->Push(symbol->GetName(), symbol);
}

void SymbolTableStack::CreateTable() {
    data.push_back(new SymbolTable());
}

bool SymbolTableStack::ContainsInScope(std::string name) {
    for (auto &it: data) {
        if (it->Contains(name)) {
            return true;
        }
    }
    return false;
}

void SymbolTableStack::Push(SymbolTable *table) {
    data.push_back(table);
}

void SymbolTableStack::Pop() {
    data.pop_back();
}

void SymbolTableStack::Draw(std::ostream &os) {
    int i = 0;
    for (auto &table: data) {
        table->Draw(os, i);
        i++;
    }
}

SymbolType *SymbolType::Resolve() {
    return this;
}

bool SymbolType::is(SymbolType *b) {
    return Resolve()->GetName() == b->Resolve()->GetName();
}

int SymbolProcedure::GetCountOfArguments() {
    int answer = 0;
    for (auto name: locals->ordered) {
        if (dynamic_cast<SymbolParam *>(locals->Get(name)) != nullptr) {
            answer++;
        }
    }
    return answer;
}

SymbolType *SymbolAlias::Resolve() {
    return original;
}

bool SymbolRecord::is(SymbolType *b) {
    auto b_casted = dynamic_cast<SymbolRecord *>(b);
    if (b_casted == nullptr) { return false; }
    for (auto &name: fields->ordered) {
        auto a_field = dynamic_cast<SymbolVar *>(fields->Get(name));
        if (!b_casted->fields->Contains(name)) { return false; }
        auto b_field = dynamic_cast<SymbolVar *>(b_casted->fields->Get(name));
        if (!b_field->type->is(a_field->type)) { return false; }
    }
    return true;
}

bool SymbolArray::is(SymbolType *b) {
    auto b_casted = dynamic_cast<SymbolArray *>(b);
    if (b_casted == nullptr) { return false; }
    return type->is(b_casted);
}
