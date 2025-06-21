#include "compiler_visitor.h"

#include "byte_code_enum_translation.h"
#include "byte_code_generator.h"
#include "byte_code_vm.h"
#include "binary_ops.h" 
#include "unary_ops.h" 

#include "SimpleLangVisitor.h"

#include "stack_logger.h"

class BytecodeEmitter : public SimpleLangVisitor {
public:
    ByteCodeGenerator gen;
    StackLogger logger;

    void panic(const antlr4::ParserRuleContext* context, CompilationErrorType type, CompilationErrorVariant info) {
        gen.set_error({
            type,
            context->start->getStartIndex(),
            context->start->getLine(),
            context->start->getCharPositionInLine(),
            context->stop->getStartIndex(),
            context->stop->getLine(),
            context->stop->getCharPositionInLine(),
            info
        });

        throw std::runtime_error("panic");
    }

    Program visit_program(antlr4::tree::ParseTree* tree) {
        return std::any_cast<Program>(visit(tree));
    }

    Type visit_expression(SimpleLangParser::ExpressionContext* context) {
        return std::any_cast<Type>(visit(context));
    }

    Type visit_expression(SimpleLangParser::ExpressionCallFunctionContext* context) {
        return std::any_cast<Type>(visit(context));
    }

    std::vector<Type> visit_expression_list(SimpleLangParser::ExpressionListContext* context) {
        if (!context) {
            return {};
        }

        return std::any_cast<std::vector<Type>>(visit(context));
    }

    Type visit_literal(SimpleLangParser::LiteralContext* context) {
        return std::any_cast<Type>(visit(context));
    }

    Type visit_type(SimpleLangParser::TypeContext* context) {
        return std::any_cast<Type>(visit(context));
    }

    Variable visit_argument(SimpleLangParser::ArgumentContext* context) {
        return std::any_cast<Variable>(visit(context));
    }

    std::vector<Variable> visit_argument_list(SimpleLangParser::ArgumentListContext* context) {
        if (!context) {
            return {};
        }

        return std::any_cast<std::vector<Variable>>(visit(context));
    }
    
    void emit(OpType op) {
        gen.emit(ByteCodeOp{ op, {} });
    }

    void emit(ByteCodeOp&& op) {
        gen.emit(std::move(op));
    }

    Type emit_unary_op(const antlr4::ParserRuleContext* context, Type right_type, UnaryOperatorType op) {
        CompilationErrorType err = map_unary_op_validate(right_type, op);
        if (err != CompilationErrorType::NONE) {
            panic(context, err, {});
        }

        std::optional<UnaryOpMapOut> out = map_unary_op(right_type, op);
        if (!out.has_value()) {
            panic(context, CompilationErrorType::TYPE_MISMATCH, {});
        }

        UnaryOpMapOut value = out.value();
        
        emit(value.code);

        return value.result_type;
    }

    Type emit_binary_op(const antlr4::ParserRuleContext* context, Type left_type, Type right_type, BinaryOperatorType op) {
        CompilationErrorType err = map_binary_op_validate(left_type, right_type, op);
        if (err != CompilationErrorType::NONE) {
            panic(context, err, {});
        }

        std::optional<BinaryOpMapOut> out = map_binary_op(left_type, right_type, op);
        if (!out.has_value()) {
            panic(context, CompilationErrorType::TYPE_MISMATCH, {});
        }

        BinaryOpMapOut value = out.value();

        emit(value.code);

        return value.result_type;
    }

    // Visitor

    // Top level program

    std::any visitProgram(SimpleLangParser::ProgramContext *context) {
        logger("program %s", context->getText().c_str());
        logger.push();
        gen.scope_push(ScopeType::GLOBAL);
        visitChildren(context);
        Program program = gen.get_program();
        gen.scope_pop();
        logger.pop();
        return program;
    }

    // Functions

