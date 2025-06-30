#pragma once

enum class BuiltinType : char {
    VOID,    // only for functions
    STRING,
    BOOL,
    INT,
    FLOAT,
    OBJECT,
    OBJECT_POINTER
};

enum UnaryOperatorType : char {
    NOT,
    NEGATE
};

enum BinaryOperatorType : char {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL
};

enum class OpType {
    PLACEHOLDER,

    HALT,

    PUSH_LITERAL,
    PUSH_VARIABLE,

    POP,

    STORE_VARIABLE,

    CALL_FUNCTION,
    CALL_FUNCTION_EXTERNAL,
    
    RETURN,

    JUMP,
    JUMP_IF_FALSE,
    
    // Right now the int and int2 are separate
    // but i could see a world where its int_n and everything can be vectorized

    NOT_BOOL,

    NEGATE_INT,
    NEGATE_FLOAT,

    ADD_INT,
    ADD_FLOAT,

    SUBTRACT_INT,
    SUBTRACT_FLOAT,

    MULTIPLY_INT,
    MULTIPLY_FLOAT,
    
    DIVIDE_INT,
    DIVIDE_FLOAT,

    EQUALS_STRING,
    EQUALS_BOOL,
    EQUALS_INT,
    EQUALS_FLOAT,

    NOT_EQUALS_STRING,
    NOT_EQUALS_BOOL,
    NOT_EQUALS_INT,
    NOT_EQUALS_FLOAT,

    LESS_THAN_INT,
    LESS_THAN_FLOAT,

    GREATER_THAN_INT,
    GREATER_THAN_FLOAT,

    LESS_THAN_EQUALS_INT,
    LESS_THAN_EQUALS_FLOAT,

    GREATER_THAN_EQUALS_INT,
    GREATER_THAN_EQUALS_FLOAT
};

enum class CompilationErrorType {
    NONE,
    PARSE_ERROR,
    TYPE_MISMATCH,
    NON_VOID_FUNCTION_MISSING_RETURN,
    FUNCTION_CALLED_WITH_WRONG_NUMBER_OF_ARGS,
    IDENTIFIED_NOT_DECLARED,
    IDENTIFIED_ALREADY_DECLARED,
    MATH_OPERATION_ON_STRING
};