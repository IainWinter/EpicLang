#include "byte_code_enum_translation.h"

static std::string_view s_type_names[] = {
    "VOID",
    "STRING",
    "BOOL",
    "INT",
    "FLOAT",
    "INT2",
    "FLOAT2"
};

static std::string_view s_op_type_names[] = {
    "PLACEHOLDER",
    "HALT",
    "PUSH_LITERAL",
    "PUSH_VARIABLE",
    "STORE_VARIABLE",
    "CALL_FUNCTION",
    "RETURN",
    "JUMP",
    "JUMP_IF_FALSE",
    "ADD_INT",
    "ADD_FLOAT",
    "ADD_INT2",
    "ADD_FLOAT2",
    "SUBTRACT_INT",
    "SUBTRACT_FLOAT",
    "SUBTRACT_INT2",
    "SUBTRACT_FLOAT2",
    "MULTIPLY_INT",
    "MULTIPLY_FLOAT",
    "MULTIPLY_INT2",
    "MULTIPLY_INT2_INT",
    "MULTIPLY_FLOAT2",
    "MULTIPLY_FLOAT2_FLOAT",
    "DIVIDE_INT",
    "DIVIDE_FLOAT",
    "DIVIDE_INT2_INT",
    "DIVIDE_INT2",
    "DIVIDE_FLOAT2",
    "DIVIDE_FLOAT2_FLOAT",
    "EQUALS_STRING",
    "EQUALS_BOOl",
    "EQUALS_INT",
    "EQUALS_INT2",
    "EQUALS_FLOAT",
    "EQUALS_FLOAT2",
    "NOT_EQUALS_STRING",
    "NOT_EQUALS_BOOl",
    "NOT_EQUALS_INT",
    "NOT_EQUALS_INT2",
    "NOT_EQUALS_FLOAT",
    "NOT_EQUALS_FLOAT2",
    "LESS_THAN_INT",
    "LESS_THAN_FLOAT",
    "GREATER_THAN_INT",
    "GREATER_THAN_FLOAT",
    "LESS_THAN_EQUALS_INT",
    "LESS_THAN_EQUALS_FLOAT",
    "GREATER_THAN_EQUALS_INT",
    "GREATER_THAN_EQUALS_FLOAT",
};

std::string_view s_compiler_error_type_names[] = {
    "NONE",
    "PARSE_ERROR",
    "TYPE_MISMATCH",
    "NON_VOID_FUNCTION_MISSING_RETURN",
    "IDENTIFIED_NOT_DECLARED",
    "IDENTIFIED_ALREADY_DECLARED",
    "MATH_OPERATION_ON_STRING",
};

std::string_view type_to_string(Type type) {
    return s_type_names[static_cast<int>(type)];
}

std::string_view op_type_to_string(OpType type) {
    return s_op_type_names[static_cast<int>(type)];
}

std::string_view compilation_error_type_to_string(CompilationErrorType type) {
    return s_compiler_error_type_names[static_cast<int>(type)];
}

Type type_from_string(std::string_view name) {
    if (name == "void") {
        return Type::VOID;
    }

    if (name == "string") {
        return Type::STRING;
    }

    if (name == "bool") {
        return Type::BOOL;
    }

    if (name == "int") {
        return Type::INT;
    }

    if (name == "ivec2") {
        return Type::INT2;
    }

    if (name == "float") {
        return Type::FLOAT;
    }

    if (name == "vec2") {
        return Type::FLOAT2;
    }

    throw 0;
}

BinaryOperatorType binary_operator_type_from_string(std::string_view name) {
    if (name == "+") {
        return BinaryOperatorType::ADD;
    }

    if (name == "-") {
        return BinaryOperatorType::SUBTRACT;
    }

    if (name == "*") {
        return BinaryOperatorType::MULTIPLY;
    }

    if (name == "/") {
        return BinaryOperatorType::DIVIDE;
    }

    if (name == "==") {
        return BinaryOperatorType::EQUAL;
    }

    if (name == "!=") {
        return BinaryOperatorType::NOT_EQUAL;
    }

    if (name == "<") {
        return BinaryOperatorType::LESS_THAN;
    }

    if (name == ">") {
        return BinaryOperatorType::GREATER_THAN;
    }

    if (name == "<=") {
        return BinaryOperatorType::LESS_THAN_EQUAL;
    }

    if (name == ">=") {
        return BinaryOperatorType::GREATER_THAN_EQUAL;
    }

    throw 0;
}