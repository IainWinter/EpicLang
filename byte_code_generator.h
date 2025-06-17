#pragma once

#include "compiler_result.h"

#include <vector>
#include <unordered_map>
#include <optional>

enum class IdentifierType {
    FUNCTION,
    VARIABLE
};

struct Identifier {
    IdentifierType type;
    std::string name;
};

enum class ScopeType {
    GLOBAL,
    FUNCTION,
    BLOCK
};

struct Scope {
    ScopeType type;
    std::vector<Identifier> identifiers;
};

struct FunctionInfo {
    size_t code_index;
    Type return_type;
};

class ByteCodeGenerator {
public:
    // Operations

    void emit(ByteCodeOp&& operation);

    void emit_placeholder();

    void patch(size_t code_index, ByteCodeOp&& replacement_operation);

    size_t get_code_index() const;

    // Scopes, variables, and functions

    void scope_push(ScopeType type);

    CompilationErrorType scope_pop();

    CompilationErrorType scope_declare_identifier(IdentifierType type, const std::string& name);

    bool scope_is_identifier_declared(const std::string& name) const;

    ScopeType scope_get_current_type() const;

    CompilationErrorType variable_declare(Type type, const std::string& identifier);

    std::optional<Type> variable_get_type(const std::string& identifier) const;
    
    CompilationErrorType function_declare(Type return_type, const std::string& identifier, size_t argument_count);

    std::optional<FunctionInfo> function_get_info(const std::string& identifier) const;
    
    std::optional<FunctionInfo> function_get_current_info() const;

    // Error state

    void set_error(CompilationError&& error);

    // Getting results

    const CompilationError& get_error() const;

    Program get_program() const;

private:

    std::vector<Scope> m_scopes;

    std::vector<Variable> m_global_variables;

    std::vector<Function> m_functions;

    // Store a visibility stack for when loading a variable (check if its in scope)
    // Store a local variable stack for keeping track of stack frames ()

    // When declaring a var
    //    add to current visibility scope
    //    add to current local vars frame

    // When loading a var
    //    check if its in the current scope or any of its parents

    // at begining of scope
    //    add a new scope to the visibility stack
    
    // at end of scope
    //    pop from visibility stack

    // at begining of function
    //    add a new local var scope

    // at end of function
    //    pop local var scope

    // visibility scopes are used for comp time error checking

    // frame scopes are used to generate the bytecode needed for function calls and pushing
    // args to the stack

    /*
    
        int test(int z) {
            int y = z;
            {
                int w = y;
            }

            int test2(int z) {
                int q = z;
            }
        }

        Vscope (
            z <type> <name>
            Vscope (
                y <type> <name>
                Vscope (
                    w <type> <name>
                )
                Vscope (
                    z  <type> <name> <- prob cannot have this unless their is shadowing
                    Vscope (
                        q <type> <name>
                    )
                )
            )
        )

        Fscope (
            name
            return_type
            code_index
            local vars:
                z <type> <slot>
                y <type> <slot>
                w <type> <slot>
            Fscope (
                name
                return_type
                code_index
                local vars:
                    z <type> <slot>
                    q <type> <slot>
            )
        )
    
    */

    // Store variables in frames
    // Store functions in global table

    // struct FrameScopeVisibilityIndex {
    //     int frame_index;
    //     int item_index;
    // };

    // struct VisibilityScope {
    //     std::unordered_map<std::string, FrameScopeVisibilityIndex> variables;
    //     std::unordered_map<std::string, FrameScopeVisibilityIndex> functions;
    // };

    // struct FrameScope {
    //     std::vector<VariableDeclaration> variables;
    //     std::vector<FunctionDeclaration> functions;
    // };

    // std::vector<VisibilityScope> m_visibility_scopes;
    // std::vector<int> m_visibility_scope_stack;
    
    // std::vector<FrameScope> m_frame_scopes;
    // std::vector<int> m_frame_scope_stack;

    std::vector<ByteCodeOp> m_operations;

    CompilationError m_error;

    // std::vector<ByteCodeOp> m_operations;
    // std::vector<Function> m_functions;

    // // these dont map to stack frames!
    // std::unordered_map<int, Scope> m_scopes;
    // std::vector<int> m_scope_stack;
    // int m_next_scope = 0;
    
    // // grammar has functions only in global scope
    // Type m_current_function_type;

    // CompilationError m_error;
};





/*
    int main(int x) {
        int y = 1;
        {
            int z = x + y;
        }

        int lambda(int z) {
            return y + z;
        }

        y = lambda(x);
        return y;
    }

    int x = main(5);


(
    main
    (
        x
        (
            y
            (
                z
            )

            lambda
            (
                z
            )
        )
    )
    x
)

















*/