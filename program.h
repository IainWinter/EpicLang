#pragma once

#include "byte_code_types.h"

#include <vector>
#include <functional>
#include <optional>

struct Variable {
    Type type;
    std::string name;
};

// struct Type {

// };

struct Function {
    size_t code_index;
    Type return_type;
    std::string name;
    size_t argument_count;
    std::vector<Variable> local_variables;
};

struct ExternalFunction {
    Type return_type;
    std::string name;
    std::vector<Variable> arguments;
    std::function<TypeVariant(const std::vector<TypeVariant>&)> proc;
};

enum class FunctionType {
    SCRIPT,
    EXTERNAL
};

struct CallableFunctionInfo {
    FunctionType type;
    size_t function_index;
};

struct Program {
    std::vector<ByteCodeOp> operations;
    std::vector<Function> functions;
    std::vector<ExternalFunction> external_functions;
    size_t main_code_index;

    std::optional<CallableFunctionInfo> find_function(const std::string& identifier) const;

    void print() const;

    void print(size_t highlight_code_index) const;
};