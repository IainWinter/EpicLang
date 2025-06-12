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

    void execute();
    
    void execute_op();

    void print() const;

    bool get_is_not_halted() const;

    size_t get_program_counter() const;

    const ByteCodeVmState get_state() const;

private:
    void execute_op_switch();

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
