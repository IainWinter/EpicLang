#include "byte_code_vm.h"

#include "byte_code_enum_translation.h"
#include "byte_code_printer.h"

#include <unordered_map>

ByteCodeVm::ByteCodeVm(const std::vector<ByteCodeOp>& operations, size_t program_counter)
    : m_operations (operations)
{
    m_program_counter = program_counter;
    m_next_program_counter = program_counter;
}

void ByteCodeVm::execute() {
    while (get_is_not_halted()) {
        execute_op();
    }
}

void ByteCodeVm::execute_op() {
    m_next_program_counter = m_program_counter + 1;
    execute_op_switch();
    m_program_counter = m_next_program_counter;
}

void ByteCodeVm::print() const {
    printf("\033[2J\033[H");
    printf("Program:\n");
    for (size_t i = 0; i < m_operations.size(); i++) {
        const ByteCodeOp& op_ = m_operations.at(i);

        bool isCurrent = (i == m_program_counter);
        if (isCurrent) {
            printf("\033[1;33m");
        }

        printf("%s%3zu : ",  isCurrent ? ">" : " ", i);
        print_byte_code(op_);

        if (isCurrent) {
            printf("\033[0m");
        }

        printf("\n");
    }

    printf("\nProgram Counter: %zu\n", m_program_counter);

    printf("\nCall Stack:\n");
    for (size_t i = 0; i < m_call_stack.size(); ++i) {
        printf("  [%zu] -> %zu\n", i, m_call_stack[i]);
    }

    printf("\nStack:\n");
    m_stack.print();

    printf("\nVariables:\n");
    for (const auto& [name, pair] : m_variables) {
        const Type& type = pair.first;
        const TypeVariant& value = pair.second;

        printf("  %s : ", name.c_str());
        print_type_variant(type, value);
        printf("\n");
    }
}

bool ByteCodeVm::get_is_not_halted() const {
    return m_program_counter < m_operations.size();
}

size_t ByteCodeVm::get_program_counter() const {
    return m_program_counter;
}

const ByteCodeVmState ByteCodeVm::get_state() const {
    return { 
        m_stack,
        m_variables,
        m_call_stack,
        m_program_counter
    };
}

