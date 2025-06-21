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
    // Left         Right         Operator                                  Operation                           Result Type
    {{Type::INT,    Type::INT,    BinaryOperatorType::ADD},                 {OpType::ADD_INT,                   Type::INT}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::ADD},                 {OpType::ADD_FLOAT,                 Type::FLOAT}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::SUBTRACT},            {OpType::SUBTRACT_INT,              Type::INT}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::SUBTRACT},            {OpType::SUBTRACT_FLOAT,            Type::FLOAT}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::MULTIPLY},            {OpType::MULTIPLY_INT,              Type::INT}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::MULTIPLY},            {OpType::MULTIPLY_FLOAT,            Type::FLOAT}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::DIVIDE},              {OpType::DIVIDE_INT,                Type::INT}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::DIVIDE},              {OpType::DIVIDE_FLOAT,              Type::FLOAT}},
    {{Type::STRING, Type::STRING, BinaryOperatorType::EQUAL},               {OpType::EQUALS_STRING,             Type::BOOL}},
    {{Type::BOOL,   Type::BOOL,   BinaryOperatorType::EQUAL},               {OpType::EQUALS_BOOL,               Type::BOOL}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::EQUAL},               {OpType::EQUALS_INT,                Type::BOOL}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::EQUAL},               {OpType::EQUALS_FLOAT,              Type::BOOL}},
    {{Type::STRING, Type::STRING, BinaryOperatorType::NOT_EQUAL},           {OpType::NOT_EQUALS_STRING,         Type::BOOL}},
    {{Type::BOOL,   Type::BOOL,   BinaryOperatorType::NOT_EQUAL},           {OpType::NOT_EQUALS_BOOL,           Type::BOOL}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::NOT_EQUAL},           {OpType::NOT_EQUALS_INT,            Type::BOOL}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::NOT_EQUAL},           {OpType::NOT_EQUALS_FLOAT,          Type::BOOL}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::LESS_THAN},           {OpType::LESS_THAN_INT,             Type::BOOL}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::LESS_THAN},           {OpType::LESS_THAN_FLOAT,           Type::BOOL}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::GREATER_THAN},        {OpType::GREATER_THAN_INT,          Type::BOOL}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::GREATER_THAN},        {OpType::GREATER_THAN_FLOAT,        Type::BOOL}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::LESS_THAN_EQUAL},     {OpType::LESS_THAN_EQUALS_INT,      Type::BOOL}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::LESS_THAN_EQUAL},     {OpType::LESS_THAN_EQUALS_FLOAT,    Type::BOOL}},
    {{Type::INT,    Type::INT,    BinaryOperatorType::GREATER_THAN_EQUAL},  {OpType::GREATER_THAN_EQUALS_INT,   Type::BOOL}},
    {{Type::FLOAT,  Type::FLOAT,  BinaryOperatorType::GREATER_THAN_EQUAL},  {OpType::GREATER_THAN_EQUALS_FLOAT, Type::BOOL}},
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

    return CompilationErrorType::NONE;
}