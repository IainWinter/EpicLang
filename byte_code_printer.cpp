#include "byte_code_printer.h"

#include "byte_code_enum_translation.h"

void print_byte_code(const ByteCodeOp& op) {
    std::string_view op_name = op_type_to_string(op.type);
    printf("%s\t", op_name.data());

    switch (op.type) {
        case OpType::PUSH_LITERAL: {
            const auto& operand = std::get<ByteCodePushLiteralOp>(op.operand);
            print_type_variant(operand.type, operand.value);
            break;
        }
        case OpType::PUSH_VARIABLE: {
            const auto& operand = std::get<ByteCodePushVariableOp>(op.operand);
            printf("%s %s", type_to_string(operand.type).data(), operand.identifier.c_str());
            break;
        }
        case OpType::STORE_VARIABLE: {
            const auto& operand = std::get<ByteCodeStoreVariableOp>(op.operand);
            printf("%s %s", type_to_string(operand.type).data(), operand.identifier.c_str());
            break;
        }
        case OpType::CALL_FUNCTION: {
            const auto& operand = std::get<ByteCodeCallFunctionOp>(op.operand);
            printf("%zd", operand.code_index);
            break;
        }
        case OpType::CALL_FUNCTION_EXTERNAL: {
            const auto& operand = std::get<ByteCodeCallFunctionOp>(op.operand);
            printf("%zd", operand.code_index);
            break;
        }
        case OpType::JUMP: {
            const auto& operand = std::get<ByteCodeJumpOp>(op.operand);
            printf("%zd", operand.code_index);
            break;
        }
        case OpType::JUMP_IF_FALSE: {
            const auto& operand = std::get<ByteCodeJumpOp>(op.operand);
            printf("%zd", operand.code_index);
            break;
        }
    }
}

void print_type_variant(Type type, const TypeVariant& value) {
    switch (type) {
        case Type::VOID:
            print_void();
            break;
        case Type::STRING:
            print_string(std::get<std::string>(value));
            break;
        case Type::BOOL:
            print_bool(std::get<bool>(value));
            break;
        case Type::INT:
            print_int(std::get<int>(value));
            break;
        case Type::FLOAT:
            print_float(std::get<float>(value));
            break;
    }
}

void print_void() {
    printf("void");
}

void print_string(std::string_view value) {
    printf("\"%.*s\"", static_cast<int>(value.size()), value.data());
}

void print_bool(bool value) {
    printf(value ? "true" : "false");
}

void print_int(int value) {
    printf("%d", value);
}

void print_float(float value) {
    printf("%f", value);
}