#pragma once

#include "byte_code_types.h"

#include <vector>

struct Function {
    size_t code_index;
};

struct Program {
    std::vector<ByteCodeOp> operations;
    std::vector<Function> functions;
    size_t main_code_index;

    void print() const;

    void print(size_t highlight_code_index) const;
};