#include "compiler_visitor.h"

#include "byte_code_enum_translation.h"
#include "byte_code_generator.h"
#include "byte_code_vm.h"
#include "binary_ops.h" 

#include "SimpleLangVisitor.h"

class BytecodeEmitter : public SimpleLangVisitor {
public:
    ByteCodeGenerator gen;
    bool m_next_block_push_scope = true;

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

    Type visit_expression(SimpleLangParser::ExpressionContext* context) {
        return std::any_cast<Type>(visit(context));
    }

    Type visit_type(SimpleLangParser::TypeContext* context) {
        return std::any_cast<Type>(visit(context));
    }

    void emit(ByteCodeOp&& op) {
        gen.emit(std::move(op));
    }

    Type emit_binary_op(const antlr4::ParserRuleContext* context, Type left_type, Type right_type, BinaryOperatorType op) {
        CompilationErrorType err = map_binary_op_validate(left_type, right_type, op);
        if (err != CompilationErrorType::NONE) {
            panic(context, err, {});
        }

        auto out = map_binary_op(left_type, right_type, op);
        if (!out.has_value()) {
            panic(context, CompilationErrorType::TYPE_MISMATCH, {});
        }

        BinaryOpMapOut value = out.value();
        emit({value.code, ByteCodeBinaryOp { value.result_type } });

        return value.result_type;
    }

    // Visitor

    // Top level program

    std::any visitProgram(SimpleLangParser::ProgramContext *context) {
        printf("program %s\n", context->getText().c_str());
        return visitChildren(context); 
    }

    // Functions

    std::any visitDeclarationFunction(SimpleLangParser::DeclarationFunctionContext *context) {
        printf("declaration function %s\n", context->getText().c_str());

        // std::vector<ArgumentListContext *> argumentList();
        // ArgumentListContext* argumentList(size_t i);

        Type type = visit_type(context->type());
        std::string identifier = context->ID()->getText();

        if (gen.find_function(identifier).has_value()) {
            panic(context, CompilationErrorType::IDENTIFIED_ALREADY_DECLARED, {});
        }

        gen.store_function(identifier, type, gen.get_code_index());

        gen.push_scope();
        m_next_block_push_scope = false;

        if (auto argumentList = context->argumentList()) {
            visit(argumentList);
        }

        visit(context->block());

        if (   gen.get_op_count() == 0 
            || gen.get_last_op_type() != OpType::RETURN) 
        {
            if (type == Type::VOID) {
                emit(ByteCodeOp{ OpType::RETURN, {} });
            }

            else {
                panic(context, CompilationErrorType::NON_VOID_FUNCTION_MISSING_RETURN, {});
            }
        }

        gen.pop_scope();

        return nullptr;
    }

    std::any visitArgumentList(SimpleLangParser::ArgumentListContext *context) {
        printf("argument list %s\n", context->getText().c_str());

        // load in reverse order cus stack
        for (auto itr = context->children.rbegin(); itr != context->children.rend(); itr++) {
            visit(*itr);
        }

        return nullptr;
    }

    std::any visitArgument(SimpleLangParser::ArgumentContext *context) {
        printf("argument %s\n", context->getText().c_str());

        Type type = visit_type(context->type());
        std::string identifier = context->ID()->getText();

        if (gen.find_variable(identifier).has_value()) {
            panic(context, CompilationErrorType::IDENTIFIED_ALREADY_DECLARED, {});
        }

        gen.store_variable(identifier, type);

        emit({
            OpType::STORE_VARIABLE,
            ByteCodeStoreVariableOp { type, identifier }
        });

        return nullptr;
    }

    std::any visitBlock(SimpleLangParser::BlockContext *context) {
        printf("block %s\n", context->getText().c_str());

        if (m_next_block_push_scope) {
            gen.push_scope();
            visitChildren(context);
            gen.pop_scope();
        }

        else {
            m_next_block_push_scope = true;
            visitChildren(context);
        }

        return nullptr;
    }

    // Statement

    std::any visitStatement(SimpleLangParser::StatementContext *context) {
        printf("statement %s\n", context->getText().c_str());
        return visitChildren(context); 
    }

    std::any visitStatementVariableDeclaration(SimpleLangParser::StatementVariableDeclarationContext *context) {
        printf("statement variable declaration %s\n", context->getText().c_str());

        std::string identifier = context->ID()->getText();

        if (gen.find_variable(identifier).has_value()) {
            panic(context, CompilationErrorType::IDENTIFIED_ALREADY_DECLARED, {});
        }

        Type typeOfVariable = visit_type(context->type());
        Type typeOfExpression = visit_expression(context->expression());

        if (typeOfVariable != typeOfExpression) {
            panic(context, CompilationErrorType::TYPE_MISMATCH, {});
        }

        emit({
            OpType::STORE_VARIABLE,
            ByteCodeStoreVariableOp{
                typeOfVariable,
                identifier
            }
        });

        gen.store_variable(identifier, typeOfVariable);

        return nullptr;
    }

