#include "byte_code_vm_debugger.h"
#include "compiler.h"

#include "test.h"

#include <thread>
#include <fstream>
#include <sstream>
#include <charconv>

int main() {
    run_tests();

    std::string x;
    {
        std::ifstream t("test.sl");
        std::stringstream buffer;
        buffer << t.rdbuf();
        x = buffer.str();
    }
    
    std::vector<ExternalFunction> external_functions = {
        {
            Type::VOID,
            "print",
            {
                { Type::STRING, "string" }
            },
            [](const std::vector<TypeVariant>& args) -> TypeVariant {
                printf("%s\n", std::get<std::string>(args.at(0)).c_str());
                return {};
            }
        },
        {
            Type::STRING,
            "to_string",
            {
                { Type::INT, "value" },
            },
            [](const std::vector<TypeVariant>& args) -> TypeVariant {
                char buf[32];
                std::to_chars_result r = std::to_chars(buf, buf + sizeof(buf), std::get<int>(args.at(0)));
                return std::string(buf, r.ptr);
            }
        }
    };

    CompilationResults results = compile(x, external_functions);

    if (results.error.type != CompilationErrorType::NONE) {
        return 1;
    }

    results.program.print();

    ByteCodeVm vm(results.program);
    vm.set_main_args({ { Type::INT, 0 } });
    vm.execute();

    // ByteCodeVmDebugger debugger(vm);

    // while (vm.get_is_not_halted()) {
    //     debugger.breakpoint_step();

    //     // vm.call_function("print", {{ Type::STRING, std::string("from debugger") }});

    //     std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // }
    
    return 0;
}