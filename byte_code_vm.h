#pragma once

#include "byte_stack.h"
#include "byte_code_types.h"
#include "program.h"

#include <unordered_map>

struct ByteCodeVmState {
    ByteStack stack;
    std::unordered_map<std::string, std::pair<Type, TypeVariant>> variables;
    std::vector<size_t> call_stack;
    size_t program_counter;
};

class ByteCodeVm {
public:
    ByteCodeVm(const Program& program);

    void set_main_args(const std::vector<std::pair<Type, TypeVariant>>& args);

    void execute();
    
    void execute_op();

    void halt();

    void call_function(const std::string& identifier, const const std::vector<std::pair<Type, TypeVariant>>& args);

    void print() const;

    bool get_is_not_halted() const;

    size_t get_program_counter() const;

    const ByteCodeVmState get_state() const;

private:
    void execute_op_switch();

    void execute_op_call_function(size_t code_index);

    void execute_op_call_external_function(size_t function_index);

    void push_variant(Type type, const TypeVariant& variant);

    std::pair<Type, TypeVariant> pop_variant();

private:
    ByteStack m_stack;
    std::unordered_map<std::string, std::pair<Type, TypeVariant>> m_variables;
    std::vector<size_t> m_call_stack;
    size_t m_program_counter;
    size_t m_next_program_counter;

    const Program& m_program;
};
