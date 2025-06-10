#pragma once

#include "byte_code_vm.h"

struct Breakpoint {
    size_t code_index;
};

class ByteCodeVmDebugger {
public:
    ByteCodeVmDebugger(ByteCodeVm& vm);

    void execute();

    void breakpoint_add(size_t code_index);

    void breakpoint_remove(size_t code_index);

    void breakpoint_step();

    void breakpoint_continue();

private:
    ByteCodeVm& m_vm;

    std::vector<Breakpoint> m_breakpoints;
    bool m_breakpoint_hit;
};
