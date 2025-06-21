#pragma once

#include "byte_code_types.h"

void print_byte_code(const ByteCodeOp& op);

void print_type_variant(Type type, const TypeVariant& value);

void print_void();
void print_string(std::string_view value);
void print_bool(bool value);
void print_int(int value);
void print_float(float value);