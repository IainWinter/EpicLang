#include "byte_code_generator.h"

// Operations

void ByteCodeGenerator::emit(ByteCodeOp&& operation) {
    m_operations.emplace_back(std::move(operation));
}

void ByteCodeGenerator::emit_placeholder() {
    m_operations.emplace_back(ByteCodeOp { OpType::PLACEHOLDER, {} });
}

void ByteCodeGenerator::patch(size_t code_index, ByteCodeOp&& replacement_operation) {
    m_operations.at(code_index) = std::move(replacement_operation);
}

size_t ByteCodeGenerator::get_code_index() const {
    return m_operations.size();
}

// Scopes, variables, and functions

void ByteCodeGenerator::scope_push(ScopeType type) {
    m_scopes.push_back({type});
}

CompilationErrorType ByteCodeGenerator::scope_pop() {
    if (m_scopes.size() == 0) {
        return CompilationErrorType::PARSE_ERROR;
    }

    m_scopes.pop_back();

    return CompilationErrorType::NONE;
}

CompilationErrorType ByteCodeGenerator::scope_declare_identifier(IdentifierType type, const std::string& name) {
    if (scope_is_identifier_declared(name)) {
        return CompilationErrorType::IDENTIFIED_ALREADY_DECLARED;
    }

    m_scopes.back().identifiers.push_back({type, name});

    return CompilationErrorType::NONE;
}

bool ByteCodeGenerator::scope_is_identifier_declared(const std::string& name) const {
    for (const Scope& scope : m_scopes) {
        for (const Identifier& identifier : scope.identifiers) {
            if (identifier.name == name) {
                return true;
            }
        }
    }

    return false;
}

ScopeType ByteCodeGenerator::scope_get_current_type() const {
    return m_scopes.back().type;
}

CompilationErrorType ByteCodeGenerator::variable_declare(Type type, const std::string& identifier) {
    CompilationErrorType err = scope_declare_identifier(IdentifierType::VARIABLE, identifier);

    if (err != CompilationErrorType::NONE) {
        return err;
    }

    Variable variable{};
    variable.type = type;
    variable.name = identifier;

    switch (scope_get_current_type()) {
        case ScopeType::GLOBAL: {
            m_global_variables.push_back(variable);
            break;
        }
        case ScopeType::BLOCK:
        case ScopeType::FUNCTION: {
            m_functions.back().local_variables.push_back(variable);
            break;
        }
    }

    return CompilationErrorType::NONE;
}

std::optional<Type> ByteCodeGenerator::variable_get_type(const std::string& identifier) const {
    // First check globals to guard against no functions yet
    for (const Variable& var : m_global_variables) {
        if (var.name == identifier) {
            return var.type;
        }
    }

    for (const Variable& var : m_functions.back().local_variables) {
        if (var.name == identifier) {
            return var.type;
        }
    }

    return std::nullopt;
}

CompilationErrorType ByteCodeGenerator::function_declare(Type return_type, const std::string& identifier, size_t argument_count) {
    CompilationErrorType err = scope_declare_identifier(IdentifierType::FUNCTION, identifier);

    if (err != CompilationErrorType::NONE) {
        return err;
    }

    Function function{};
    function.code_index = get_code_index();
    function.return_type = return_type;
    function.name = identifier;
    function.argument_count = argument_count;

    m_functions.push_back(function);

    return CompilationErrorType::NONE;
}

std::optional<FunctionInfo> ByteCodeGenerator::function_get_info(const std::string& identifier) const {
    if (!scope_is_identifier_declared(identifier)) {
        return std::nullopt;
    }

    for (const Function& function : m_functions) {
        if (function.name == identifier) {
            return FunctionInfo { function.code_index, function.return_type };
        }
    }

    return std::nullopt;
}

std::optional<FunctionInfo> ByteCodeGenerator::function_get_current_info() const {
    if (m_functions.size() > 0) {
        const Function& function = m_functions.back();
        return FunctionInfo { function.code_index, function.return_type };    
    }
    
    return std::nullopt;
}

// Errors

void ByteCodeGenerator::set_error(CompilationError&& error) {
    m_error = std::move(error);
}

