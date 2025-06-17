#pragma once

#include "byte_code_enum.h"

#include <string_view>

std::string_view type_to_string(Type type);

std::string_view op_type_to_string(OpType type);

std::string_view compilation_error_type_to_string(CompilationErrorType type);

Type type_from_string(std::string_view name);

UnaryOperatorType unary_operator_type_from_string(std::string_view name);

BinaryOperatorType binary_operator_type_from_string(std::string_view name);