void ByteCodeVm::execute_op_switch() {
    const ByteCodeOp& op = m_operations.at(m_program_counter);

    switch (op.type) {
        case OpType::PUSH_LITERAL: {
            const ByteCodePushLiteralOp& operand = std::get<ByteCodePushLiteralOp>(op.operand);
            push_variant(operand.type, operand.value);
            break;
        }

        case OpType::STORE_VARIABLE: {
            const ByteCodeStoreVariableOp& operand = std::get<ByteCodeStoreVariableOp>(op.operand);
            m_variables[operand.identifier] = pop_variant();
            break;
        }

        case OpType::PUSH_VARIABLE: {
            const ByteCodePushVariableOp& operand = std::get<ByteCodePushVariableOp>(op.operand);
            auto [type, value] = m_variables.at(operand.identifier);
            push_variant(type, value);
            break;
        }

        case OpType::POP: {
            m_stack.pop();
            break;
        }

        case OpType::CALL_FUNCTION: {
            const ByteCodeCallFunctionOp& operand = std::get<ByteCodeCallFunctionOp>(op.operand);
            m_call_stack.push_back(m_program_counter);
            m_next_program_counter = operand.code_index;
            // need to push args
            break;
        }

        case OpType::RETURN: {
            if (m_call_stack.size() == 0) {
                m_next_program_counter = m_operations.size();
                break;
            }

            size_t code_index = m_call_stack.back();
            m_call_stack.pop_back();
            m_next_program_counter = code_index + 1;
            break;
        }

        case OpType::JUMP: {
            const ByteCodeJumpOp& operand = std::get<ByteCodeJumpOp>(op.operand);
            m_next_program_counter = operand.code_index;
            break;
        }

        case OpType::JUMP_IF_FALSE: {
            bool value = m_stack.top_as_bool();
            m_stack.pop();
            
            if (!value) {
                const ByteCodeJumpOp& operand = std::get<ByteCodeJumpOp>(op.operand);
                m_next_program_counter = operand.code_index;
            }

            break;
        }

        // Math

        case OpType::ADD_INT: {
            int result = m_stack.top_as_int(1) + m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_int(result);
            break;
        }
        case OpType::ADD_FLOAT: {
            float result = m_stack.top_as_float(1) + m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_float(result);
            break;
        }
        case OpType::ADD_INT2: {
            int2 result = m_stack.top_as_int2(1) + m_stack.top_as_int2(0);
            m_stack.pop(2);
            m_stack.push_int2(result);
            break;
        }
        case OpType::ADD_FLOAT2: {
            float2 result = m_stack.top_as_float2(1) + m_stack.top_as_float2(0);
            m_stack.pop(2);
            m_stack.push_float2(result);
            break;
        }
        case OpType::SUBTRACT_INT: {
            int result = m_stack.top_as_int(1) - m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_int(result);
            break;
        }
        case OpType::SUBTRACT_FLOAT: {
            float result = m_stack.top_as_float(1) - m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_float(result);
            break;
        }
        case OpType::SUBTRACT_INT2: {
            int2 result = m_stack.top_as_int2(1) - m_stack.top_as_int2(0);
            m_stack.pop(2);
            m_stack.push_int2(result);
            break;
        }
        case OpType::SUBTRACT_FLOAT2: {
            float2 result = m_stack.top_as_float2(1) - m_stack.top_as_float2(0);
            m_stack.pop(2);
            m_stack.push_float2(result);
            break;
        }
        case OpType::MULTIPLY_INT: {
            int result = m_stack.top_as_int(1) * m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_int(result);
            break;
        }
        case OpType::MULTIPLY_FLOAT: {
            float result = m_stack.top_as_float(1) * m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_float(result);
            break;
        }
        case OpType::MULTIPLY_INT2: {
            int2 result = m_stack.top_as_int2(1) * m_stack.top_as_int2(0);
            m_stack.pop(2);
            m_stack.push_int2(result);
            break;
        }
        case OpType::MULTIPLY_INT2_INT: {
            int2 result = m_stack.top_as_int2(1) * m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_int2(result);
            break;
        }
        case OpType::MULTIPLY_FLOAT2: {
            float2 result = m_stack.top_as_float2(1) * m_stack.top_as_float2(0);
            m_stack.pop(2);
            m_stack.push_float2(result);
            break;
        }
        case OpType::MULTIPLY_FLOAT2_FLOAT: {
            float2 result = m_stack.top_as_float2(1) * m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_float2(result);
            break;
        }
        case OpType::DIVIDE_INT: {
            int result = m_stack.top_as_int(1) / m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_int(result);
            break;
        }
        case OpType::DIVIDE_FLOAT: {
            float result = m_stack.top_as_float(1) / m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_float(result);
            break;
        }
        case OpType::DIVIDE_INT2_INT: {
            int2 result = m_stack.top_as_int2(1) / m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_int2(result);
            break;
        }
        case OpType::DIVIDE_INT2: {
            int2 result = m_stack.top_as_int2(1) / m_stack.top_as_int2(0);
            m_stack.pop(2);
            m_stack.push_int2(result);
            break;
        }
        case OpType::DIVIDE_FLOAT2: {
            float2 result = m_stack.top_as_float2(1) / m_stack.top_as_float2(0);
            m_stack.pop(2);
            m_stack.push_float2(result);
            break;
        }
        case OpType::DIVIDE_FLOAT2_FLOAT: {
            float2 result = m_stack.top_as_float2(1) / m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_float2(result);
            break;
        }
        case OpType::EQUALS_STRING: {
            bool result = m_stack.top_as_string(1) == m_stack.top_as_string(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::EQUALS_BOOl: {
            bool result = m_stack.top_as_bool(1) == m_stack.top_as_bool(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::EQUALS_INT: {
            bool result = m_stack.top_as_int(1) == m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::EQUALS_INT2: {
            bool result = m_stack.top_as_int2(1) == m_stack.top_as_int2(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::EQUALS_FLOAT: {
            bool result = m_stack.top_as_float(1) == m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::EQUALS_FLOAT2: {
            bool result = m_stack.top_as_float2(1) == m_stack.top_as_float2(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::NOT_EQUALS_STRING: {
            bool result = m_stack.top_as_string(1) != m_stack.top_as_string(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::NOT_EQUALS_BOOl: {
            bool result = m_stack.top_as_bool(1) != m_stack.top_as_bool(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::NOT_EQUALS_INT: {
            bool result = m_stack.top_as_int(1) != m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::NOT_EQUALS_INT2: {
            bool result = m_stack.top_as_int2(1) != m_stack.top_as_int2(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::NOT_EQUALS_FLOAT: {
            bool result = m_stack.top_as_float(1) != m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::NOT_EQUALS_FLOAT2: {                
            bool result = m_stack.top_as_float2(1) != m_stack.top_as_float2(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::LESS_THAN_INT: {
            bool result = m_stack.top_as_int(1) < m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::LESS_THAN_FLOAT: {
            bool result = m_stack.top_as_float(1) < m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::GREATER_THAN_INT: {
            bool result = m_stack.top_as_int(1) > m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::GREATER_THAN_FLOAT: {
            bool result = m_stack.top_as_float(1) > m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::LESS_THAN_EQUALS_INT: {
            bool result = m_stack.top_as_int(1) <= m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::LESS_THAN_EQUALS_FLOAT: {
            bool result = m_stack.top_as_float(1) <= m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::GREATER_THAN_EQUALS_INT: {
            bool result = m_stack.top_as_int(1) >= m_stack.top_as_int(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::GREATER_THAN_EQUALS_FLOAT: {
            bool result = m_stack.top_as_float(1) >= m_stack.top_as_float(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        default: {
            exit(1);
        }
    }
}

void ByteCodeVm::push_variant(Type type, const TypeVariant& variant) {
    switch (type) {
        case Type::STRING: {
            m_stack.push_string(std::get<std::string>(variant));
            break;
        }
        case Type::BOOL: {
            m_stack.push_bool(std::get<bool>(variant));
            break;
        }
        case Type::INT: {
            m_stack.push_int(std::get<int>(variant));
            break;
        }
        case Type::INT2: {
            m_stack.push_int2(std::get<int2>(variant));
            break;
        }
        case Type::FLOAT: {
            m_stack.push_float(std::get<float>(variant));
            break;
        }
        case Type::FLOAT2: {
            m_stack.push_float2(std::get<float2>(variant));
            break;
        }
        default: {
            exit(1);
            break;
        }
    }
}

std::pair<Type, TypeVariant> ByteCodeVm::pop_variant() {
    Type type = m_stack.top_value_type();
    TypeVariant value = {};

    switch (type) {
        case Type::STRING: {
            value = std::string(m_stack.top_as_string());
            break;
        }
        case Type::BOOL: {
            value = m_stack.top_as_bool();
            break;
        }
        case Type::INT: {
            value = m_stack.top_as_int();
            break;
        }
        case Type::INT2: {
            value = m_stack.top_as_int2();
            break;
        }
        case Type::FLOAT: {
            value = m_stack.top_as_float();
            break;
        }
        case Type::FLOAT2: {
            value = m_stack.top_as_float2();
            break;
        }
        default: {
            exit(1);
            break;
        }
    }

    m_stack.pop();

    return { type, value };
}