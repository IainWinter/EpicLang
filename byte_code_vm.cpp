#include "byte_code_vm.h"

#include "byte_code_enum_translation.h"
#include "byte_code_printer.h"

#include <unordered_map>

ByteCodeVm::ByteCodeVm(const Program& program)
    : m_program (program)
{
    m_program_counter = program.main_code_index;
    m_next_program_counter = m_program_counter;
}

void ByteCodeVm::set_main_args(const std::vector<std::pair<Type, TypeVariant>>& args) {
    for (auto arg : args) {
        push_variant(arg.first, arg.second);
    }
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

void ByteCodeVm::halt() {
    m_program_counter = m_program.operations.size();
    m_next_program_counter = m_program.operations.size();
}

void ByteCodeVm::call_function(const std::string& identifier, const const std::vector<std::pair<Type, TypeVariant>>& args) {
    auto index = m_program.find_function(identifier);
    
    if (!index.has_value()) {
        halt();
    }

    for (const auto& [type, arg] : args) {
        push_variant(type, arg);
    }

    switch (index.value().type) {
        case FunctionType::SCRIPT: {
            const Function& function = m_program.functions.at(index.value().function_index);
            execute_op_call_function(function.code_index);
            break;
        }
        case FunctionType::EXTERNAL: {
            execute_op_call_external_function(index.value().function_index);
            break;
        }
        default: {
            halt();
            break;
        }
    }
}

void ByteCodeVm::print() const {
    printf("\033[2J\033[H");
    m_program.print(m_program_counter);

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
    return m_program_counter < m_program.operations.size();
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
    const ByteCodeOp& op = m_program.operations.at(m_program_counter);

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
            execute_op_call_function(operand.code_index);
            break;
        }

        case OpType::CALL_FUNCTION_EXTERNAL: {
            const ByteCodeCallFunctionOp& operand = std::get<ByteCodeCallFunctionOp>(op.operand);
            execute_op_call_external_function(operand.code_index);
            break;
        }

        case OpType::RETURN: {
            if (m_call_stack.size() == 0) {
                m_next_program_counter = m_program.operations.size();
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

        // Unary

        case OpType::NOT_BOOL: {
            bool result = !m_stack.top_as_bool();
            m_stack.pop();
            m_stack.push_bool(result);
            break;
        }
        case OpType::NEGATE_INT: {
            int result = -m_stack.top_as_int();
            m_stack.pop();
            m_stack.push_int(result);
            break;
        }
        case OpType::NEGATE_FLOAT: {
            float result = -m_stack.top_as_float();
            m_stack.pop();
            m_stack.push_float(result);
            break;
        }

        // Binary

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

        // Comparisons

        case OpType::EQUALS_STRING: {
            bool result = m_stack.top_as_string(1) == m_stack.top_as_string(0);
            m_stack.pop(2);
            m_stack.push_bool(result);
            break;
        }
        case OpType::EQUALS_BOOL: {
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
        case OpType::EQUALS_FLOAT: {
            bool result = m_stack.top_as_float(1) == m_stack.top_as_float(0);
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
        case OpType::NOT_EQUALS_BOOL: {
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
        case OpType::NOT_EQUALS_FLOAT: {
            bool result = m_stack.top_as_float(1) != m_stack.top_as_float(0);
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

void ByteCodeVm::execute_op_call_function(size_t code_index) {
    m_call_stack.push_back(m_program_counter);
    m_next_program_counter = code_index;
}

void ByteCodeVm::execute_op_call_external_function(size_t function_index) {
    const ExternalFunction& function = m_program.external_functions.at(function_index);

    std::vector<TypeVariant> args;
    for (size_t i = 0; i < function.arguments.size(); i++) {
        auto [value_type, value] = pop_variant();
        args.push_back(value);
    }

    if (function.return_type == Type::VOID) {
        function.proc(args);
    }

    else {
        TypeVariant result =  function.proc(args);
        push_variant(function.return_type, result);
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
        case Type::FLOAT: {
            m_stack.push_float(std::get<float>(variant));
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
        case Type::FLOAT: {
            value = m_stack.top_as_float();
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