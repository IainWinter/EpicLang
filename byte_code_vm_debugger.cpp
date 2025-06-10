#include "byte_code_vm_debugger.h"

ByteCodeVmDebugger::ByteCodeVmDebugger(ByteCodeVm& vm) 
    : m_vm             (vm)
    , m_breakpoint_hit (false)
{}

void ByteCodeVmDebugger::execute() {
    breakpoint_continue();
}

void ByteCodeVmDebugger::breakpoint_add(size_t code_index) {
    m_breakpoints.push_back({ code_index });
}

void ByteCodeVmDebugger::breakpoint_remove(size_t code_index) {
    m_breakpoints.erase(std::find_if(m_breakpoints.begin(), m_breakpoints.end(), 
        [&](const Breakpoint& a) {
            return a.code_index == code_index;
        }
    ));
}

void ByteCodeVmDebugger::breakpoint_step() {
    if (m_vm.get_is_not_halted()) {
        m_vm.execute_op();
        m_vm.print();
    }
}

void ByteCodeVmDebugger::breakpoint_continue() {
    m_breakpoint_hit = false;
    while (m_vm.get_is_not_halted() && !m_breakpoint_hit) {
        m_vm.execute_op();

        for (const Breakpoint& breakpoint : m_breakpoints) {
            if (breakpoint.code_index == m_vm.get_program_counter()) {
                m_breakpoint_hit = true;
                break;
            }
        }
    }

    m_vm.print();
}