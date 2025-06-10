#pragma once

#include "compiler_result.h"

#include <vector>
#include <unordered_map>
#include <optional>

struct VariableDeclaration {
    Type type;
};

struct FunctionDeclaration {
    Type type;
    size_t code_index;
};

struct Scope {
    int parent;
    std::unordered_map<std::string, VariableDeclaration> variables;
    std::unordered_map<std::string, FunctionDeclaration> functions;

    Scope(int parent) 
        : parent (parent)
    {}
};

class ByteCodeGenerator {
public:
    ByteCodeGenerator();

    void push_scope();

    void pop_scope();

    void store_variable(const std::string& identifier, Type type);

    std::optional<VariableDeclaration> find_variable(const std::string& identifier) const;

    void store_function(const std::string& identifier, Type type, size_t code_index);

    std::optional<FunctionDeclaration> find_function(const std::string& identifier) const;

    void emit_placeholder();

    void emit(ByteCodeOp&& op);

    void patch(size_t code_index, ByteCodeOp&& op);

    void set_error(CompilationError&& error);

    size_t get_code_index() const;

    Type get_current_function_type() const;

    size_t get_op_count() const;

    OpType get_last_op_type() const;

    CompilationResults get_results() const;

private:
    std::vector<ByteCodeOp> m_ops;

    CompilationError m_error;

    std::unordered_map<int, Scope> m_scopes;
    std::vector<int> m_scope_stack;
    int m_next_scope = 0;

    Type m_current_function_type;
};