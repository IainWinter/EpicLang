#include "program.h"

#include "byte_code_printer.h"

void Program::print() const {
    print(main_code_index);
}

void Program::print(size_t highlight_code_index) const {
    printf("\033[2J\033[H");
    printf("Program:\n");
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
    
    printf("\nProgram Counter: %zu\n", highlight_code_index);


}