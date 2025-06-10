#include <iostream>
#include <fstream>
#include <variant>
#include <typeinfo>
#include <optional>
#include <chrono>
#include <sstream>

#include "byte_code_vm_debugger.h"
#include "compiler.h"

#include "test.h"

void print(const std::vector<int>& stack) {
    printf("Stack:\n");
    for (auto x : stack) {
        printf("%d\n", x);
    }
}

void print(const std::unordered_map<std::string, int>& variables) {
    printf("Variables:\n");
    for (auto x : variables) {
        printf("%s : %d\n", x.first.c_str(), x.second);
    }
}

std::chrono::high_resolution_clock::time_point g_start_time;

void time_start() {
    g_start_time = std::chrono::high_resolution_clock::now();
}

void time_end(const std::string& label) {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end_time - g_start_time).count();
    std::cout << label << " took " << duration << " ms\n";
}

int main() {
    run_tests();

    std::string x;
    {
        std::ifstream t("test.sl");
        std::stringstream buffer;
        buffer << t.rdbuf();
        x = buffer.str();
    }
    
    time_start();
    CompilationResults results = compile(x);
    time_end("Compilation");

    if (results.error.type != CompilationErrorType::NONE) {
        return 1;
    }

    time_start();
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
    
    time_end("Execution");

    return 0;
}