    std::any visitStatementVariableAssignment(SimpleLangParser::StatementVariableAssignmentContext *context) {
        printf("expression variable assignment %s\n", context->getText().c_str());

        std::string identifier = context->ID()->getText();

        if (!gen.find_variable(identifier).has_value()) {
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

        return nullptr;
    }

    std::any visitStatementReturn(SimpleLangParser::StatementReturnContext *context) {
        printf("statement return %s\n", context->getText().c_str());
        
        Type type = Type::VOID;

        if (context->expression()) {
            type = visit_expression(context->expression());
        }

        // verify that the return type matches the current function

        if (type != gen.get_current_function_type()) {
            panic(context, CompilationErrorType::TYPE_MISMATCH, {});
        }

        emit({ OpType::RETURN, {} });

        return nullptr;
    }

    std::any visitStatementIf(SimpleLangParser::StatementIfContext *context) {
        printf("statement if %s\n", context->getText().c_str());

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

        return nullptr;
    }

    std::any visitStatementWhile(SimpleLangParser::StatementWhileContext *context) {
        printf("statement while %s\n", context->getText().c_str());

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

        return nullptr;
    }
    
    // Expression

    std::any visitExpressionList(SimpleLangParser::ExpressionListContext *context) {
        printf("expression list %s\n", context->getText().c_str());
        return visitChildren(context);
    }

    std::any visitExpressionComparison(SimpleLangParser::ExpressionComparisonContext *context) {
        printf("expression comparaison %s\n", context->getText().c_str());

        Type left_type = visit_expression(context->expression(0));
        Type right_type = visit_expression(context->expression(1));
        BinaryOperatorType op = binary_operator_type_from_string(context->op->getText());

        return emit_binary_op(context, left_type, right_type, op);
    }

    std::any visitExpressionAddSubtract(SimpleLangParser::ExpressionAddSubtractContext *context) {
        printf("expression add subtract %s\n", context->getText().c_str());

        Type left_type = visit_expression(context->expression(0));
        Type right_type = visit_expression(context->expression(1));
        BinaryOperatorType op = binary_operator_type_from_string(context->op->getText());

        return emit_binary_op(context, left_type, right_type, op);
    }

    std::any visitExpressionMultiplyDivide(SimpleLangParser::ExpressionMultiplyDivideContext *context) {
        printf("expression multiply divide %s\n", context->getText().c_str());

        Type left_type = visit_expression(context->expression(0));
        Type right_type = visit_expression(context->expression(1));
        BinaryOperatorType op = binary_operator_type_from_string(context->op->getText());

        return emit_binary_op(context, left_type, right_type, op);
    }

    std::any visitExpressionUnary(SimpleLangParser::ExpressionUnaryContext *context) {
        printf("expression unary %s\n", context->getText().c_str());
        // todo:
        return visitChildren(context);
    }

    std::any visitExpressionCallFunction(SimpleLangParser::ExpressionCallFunctionContext *context) {
        printf("expression call function %s\n", context->getText().c_str());

        std::string identifier = context->ID()->getText();
        
        std::optional<FunctionDeclaration> function = gen.find_function(identifier);

        if (!function.has_value()) {
            panic(context, CompilationErrorType::IDENTIFIED_NOT_DECLARED, {});
        }

        if (auto expressionList = context->expressionList()) {
            visit(expressionList);
        }

        emit({
            OpType::CALL_FUNCTION,
            ByteCodeCallFunctionOp { function.value().code_index }
        });

        return function.value().type;
    }

    std::any visitExpressionParentheses(SimpleLangParser::ExpressionParenthesesContext *context) {
        printf("expression parentheses %s\n", context->getText().c_str());
        return visit(context->expression());
    }

    std::any visitExpressionVariableReference(SimpleLangParser::ExpressionVariableReferenceContext *context) {
        printf("expression variable reference %s\n", context->getText().c_str());

        std::string identifier = context->ID()->getText();
        std::optional<VariableDeclaration> variable = gen.find_variable(identifier);

        if (!variable.has_value()) {
            panic(context, CompilationErrorType::IDENTIFIED_NOT_DECLARED, {});
        }
 
        emit({
            OpType::PUSH_VARIABLE,
            ByteCodePushVariableOp {
                variable.value().type,
                identifier
            }
        });

        return variable.value().type; 
    }

    std::any visitExpressionLiteral(SimpleLangParser::ExpressionLiteralContext *context) {
        printf("expression literal %s\n", context->getText().c_str());
        return visitChildren(context); 
    }

    std::any visitLiteral(SimpleLangParser::LiteralContext *context) {
        std::string valueString = context->getText();

        printf("literal %s\n", valueString.c_str());

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

        return literal.type;
    }

    std::any visitType(SimpleLangParser::TypeContext *context) {
        printf("type %s\n", context->getText().c_str());

        std::string typeString = context->getText();
        Type type = type_from_string(typeString);

        return type;
    }
};

CompilationResults generate_byte_code(antlr4::tree::ParseTree* tree) {
    BytecodeEmitter emitter;
    CompilationResults result;

    try {
        emitter.visit(tree);
        result = emitter.gen.get_results();
    }

    catch (std::runtime_error e) {
        printf("Exception: %s\n", e.what());
        result = emitter.gen.get_results();
    }

    return result;
}