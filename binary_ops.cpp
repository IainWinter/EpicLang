#include "binary_ops.h"

#include <unordered_map>

struct BinaryOpMapIn {
    Type left_type;
    Type right_type;
    BinaryOperatorType op;

    bool operator==(const BinaryOpMapIn& r) const {
        return left_type == r.left_type && right_type == r.right_type && op == r.op;
    }
};

struct InHash {
    std::size_t operator()(const BinaryOpMapIn& key) const {
        auto h1 = std::hash<int>{}(static_cast<int>(key.left_type));
        auto h2 = std::hash<int>{}(static_cast<int>(key.right_type));
        auto h3 = std::hash<int>{}(static_cast<int>(key.op));
        return h1 ^ (h2 << 1) ^ (h3 << 2);  // Simple XOR combine
    }
};

static const std::unordered_map<BinaryOpMapIn, BinaryOpMapOut, InHash> s_valid = {
    // ADD
    {{Type::INT,   Type::INT,   BinaryOperatorType::ADD}, {OpType::ADD_INT,    Type::INT}},
    {{Type::FLOAT, Type::FLOAT, BinaryOperatorType::ADD}, {OpType::ADD_FLOAT,  Type::FLOAT}},
    {{Type::INT2,  Type::INT2,  BinaryOperatorType::ADD}, {OpType::ADD_INT2,   Type::INT2}},
    {{Type::FLOAT2,Type::FLOAT2,BinaryOperatorType::ADD}, {OpType::ADD_FLOAT2, Type::FLOAT2}},

    // SUBTRACT
    {{Type::INT,   Type::INT,   BinaryOperatorType::SUBTRACT}, {OpType::SUBTRACT_INT,    Type::INT}},
    {{Type::FLOAT, Type::FLOAT, BinaryOperatorType::SUBTRACT}, {OpType::SUBTRACT_FLOAT,  Type::FLOAT}},
    {{Type::INT2,  Type::INT2,  BinaryOperatorType::SUBTRACT}, {OpType::SUBTRACT_INT2,   Type::INT2}},
    {{Type::FLOAT2,Type::FLOAT2,BinaryOperatorType::SUBTRACT}, {OpType::SUBTRACT_FLOAT2, Type::FLOAT2}},

    // MULTIPLY
    {{Type::INT,   Type::INT,   BinaryOperatorType::MULTIPLY}, {OpType::MULTIPLY_INT,          Type::INT}},
    {{Type::FLOAT, Type::FLOAT, BinaryOperatorType::MULTIPLY}, {OpType::MULTIPLY_FLOAT,        Type::FLOAT}},
    {{Type::INT2,  Type::INT2,  BinaryOperatorType::MULTIPLY}, {OpType::MULTIPLY_INT2,         Type::INT2}},
    {{Type::INT2,  Type::INT,   BinaryOperatorType::MULTIPLY}, {OpType::MULTIPLY_INT2_INT,     Type::INT2}},
    {{Type::FLOAT2,Type::FLOAT2,BinaryOperatorType::MULTIPLY}, {OpType::MULTIPLY_FLOAT2,       Type::FLOAT2}},
    {{Type::FLOAT2,Type::FLOAT, BinaryOperatorType::MULTIPLY}, {OpType::MULTIPLY_FLOAT2_FLOAT, Type::FLOAT2}},

    // DIVIDE
    {{Type::INT,   Type::INT,   BinaryOperatorType::DIVIDE}, {OpType::DIVIDE_INT,           Type::INT}},
    {{Type::FLOAT, Type::FLOAT, BinaryOperatorType::DIVIDE}, {OpType::DIVIDE_FLOAT,         Type::FLOAT}},
    {{Type::INT2,  Type::INT2,  BinaryOperatorType::DIVIDE}, {OpType::DIVIDE_INT2,          Type::INT2}},
    {{Type::INT2,  Type::INT,   BinaryOperatorType::DIVIDE}, {OpType::DIVIDE_INT2_INT,      Type::INT2}},
    {{Type::FLOAT2,Type::FLOAT2,BinaryOperatorType::DIVIDE}, {OpType::DIVIDE_FLOAT2,        Type::FLOAT2}},
    {{Type::FLOAT2,Type::FLOAT, BinaryOperatorType::DIVIDE}, {OpType::DIVIDE_FLOAT2_FLOAT,  Type::FLOAT2}},

    // EQUAL
    {{Type::STRING, Type::STRING, BinaryOperatorType::EQUAL},      {OpType::EQUALS_STRING, Type::BOOL}},
    {{Type::BOOL,   Type::BOOL,   BinaryOperatorType::EQUAL},      {OpType::EQUALS_BOOl,   Type::BOOL}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::EQUAL},      {OpType::EQUALS_INT,    Type::BOOL}},
    {{Type::INT2,   Type::INT2,   BinaryOperatorType::EQUAL},      {OpType::EQUALS_INT2,   Type::BOOL}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::EQUAL},      {OpType::EQUALS_FLOAT,  Type::BOOL}},
    {{Type::FLOAT2, Type::FLOAT2, BinaryOperatorType::EQUAL},      {OpType::EQUALS_FLOAT2, Type::BOOL}},

    // NOT_EQUAL
    {{Type::STRING, Type::STRING, BinaryOperatorType::NOT_EQUAL},  {OpType::NOT_EQUALS_STRING, Type::BOOL}},
    {{Type::BOOL,   Type::BOOL,   BinaryOperatorType::NOT_EQUAL},  {OpType::NOT_EQUALS_BOOl,   Type::BOOL}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::NOT_EQUAL},  {OpType::NOT_EQUALS_INT,    Type::BOOL}},
    {{Type::INT2,   Type::INT2,   BinaryOperatorType::NOT_EQUAL},  {OpType::NOT_EQUALS_INT2,   Type::BOOL}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::NOT_EQUAL},  {OpType::NOT_EQUALS_FLOAT,  Type::BOOL}},
    {{Type::FLOAT2, Type::FLOAT2, BinaryOperatorType::NOT_EQUAL},  {OpType::NOT_EQUALS_FLOAT2, Type::BOOL}},

    // LESS_THAN
    {{Type::INT,   Type::INT,   BinaryOperatorType::LESS_THAN},           {OpType::LESS_THAN_INT,       Type::BOOL}},
    {{Type::FLOAT, Type::FLOAT, BinaryOperatorType::LESS_THAN},           {OpType::LESS_THAN_FLOAT,     Type::BOOL}},

    // GREATER_THAN
    {{Type::INT,   Type::INT,   BinaryOperatorType::GREATER_THAN},        {OpType::GREATER_THAN_INT,    Type::BOOL}},
    {{Type::FLOAT, Type::FLOAT, BinaryOperatorType::GREATER_THAN},        {OpType::GREATER_THAN_FLOAT,  Type::BOOL}},

    // LESS_THAN_EQUAL (Corrected the enum spelling to match)
    {{Type::INT,   Type::INT,   BinaryOperatorType::LESS_THAN_EQUAL},     {OpType::LESS_THAN_EQUALS_INT,    Type::BOOL}},
    {{Type::FLOAT, Type::FLOAT, BinaryOperatorType::LESS_THAN_EQUAL},     {OpType::LESS_THAN_EQUALS_FLOAT,  Type::BOOL}},

    // GREATER_THAN_EQUAL
    {{Type::INT,   Type::INT,   BinaryOperatorType::GREATER_THAN_EQUAL},  {OpType::GREATER_THAN_EQUALS_INT, Type::BOOL}},
    {{Type::FLOAT, Type::FLOAT, BinaryOperatorType::GREATER_THAN_EQUAL},  {OpType::GREATER_THAN_EQUALS_FLOAT, Type::BOOL}},
};

static const std::unordered_map<BinaryOpMapIn, CompilationErrorType, InHash> s_invalid = {
    // fill me
};

std::optional<BinaryOpMapOut> map_binary_op(Type left_type, Type right_type, BinaryOperatorType op) {
    auto itr = s_valid.find(BinaryOpMapIn{left_type, right_type, op});
    if (itr == s_valid.end()) {
        return std::nullopt;
    }

    return itr->second;
}

CompilationErrorType map_binary_op_validate(Type left_type, Type right_type, BinaryOperatorType op) {
    if (   left_type == Type::STRING 
        && right_type == Type::STRING 
        && op != BinaryOperatorType::EQUAL 
        && op != BinaryOperatorType::NOT_EQUAL)
    {
        return CompilationErrorType::MATH_OPERATION_ON_STRING;
    }

    auto itr = s_invalid.find(BinaryOpMapIn{left_type, right_type, op});
    if (itr == s_invalid.end()) {
        return CompilationErrorType::NONE;
    }

    return itr->second;
}