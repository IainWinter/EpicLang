#include "program.h"

#include "byte_code_printer.h"
#include "byte_code_enum_translation.h"

std::optional<CallableFunctionInfo> Program::find_function(const std::string& identifier) const {
    for (size_t i = 0; i < functions.size(); i++) {
        if (functions.at(i).name != identifier) {
            continue;
        }

        return CallableFunctionInfo { FunctionType::SCRIPT, i };
    }

    for (size_t i = 0; i < external_functions.size(); i++) {
        if (external_functions.at(i).name != identifier) {
            continue;
        }

        return CallableFunctionInfo{ FunctionType::EXTERNAL, i };
    }

    return std::nullopt;
}

void Program::print() const {
    print(main_code_index);
}

void Program::print(size_t highlight_code_index) const {
    printf("\033[2J\033[H");
    
    printf("\nProgram Counter: %zu\n", highlight_code_index);
    
    printf("\nProgram:\n");
    for (size_t i = 0; i < operations.size(); i++) {
        const ByteCodeOp& op_ = operations.at(i);

        bool isCurrent = (i == highlight_code_index);

        printf("%s%s%3zu : ", 
            isCurrent ? "\033[1;33m" : "", 
            isCurrent ? ">" : " ", 
            i
        );

        print_byte_code(op_);
        
        if (isCurrent) {
            printf("\033[0m");
        }
        
        printf("\n");
    }

    printf("\nFunctions:\n");
    for (size_t i = 0; i < functions.size(); i++) {
        const Function& function = functions.at(i);

        printf("%4zu : %s %s(", function.code_index, type_to_string(function.return_type).data(), function.name.c_str());

        for (size_t j = 0; j < function.argument_count; j++) {
            const Variable& variable = function.local_variables.at(j); 
            printf("%s %s", type_to_string(variable.type).data(), variable.name.c_str());

            if (j != function.argument_count - 1) {
                printf(", ");
            }
        }

        printf(")");

        for (size_t j = function.argument_count; j < function.local_variables.size(); j++) {
            const Variable& variable = function.local_variables.at(j); 
            printf("\n        %s %s", type_to_string(variable.type).data(), variable.name.c_str());
        }
        
        printf("\n");
    }

    printf("\nExternal functions:\n");
    for (size_t i = 0; i < external_functions.size(); i++) {
        const ExternalFunction& function = external_functions.at(i);

        printf("%4zu : %s %s(", i, type_to_string(function.return_type).data(), function.name.c_str());

        for (size_t j = 0; j < function.arguments.size(); j++) {
            const Variable& variable = function.arguments.at(j);
            printf("%s %s", type_to_string(variable.type).data(), variable.name.c_str());

            if (j != function.arguments.size() - 1) {
                printf(", ");
            }
        }

        printf(")\n");
    }
}