// Getting results

const CompilationError& ByteCodeGenerator::get_error() const {
    return m_error;
}

Program ByteCodeGenerator::get_program() const {
    Program program;
    program.operations = m_operations;
    program.functions = m_functions;

    std::optional<FunctionInfo> main_function = function_get_info("main");

    if (main_function.has_value()) {
        program.main_code_index = main_function.value().code_index;
    }

    return program;
}

// ByteCodeGenerator::ByteCodeGenerator() {
//     m_scopes.emplace(0, Scope(-1));
//     m_scope_stack.push_back(0);
//     m_next_scope += 1;
// }

// void ByteCodeGenerator::push_scope() {
//     int cur = m_scope_stack.back();
//     m_scopes.emplace(m_next_scope, Scope(cur));
//     m_scope_stack.push_back(m_next_scope);
//     m_next_scope += 1;
// }

// void ByteCodeGenerator::pop_scope() {
//     int cur = m_scope_stack.back();
//     m_scope_stack.pop_back();
//     m_scopes.erase(cur);
// }

// void ByteCodeGenerator::store_variable(const std::string& identifier, Type type) {
//     int cur = m_scope_stack.back();
//     m_scopes.at(cur).variables.emplace(identifier, VariableDeclaration{type});
// }

// std::optional<VariableDeclaration> ByteCodeGenerator::find_variable(const std::string& identifier) const {
//     int cur = m_scope_stack.back();
//     const Scope* itr = &m_scopes.at(cur);

//     while (itr->parent != -1 
//         && itr->variables.find(identifier) == itr->variables.end()) 
//     {
//         itr = &m_scopes.at(itr->parent);
//     }

//     auto type = itr->variables.find(identifier);
//     if (type == itr->variables.end()) {
//         return std::nullopt;
//     }

//     return type->second;
// }

// FunctionDeclaration& ByteCodeGenerator::store_function(const std::string& identifier, Type type, size_t code_index) {
//     auto itr = m_functions.emplace(identifier, FunctionDeclaration{type, code_index, argument_count});
//     m_current_function_type = type;

//     return itr.first->second;
// }

// std::optional<FunctionDeclaration> ByteCodeGenerator::find_function(const std::string& identifier) const {
//     auto type = m_functions.find(identifier);
//     if (type == m_functions.end()) {
//         return std::nullopt;
//     }

//     return type->second;
// }

// void ByteCodeGenerator::emit_placeholder() {
//     m_operations.push_back({
        
//     });
// }

// void ByteCodeGenerator::emit(ByteCodeOp&& op) {
//     m_operations.push_back(std::move(op));
// }

// void ByteCodeGenerator::patch(size_t code_index, ByteCodeOp&& op) {
//     m_operations.at(code_index) = std::move(op);
// }

// void ByteCodeGenerator::set_error(CompilationError&& error) {
//     m_error = std::move(error);
// }

// CompilationError ByteCodeGenerator::get_error() const {
//     return m_error;
// }

// size_t ByteCodeGenerator::get_code_index() const {
//     return m_operations.size();
// }

// Type ByteCodeGenerator::get_current_function_type() const {
//     return m_current_function_type;
// }

// size_t ByteCodeGenerator::get_op_count() const {
//     return m_operations.size();
// }

// OpType ByteCodeGenerator::get_last_op_type() const {
//     return m_operations.back().type;
// }

// Program ByteCodeGenerator::generate_program() const {
//     // collect all functions
//     // the grammar says they can only be in the root scope so for now only look there

//     Program program;
//     program.operations = m_operations;

//     for (auto&& [identifier, function_declaration] : m_functions) {
//         Function function;
//         function.code_index = function_declaration.code_index;
//         function.identifier = identifier;
//         function.return_type = function_declaration.type;
//         function.argument_count 
//         Function function {
//             function_declaration.code_index,
//             function
//         };

//         program.functions.push_back({

//         });
//     }

//     auto main_function_declaration = find_function("main");

//     if (main_function_declaration.has_value()) {
//         program.main_code_index = main_function_declaration.value().code_index;
//     }

//     return {
//         m_operations,
//         {},
//         main_code_index,
//     };
// }