    std::any visitDeclarationFunction(SimpleLangParser::DeclarationFunctionContext *context) {
        logger("declaration function %s", context->getText().c_str());
        logger.push();

        Type return_type = visit_type(context->type());
        std::vector<Variable> arguments = visit_argument_list(context->argumentList());
        std::string identifier = context->ID()->getText();
        
        CompilationErrorType err = gen.function_declare(return_type, identifier, arguments.size());

        if (err != CompilationErrorType::NONE) {
            panic(context, err, {});
        }

        gen.scope_push(ScopeType::FUNCTION);

        for (const Variable& variable : arguments) {
            err = gen.variable_declare(variable.type, variable.name);
            
            if (err != CompilationErrorType::NONE) {
                return err;
            }

            emit({
                OpType::STORE_VARIABLE,
                ByteCodeStoreVariableOp {
                    variable.type,
                    variable.name
                }
            });
        }

        if (err != CompilationErrorType::NONE) {
            panic(context, err, {});
        }

        visit(context->block());

        if (   gen.get_code_index() == 0 
            || gen.get_last_op_type() != OpType::RETURN) 
        {
            if (return_type == Type::VOID) {
                emit(OpType::RETURN);
            }

            else {
                panic(context, CompilationErrorType::NON_VOID_FUNCTION_MISSING_RETURN, {});
            }
        }

        gen.scope_pop();

        logger.pop();

        return nullptr;
    }

    std::any visitArgumentList(SimpleLangParser::ArgumentListContext *context) {
        logger("argument list %s", context->getText().c_str());
        logger.push();

        std::vector<Variable> arguments;

        std::vector<SimpleLangParser::ArgumentContext*> contexts = context->argument();

        // load in reverse order cus stack
        for (auto itr = contexts.rbegin(); itr != contexts.rend(); itr++) {
            arguments.push_back(visit_argument(*itr));
        }

        logger.pop();

        return arguments;
    }

    std::any visitArgument(SimpleLangParser::ArgumentContext *context) {
        logger("argument %s", context->getText().c_str());
        logger.push();

        Type type = visit_type(context->type());
        std::string identifier = context->ID()->getText();

        logger.pop();

        return Variable { type, identifier };
    }

    std::any visitBlock(SimpleLangParser::BlockContext *context) {
        logger("block %s", context->getText().c_str());
        logger.push();
        
        gen.scope_push(ScopeType::BLOCK);
        visitChildren(context);
        gen.scope_pop();

        logger.pop();

        return nullptr;
    }

    // Statement

    std::any visitStatement(SimpleLangParser::StatementContext *context) {
        logger("statement %s", context->getText().c_str());
        logger.push();
        std::any out = visitChildren(context); 
        logger.pop();
        return out;
    }

    std::any visitStatementExpression(SimpleLangParser::StatementExpressionContext *context) {
        logger("statement expression %s", context->getText().c_str());
        logger.push();

        visitChildren(context);

        emit({ OpType::POP, {}});

        logger.pop();

        return nullptr;
    }

    std::any visitStatementVariableDeclaration(SimpleLangParser::StatementVariableDeclarationContext *context) {
        logger("statement variable declaration %s", context->getText().c_str());
        logger.push();

        Type expression_type = visit_expression(context->expression());

        Type type = visit_type(context->type());

        if (type != expression_type) {
            panic(context, CompilationErrorType::TYPE_MISMATCH, {});
        }

        std::string identifier = context->ID()->getText();

        CompilationErrorType err = gen.variable_declare(type, identifier);

        if (err != CompilationErrorType::NONE) {
            panic(context, err, {});
        }

        emit({
            OpType::STORE_VARIABLE,
            ByteCodeStoreVariableOp{
                type,
                identifier
            }
        });

        logger.pop();

        return nullptr;
    }

