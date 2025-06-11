#include "byte_code_vm_debugger.h"
#include "compiler.h"

#include "test.h"

#include <fstream>
#include <sstream>

int main() {
    run_tests();

    std::string x;
    {
        std::ifstream t("test.sl");
        std::stringstream buffer;
        buffer << t.rdbuf();
        x = buffer.str();
    }
    
    CompilationResults results = compile(x);

    if (results.error.type != CompilationErrorType::NONE) {
        return 1;
    }

    ByteCodeVm vm(results.operations, results.main_code_index);
    ByteCodeVmDebugger debugger(vm);

    debugger.breakpoint_add(3);
    debugger.breakpoint_add(6);
    debugger.breakpoint_continue();
    debugger.breakpoint_step();
    debugger.breakpoint_step();
    debugger.breakpoint_step();
    debugger.breakpoint_remove(6);
    debugger.breakpoint_continue();
    debugger.breakpoint_remove(3);
    debugger.breakpoint_step();
    debugger.breakpoint_continue();
    
    return 0;
}