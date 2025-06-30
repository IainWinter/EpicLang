#include "compiler.h"

#include "compiler_visitor.h"
#include "byte_code_enum_translation.h"

#include "SimpleLangLexer.h"
#include "SimpleLangParser.h"

CompilationResults compile(std::string_view text, const std::vector<ExternalFunction>& external_functions) {
    antlr4::ANTLRInputStream input(text);
    SimpleLangLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    SimpleLangParser parser(&tokens);

    antlr4::tree::ParseTree* tree = parser.program();

    if (parser.getNumberOfSyntaxErrors() > 0) {
        printf("Parsing failed: syntax errors encountered\n");
        return {{}, { CompilationErrorType::PARSE_ERROR }};
    }
    
    printf("\nAST:\n");
    std::cout << tree->toStringTree(&parser) << std::endl;

    printf("\nVisitor Trace:\n");

    CompilationResults result = generate_byte_code(tree, external_functions);
    
    if (result.error.type != CompilationErrorType::NONE) {
        CompilationError error = result.error;
        
        printf("Compiler error: %s\n", compilation_error_type_to_string(error.type).data());
        printf("from %zd:%zd\n", error.startLine, error.startCharacterIndex);
        printf("to %zd:%zd\n", error.stopLine, error.stopCharacterIndex);
        
        std::string error_string = input.getText(antlr4::misc::Interval(error.start, error.stop));
        
        printf("\n------------------------------------------------\n");
        printf("%s", error_string.c_str());
        printf("\n------------------------------------------------\n");
    }

    return result;
}