    std::any visitStatementVariableAssignment(SimpleLangParser::StatementVariableAssignmentContext *context) {
        logger("expression variable assignment %s", context->getText().c_str());
        logger.push();

        std::string identifier = context->ID()->getText();

        if (!gen.scope_is_identifier_declared(identifier)) {
            panic(context, CompilationErrorType::IDENTIFIED_NOT_DECLARED, {});
        }
        
        Type type = visit_expression(context->expression());

        emit({
            OpType::STORE_VARIABLE,
            ByteCodeStoreVariableOp {
                type,
                identifier
            }
        });

        logger.pop();

        return nullptr;
    }

    std::any visitStatementReturn(SimpleLangParser::StatementReturnContext *context) {
        logger("statement return %s", context->getText().c_str());
        logger.push();
        
        Type type = Type::VOID;

        if (context->expression()) {
            type = visit_expression(context->expression());
        }

        std::optional<Function> function = gen.function_get_current_info();
        
        if (!function.has_value()) {
            panic(context, CompilationErrorType::PARSE_ERROR, {});
        }

        if (type != function.value().return_type) {
            panic(context, CompilationErrorType::TYPE_MISMATCH, {});
        }

        emit({ OpType::RETURN, {} });

        logger.pop();

        return nullptr;
    }

    std::any visitStatementIf(SimpleLangParser::StatementIfContext *context) {
        logger("statement if %s", context->getText().c_str());
        logger.push();
        
        Type type = visit_expression(context->expression());

        if (type != Type::BOOL) {
            panic(context, CompilationErrorType::TYPE_MISMATCH, {});
        }

        size_t jump_to_patch_code_index = gen.get_code_index();

        gen.emit_placeholder();

        visit(context->block());

        size_t after_block_code_index = gen.get_code_index();

        gen.patch(jump_to_patch_code_index, {
            OpType::JUMP_IF_FALSE,
            ByteCodeJumpOp {after_block_code_index}
        });

        logger.pop();

        return nullptr;
    }

    std::any visitStatementWhile(SimpleLangParser::StatementWhileContext *context) {
        logger("statement while %s", context->getText().c_str());
        logger.push();

        size_t before_condition_code_index = gen.get_code_index();

        Type type = visit_expression(context->expression());

        if (type != Type::BOOL) {
            panic(context, CompilationErrorType::TYPE_MISMATCH, {});
        }

        size_t jump_to_patch_code_index = gen.get_code_index();

        gen.emit_placeholder();

        visit(context->block());

        gen.emit({
            OpType::JUMP,
            ByteCodeJumpOp { before_condition_code_index }
        });

        size_t after_block_code_index = gen.get_code_index();

        gen.patch(jump_to_patch_code_index, {
            OpType::JUMP_IF_FALSE,
            ByteCodeJumpOp {after_block_code_index}
        });

        logger.pop();

        return nullptr;
    }
    
    // Expression

    std::any visitExpressionList(SimpleLangParser::ExpressionListContext *context) {
        logger("expression list %s", context->getText().c_str());
        logger.push();

        std::vector<Type> types;

        for (SimpleLangParser::ExpressionContext* expression : context->expression()) {
            types.push_back(visit_expression(expression));
        }

        logger.pop();
        
        return types;
    }

