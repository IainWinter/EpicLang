#include "unary_ops.h"

#include <unordered_map>

struct UnaryOpMapIn {
    Type right_type;
    UnaryOperatorType op;

    bool operator==(const UnaryOpMapIn& r) const {
        return right_type == r.right_type && op == r.op;
    }
};

struct InHash {
    std::size_t operator()(const UnaryOpMapIn& key) const {
        auto h1 = std::hash<int>{}(static_cast<int>(key.right_type));
        auto h2 = std::hash<int>{}(static_cast<int>(key.op));
        return h1 ^ (h2 << 1);
    }
};

static const std::unordered_map<UnaryOpMapIn, UnaryOpMapOut, InHash> s_valid = {
    // Right        Operator                    Operation              Result Type
    {{Type::BOOL,   UnaryOperatorType::NOT},    {OpType::NOT_BOOL,     Type::BOOL}},
    {{Type::INT,    UnaryOperatorType::NEGATE}, {OpType::NEGATE_INT,   Type::INT}},
    {{Type::FLOAT,  UnaryOperatorType::NEGATE}, {OpType::NEGATE_FLOAT, Type::FLOAT}}
}; 

std::optional<UnaryOpMapOut> map_unary_op(Type right_type, UnaryOperatorType op) {
    auto itr = s_valid.find(UnaryOpMapIn{right_type, op});
    if (itr == s_valid.end()) {
        return std::nullopt;
    }

    return itr->second;
}

CompilationErrorType map_unary_op_validate(Type right_type, UnaryOperatorType op) {
    if (right_type == Type::STRING) {
        return CompilationErrorType::MATH_OPERATION_ON_STRING;
    }

    return CompilationErrorType::NONE;
}