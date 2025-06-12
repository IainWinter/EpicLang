#include "byte_code_generator.h"

ByteCodeGenerator::ByteCodeGenerator() {
    m_scopes.emplace(0, Scope(-1));
    m_scope_stack.push_back(0);
    m_next_scope += 1;
}

void ByteCodeGenerator::push_scope() {
    int cur = m_scope_stack.back();
    m_scopes.emplace(m_next_scope, Scope(cur));
    m_scope_stack.push_back(m_next_scope);
    m_next_scope += 1;
}

void ByteCodeGenerator::pop_scope() {
    int cur = m_scope_stack.back();
    m_scope_stack.pop_back();
    m_scopes.erase(cur);
}

void ByteCodeGenerator::store_variable(const std::string& identifier, Type type) {
    int cur = m_scope_stack.back();
    m_scopes.at(cur).variables.emplace(identifier, VariableDeclaration{type});
}

std::optional<VariableDeclaration> ByteCodeGenerator::find_variable(const std::string& identifier) const {
    int cur = m_scope_stack.back();
    const Scope* itr = &m_scopes.at(cur);

    while (itr->parent != -1 
        && itr->variables.find(identifier) == itr->variables.end()) 
    {
        itr = &m_scopes.at(itr->parent);
    }

    auto type = itr->variables.find(identifier);
    if (type == itr->variables.end()) {
        return std::nullopt;
    }

    return type->second;
}

void ByteCodeGenerator::store_function(const std::string& identifier, Type type, size_t code_index) {
    int cur = m_scope_stack.back();
    m_scopes.at(cur).functions.emplace(identifier, FunctionDeclaration{type, code_index});

    m_current_function_type = type;
}

std::optional<FunctionDeclaration> ByteCodeGenerator::find_function(const std::string& identifier) const {
    int cur = m_scope_stack.back();
    const Scope* itr = &m_scopes.at(cur);

    while (itr->parent != -1 
        && itr->functions.find(identifier) == itr->functions.end()) 
    {
        itr = &m_scopes.at(itr->parent);
    }

    auto type = itr->functions.find(identifier);
    if (type == itr->functions.end()) {
        return std::nullopt;
    }

    return type->second;
}

void ByteCodeGenerator::emit_placeholder() {
    m_ops.push_back({
        
    });
}

void ByteCodeGenerator::emit(ByteCodeOp&& op) {
    m_ops.push_back(std::move(op));
}

void ByteCodeGenerator::patch(size_t code_index, ByteCodeOp&& op) {
    m_ops.at(code_index) = std::move(op);
}

void ByteCodeGenerator::set_error(CompilationError&& error) {
    m_error = std::move(error);
}

size_t ByteCodeGenerator::get_code_index() const {
    return m_ops.size();
}

Type ByteCodeGenerator::get_current_function_type() const {
    return m_current_function_type;
}

size_t ByteCodeGenerator::get_op_count() const {
    return m_ops.size();
}

OpType ByteCodeGenerator::get_last_op_type() const {
    return m_ops.back().type;
}

CompilationResults ByteCodeGenerator::get_results() const {
    size_t main_code_index = 0;
    
    auto main_function_declaration = find_function("main");

    if (main_function_declaration.has_value()) {
        main_code_index = main_function_declaration.value().code_index;
    }

    return {
        {
            m_ops,
            {},
            main_code_index,
        },
        m_error
    };
}