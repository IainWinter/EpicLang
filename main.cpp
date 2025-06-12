#include "byte_code_vm_debugger.h"
#include "compiler.h"

#include "test.h"

#include <thread>
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

    debugger.breakpoint_add(results.main_code_index);
    while (vm.get_is_not_halted()) {
        debugger.breakpoint_step();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    return 0;
}