    std::any visitExpression(SimpleLangParser::ExpressionContext *context) {
        logger("expression %s", context->getText().c_str());
        logger.push();

        Type out_expression_type = Type::VOID;

        size_t expression_count = context->expression().size();

        // Operations
        if (context->op) {
            switch (expression_count) {
                case 1: {
                    Type right_type = visit_expression(context->expression(0));
                    UnaryOperatorType op = unary_operator_type_from_string(context->op->getText());
                    out_expression_type = emit_unary_op(context, right_type, op);
                    break;
                }

                case 2: {
                    Type left_type = visit_expression(context->expression(0));
                    Type right_type = visit_expression(context->expression(1));
                    BinaryOperatorType op = binary_operator_type_from_string(context->op->getText());
                    out_expression_type = emit_binary_op(context, left_type, right_type, op);
                    break;
                }

                // Catch all
                default: {
                    panic(context, CompilationErrorType::PARSE_ERROR, {});
                    break;
                }
            }
        }

        // Parentheses
        else if (expression_count > 0) {
            out_expression_type = visit_expression(context);
        }

        // Function call
        else if (context->expressionCallFunction()) {
            out_expression_type = visit_expression(context->expressionCallFunction());
        }
        
        // Function variable lookup
        else if (context->ID()) {
            std::string identifier = context->ID()->getText();
            std::optional<Type> variable_type = gen.variable_get_type(identifier);

            if (!variable_type.has_value()) {
                panic(context, CompilationErrorType::IDENTIFIED_NOT_DECLARED, {});
            }

            emit({
                OpType::PUSH_VARIABLE,
                ByteCodePushVariableOp {
                    variable_type.value(),
                    identifier
                }
            });

            out_expression_type = variable_type.value();
        }

        // Literal lookup
        else if (context->literal()) {
            out_expression_type = visit_literal(context->literal());
        }

        // Catch all
        else {
            panic(context, CompilationErrorType::PARSE_ERROR, {});
        }

        logger.pop();
        return out_expression_type;
    }

    std::any visitExpressionCallFunction(SimpleLangParser::ExpressionCallFunctionContext *context) {
        logger("expression call function %s", context->getText().c_str());
        logger.push();

        std::string identifier = context->ID()->getText();

        if (!gen.scope_is_identifier_declared(identifier)) {
            panic(context, CompilationErrorType::IDENTIFIED_NOT_DECLARED, {});
        }

        std::optional<Function> function = gen.function_get_info(identifier);

        if (!function.has_value()) {
            panic(context, CompilationErrorType::PARSE_ERROR, {});
        }

        std::vector<Type> expression_types = visit_expression_list(context->expressionList());

        if (function.value().argument_count != expression_types.size()) {
            panic(context, CompilationErrorType::FUNCTION_CALLED_WITH_WRONG_NUMBER_OF_ARGS, {});
        }

        for (size_t i = 0; i < expression_types.size(); i++) {
            Type expression_type = expression_types.at(i);
            Type argument_type = function.value().local_variables.at(i).type;

            if (expression_type != argument_type) {
                panic(context, CompilationErrorType::TYPE_MISMATCH, {});
            }
        }

        emit({
            OpType::CALL_FUNCTION,
            ByteCodeCallFunctionOp { function.value().code_index }
        });

        logger.pop();

        return function.value().return_type;
    }

    std::any visitLiteral(SimpleLangParser::LiteralContext *context) {
        std::string valueString = context->getText();

        logger("literal %s", valueString.c_str());
        logger.push();

        ByteCodePushLiteralOp literal;

        if (context->BOOL()) {
            bool value = valueString == "true"; // grammar requires this is 'true' or 'false'
            literal = { Type::BOOL, value};
        }

        else if (context->INT()) {
            int value = std::stoi(valueString);
            literal = { Type::INT, value };
        }

        else if (context->FLOAT()) {
            float value = std::stof(valueString);
            literal = { Type::FLOAT, value };
        }

        else if (context->STRING()) {
            literal = { Type::STRING, valueString };
        }

        emit({
            OpType::PUSH_LITERAL,
            literal
        });

        logger.pop();

        return literal.type;
    }

    std::any visitType(SimpleLangParser::TypeContext *context) {
        logger("type %s", context->getText().c_str());
        logger.push();

        std::string typeString = context->getText();
        Type type = type_from_string(typeString);

        logger.pop();

        return type;
    }
};

CompilationResults generate_byte_code(antlr4::tree::ParseTree* tree) {
    BytecodeEmitter emitter;

    try {
        return {
            emitter.visit_program(tree),
            {}
        };
    }

    catch (std::runtime_error e) {
        printf("Exception: %s\n", e.what());

        return {
            {},
            emitter.gen.get_error()
        };
    }
}