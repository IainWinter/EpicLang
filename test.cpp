#include "test.h"

#include "compiler.h"
#include "byte_code_vm.h"

#include <assert.h>

struct TestResults {
    CompilationResults compilation;
    ByteCodeVmState execution; 
};

TestResults test_run(std::string_view text) {
    CompilationResults compilation = compile(text, {});

    if (compilation.error.type != CompilationErrorType::NONE) {
        return { compilation, {} };
    }

    compilation.program.print();

    ByteCodeVm vm(compilation.program);
    vm.execute();

    return { compilation, vm.get_state() };
}

struct Test {
    std::string_view name;
    void(*func)();
};

std::vector<Test> tests;

static int declare_test(std::string_view name, void(*func)()) {
    tests.push_back({ name, func });
    return 0;
}

#define TEST(func_name) \
    void _test_##func_name(); \
    int _##func_name = declare_test(#func_name, _test_##func_name); \
    void _test_##func_name()

TEST(identifier_not_declared) {
    TestResults test = test_run(
        "void main() {"
        "   int x = x;"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::IDENTIFIED_NOT_DECLARED);
}

TEST(identifier_already_declared) {
    TestResults test = test_run(
        "void main() {"
        "   int x = 0;"
        "   int x = 0;"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::IDENTIFIED_ALREADY_DECLARED);
}

TEST(math_type_mismatch) {
    TestResults test = test_run(
        "void main() {"
        "   string x = \"\";"
        "   string z = x + 1;"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::TYPE_MISMATCH);
}

TEST(no_strings_in_math_operations) {
    TestResults test = test_run(
        "void main() {"
        "   string x = \"\";"
        "   string z = x + \"\";"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::MATH_OPERATION_ON_STRING);
}

TEST(scope_owns_declaration) {
    TestResults test = test_run(
        "void main() {"
        "   int x = 4;"
        "   {"
        "       int y = 3;"
        "   }"
        "   int z = x + y;"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::IDENTIFIED_NOT_DECLARED);
}

TEST(no_math_comparisons_on_strings) {
    TestResults test = test_run(
        "void main() {"
        "   string x = \"\";"
        "   bool z = x > \"\";"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::MATH_OPERATION_ON_STRING);
}

TEST(can_compare_strings) {
    TestResults test = test_run(
        "void main() {"
        "   string x = \"\";"
        "   bool z = x == \"\";"
        "   bool w = x != \"\";"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::NONE);
}

TEST(return_statement_needs_same_type) {
    TestResults test = test_run(
        "float main() {"
        "   return 1;"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::TYPE_MISMATCH);
}

TEST(return_statement) {
    TestResults test = test_run(
        "int main() {"
        "   return 1;"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::NONE);

    assert(test.compilation.program.operations == std::vector<ByteCodeOp>({
        { OpType::PUSH_LITERAL, ByteCodePushLiteralOp{ Type::INT, 1 } },
        { OpType::RETURN }
    }));

    ByteStack stack;
    stack.push_int(1);
    assert(test.execution.stack.equals(stack));
}

TEST(call_function) {
    TestResults test = test_run(
        "int test() {"
        "    return 1;"
        "}"
        ""
        "void main() {"
        "    int x = test();"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::NONE);
    assert(std::get<int>(test.execution.variables.at("x").second) == 1);
}

TEST(if_statement) {
    TestResults test = test_run(
        "void main() {"
        "    int x = 0;"
        ""
        "    if (x == 0) {"
        "        x = 1;"
        "    }"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::NONE);

    assert(test.compilation.program.operations == std::vector<ByteCodeOp>({
        { OpType::PUSH_LITERAL, ByteCodePushLiteralOp{ Type::INT, 0 } },
        { OpType::STORE_VARIABLE, ByteCodeStoreVariableOp{ Type::INT, "x" } },
        { OpType::PUSH_VARIABLE, ByteCodePushVariableOp{ Type::INT, "x" } },
        { OpType::PUSH_LITERAL, ByteCodePushLiteralOp{ Type::INT, 0 } },
        { OpType::EQUALS_INT, { } },
        { OpType::JUMP_IF_FALSE, ByteCodeJumpOp{ 8 } },
        { OpType::PUSH_LITERAL, ByteCodePushLiteralOp{ Type::INT, 1 } },
        { OpType::STORE_VARIABLE, ByteCodeStoreVariableOp{ Type::INT, "x" } },
        { OpType::RETURN, { } },
    }));

    assert(std::get<int>(test.execution.variables.at("x").second) == 1);
}

TEST(function_args) {
    TestResults test = test_run(
        "int test(int x, int y) {"
        "    return x + y;"
        "}"
        ""
        "void main() {"
        "    int x = test(1, 2);"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::NONE);
    assert(std::get<int>(test.execution.variables.at("x").second) == 3);
}

TEST(for_loop) {
    TestResults test = test_run(
        "void main() {"
        "    int x = 0;"
        ""
        "    while (x < 10) {"
        "        x = x + 1;"
        "    }"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::NONE);
    assert(std::get<int>(test.execution.variables.at("x").second) == 10);
}

TEST(statement_expression_cleans_up_stack) {
    TestResults test = test_run(
        "void test() {"
        "}"
        ""
        "void main() {"
        "   test();"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::NONE);
    assert(test.execution.stack.size() == 0);
}

TEST(function_needs_correct_number_of_args) {
    TestResults test = test_run(
        "void test(int x) {"
        "}"
        ""
        "void main() {"
        "   test();"
        "   test(1, 2);"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::FUNCTION_CALLED_WITH_WRONG_NUMBER_OF_ARGS);
}

TEST(function_needs_correct_type_of_args) {
    TestResults test = test_run(
        "void test(int x) {"
        "}"
        ""
        "void main() {"
        "   test(1.1);"
        "}"
    );

    assert(test.compilation.error.type == CompilationErrorType::TYPE_MISMATCH);
}

TEST(global_state_block_defines_global_identifier) {
    TestResults test = test_run(
        "state { int x = 0; }"
        "void main(int x) {}"
    );

    assert(test.compilation.error.type == CompilationErrorType::IDENTIFIED_ALREADY_DECLARED);
}

void run_tests() {
    for (const Test& test : tests) {
        printf("Test %s\n", test.name.data());
        test.func();
        printf("Passed!\n\n");
    }
}