#pragma once

#include "byte_code_enum.h"

#include <optional>

struct UnaryOpMapOut {
    OpType code;
    Type result_type;
};

std::optional<UnaryOpMapOut> map_unary_op(Type right_type, UnaryOperatorType op);

CompilationErrorType map_unary_op_validate(Type right_type, UnaryOperatorType op);