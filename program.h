#pragma once

#include "byte_code_types.h"

#include <vector>

struct Variable {
    Type type;
    std::string name;
};

struct Function {
    size_t code_index;
    Type return_type;
    std::string name;
    size_t argument_count;
    std::vector<Variable> local_variables;
};

struct Program {
    std::vector<ByteCodeOp> operations;
    std::vector<Function> functions;
    size_t main_code_index;

    void print() const;

    void print(size_t highlight_code_index) const;
};