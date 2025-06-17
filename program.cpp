#include "program.h"

#include "byte_code_printer.h"
#include "byte_code_enum_translation.h"

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

        printf("%3zu :", i);
        printf("\n     code_index: %zu", function.code_index);
        printf("\n     identifier: %s", function.name.c_str());
        printf("\n     return_type: %s", type_to_string(function.return_type).data());
        printf("\n     argument_count: %zu", function.argument_count);
        printf("\n     local_variables: %zu\n", function.local_variables.size());

        for (size_t j = 0; j < function.local_variables.size(); j++) {
            const Variable& variable = function.local_variables.at(j); 

            printf("     %3zu :", j);
            // printf("\n          slot_index: %zu", variable.slot_index);
            printf("\n          identifier: %s", variable.name.c_str());
            printf("\n          type: %s", type_to_string(variable.type).data());
            printf("\n");
        }
    }
}