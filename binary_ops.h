#pragma once

#include "byte_code_enum.h"

#include <optional>

struct BinaryOpMapOut {
    OpType code;
    Type result_type;
};

std::optional<BinaryOpMapOut> map_binary_op(Type left_type, Type right_type, BinaryOperatorType op);

CompilationErrorType map_binary_op_validate(Type left_type, Type right_type, BinaryOperatorType op);