#pragma once

#include "byte_code_enum.h"

#include <variant>
#include <string>

using TypeVariant = std::variant<std::string, bool, int, float>;

struct ByteCodePushLiteralOp {
    Type type;
    TypeVariant value;

    bool operator==(const ByteCodePushLiteralOp& other) const {
        return type == other.type && value == other.value;
    }
};

struct ByteCodePushVariableOp {
    Type type;
    std::string identifier;

    bool operator==(const ByteCodePushVariableOp& other) const {
        return type == other.type && identifier == other.identifier;
    }
};

struct ByteCodeStoreVariableOp {
    Type type;
    std::string identifier;

    bool operator==(const ByteCodeStoreVariableOp& other) const {
        return type == other.type && identifier == other.identifier;
    }
};

struct ByteCodeCallFunctionOp {
    size_t code_index;

    bool operator==(const ByteCodeCallFunctionOp& other) const {
        return code_index == other.code_index;
    }
};

struct ByteCodeJumpOp {
    size_t code_index;

    bool operator==(const ByteCodeJumpOp& other) const {
        return code_index == other.code_index;
    }
};

struct ByteCodeOp {
    OpType type;
    std::variant<
        std::monostate, 
        ByteCodePushLiteralOp, 
        ByteCodePushVariableOp, 
        ByteCodeStoreVariableOp,
        ByteCodeCallFunctionOp,
        ByteCodeJumpOp
    > operand;

    bool operator==(const ByteCodeOp& other) const {
        return type == other.type && operand == other.operand;
    }
};