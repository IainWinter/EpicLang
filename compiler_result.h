#pragma once

#include "byte_code_types.h"

#include <vector>
#include <variant>

using CompilationErrorVariant = std::variant<std::monostate>;

struct CompilationError {
    CompilationErrorType type = CompilationErrorType::NONE;

    size_t start;
    size_t startLine;
    size_t startCharacterIndex;
    
    size_t stop;
    size_t stopLine;
    size_t stopCharacterIndex;

    CompilationErrorVariant info;
};

struct CompilationResults {
    std::vector<ByteCodeOp> operations;
    size_t main_code_index;
    CompilationError error;
};