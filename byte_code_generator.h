#pragma once

#include "compiler_result.h"

#include <vector>
#include <unordered_map>
#include <optional>

enum class IdentifierType {
    FUNCTION,
    VARIABLE
};

struct Identifier {
    IdentifierType type;
    std::string name;
};

enum class ScopeType {
    GLOBAL,
    FUNCTION,
    BLOCK
};

struct Scope {
    ScopeType type;
    std::vector<Identifier> identifiers;
};

struct FunctionInfo {
    size_t code_index;
    Type return_type;
};

class ByteCodeGenerator {
public:
    // Operations

    void emit(ByteCodeOp&& operation);

    void emit_placeholder();

    void patch(size_t code_index, ByteCodeOp&& replacement_operation);

    size_t get_code_index() const;

    // Scopes, variables, and functions

    void scope_push(ScopeType type);

    CompilationErrorType scope_pop();

    CompilationErrorType scope_declare_identifier(IdentifierType type, const std::string& name);

    bool scope_is_identifier_declared(const std::string& name) const;

    ScopeType scope_get_current_type() const;

    CompilationErrorType variable_declare(Type type, const std::string& identifier);

    std::optional<Type> variable_get_type(const std::string& identifier) const;
    
    CompilationErrorType function_declare(Type return_type, const std::string& identifier, size_t argument_count);

    std::optional<FunctionInfo> function_get_info(const std::string& identifier) const;
    
    std::optional<FunctionInfo> function_get_current_info() const;

    // Error state

    void set_error(CompilationError&& error);

    // Getting results

    const CompilationError& get_error() const;

    Program get_program() const;

private:
    std::vector<ByteCodeOp> m_operations;
    std::vector<Scope> m_scopes;
    std::vector<Variable> m_global_variables;
    std::vector<Function> m_functions;

    CompilationError m_error;
};