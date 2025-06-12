#pragma once

struct int2 { 
    int x, y;

    int2 operator+(const int2& r) const { return {x + r.x, y + r.y}; }
    int2 operator-(const int2& r) const { return {x - r.x, y - r.y}; }
    int2 operator*(const int2& r) const { return {x * r.x, y * r.y}; }
    int2 operator/(const int2& r) const { return {x / r.x, y / r.y}; }
    int2 operator*(int r) const { return {x * r, y * r}; }
    int2 operator/(int r) const { return {x / r, y / r}; }
    bool operator==(const int2& r) const { return x == r.x && y == r.y; }
    bool operator!=(const int2& r) const { return x != r.x && y != r.y; }
};

struct float2 { 
    float x, y; 

    float2 operator+(const float2& r) const { return {x + r.x, y + r.y}; }
    float2 operator-(const float2& r) const { return {x - r.x, y - r.y}; }
    float2 operator*(const float2& r) const { return {x * r.x, y * r.y}; }
    float2 operator/(const float2& r) const { return {x / r.x, y / r.y}; }
    float2 operator*(float r) const { return {x * r, y * r}; }
    float2 operator/(float r) const { return {x / r, y / r}; }
    bool operator==(const float2& r) const { return x == r.x && y == r.y; }
    bool operator!=(const float2& r) const { return x != r.x && y != r.y; }
};

enum class Type : char {
    VOID,    // only for functions
    STRING,
    BOOL,
    INT,
    FLOAT,
    INT2,
    FLOAT2
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
    RETURN,

    JUMP,
    JUMP_IF_FALSE,
    
    // Right now the int and int2 are separate
    // but i could see a world where its int_n and everything can be vectorized

    ADD_INT,
    ADD_FLOAT,
    ADD_INT2,
    ADD_FLOAT2,

    SUBTRACT_INT,
    SUBTRACT_FLOAT,
    SUBTRACT_INT2,
    SUBTRACT_FLOAT2,

    MULTIPLY_INT,
    MULTIPLY_FLOAT,
    MULTIPLY_INT2,
    MULTIPLY_INT2_INT,
    MULTIPLY_FLOAT2,
    MULTIPLY_FLOAT2_FLOAT,
    
    DIVIDE_INT,
    DIVIDE_FLOAT,
    DIVIDE_INT2_INT,
    DIVIDE_INT2,
    DIVIDE_FLOAT2,
    DIVIDE_FLOAT2_FLOAT,

    EQUALS_STRING,
    EQUALS_BOOl,
    EQUALS_INT,
    EQUALS_INT2,
    EQUALS_FLOAT,
    EQUALS_FLOAT2,

    NOT_EQUALS_STRING,
    NOT_EQUALS_BOOl,
    NOT_EQUALS_INT,
    NOT_EQUALS_INT2,
    NOT_EQUALS_FLOAT,
    NOT_EQUALS_FLOAT2,

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
    IDENTIFIED_NOT_DECLARED,
    IDENTIFIED_ALREADY_DECLARED,
    MATH_OPERATION_ON_STRING,
};